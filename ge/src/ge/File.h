/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#pragma once

#include <ge/Config.h>
#include <vector>
#include <string>

namespace ge {
class File {
public:

    File();
    File(const File &directory, const std::string & filename);
    File(const std::string & path);
    File(const File &);

    void operator = (const std::string &);
    void operator = (const File &);

    bool operator == (const File &f) const { return path == f.path; }
    bool operator != (const File &f) const { return path != f.path; }
    bool operator < (const File &f) const { return path < f.path; }
    bool operator > (const File &f) const { return path > f.path; }
    bool operator <= (const File &f) const { return path <= f.path; }
    bool operator >= (const File &f) const { return path >= f.path; }

    operator std::string () const { return path; }

    const std::string& getPath() const { return path; }
    std::string getParent() const;
    std::string getName() const;
    std::string getExtension() const;

    File getParentFile() const;

    std::vector<File> listFiles() const;

    void rename(const File& f) const;
    void deleteFile() const;
    void deleteDirectory() const;

    bool exists() const;
    bool isDirectory() const;

    void mkdir() const;
    void mkdirs() const;

private:

    std::string path;
};

std::ostream& operator << (std::ostream &, const File &);
}