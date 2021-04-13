/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#include <ge/Config.h>
#include <ge/Logger.h>
#include <ge/Exception.h>
//#include <ge/StringUtils.h>

#include <sstream>
#include <iostream>

#include <map>
#include <vector>
#include <mutex>
#include <chrono>
#include <ctime>

#include <stdio.h>

#ifdef BTS_TARGET_UNIX
#include <sys/types.h>
#include <pwd.h>
#endif

using namespace ge;
using namespace std;

namespace {
	class Appender {
	public:

		virtual ~Appender() {
		}

		virtual void output(LoggerLevel level, const string& cat, const string&, int line, const string& msg) = 0;
	};

	// This implementation makes duplicate loggers illegal.
	// It wouldn't be very hard to support duplicate loggers through a level of indirection.

	Logger* root;
	map<string, Logger*>* loggers;
	map<string, LoggerLevel>* configuration;

	vector<Appender*> appenders;

	struct AppenderConfig {
		string type;
		map<string, string> config;
	};

	map<string, AppenderConfig> appenderConfigs;

	string
		toString(LoggerLevel level) {
		switch (level) {
		case LoggerLevel::All:
			return "ALL";
		case LoggerLevel::Trace:
			return "Trace";
		case LoggerLevel::Debug:
			return "Debug";
		case LoggerLevel::Info:
			return "Info";
		case LoggerLevel::Warn:
			return "Warn";
		case LoggerLevel::Error:
			return "Error";
		case LoggerLevel::Fatal:
			return "Fatal";
		case LoggerLevel::Output:
			return "Output";
		case LoggerLevel::Off:
			return "Off";
		case LoggerLevel::UNSET:
			return "UNSET";
		}
		return "UNKNOWN";
	}

	class ConsoleAppender : public Appender {
		mutex outputLock;
	public:
		ConsoleAppender(const map<string, string>&config) {
		}

		virtual void output(LoggerLevel level, const string& cat, const string& file, int line,
			const string& msg) {
			lock_guard<mutex> lock(outputLock);
			time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
			cout << ctime(&now) << ": ";
			cout << cat << ": ";
			//        if(file != 0 && *file != '\0') {
			//            cout << file << ": ";
			//        }
			if (level != LoggerLevel::Output) {
				cout << toString(level) << ": ";
			}
			cout << msg;
			cout << endl;
		}
	};

	class FileAppender : public Appender {
		mutex outputLock;
		FILE* fp;

	public:

		FileAppender(const map<string, string>&config) {
			string name = "client.log";
			map<string, string>::const_iterator p = config.find("File");
			if (p != config.end()) {
				name = p->second;
			}
#ifdef GE_TARGET_UNIX
			size_t tilde = name.find_first_of("~");
			if (tilde != string::npos) {
				struct passwd pwd;
				struct passwd *result;
				char buf[16384];

				if (getpwuid_r(geteuid(), &pwd, buf, sizeof(buf), &result) != 0) {
					ostringstream os;
					os << "FileAppender: getpwuid_r failed";
					throw Exception(__FILE__, __LINE__, os.str());
				}
				name = name.replace(tilde, 1, pwd.pw_dir);
			}
#endif
#ifdef NE_TARGET_WINDOWS
			size_t percent = name.find_first_of("%");
			if (percent != string::npos) {
				//char buf[MAX_PATH];
				//HANDLE hProcessToken;
				//if(OpenProcessToken(GetCurrentProcess(),TOKEN_IMPERSONATE|TOKEN_QUERY|TOKEN_DUPLICATE, &hProcessToken)) {
				//	if(ExpandEnvironmentStringsForUser(hProcessToken, name.c_str(), buf, sizeof(buf))) {
				//		name = buf;
				//	}
				//	CloseHandle(hProcessToken);
				//}
				size_t end = name.find_first_of("%", percent + 1);
				char buf[16384];
				if (GetEnvironmentVariable(name.substr(percent + 1, end - percent - 1).c_str(), buf, sizeof(buf)) != 0) {
					name = name.replace(percent, end - percent + 1, buf);
				}
			}
#endif
			bool append = false;
			p = config.find("Append");
			if (p != config.end()) {
				append = (p->second == "true");
			}
			else {
				p = config.find("MaxBackupIndex");
				if (p != config.end()) {
					int maxBackupIndex = atoi(p->second.c_str());
					while (maxBackupIndex > 0) {
						string srcName;
						if (maxBackupIndex == 1) {
							srcName = name;
						}
						else {
							ostringstream sstm;
							sstm << name << "." << (maxBackupIndex - 1);
							srcName = sstm.str();
						}
						ostringstream sstm;
						sstm << name << "." << maxBackupIndex;
						string destName = sstm.str();
						remove(destName.c_str());
						if (rename(srcName.c_str(), destName.c_str()) != 0) {
							// oh well.
						}
						maxBackupIndex--;
					}
				}
			}
			if (append) {
				fp = fopen(name.c_str(), "a");
			}
			else {
				fp = fopen(name.c_str(), "w");
			}
			if (fp == 0) {
				ostringstream os;
				os << "FileAppender: failed to open file `" << name << "': error: `" << strerror(errno) << "'";
				throw Exception(__FILE__, __LINE__, os.str());
			}
		}

