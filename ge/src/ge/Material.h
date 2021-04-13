/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <ge/RenderSettings.h>
#include <ge/TextureSettings.h>
#include <ge/Uniform.h> // For UniformType.

#include <vector>

namespace ge {
class ShaderProgram;
class Renderer;
class Camera;
class MeshR;
class Renderable;

class Material {

	RenderSettings _renderSettings;
	TextureSettings _textureSettings0;
	TextureSettings _textureSettings1;
	ShaderProgram* _shader;
	std::vector<UniformValue> _uniforms;
	int _renderQueue;

	int getIndex(const std::string& name) const;

public:

	Material();
	Material(const Material&);
	Material& operator=(const Material&);

	Material(ShaderProgram* program);
	virtual ~Material();

	void setShaderProgram(ShaderProgram* program);

	void bind(const glm::mat4& worldMatrix, Renderer* renderer, Camera* camera, Renderable* meshR);

	void setRenderQueue(int index) {
		_renderQueue = index;
	}
	int getRenderQueue() const {
		return _renderQueue;
	}

	RenderSettings& getRenderSettings() {
		return _renderSettings;
	}

	TextureSettings& getTextureSettings0() {
		return _textureSettings0;
	}
	TextureSettings& getTextureSettings1() {
		return _textureSettings1;
	}

	// Set uniform values.
	void setFloat(const std::string& name, float v);
	void setFloat(int index, float v);
	void setInt(const std::string& name, int v);
	void setInt(int index, int v);
	void setMatrix(const std::string& name, const glm::mat3& v);
	void setMatrix(int index, const glm::mat3& v);
	void setMatrix(const std::string& name, const glm::mat4& v);
	void setMatrix(int index, const glm::mat4& v);
	void setVector(const std::string& name, const glm::vec2& v);
	void setVector(int index, const glm::vec2& v);
	void setVector(const std::string& name, const glm::vec3& v);
	void setVector(int index, const glm::vec3& v);
	void setVector(const std::string& name, const glm::vec4& v);
	void setVector(int index, const glm::vec4& v);
	void setColor(const std::string& name, const glm::vec4& v);
	void setColor(int index, const glm::vec4& v);

	// For internal use.
	const ShaderProgram* getShader() const {
		return _shader;
	}
};
}