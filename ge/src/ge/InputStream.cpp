/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/InputStream.h>

#include <ge/Exception.h>
#include <sstream>

using namespace ge;
using namespace std;

FileInputStream::FileInputStream() :
	_file(NULL)
{
}

FileInputStream::FileInputStream(const string& path) :
	_path(path),
	_file(NULL)
{
}

FileInputStream::~FileInputStream()
{
	if (_file) {
		std::fclose(_file);
	}
}

bool 
FileInputStream::open(const std::string& filename)
{
	if (_file) {
		std::fclose(_file);
	}

	if (!_path.empty()) {
		_file = std::fopen((_path + "/" + filename).c_str(), "rb");
	}
	if (_file == 0) {
		_file = std::fopen(filename.c_str(), "rb");
	}

	return _file != NULL;
}

sf::Int64
FileInputStream::read(void* data, sf::Int64 size)
{
	if (_file) {
		return std::fread(data, 1, static_cast<std::size_t>(size), _file);
	}
	return -1;
}

sf::Int64
FileInputStream::seek(sf::Int64 position)
{
	if (_file) {
		std::fseek(_file, static_cast<std::size_t>(position), SEEK_SET);
		return tell();
	}
	return -1;
}

sf::Int64 FileInputStream::tell()
{
	if (_file) {
		return std::ftell(_file);
	}
	return -1;
}

sf::Int64 
FileInputStream::getSize()
{
	if (_file)
	{
		sf::Int64 position = tell();
		std::fseek(_file, 0, SEEK_END);
		sf::Int64 size = tell();
		seek(position);
		return size;
	}
	return -1;
}

#ifdef never
StringInputStream::StringInputStream(const char* data, bool copy) :
	_data(copy ? new char[strlen(data)] : data),
	_size(strlen(data)),
	_copy(copy) {
	if (_copy) {
		strcpy(const_cast<char*>(_data), data);
	}
}

StringInputStream::StringInputStream(const char* data, size_t size, bool copy) :
	_data(copy ? new char[size] : data),
	_size(size), _copy(copy) {
	if (_copy) {
		memcpy(const_cast<char*>(_data), data, size);
	}
}

StringInputStream::~StringInputStream() {
	if (_copy) {
		delete[] _data;
	}
}
size_t
StringInputStream::read(void* buf, size_t n) {
	if (_off == _size) {
		return -1;
	}

	if (_off + n > _size) {
		n = _size - _off;
	}
	memcpy(buf, _data + _off, n);
	_off += n;
	return n;
}

size_t
StringInputStream::getLength() {
	return _size;
}
#endif