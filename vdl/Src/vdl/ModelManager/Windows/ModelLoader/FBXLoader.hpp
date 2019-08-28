#pragma once
#include "MeshData.hpp"

#pragma warning(disable:26451)
#pragma warning(disable:26495)
#include <ThirdParty/FBXSDK/include/fbxsdk.h>
#pragma warning(default:26495)
#pragma warning(default:26451)

class TextureLoader;

class FBXLoader
{
public:
  static bool CheckSupportFormat(const std::string& _FileFormat);
private:
  struct BoneInfluence
  {
    vdl::uint Index;
    float Weight;
    float SumWeight;
  };
  using BoneInfluencePerControlPoint = std::vector<BoneInfluence>;
  using BoneInfluencePerControlPoints = std::vector<BoneInfluencePerControlPoint>;
private:
  static constexpr vdl::uint kAnimationSamplingRate = 24;
  static constexpr float kAnimationSamplingTime = 1.0f / kAnimationSamplingRate;
private:
  fbxsdk::FbxManager* pManager_;
private:
  fbxsdk::FbxScene* ImportScene(const char* _FilePath)const;
  std::vector<fbxsdk::FbxMesh*> GetFbxMeshes(fbxsdk::FbxScene* _pScene)const;
  void FetchMaterials(fbxsdk::FbxMesh* _pMesh, Materials* _pMaterials, const std::string& _FileDirectory, const std::string& _FileFormat, const TextureLoader& _TextureLoader)const;
  void FetchInfluences(fbxsdk::FbxMesh* _pMesh, BoneInfluencePerControlPoints* _pInfluences)const;
  void FetchVertices(fbxsdk::FbxMesh* _pMesh, Vertices* _pVertices, const BoneInfluencePerControlPoints& _BoneInfluencePerControlPoints)const;
  void FetchIndices(fbxsdk::FbxMesh* _pMesh, Indices* _pIndices, Materials* _pMaterials)const;
  void FetchAnimations(fbxsdk::FbxMesh* _pMesh, Animations* _pAnimations)const;
  void FetchBoneMatrices(fbxsdk::FbxMesh* _pMesh, Skeletal* _pSkeletal, fbxsdk::FbxTime _Time)const;
public:
  FBXLoader();

  ~FBXLoader();

  MeshDatas Load(const char* _FilePath)const;
};