		~FileAppender() {
			if (fp != 0) {
				fclose(fp);
			}
		}

		virtual void output(LoggerLevel level, const string& cat, const string& file, int line,
			const string& msg) {
			ostringstream os;
			time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
			os << ctime(&now) << ": ";
			os << cat << ": ";
			if (!file.empty()) {
				os << file << ": ";
			}
			if (level != LoggerLevel::Output) {
				os << toString(level) << ": ";
			}
			os << msg;
			os << "\n";

			lock_guard<mutex> lock(outputLock);
			string data = os.str();
			if (fwrite(data.c_str(), data.size(), 1, fp) != 1) {
				cerr << "fwrite failed: `" << strerror(errno) << "'" << endl;
			}
			fflush(fp);
		}
	};

#ifdef NE_TARGET_WINDOWS
	class DebugAppender : public Appender {
		mutex outputLock;
	public:
		DebugAppender(const map<string, string>&config) {
		}

		virtual void output(LoggerLevel level, const string& cat, const string& file, int line,
			const string& msg) {
			lock_guard<mutex> lock(outputLock);
			time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
			ostringstream os;
			os << ctime(&now) << ": ";
			os << cat << ": ";
			if (!file.empty()) {
				os << file << ": ";
			}
			if (level != LoggerLevel::Output) {
				os << toString(level) << ": ";
			}
			os << msg;
			os << "\n";
			OutputDebugString(os.str().c_str());
		}
	};
#endif

	void initialize() {
		if (root == 0) {
			loggers = new map<string, Logger*>();
			configuration = new map<string, LoggerLevel>();
			root = new Logger();
		}
	}

	bool isAncestorOf(const string& parent, const string& child) {
		return parent.empty() || child.compare(0, parent.size(), parent) == 0;
	}

	Logger* findParent(const string& cat) {
		string::size_type dot = cat.find_last_of('.');
		if (dot == string::npos) {
			return root;
		}
		string pcat = cat.substr(0, dot);
		map<string, Logger*>::const_iterator p = loggers->find(pcat);
		if (p != loggers->end()) {
			return p->second;
		}
		return findParent(pcat);
	}

	Logger* findLogger(const string& cat) {
		if (cat.empty()) {
			return root;
		}
		map<string, Logger*>::const_iterator p = loggers->find(cat);
		if (p != loggers->end()) {
			return p->second;
		}
		return 0;
	}

	void configureFromParent(Logger* logger, const string& cat) {
		map<string, LoggerLevel>::const_iterator p = configuration->find(cat);
		if (p != configuration->end()) {
			logger->setLevel(p->second);
		}
		else {
			// Take level from parent.
			Logger* parent = findParent(cat);
			logger->inheritLevel(parent->getLevel());
		}
	}

	void propagateLevelToChildren(Logger* parent, const string& cat, LoggerLevel level) {
		for (map<string, Logger*>::const_iterator p = loggers->begin(); p != loggers->end(); ++p) {
			if (isAncestorOf(cat, p->first) && findParent(p->first) == parent) {
				p->second->inheritLevel(level);
			}
		}
	}

	string filterFile(const string& file) {
		//if (startsWith(file, "..\\src\\cpp\\")) {
		//	return file.substr(11);         // Strip the leading prefix.
		//}
		//if (startsWith(file, "..\\")) {
		//	return file.substr(3);                 // Strip the leading prefix.
		//}
		//size_t pos = file.find("levelserver\\src\\cpp\\");
		//if (pos != string::npos) {
		//	return file.substr(pos + 20);         // Strip the leading prefix.
		//}

		//pos = file.find("client\\src\\cpp\\");
		//if (pos != string::npos) {
		//	return file.substr(pos + 15);                 // Strip the leading prefix.
		//}

		return file;
	}

	string filterCat(const string& file) {
		return file;
	}
}

Logger&
Logger::getRootLogger() {
	return *root;
}

