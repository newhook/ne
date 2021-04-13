/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

//
// Adopted from tiny_obj_loader.
// Original copyright notice.
//
// Copyright 2012-2013, Syoyo Fujita.
//
// Licensed under 2-clause BSD liecense.
//
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ge {
class Mesh;

struct ObjMaterial {
    std::string name;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 transmittance;
	glm::vec3 emission;
    float shininess;
    float ior;                // index of refraction
    float dissolve;           // 1 == opaque; 0 == fully transparent
    // illumination model (see http://www.fileformat.info/format/material/)
    int illum;

    std::string ambient_texname;
    std::string diffuse_texname;
    std::string specular_texname;
    std::string normal_texname;
    std::map<std::string, std::string> unknown_parameter;
};

struct ObjShape {
    std::string  name;
    ObjMaterial   material;
    std::shared_ptr<ge::Mesh>       mesh;
};

class MaterialReader {
public:
    MaterialReader(){}
    virtual ~MaterialReader(){}

    virtual std::string operator() (
        const std::string& matId,
        std::map<std::string, ObjMaterial>& matMap) = 0;
};

class MaterialFileReader:
  public MaterialReader
{
    public:
        MaterialFileReader(const std::string& mtl_basepath): m_mtlBasePath(mtl_basepath) {}
        virtual ~MaterialFileReader() {}
        virtual std::string operator() (
          const std::string& matId,
          std::map<std::string, ObjMaterial>& matMap);

    private:
        std::string m_mtlBasePath;
};

/// Loads .obj from a file.
/// 'shapes' will be filled with parsed shape data
/// The function returns error string.
/// Returns empty string when loading .obj success.
/// 'mtl_basepath' is optional, and used for base path for .mtl file.
std::string LoadObj(
    std::vector<ObjShape>& shapes,   // [output]
    const char* filename,
    const char* mtl_basepath = NULL);

/// Loads object from a std::istream, uses GetMtlIStreamFn to retrieve
/// std::istream for materials.
/// Returns empty string when loading .obj success.
std::string LoadObj(
    std::vector<ObjShape>& shapes,   // [output]
    std::istream& inStream,
    MaterialReader& readMatFn);

/// Loads materials into std::map
/// Returns an empty string if successful
std::string LoadMtl (
  std::map<std::string, ObjMaterial>& material_map,
  std::istream& inStream);
}
