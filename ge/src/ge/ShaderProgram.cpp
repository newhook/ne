/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/ShaderProgram.h>
#include <ge/PsShader.h>
#include <ge/VsShader.h>
#include <ge/Uniform.h>
#include <ge/InputStream.h>
#include <ge/Logger.h>

#include <ge/json/json.h>

#include <iostream>
#include <memory>

using namespace ge;
using namespace std;

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

namespace {
Logger logger(__FILE__);
class ShaderProgramRegistry {
	std::map<string, unique_ptr<ShaderProgram> > _programs;
public:
		
	ShaderProgramRegistry() {

	}

	void load(sf::InputStream& stream) {
		std::vector<char> data;
		data.resize(static_cast<size_t>(stream.getSize()) + 1);
		data[data.size() - 1] = '\0';
		stream.read(&data[0], data.size() - 1);
		
		Json::Value root;
		Json::Reader reader;
		bool rc = reader.parse(&data[0], &data[data.size()], root);
		if (!rc) {
			LOG_W("cannot parse JSON");
			return;
		}
		if (!root.isObject()) {
			LOG_W("JSON is malformed");
			return;
		}

		string path = "";
		// TODO: Handle array.
		const Json::Value pathV = root["path"];
		if (pathV.isString()) {
			path = pathV.asString();
		}

		const Json::Value shaders = root["shaders"];
		if (!shaders.isObject()) {
			LOG_W("JSON is malformed");
			return;
		}

		for (auto p = shaders.begin(); p != shaders.end(); ++p) {
			std::string name = p.memberName();
			const Json::Value frag = (*p)["frag"];
			const Json::Value vert = (*p)["vert"];

			ge::FileInputStream vSource(path);
			rc = vSource.open(frag.asString());
			if (!rc) {
				LOG_W("opening `%s' failed: %s", frag.asString(), strerror(errno));
				continue;
			}
			ge::PsShader pShader;
			rc = pShader.load(vSource);
			if (!rc) {
				LOG_W("loading `%s' failed: %s", frag.asString(), strerror(errno));
				continue;
			}
			ge::FileInputStream fSource(path);
			rc = fSource.open(vert.asString());
			if (!rc) {
				LOG_W("opening `%s' failed: %s", vert.asString(), strerror(errno));
				continue;
			}
			ge::VsShader vShader;
			rc = vShader.load(fSource);
			if (!rc) {
				LOG_W("loading `%s' failed: %s", vert.asString(), strerror(errno));
				continue;
			}


			unique_ptr<ge::ShaderProgram> prog(new ge::ShaderProgram(pShader, vShader));
			prog->init();

			_programs.insert(make_pair(name, move(prog)));
		}
	}
	ShaderProgram* find(const std::string& name) {
		auto p = _programs.find(name);
		if (p != _programs.end()) {
			return p->second.get();
		}
		return 0;
	}
};
ShaderProgramRegistry* registry;
}

ShaderProgram::ShaderProgram(const PsShader& ps, const VsShader& vs) : _ps(ps), _vs(vs), _programId(0) {

}

ShaderProgram::~ShaderProgram() {
	if (_programId != 0) {
		// XXX:
	}
	destroyUniformValues(_uniformValues);
}

void
ShaderProgram::initialize(sf::InputStream& input) {
	if (registry == 0) {
		registry = new ShaderProgramRegistry();
		registry->load(input);
	}
}

ShaderProgram*
ShaderProgram::find(const std::string& name) {
	if (registry == 0) {
		FileInputStream stream;
		stream.open("shaders.json");
		initialize(stream);
	}
	return registry->find(name);
}

