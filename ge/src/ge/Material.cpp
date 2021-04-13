/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Material.h>
#include <ge/ShaderProgram.h>
#include <ge/Renderable.h>
#include <ge/RenderContext.h>
#include <ge/MeshR.h>
#include <ge/Renderer.h>
#include <ge/Camera.h>

using namespace ge;
using namespace std;

Material::Material() : _shader(0), _renderQueue(RQ_OPAQUE) {

}

Material::Material(const Material& other) : _renderSettings(other._renderSettings),
	_textureSettings0(other._textureSettings0),
	_textureSettings1(other._textureSettings1),
	_shader(other._shader),
	_renderQueue(other._renderQueue)
{
	if (_shader) {
		_shader->copyUniformValues(_uniforms, other._uniforms);
	}
}

Material&
Material::operator=(const Material& other) {
	if (this != &other) {
		if (_shader) {
			_shader->destroyUniformValues(_uniforms);
		}
		_renderSettings = other._renderSettings;
		_textureSettings0 = other._textureSettings0;
		_textureSettings1 = other._textureSettings1;
		_renderQueue = other._renderQueue;
		_shader = other._shader;
		if (_shader) {
			_shader->copyUniformValues(_uniforms, other._uniforms);
		}
	}
	return *this;
}

Material::Material(ShaderProgram* shader) : _shader(shader), _renderQueue(RQ_OPAQUE) {
	if (_shader) {
		_shader->createUniformValues(_uniforms);
	}
}

Material::~Material() {
	if (_shader) {
		_shader->destroyUniformValues(_uniforms);
	}
}

void
Material::setShaderProgram(ShaderProgram* shader) {
	_shader = shader;
	if (_shader) {
		_shader->createUniformValues(_uniforms);
	}
}

void
Material::bind(const glm::mat4& worldMatrix, Renderer* renderer, Camera* cam, Renderable* meshR) {
	RenderContext& context = renderer->getContext();
	_renderSettings.bind(context);
	_textureSettings0.bind(0, context);
	_textureSettings1.bind(1, context);

	if (context.shaderId != _shader->getProgramId()) {
		glUseProgram(_shader->getProgramId());
		context.shaderId = _shader->getProgramId();
	}

	if (_shader->worldMatrix != -1) {
		setMatrix(_shader->worldMatrix, worldMatrix);
	}
	if (_shader->projectionMatrix != -1) {
		const glm::mat4& mat = cam->getProjectionMatrix();
		setMatrix(_shader->projectionMatrix, mat);
	}
	if (_shader->invProjectionMatrix != -1) {
		const glm::mat4& mat = glm::inverse(cam->getProjectionMatrix());
		setMatrix(_shader->invProjectionMatrix, mat);
	}
	glm::mat4 modelViewMatrix = cam->getModelViewMatrix() * worldMatrix;
	
	if (_shader->modelViewMatrix != -1) {
		setMatrix(_shader->modelViewMatrix, modelViewMatrix);
	}
	if (_shader->normalMatrix != -1) {
		setMatrix(_shader->normalMatrix, glm::transpose(glm::inverse(modelViewMatrix)));
	}
	if (_shader->viewMatrix != -1) {
		setMatrix(_shader->viewMatrix, cam->getModelViewMatrix());
	}
	if (_shader->worldViewProjectionMatrix != -1) {
		glm::mat4 modelViewProjectionMatrix = cam->getProjectionMatrix() * modelViewMatrix;
		setMatrix(_shader->worldViewProjectionMatrix, modelViewProjectionMatrix);
	}
	//if (_shader->viewProjectionMatrix!= -1) {
	//    glm::mat4 mat = renderer->getProjectionMatrix() * cam->getModelViewMatrix();
	//    _shader->viewProjectionMatrix->set(mat, false);
	//}
	//if (_shader->viewPort!= -1) {
	//    glm::vec4 vp(cam->getViewPortLeft(), cam->getViewPortTop(), cam->getViewPortRight(), cam->getViewPortBottom());
	//    _shader->viewPort->set(vp);
	//}
	//if (_shader->frustumNearFar!= -1) {
	//    glm::vec2 frustum(cam->getFrustumNear(), cam->getFrustumFar());
	//    _shader->frustumNearFar->set(frustum);
	//}
	//if (_shader->resolution!= -1) {
	//    glm::vec2 resolution(cam->getWidth(), cam->getHeight());
	//    _shader->resolution->set(resolution);
	//}
	//if (_shader->aspectRatio!= -1) {
	//    float aspect = ((float)cam->getWidth()) / cam->getHeight();
	//    _shader->aspectRatio->set(aspect);
	//}
	//if (_shader->cameraPosition!= -1) {
	//	setVector(_shader->cameraPosition, cam->getLocation());
	//}
	//if (_shader->cameraDirection!= -1) {
	//    _shader->cameraDirection->set(cam->getDirection());
	//}
	//if (_shader->cameraLeft!= -1) {
	//    _shader->cameraLeft->set(cam->getLeft());
	//}
	//if (_shader->matrixPalette!= -1) {
	//	const SkinnedMesh* skin = mesh->asSkinnedMesh();
	//	if (skin) {
	//		const SkeletonPose* pose = skin->getCurrentPose();
	//		if (pose) {
	//			_shader->matrixPalette->set(pose->getMatrixPalette(), false);
	//		}
	//	}
	//}

	_shader->bind(_uniforms);
}

