/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once
#include <ge/Config.h>

#include <ge/tinyformat.h>

#include <string>

namespace ge {
class LoggerLevel {
public:
	enum Type {
		UNSET = -1,
		All = 0,
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Fatal,
		Output,
		Off,
	};
private:
	Type _val;
public:

	LoggerLevel(Type val) : _val(val) {
	}

	LoggerLevel() : _val(UNSET) {
	}

	operator Type() const { return _val; }

	std::string toString() const {
		switch (_val) {
		case UNSET: return "UNSET";
		case All: return "All";
		case Trace: return "Trace";
		case Debug: return "Debug";
		case Info: return "Info";
		case Warn: return "Warn";
		case Error: return "Error";
		case Fatal: return "Fatal";
		case Output: return "Output";
		case Off: return "Off";
		default: return "UNKNOWN";
		}
	}
};

class Logger {
private:

	bool _inherit;
	LoggerLevel _level;

	const std::string _cat;
	const std::string _shortcat;
	const std::string _file;

public:

	static void destroy();

	static Logger& getRootLogger();
	static Logger* getLogger(const std::string&);

	static void configureLevel(const std::string &, LoggerLevel);
	static void configureAppender(const std::string&, const std::string&);

	static void clearConfig();
	static void reconfig();

	Logger();
	Logger(const char*);
	Logger(const char*, const char*);
	~Logger();

	bool isTraceEnabled() const;
	void trace(const char* file, int, const std::string&) const;
	void trace(int, const std::string&) const;
	void trace(const std::string&) const;

	bool isDebugEnabled() const;
	void debug(const char* file, int, const std::string&) const;
	void debug(int, const std::string&) const;
	void debug(const std::string&) const;

	bool isInfoEnabled() const;
	void info(const char* file, int, const std::string&) const;
	void info(int, const std::string&) const;
	void info(const std::string&) const;

	void warn(const char* file, int, const std::string&) const;
	void warn(int, const std::string&) const;
	void warn(const std::string&) const;

	void error(const char* file, int, const std::string&) const;
	void error(int, const std::string&) const;
	void error(const std::string&) const;

	void fatal(const char* file, int, const std::string&) const;
	void fatal(int, const std::string&) const;
	void fatal(const std::string&) const;

	// Unconditionally write log output.
	void output(const char* file, int, const std::string&) const;
	void output(int, const std::string&) const;
	void output(const std::string&) const;

	void log(LoggerLevel level, const std::string& file, int line, const std::string& msg) const;
	void log(LoggerLevel level, const std::string& msg) const;

	void setLevel(LoggerLevel);
	void clearLevel();
	LoggerLevel getLevel() const;

	void inheritLevel(LoggerLevel);

	const std::string& getCategory() const;
};


// Macros to make using the logger less verbose.
#define LOG_T(...) if(logger.isTraceEnabled()) { logger.trace(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_D(...) if(logger.isDebugEnabled()) { logger.debug(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_I(...) if(logger.isInfoEnabled()) { logger.info(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_W(...) { logger.warn(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_E(...) { logger.error(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_F(...) { logger.fatal(__LINE__, tfm::format(__VA_ARGS__)); }
#define LOG_O(...) { logger.output(__LINE__, tfm::format(__VA_ARGS__)); }

#define LOG_S_T(s) if(logger.isTraceEnabled()) { logger.trace(__LINE__, s); }
#define LOG_S_D(s) if(logger.isDebugEnabled()) { logger.debug(__LINE__, s); }
#define LOG_S_I(s) if(logger.isInfoEnabled()) { logger.info(__LINE__, s); }
#define LOG_S_W(s) { logger.warn(__LINE__, s); }
#define LOG_S_E(s) { logger.error(__LINE__, s); }
#define LOG_S_F(s) { logger.fatal(__LINE__, s); }
#define LOG_S_O(s) { logger.output(__LINE__, s); }
}