void
ShaderProgram::init() {
	// Create program
	_programId = glCreateProgram();
	int vertexShaderId = _vs.getShaderId();
	int fragmentShaderId = _ps.getShaderId();
	glAttachShader(_programId, vertexShaderId);
	glAttachShader(_programId, fragmentShaderId);

	// BUGFIX: Make it work on mac
	glBindAttribLocation(_programId, 0, "b3d_Position");

	glLinkProgram(_programId);
	int attributeCount;
	glGetProgramiv(_programId, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	for (int i = 0; i < attributeCount; ++i) {
		char name[65];
		GLint size;
		GLenum type;
		glGetActiveAttrib(_programId, i, 64, NULL, &size, &type, name);
		// Eliminate array notation if it exists
		char* brace = strstr(name, "[");
		if (brace) {
			*brace = '\0';
		}
		int location = glGetAttribLocation(_programId, name);
		_attributes.insert(make_pair(name, ShaderAttribute(name, location)));
	}
	// Poll uniforms
	int uniformCount;
	glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &uniformCount);
	for (int i = 0; i < uniformCount; ++i) {
		char name[65];
		GLint size;
		GLenum type;
		glGetActiveUniform(_programId, i, 64, NULL, &size, &type, name);
		// Eliminate array notation if it exists
		char* brace = strstr(name, "[");
		if (brace) {
			*brace = '\0';
		}
		int location = glGetUniformLocation(_programId, name);
		_uniforms.insert(make_pair(name, ShaderUniform(name, UniformType::fromGLValue(type), location)));
	}

	vertex = getShaderAttribute("position");
	normal = getShaderAttribute("normal");
	color = getShaderAttribute("color");
	uv = getShaderAttribute("uv");
	uv2 = getShaderAttribute("uv2");

	worldMatrix = getShaderUniformIndex("worldMatrix");
	projectionMatrix = getShaderUniformIndex("projectionMatrix");
	invProjectionMatrix = getShaderUniformIndex("invProjectionMatrix");
	modelViewMatrix = getShaderUniformIndex("modelViewMatrix");
	viewMatrix = getShaderUniformIndex("viewMatrix");
	normalMatrix = getShaderUniformIndex("normalMatrix");
	worldViewProjectionMatrix = getShaderUniformIndex("worldViewProjectionMatrix");
	cameraPosition = getShaderUniformIndex("cameraPosition");

	createUniformValues(_uniformValues);
}

void
ShaderProgram::copyUniformValues(std::vector<UniformValue>& values, const std::vector<UniformValue>& other) {
	if (values.empty()) {
		createUniformValues(values);
	}

	for (map<string, ShaderUniform>::const_iterator p = _uniforms.begin(); p != _uniforms.end(); ++p) {
		size_t sz = p->second.getType().getSize();
		if (sz > 4) {
			memcpy(values[p->second.getIndex()].raw, other[p->second.getIndex()].raw, sz);
		}
		else {
			values[p->second.getIndex()].intValue0 = other[p->second.getIndex()].intValue0;
		}
	}
}
void
ShaderProgram::createUniformValues(std::vector<UniformValue>& values) {
	int index = -1;
	for (map<std::string, ShaderUniform>::const_iterator p = _uniforms.begin(); p != _uniforms.end(); ++p) {
		if (index < p->second.getIndex()) {
			index = p->second.getIndex();
		}
	}
	values.resize(index+1);
	for (auto p = _uniforms.begin(); p != _uniforms.end(); ++p) {
		size_t sz = p->second.getType().getSize();
		if (sz > 4) {
			values[p->second.getIndex()].raw = new char[sz];
			memset(values[p->second.getIndex()].raw, 0, sz);
		}
	}
}

void
ShaderProgram::destroyUniformValues(std::vector<UniformValue>& values) {
	for (auto p = _uniforms.begin(); p != _uniforms.end(); ++p) {
		size_t sz = p->second.getType().getSize();
		if (sz > 4) {
			delete[] values[p->second.getIndex()].raw;
		}
	}
	values.clear();
}

ShaderAttribute*
ShaderProgram::getShaderAttribute(const std::string& name) {
	auto p = _attributes.find(name);
	if (p == _attributes.end()) {
		return 0;
	}
	return &p->second;
}

const ShaderUniform*
ShaderProgram::getShaderUniform(const std::string& name) const {
	std::map<std::string, ShaderUniform>::const_iterator p = _uniforms.find(name);
	if (p == _uniforms.end()) {
		return 0;
	}
	return &p->second;
}