Logger*
Logger::getLogger(const string& name) {
	map<string, Logger*>::const_iterator p = loggers->find(name);
	if (p != loggers->end()) {
		return p->second;
	}
	return 0;
}

void
Logger::clearConfig() {
	for (vector<Appender*>::iterator p = appenders.begin(); p != appenders.end(); ++p) {
		delete *p;
	}
	appenders.clear();
}

void
Logger::reconfig() {
	for (map<string, AppenderConfig>::const_iterator p = appenderConfigs.begin(); p != appenderConfigs.end(); ++p) {
		try {
			if (p->second.type == "ConsoleAppender") {
				appenders.push_back(new ConsoleAppender(p->second.config));
				continue;
			}
			if (p->second.type == "FileAppender") {
				appenders.push_back(new FileAppender(p->second.config));
				continue;
			}
#ifdef NE_TARGET_WINDOWS
			if (p->second.type == "DebugAppender") {
				appenders.push_back(new DebugAppender(p->second.config));
				continue;
			}
#endif
			cerr << "unknown appender: `" << p->second.type << "'" << endl;
		}
		catch (const Exception &ex) {
			//cerr << "Error creating appender:\n" << ex << endl;
		}
	}

	if (appenders.empty()) {
#ifdef BTS_TARGET_WIN32
		appenders.push_back(new DebugAppender(map<string, string>()));
#else
		appenders.push_back(new ConsoleAppender(map<string, string>()));
#endif
	}
}

void
Logger::destroy() {
	delete root;
	root = 0;

	if (loggers) {
		loggers->clear();
	}
	delete loggers;
	loggers = 0;

	if (configuration) {
		configuration->clear();
	}
	delete configuration;
	configuration = 0;

	appenderConfigs.clear();

	for (vector<Appender*>::iterator p = appenders.begin(); p != appenders.end(); ++p) {
		delete *p;
	}
	appenders.clear();
}

void
Logger::configureAppender(const string& key, const string& value) {
	string::size_type dot = key.find_first_of('.');
	string cat;
	if (dot != string::npos) {
		cat = key.substr(0, dot);
	}
	else {
		cat = key;
	}
	map<string, AppenderConfig>::iterator p = appenderConfigs.find(cat);
	if (p == appenderConfigs.end()) {
		p = appenderConfigs.insert(make_pair(cat, AppenderConfig())).first;
	}
	if (dot == string::npos) {
		p->second.type = value;
	}
	else {
		p->second.config.insert(make_pair(key.substr(dot + 1), value));
	}
}

void
Logger::configureLevel(const string& cat, LoggerLevel level) {
	initialize();
	configuration->insert(make_pair(cat, level));

	Logger* l = findLogger(cat);
	if (l != 0) {
		l->setLevel(level);
	}
	else {
		propagateLevelToChildren(findParent(cat), cat, level);
	}
}

Logger::Logger() : _inherit(false),
	_level(LoggerLevel::Error),
	_cat("") {
}

Logger::Logger(const char* cat) : _inherit(true),
	_cat(cat),
	_shortcat(filterCat(cat)) {
	initialize();
	configureFromParent(this, _cat);

	pair<map<string, Logger*>::iterator, bool> p = loggers->insert(make_pair(_cat, this));
	if (!p.second) {
		cerr << "duplicate logger: " << cat << endl;
	}
}

Logger::Logger(const char* cat, const char* file) : _inherit(true),
	_cat(cat),
	_shortcat(filterCat(cat)),
	_file(filterFile(file)) {
	initialize();
	configureFromParent(this, _cat);

	pair<map<string, Logger*>::iterator, bool> p = loggers->insert(make_pair(_cat, this));
	if (!p.second) {
		cerr << "duplicate logger: " << _cat << endl;
	}
}

Logger::~Logger() {
}

bool
Logger::isTraceEnabled() const {
	return _level <= LoggerLevel::Trace;
}

void
Logger::trace(const char* file, int line, const string& msg) const {
	if (!isTraceEnabled()) {
		return;
	}
	log(LoggerLevel::Trace, filterFile(file), line, msg);
}

void
Logger::trace(int line, const string& msg) const {
	if (!isTraceEnabled()) {
		return;
	}
	log(LoggerLevel::Trace, _file, line, msg);
}

void
Logger::trace(const string& msg) const {
	if (!isTraceEnabled()) {
		return;
	}
	log(LoggerLevel::Trace, msg);
}

bool
Logger::isDebugEnabled() const {
	return _level <= LoggerLevel::Debug;
}

void
Logger::debug(const char* file, int line, const string& msg) const {
	if (!isDebugEnabled()) {
		return;
	}
	log(LoggerLevel::Debug, filterFile(file), line, msg);
}

