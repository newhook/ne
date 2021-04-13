/**
 *
 * Copyright (c) 2010-2013 Rocket City Studios, Inc. All rights reserved.
 *
 */
#include <ge/Config.h>
#include <ge/File.h>

#include <cctype>

#ifdef NE_TARGET_WINDOWS
#   include <io.h>
#   include <direct.h>
#   include <sys/stat.h>
#elif !defined(BTS_TARGET_PS4)
#   include <unistd.h>
#   include <dirent.h>
#endif

using namespace std;
using namespace ge;

namespace {
string
simplify(const string& path) {
    string result = path;

    string::size_type pos;

#ifdef BTS_TARGET_WIN32
    pos = 0;
    if(startsWith(result, "//")) {
        result[0] = '\\';
        result[1] = '\\';
        pos = 2;
    } else if(startsWith(result, "\\\\")) {
        pos = 2;
    }

    for(; pos < result.size(); ++pos) {
        if(result[pos] == '\\') {
            result[pos] = '/';
        }
    }
#endif

    pos = 0;
    while((pos = result.find("//", pos)) != string::npos) {
        result.erase(pos, 1);
    }

    pos = 0;
    while((pos = result.find("/./", pos)) != string::npos) {
        result.erase(pos, 2);
    }

    while(result.substr(0, 4) == "/../") {
        result.erase(0, 3);
    }

    if(result.substr(0, 2) == "./") {
        result.erase(0, 2);
    }

    if(result == "/." ||
       (result.size() == 4 && isalpha(result[0]) && result[1] == ':' &&
        result[2] == '/' && result[3] == '.')) {
        return result.substr(0, result.size() - 1);
    }

    if(result.size() >= 2 && result.substr(result.size() - 2, 2) == "/.") {
        result.erase(result.size() - 2, 2);
    }

	if (result == "/" || (result.size() == 3 && isalpha(result[0]) && result[1] == ':' &&
                         result[2] == '/')) {
        return result;
    }

    if(result.size() >= 1 && result[result.size() - 1] == '/') {
        result.erase(result.size() - 1);
    }

    if(result == "/..") {
        result = "/";
    }

    return result;
}

bool isRoot(const std::string& path) {
#ifdef BTS_TARGET_WIN32
    return path == "/" || path.size() == 3 && IceUtilInternal::isAlpha(path[0]) && path[1] == ':' &&
           path[2] == '/';
#else
    return path == "/";
#endif
}
}

File::File() {
}

File::File(const File& directory, const string& filename) : path(directory.path + "/" + filename) {
}

File::File(const string& path) : path(simplify(path)) {
}

File::File(const File& f) : path(f.path) {
}

void
File::operator = (const File &f) {
    path = f.path;
}

void
File::operator = (const string &p) {
    path = simplify(p);
}

bool
File::exists() const {
#ifdef NE_TARGET_WINDOWS
	struct _stat st;
	if (_stat(path.c_str(), &st) != 0 && !((st.st_mode & S_IFMT) == S_IFREG)) {
		return false;
	}
	return true;
#else
	// XXX:
#endif
}

bool
File::isDirectory() const {
#ifdef NE_TARGET_WINDOWS
	struct _stat st;
	if (_stat(path.c_str(), &st) != 0 && !((st.st_mode & S_IFMT) == S_IFDIR)) {
		return false;
	}
	return true;
#else
	// XXX:
#endif
}

vector<File>
File::listFiles() const {
#ifdef NE_TARGET_WINDOWS

    //vector<File> result;
    //const wstring fs = IceUtil::stringToWstring(path + "/*");

    //struct _wfinddata_t data;
    //intptr_t h = _wfindfirst(fs.c_str(), &data);
    //if(h == -1) {
    //    string ex = "cannot read directory `" + path + "':\n" + IceUtilInternal::lastErrorToString();
    //    throw IOException(__FILE__, __LINE__, ex);
    //}

    //while(true) {
    //    string name = IceUtil::wstringToString(data.name);
    //    b3dassert(!name.empty());

    //    if(name != ".." && name != ".") {
    //        result.push_back(File(*this, name));
    //    }

    //    if(_wfindnext(h, &data) == -1) {
    //        if(errno == ENOENT) {
    //            break;
    //        }

    //        string ex = "cannot read directory `" + path + "':\n" + IceUtilInternal::lastErrorToString();
    //        _findclose(h);
    //        throw IOException(__FILE__, __LINE__, ex);
    //    }
    //}

    //_findclose(h);
    //return result;

#elif !defined(BTS_TARGET_PS4)

    struct dirent **namelist;
    int n = scandir(path.c_str(), &namelist, 0, alphasort);
    if(n < 0) {
        string ex = "cannot read directory `" + path + "':\n" + IceUtilInternal::lastErrorToString();
        throw IOException(__FILE__, __LINE__, ex);
    }

    vector<File> result;
    result.reserve(n - 2);

    for(int i = 0; i < n; ++i) {
        string name = namelist[i]->d_name;
        b3dassert(!name.empty());

        free(namelist[i]);

        if(name != ".." && name != ".") {
            result.push_back(File(*this, name));
        }
    }

    free(namelist);
    return result;

#else
    vector<File> result;
    return result;
#endif
	vector<File> result;
	return result;
}

void
File::rename(const File& f) const {
    //IceUtilInternal::rename(path, f.getPath());
}

void
File::deleteFile() const {
    //IceUtilInternal::unlink(path);
}

void
File::deleteDirectory() const {
    if(!isDirectory()) {
        return;
    }

    vector<File> files = listFiles();
    for(vector<File>::const_iterator p = files.begin(); p != files.end(); ++p) {
        if(p->isDirectory()) {
            p->deleteDirectory();
        } else {
            p->deleteFile();
        }
    }
    //IceUtilInternal::rmdir(path);
}

void
File::mkdir() const {
    /*if(IceUtilInternal::mkdir(path, 0777) != 0) {
        if(errno != EEXIST) {
            string ex = "cannot create directory `" + path + "':\n" + IceUtilInternal::lastErrorToString();
            throw IOException(__FILE__, __LINE__, ex);
        }
    }*/
}

string
File::getParent() const {
    string::size_type pos = path.rfind('/');
    if(pos == string::npos) {
        return string();
    } else {
        return path.substr(0, pos);
    }
}

string
File::getName() const {
    string::size_type pos = path.rfind('/');
    if(pos == string::npos) {
        return path;
    }else if(pos < path.size() - 1) {
        return path.substr(pos + 1);
    } else {
        return string();
    }
}

string
File::getExtension() const {
    string::size_type pos = path.rfind('.');
    if(pos == string::npos) {
        return string();
    }else if(pos < path.size() - 1) {
        return path.substr(pos + 1);
    } else {
        return string();
    }
}

File
File::getParentFile() const {
    return File(getParent());
}

void
File::mkdirs() const {
    if(isRoot(path)) {
        return;
    }

    string dir = getParent() + "/";
    if(!dir.empty()) {
        File f(dir);
        if(!f.isDirectory()) {
            f.mkdirs();
        }
    }

    if(!isDirectory()) {
        mkdir();
    }
}

std::ostream&
ge::operator << (std::ostream & os, const File &file) {
    os << file.getPath();
    return os;
}

