/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>
#include <ge/File.h>
#include <string>
#include <vector>
#include <SFML/System/InputStream.hpp>
#include <cstdio>

namespace ge {

class FileInputStream : public sf::InputStream
{
public:

	FileInputStream();
	// TODO: vector<std::string>
	FileInputStream(const std::string&);

	~FileInputStream();

	bool open(const std::string& filename);

	virtual sf::Int64 read(void* data, sf::Int64 size);

	virtual sf::Int64 seek(sf::Int64 position);

	virtual sf::Int64 tell();

	virtual sf::Int64 getSize();

private:

	std::string _path;
	std::FILE* _file;
};

#ifdef never
class StringInputStream : public InputStream {
	const char* _data;
	size_t _size;
	bool _copy;
	size_t _off;

public:

	StringInputStream(const char* data, bool copy);
	StringInputStream(const char* data, size_t len, bool copy);
	~StringInputStream();

	virtual size_t read(void* buf, size_t n);
	virtual size_t getLength();
};
#endif
}