int
ShaderProgram::getShaderUniformIndex(const std::string& name) const {
	const ShaderUniform* u = getShaderUniform(name);
	if (u != 0) {
		return u->getIndex();
	}
	return -1;
}

void
ShaderProgram::bind(vector<UniformValue>& uniforms) {
	for (map<std::string, ShaderUniform>::const_iterator p = _uniforms.begin(); p != _uniforms.end(); ++p) {
		int index = p->second.getIndex();
		
		UniformValue& u = _uniformValues[index];
		UniformValue& v = uniforms[index];
		
		switch(p->second.getType()) {
			case UniformType::INT_1:
			case UniformType::SAMPLER: 
				if (u.intValue0 != v.intValue0) {
					u.intValue0 = v.intValue0;

					glUniform1i(index, u.intValue0);
				}
				break;
			case UniformType::INT_2:		
				//glUniform2i(index, u.intValue[0], u.intValue[1]);
				break;
			case UniformType::INT_3:
				//glUniform3i(index, u.intValue[0], u.intValue[1], u.intValue[2]);
				break;
			case UniformType::INT_4:
				//glUniform4i(index, u.intValue[0], u.intValue[1], u.intValue[2], u.intValue[3]);
				break;
			case UniformType::FLOAT_1:
				if (u.floatValue0 != v.floatValue0) {
					u.floatValue0 = v.floatValue0;

					glUniform1f(index, u.floatValue0);
				}
				break;

			case UniformType::FLOAT_2:
				if (u.floatValue[0] != v.floatValue[0] || u.floatValue[1] != v.floatValue[1]) {
					u.floatValue[0] = v.floatValue[0];
					u.floatValue[1] = v.floatValue[1];

					glUniform2f(index, u.floatValue[0], u.floatValue[1]);
				}
				break;

			case UniformType::FLOAT_3:
				if (u.floatValue[0] != v.floatValue[0] || u.floatValue[1] != v.floatValue[1] || u.floatValue[2] != v.floatValue[2]) {
					u.floatValue[0] = v.floatValue[0];
					u.floatValue[1] = v.floatValue[1];
					u.floatValue[2] = v.floatValue[2];
					
					glUniform3f(index, u.floatValue[0], u.floatValue[1], u.floatValue[2]);
				}
				break;
			case UniformType::FLOAT_4:
				if (u.floatValue[0] != v.floatValue[0] || u.floatValue[1] != v.floatValue[1] || u.floatValue[2] != v.floatValue[2] ||
					u.floatValue[3] != v.floatValue[3]) {
					u.floatValue[0] = v.floatValue[0];
					u.floatValue[1] = v.floatValue[1];
					u.floatValue[2] = v.floatValue[2];
					u.floatValue[3] = v.floatValue[3];
					
					glUniform4f(index, u.floatValue[0], u.floatValue[1], u.floatValue[2], u.floatValue[3]);
				}
				break;
			case UniformType::MAT_2:
				//glUniformMatrix2fv(index, 1, false, u.floatValue);
				break;
			case UniformType::MAT_3: {
				bool dirty = false;
				float* v0 = u.floatValue;
				const float* v0End = v0 + 9;
				const float* v1 = v.floatValue;
				while (v0 != v0End) {
					if (*v0 != (float)*v1) {
						*v0 = *v1;
						dirty = true;
					}
					++v0;
					++v1;
				}

				if (dirty) {
					glUniformMatrix3fv(index, 1, false, u.floatValue);
				}
				break;
			}
			case UniformType::MAT_4: {
				bool dirty = false;
				float* v0 = u.floatValue;
				const float* v0End = v0 + 16;
				const float* v1 = v.floatValue;
				while (v0 != v0End) {
					if (*v0 != (float)*v1) {
						*v0 = *v1;
						dirty = true;
					}
					++v0;
					++v1;
				}
				if (dirty) {
					glUniformMatrix4fv(index, 1, false, u.floatValue);
				}
				break;
			}
			default:
				break;
		}
	}
}
