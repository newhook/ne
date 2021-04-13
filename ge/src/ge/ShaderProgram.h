/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/PsShader.h>
#include <ge/VsShader.h>
#include <ge/ShaderAttribute.h>
#include <ge/ShaderUniform.h>

#include <map>
#include <vector>

namespace ge {

class ShaderProgram {
	const PsShader _ps;
	const VsShader _vs;

	int _programId;

	std::map<std::string, ShaderUniform> _uniforms;
	std::map<std::string, ShaderAttribute> _attributes;

	std::vector<UniformValue> _uniformValues;

	// Noncopyable.
	ShaderProgram();
	ShaderProgram(const ShaderProgram&);
	ShaderProgram& operator==(const ShaderProgram&);

public:

	ShaderProgram(const PsShader& ps, const VsShader& vs);
	virtual ~ShaderProgram();

	static void initialize(sf::InputStream&);
	static ShaderProgram* find(const std::string& name);

	void init();
	
	void copyUniformValues(std::vector<UniformValue>& values, const std::vector<UniformValue>& other);
	void createUniformValues(std::vector<UniformValue>& values);
	void destroyUniformValues(std::vector<UniformValue>& values);

	const PsShader& getPsShader() const {
		return _ps;
	}
	const VsShader& getVsShader() const {
		return _vs;
	}

	int getProgramId() const {
		return _programId;
	}

	ShaderAttribute* getShaderAttribute(const std::string& name);
	const ShaderUniform* getShaderUniform(const std::string& name) const;

	int getShaderUniformIndex(const std::string& name) const;

	const std::map<std::string, ShaderUniform>& getShaderUniforms() const {
		return _uniforms;
	}

	// Internal use only. Cache attributes.
	ShaderAttribute* vertex;
	ShaderAttribute* normal;
	ShaderAttribute* color;
	ShaderAttribute* uv;
	ShaderAttribute* uv2;

	int worldMatrix;
	int projectionMatrix;
	int invProjectionMatrix;
	int modelViewMatrix;
	int viewMatrix; // The cameras view matrix.
	int normalMatrix;
	int worldViewProjectionMatrix;
	int cameraPosition;

	void bind(std::vector<UniformValue>& uniforms);
};
}