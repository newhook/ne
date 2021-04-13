/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/VsShader.h>
#include <ge/InputStream.h>
#include <ge/Logger.h>

using namespace ge;
using namespace std;

namespace {
Logger logger(__FILE__);
}

VsShader::VsShader() {

}

VsShader::~VsShader() {

}

bool
VsShader::load(sf::InputStream& stream) {
	std::vector<unsigned char> data;
	data.resize(static_cast<size_t>(stream.getSize()) + 1);
	data[data.size() - 1] = '\0';
	stream.read(&data[0], data.size() - 1);

	_shaderId = glCreateShader(GL_VERTEX_SHADER);

	const char* sources[1] = { reinterpret_cast<const char*>(&data[0]) };
	glShaderSource(_shaderId, 1, sources, 0);
	glCompileShader(_shaderId);

	int status;
	glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);
	if (!status) {
		int val;
		glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &val);
		std::string infoLog;
		if (val > 0) {
			char* buffer = new char[val + 1];
			glGetShaderInfoLog(_shaderId, val + 1, &val, buffer);
			infoLog = buffer;
			delete[] buffer;
		}

		LOG_W("Shader compile error:\n%s", infoLog)
		return false;
	}
	return true;
}