int
Material::getIndex(const std::string& name) const {
	const ShaderUniform* u = _shader->getShaderUniform(name);
	if (u) {
		return u->getIndex();
	}
	return -1;
}

void
Material::setFloat(const std::string& name, float v) {
	setFloat(getIndex(name), v);
}

void
Material::setFloat(int index, float v) {
	if (index == -1) {
		return;
	}
	UniformValue& u = _uniforms[index];
	if (u.floatValue0 != v) {
		u.floatValue0 = v;
	}
}

void Material::setInt(const std::string& name, int v) {
	setInt(getIndex(name), v);
}

void
Material::setInt(int index, int v){ 
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	if (u.intValue0 != v) {
		u.intValue0 = v;
	}
}

void 
Material::setMatrix(const std::string& name, const glm::mat3& v) {
	setMatrix(getIndex(name), v);
}

void
Material::setMatrix(int index, const glm::mat3& v) {
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	float* v0 = u.floatValue;
	const float* v0End = v0 + 9;
	const float* v1 = glm::value_ptr(v);
	while (v0 != v0End) {
		if (*v0 != (float)*v1) {
			*v0 = *v1;
		}
		++v0;
		++v1;
	}
}

void 
Material::setMatrix(const std::string& name, const glm::mat4& v){ 
	setMatrix(getIndex(name), v);
}

void
Material::setMatrix(int index, const glm::mat4& v){
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	// I tried various things to optimize this such as unrolling the
	// loop and send the values unconditionally, but they were all slower.
	float* v0 = u.floatValue;

	//memcpy(v0, glm::value_ptr(value), sizeof(float)*16);

	const float* v0End = v0 + 16;
	const float* v1 = glm::value_ptr(v);
	while (v0 != v0End) {
		if (*v0 != (float)*v1) {
			*v0 = *v1;
		}
		++v0;
		++v1;
	}
}

void
Material::setVector(const std::string& name, const glm::vec2& v){
	setVector(getIndex(name), v);
}

void
Material::setVector(int index, const glm::vec2& v){
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	if (u.floatValue[0] != v.x || u.floatValue[1] != v.y) {
		u.floatValue[0] = v.x;
		u.floatValue[1] = v.y;
	}
}


void
Material::setVector(const std::string& name, const glm::vec3& v){ 
	setVector(getIndex(name), v);
}

void
Material::setVector(int index, const glm::vec3& v){ 
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	if (u.floatValue[0] != v.x || u.floatValue[1] != v.y || u.floatValue[2] != v.z) {
		u.floatValue[0] = v.x;
		u.floatValue[1] = v.y;
		u.floatValue[2] = v.z;
	}
}

void
Material::setVector(const std::string& name, const glm::vec4& v){
	setVector(getIndex(name), v);
}

void
Material::setVector(int index, const glm::vec4& v){
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	if (u.floatValue[0] != v.x || u.floatValue[1] != v.y || u.floatValue[2] != v.z ||
		u.floatValue[3] != v.w) {
		u.floatValue[0] = v.x;
		u.floatValue[1] = v.y;
		u.floatValue[2] = v.z;
		u.floatValue[3] = v.w;
	}
}


void
Material::setColor(const std::string& name, const glm::vec4& v){ 
	setColor(getIndex(name), v);
}
void
Material::setColor(int index, const glm::vec4& v){ 
	if (index == -1) {
		return;
	}

	UniformValue& u = _uniforms[index];
	if (u.floatValue[0] != v.x || u.floatValue[1] != v.y || u.floatValue[2] != v.z ||
		u.floatValue[3] != v.w) {
		u.floatValue[0] = v.x;
		u.floatValue[1] = v.y;
		u.floatValue[2] = v.z;
		u.floatValue[3] = v.w;
	}
}
