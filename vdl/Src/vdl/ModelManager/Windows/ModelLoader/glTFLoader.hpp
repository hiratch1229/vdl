//#pragma once
//#include "MeshData.hpp"
//
//#undef max
//#undef min
//
//#include <glTFSDK/include/GLTFSDK/GLTF.h>
//#include <glTFSDK/include/GLTFSDK/GLBResourceReader.h>
//#include <glTFSDK/include/GLTFSDK/Deserialize.h>
//
//class TextureLoader;
//
//class glTFLoader
//{
//public:
//  static bool CheckSupportFormat(const std::string& _FileFormat);
//private:
//  Microsoft::glTF::Document LoadDocument(const char* _FilePath, const std::string& _FileFormat, std::unique_ptr<Microsoft::glTF::GLTFResourceReader>* _pResourceReader)const;
//  void FetchMaterial(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh, const std::string& _Directory,
//    Materials* _pMaterials, const TextureLoader& _TextureLoader)const;
//  void FetchVerticesAndIndices(const std::unique_ptr<Microsoft::glTF::GLTFResourceReader>& _pResourceReader, const Microsoft::glTF::Document& _Document, const Microsoft::glTF::Mesh& _Mesh,
//    Vertices* _pVertices, Indices* _pIndices, Materials* _pMaterials)const;
//public:
//  glTFLoader() = default;
//
//  ~glTFLoader() = default;
//
//  MeshDatas Load(const char* _FilePath)const;
//};

#pragma once
#include "MeshData.hpp"

//#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define STB_IMAGE_IMPLEMENTATION
//#define STBI_MSC_SECURE_CRT
#define TINYGLTF_USE_CPP14
#include <tiny_gltf/tiny_gltf.h>

class glTFLoader
{
public:
  static bool CheckSupportFormat(const std::string& _FileFormat);
public:
  void LoadMaterials(const tinygltf::Model& _Model, Materials* _pMaterials)const;
  void LoadMeshs(const tinygltf::Model& _Model, MeshDatas* _pMeshDatas, const Materials& _Materials)const;
public:
  glTFLoader() = default;

  ~glTFLoader() = default;

  MeshDatas Load(const char* _FilePath)const;
};