void
Logger::debug(int line, const string& msg) const {
	if (!isDebugEnabled()) {
		return;
	}
	log(LoggerLevel::Debug, _file, line, msg);
}

void
Logger::debug(const string& msg) const {
	if (!isDebugEnabled()) {
		return;
	}
	log(LoggerLevel::Debug, msg);
}

bool
Logger::isInfoEnabled() const {
	return _level <= LoggerLevel::Info;
}

void
Logger::info(const char* file, int line, const string& msg) const {
	if (!isInfoEnabled()) {
		return;
	}
	log(LoggerLevel::Info, filterFile(file), line, msg);
}

void
Logger::info(int line, const string& msg) const {
	if (!isInfoEnabled()) {
		return;
	}
	log(LoggerLevel::Info, _file, line, msg);
}

void
Logger::info(const string& msg) const {
	if (!isInfoEnabled()) {
		return;
	}
	log(LoggerLevel::Info, msg);
}

void
Logger::warn(const char* file, int line, const string& msg) const {
	if (_level > LoggerLevel::Warn) {
		return;
	}
	log(LoggerLevel::Warn, filterFile(file), line, msg);
}

void
Logger::warn(int line, const string& msg) const {
	if (_level > LoggerLevel::Warn) {
		return;
	}
	log(LoggerLevel::Warn, _file, line, msg);
}

void
Logger::warn(const string& msg) const {
	if (_level > LoggerLevel::Warn) {
		return;
	}
	log(LoggerLevel::Warn, msg);
}

void
Logger::error(const char* file, int line, const string& msg) const {
	if (_level > LoggerLevel::Error) {
		return;
	}
	log(LoggerLevel::Error, filterFile(file), line, msg);
}

void
Logger::error(int line, const string& msg) const {
	if (_level > LoggerLevel::Error) {
		return;
	}
	log(LoggerLevel::Error, _file, line, msg);
}

void
Logger::error(const string& msg) const {
	if (_level > LoggerLevel::Error) {
		return;
	}
	log(LoggerLevel::Error, msg);
}

void
Logger::fatal(const char* file, int line, const string& msg) const {
	if (_level > LoggerLevel::Fatal) {
		return;
	}
	log(LoggerLevel::Fatal, filterFile(file), line, msg);
}

void
Logger::fatal(int line, const string& msg) const {
	if (_level > LoggerLevel::Fatal) {
		return;
	}
	log(LoggerLevel::Fatal, _file, line, msg);
}

void
Logger::fatal(const string& msg) const {
	if (_level > LoggerLevel::Fatal) {
		return;
	}
	log(LoggerLevel::Fatal, msg);
}

void
Logger::output(const char* file, int line, const string& msg) const {
	log(LoggerLevel::Output, filterFile(file), line, msg);
}

void
Logger::output(int line, const string& msg) const {
	log(LoggerLevel::Output, _file, line, msg);
}

void
Logger::output(const string& msg) const {
	log(LoggerLevel::Output, msg);
}

void
Logger::log(LoggerLevel level, const string& msg) const {
	//#ifndef NDEBUG
	//    if(level == LoggerLevel::Error || level == LoggerLevel::Fatal) {
	//        string stacks;
	//        getStackTrace(stacks, 2);
	//        ostringstream os;
	//        os << msg << "\nstacktrace\n" << stacks;
	//    }
	//#endif

	for (vector<Appender*>::iterator p = appenders.begin(); p != appenders.end(); ++p) {
		(*p)->output(level, _shortcat, _file, -1, msg);
	}
}

void
Logger::log(LoggerLevel level, const string& file, int line, const string& msg) const {
	//#ifndef NDEBUG
	//    if(level == LoggerLevel::Error || level == LoggerLevel::Fatal) {
	//        string stacks;
	//        getStackTrace(stacks, 2);
	//        ostringstream os;
	//        os << msg << "\nstacktrace\n" << stacks;
	//    }
	//#endif

	for (vector<Appender*>::iterator p = appenders.begin(); p != appenders.end(); ++p) {
		(*p)->output(level, _shortcat, file, line, msg);
	}
}

void
Logger::setLevel(LoggerLevel level) {
	_level = level;
	propagateLevelToChildren(this, _cat, level);
	_inherit = false;
}

void
Logger::clearLevel() {
	_inherit = true;
}

LoggerLevel
Logger::getLevel() const {
	return _level;
}

void
Logger::inheritLevel(LoggerLevel level) {
	if (_inherit) {
		_level = level;
		propagateLevelToChildren(this, _cat, level);
	}
}

const string&
Logger::getCategory() const {
	return _cat;
}

