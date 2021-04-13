/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Exception.h>

#include <sstream>

using namespace ge;
using namespace std;

ostream&
operator << (ostream & os, const Exception &ex) {
	ex.print(os);
	return os;
}

const char* ge::Exception::_name = "b3d::Exception";

Exception::Exception() : _file(0),
_line(0){
#ifndef BTS_TARGET_IPHONE
	// TODO: For the "final" client we'll want to get rid of this.
//	getStackTrace(_stacks, 0);
#endif
}

Exception::Exception(const char* file, int line) : _file(file),
_line(line){
#ifndef BTS_TARGET_IPHONE
	// TODO: For the "final" client we'll want to get rid of this.
//	getStackTrace(_stacks, 0);
#endif
}

Exception::Exception(const char* file, int line, const string& reason) : _file(file),
_line(line),
_reason(reason){
#ifndef BTS_TARGET_IPHONE
	// TODO: For the "final" client we'll want to get rid of this.
	//getStackTrace(_stacks, 0);
#endif
}

Exception::~Exception() throw(){
}

void
Exception::print(ostream& out) const {
	if (_file && _line > 0) {
		out << _file << ':' << _line << ": ";
	}
	out << name();
	if (!_reason.empty()) {
		out << ": " << _reason;
	}
	out << _stacks;
}

const char*
Exception::what() const throw(){
	try {
		if (_str.empty()) {
			stringstream s;
			print(s);
			_str = s.str();     // Lazy initialization.
		}
		return _str.c_str();
	}
	catch (...) {
	}

	return "";
}

string
Exception::name() const {
	return _name;
}

const char*
Exception::file() const {
	return _file;
}

int
Exception::line() const {
	return _line;
}

const char* IllegalArgumentException::_name = "IllegalArgumentException";

IllegalArgumentException::IllegalArgumentException(const char* file, int line) : Exception(file, line) {
}

IllegalArgumentException::IllegalArgumentException(const char* file, int line, const string& reason) : Exception(file,
	line, reason) {
}

IllegalArgumentException::~IllegalArgumentException() throw() {
}

string
IllegalArgumentException::name() const {
	return _name;
}

const char* IOException::_name = "IOException";

IOException::IOException(const char* file, int line) : Exception(file, line), _err(-1) {
}

IOException::IOException(const char* file, int line, const string& reason) : Exception(file, line, reason), _err(-1) {
}

IOException::IOException(const char* file, int line, int err, const string& reason) : Exception(file, line,
	reason), _err(err) {
}

IOException::~IOException() throw() {
}

string
IOException::name() const {
	return _name;
}

void
IOException::print(ostream& out) const {
	Exception::print(out);
	if (_err != -1) {
#ifdef NE_TARGET_WINDOWS
		char buf[256];
		strerror_s(buf, sizeof(buf), _err);
		out << ": " << buf;
#else
		out << ": " << strerror(_err);
#endif
	}
}

const char* ReadException::_name = "ReadException";

ReadException::ReadException(const char* file, int line) : Exception(file, line), _err(-1) {
}

ReadException::ReadException(const char* file, int line, const string& reason) : Exception(file, line,
	reason), _err(-1) {
}

ReadException::ReadException(const char* file, int line, int err, const string& reason) : Exception(file, line,
	reason), _err(err) {
}

ReadException::~ReadException() throw() {
}

string
ReadException::name() const {
	return _name;
}

void
ReadException::print(ostream& out) const {
	Exception::print(out);
	if (_err != -1) {
#ifdef NE_TARGET_WINDOWS
		char buf[256];
		strerror_s(buf, sizeof(buf), _err);
		out << ": " << buf;
#else
		out << ": " << strerror(_err);
#endif
	}
}

namespace {
#ifdef NDEBUG
	bool nullHandleAbort = false;
#else
	bool nullHandleAbort = true;
#endif
}
