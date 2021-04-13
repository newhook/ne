/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <exception>
#include <string>
#include <ostream>

namespace ge {

class Exception : public std::exception {
public:

	Exception();
	Exception(const char*, int);
	Exception(const char*, int, const std::string&);
	virtual ~Exception() throw();
	virtual const char* what() const throw();

	virtual void print(std::ostream&) const;
	const char* file() const;
	int line() const;
	virtual std::string name() const;

private:

	static const char* _name;

	const char* _file;
	int _line;
	std::string _stacks;
	mutable ::std::string _str;     // Initialized lazily in what().

protected:

	const std::string _reason;
};

class IllegalArgumentException : public Exception {
	static const char* _name;
public:

	IllegalArgumentException(const char*, int);
	IllegalArgumentException(const char*, int, const std::string&);
	~IllegalArgumentException() throw();

	virtual std::string name() const;
};

class IOException : public Exception {
	static const char* _name;
	const int _err;
public:

	IOException(const char*, int);
	IOException(const char*, int, const std::string&);
	IOException(const char*, int, int, const std::string&);
	~IOException() throw();

	int getError() const { return _err; }

	virtual std::string name() const;

	virtual void print(std::ostream&) const;
};

class ReadException : public Exception {
	static const char* _name;
	const int _err;
public:

	ReadException(const char*, int);
	ReadException(const char*, int, const std::string&);
	ReadException(const char*, int, int, const std::string&);
	~ReadException() throw();

	int getError() const { return _err; }

	virtual std::string name() const;

	virtual void print(std::ostream&) const;
};

}