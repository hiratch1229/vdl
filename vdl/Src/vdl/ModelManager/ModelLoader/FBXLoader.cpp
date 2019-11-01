#include "FBXLoader.hpp"

#include <vdl/TextureManager/TextureLoader/TextureLoader.hpp>

#include <vdl/Math.hpp>

#include <vdl/Constants/Constants.hpp>
#include <vdl/Misc/Misc.hpp>

#include <filesystem>
#include <unordered_map>

namespace
{
  struct BoneInfluence
  {
    vdl::uint Index;
    float Weight;
    float SumWeight;
  };
  using BoneInfluencePerControlPoint = std::vector<BoneInfluence>;
  using BoneInfluencePerControlPoints = std::vector<BoneInfluencePerControlPoint>;

  inline vdl::float2 Cast(const fbxsdk::FbxVector2& _v)
  {
    return { static_cast<float>(_v[0]), static_cast<float>(_v[1]) };
  }

  inline vdl::float3 Cast(const fbxsdk::FbxVector4& _v)
  {
    return { static_cast<float>(_v[0]), static_cast<float>(_v[1]), static_cast<float>(_v[2]) };
  }

  inline vdl::Matrix Cast(const fbxsdk::FbxAMatrix& _m)
  {
    return { static_cast<float>(_m[0][0]), static_cast<float>(_m[0][1]), static_cast<float>(_m[0][2]), static_cast<float>(_m[0][3]),
    static_cast<float>(_m[1][0]), static_cast<float>(_m[1][1]), static_cast<float>(_m[1][2]), static_cast<float>(_m[1][3]),
    static_cast<float>(_m[2][0]), static_cast<float>(_m[2][1]), static_cast<float>(_m[2][2]), static_cast<float>(_m[2][3]),
    static_cast<float>(_m[3][0]), static_cast<float>(_m[3][1]), static_cast<float>(_m[3][2]), static_cast<float>(_m[3][3]) };
  }
}

bool FBXLoader::CheckSupportFormat(const std::string& _FileFormat)
{
  return _FileFormat == "fbx" || _FileFormat == "obj";
}

//--------------------------------------------------

FBXLoader::FBXLoader()
{
  //  マネージャーを作成
  pManager_ = fbxsdk::FbxManager::Create();

  //  入力を設定
  pManager_->SetIOSettings(fbxsdk::FbxIOSettings::Create(pManager_, IOSROOT));
}

FBXLoader::~FBXLoader()
{
  //  マネージャーの破棄
  pManager_->Destroy();
}

ModelData FBXLoader::Load(const char* _FilePath)const
{
  const std::string FileFormat = GetFileFormat(_FilePath);
  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "は対応していません。").c_str());

  ModelData ModelData;

  fbxsdk::FbxScene* pScene;
  {
    //  新規シーンを作成
    pScene = fbxsdk::FbxScene::Create(pManager_, "");

    //  インポーター作成&初期化
    fbxsdk::FbxImporter* pImporter = fbxsdk::FbxImporter::Create(pManager_, "");
    bool ImportStatus = pImporter->Initialize(_FilePath, -1, pManager_->GetIOSettings());
    _ASSERT_EXPR_A(ImportStatus, pImporter->GetStatus().GetErrorString());

    //  シーンをインポート
    ImportStatus = pImporter->Import(pScene);
    _ASSERT_EXPR_A(ImportStatus, pImporter->GetStatus().GetErrorString());

    //  ポリゴンの三角化
    fbxsdk::FbxGeometryConverter GeometryConverter(pManager_);
    GeometryConverter.Triangulate(pScene, true);

    //  インポーターの破棄
    pImporter->Destroy();
  }

  bool isIndexInverse = false;
  vdl::Matrix AxisSystemConversion = vdl::Matrix::Identity();
  vdl::Matrix SystemUnitConversion = vdl::Matrix::Identity();
  {
    fbxsdk::FbxGlobalSettings& GlobalSettings = pScene->GetGlobalSettings();

    //const fbxsdk::FbxAxisSystem AxisSystem = fbxsdk::FbxAxisSystem::DirectX;
    //if (AxisSystem != GlobalSettings.GetAxisSystem())
    //{
    //  AxisSystem.ConvertScene(pScene);
    //}
    //const fbxsdk::FbxSystemUnit SystemUnit = fbxsdk::FbxSystemUnit::m;
    //if (SystemUnit != GlobalSettings.GetSystemUnit())
    //{
    //  SystemUnit.ConvertScene(pScene);
    //}

    if (fbxsdk::FbxAxisSystem::MayaYUp == GlobalSettings.GetAxisSystem())
    {
      isIndexInverse = true;
      AxisSystemConversion._33 *= -1;
    }
    if (fbxsdk::FbxSystemUnit::cm == GlobalSettings.GetSystemUnit())
    {
      SystemUnitConversion._11 *= 0.01f;
      SystemUnitConversion._22 *= 0.01f;
      SystemUnitConversion._33 *= 0.01f;
    }
  }

  //  マテリアルのロード
  std::unordered_map<const char*, vdl::uint> MaterialMap;
  {
    const TextureLoader TextureLoader;
    const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();

    const vdl::uint MaterialNum = pScene->GetMaterialCount();
    ModelData.Materials.resize(MaterialNum + 1/*マテリアルを持ってないオブジェクト用*/);
    for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
    {
      const fbxsdk::FbxSurfaceMaterial* pSurfaceMaterial = pScene->GetMaterial(MaterialCount);
      MaterialMap.insert(std::make_pair(pSurfaceMaterial->GetName(), MaterialCount + 1));

      auto FetchMaterialProperty = [&](vdl::CompressionImage* _pCompressionImage, const char* _PropertyName, const char* _FactorName)->bool
      {
        const fbxsdk::FbxProperty Property = pSurfaceMaterial->FindProperty(_PropertyName);
        const fbxsdk::FbxProperty Factor = pSurfaceMaterial->FindProperty(_FactorName);

        if (!Property.IsValid())
        {
          return false;
        }

        const vdl::uint TextureNum = Property.GetSrcObjectCount<fbxsdk::FbxFileTexture>();

        if (TextureNum > 0)
        {
          if (fbxsdk::FbxFileTexture * pFileTexture = Property.GetSrcObject<fbxsdk::FbxFileTexture>())
          {
            const std::string FileName = (FileFormat == "fbx" ? Directory + pFileTexture->GetRelativeFileName() : pFileTexture->GetFileName());
            (*_pCompressionImage) = TextureLoader.LoadFromFile(FileName.c_str());
          }
        }

        return true;
      };

      Material& Material = ModelData.Materials[MaterialCount + 1];
      if (FetchMaterialProperty(&Material.Diffuse, fbxsdk::FbxSurfaceMaterial::sDiffuse, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor))
      {
        const fbxsdk::FbxDouble3 Color = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse).Get<fbxsdk::FbxDouble3>();
        const double f = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuseFactor).Get<fbxsdk::FbxDouble>();

        Material.MaterialColor.Red = static_cast<float>(Color[0] * f);
        Material.MaterialColor.Green = static_cast<float>(Color[1] * f);
        Material.MaterialColor.Blue = static_cast<float>(Color[2] * f);
        Material.MaterialColor.Alpha = 1.0f;
      }
      //FetchMaterialProperty(&Material.Ambient, fbxsdk::FbxSurfaceMaterial::sAmbient, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);
      FetchMaterialProperty(&Material.Specular, fbxsdk::FbxSurfaceMaterial::sSpecular, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
      FetchMaterialProperty(&Material.NormalMap, fbxsdk::FbxSurfaceMaterial::sNormalMap, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
      //FetchMaterialProperty(&Material.Bump, fbxsdk::FbxSurfaceMaterial::sBump, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
    }
  }

  TextureLoader TextureLoader;
  const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();

  const int MeshNum = pScene->GetSrcObjectCount<fbxsdk::FbxMesh>();
  for (int MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
  {
    const fbxsdk::FbxMesh* pMesh = pScene->GetSrcObject<fbxsdk::FbxMesh>(MeshCount);
    fbxsdk::FbxNode* pNode = pMesh->GetNode();

    BoneInfluencePerControlPoints BoneInfluencePerControlPoints;
    //  ボーン影響度のロード
    {
      const vdl::uint ControlPointNum = pMesh->GetControlPointsCount();
      BoneInfluencePerControlPoints.resize(ControlPointNum);

      const vdl::uint DeformerNum = pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
      for (vdl::uint DeformerCount = 0; DeformerCount < DeformerNum; ++DeformerCount)
      {
        fbxsdk::FbxSkin* pSkin = static_cast<fbxsdk::FbxSkin*>(pMesh->GetDeformer(DeformerCount, fbxsdk::FbxDeformer::eSkin));

        const vdl::uint ClusterNum = pSkin->GetClusterCount();
        for (vdl::uint ClusterCount = 0; ClusterCount < ClusterNum; ++ClusterCount)
        {
          fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(ClusterCount);
          const vdl::uint ControlPointIndicesNum = pCluster->GetControlPointIndicesCount();
          const int* ControlPointIndices = pCluster->GetControlPointIndices();
          const double* ControlPointWeights = pCluster->GetControlPointWeights();

          for (vdl::uint ControlPointIndicesCount = 0; ControlPointIndicesCount < ControlPointIndicesNum; ++ControlPointIndicesCount)
          {
            BoneInfluencePerControlPoint& InfluencePerControlPoint = BoneInfluencePerControlPoints[ControlPointIndices[ControlPointIndicesCount]];
            BoneInfluence Influence;
            Influence.Index = ClusterCount;
            Influence.Weight = static_cast<float>(ControlPointWeights[ControlPointIndicesCount]);
            InfluencePerControlPoint.emplace_back(std::move(Influence));
          }
        }
      }

      for (auto& BoneInfluencePerControlPoint : BoneInfluencePerControlPoints)
      {
        vdl::uint Size = static_cast<vdl::uint>(BoneInfluencePerControlPoint.size());

        if (Size > Constants::kMaxBoneInfluence)
        {
          //  影響度順にソート
          std::sort(BoneInfluencePerControlPoint.begin(), BoneInfluencePerControlPoint.end(), [](const BoneInfluence& _Influence1, const BoneInfluence& _Influence2)->bool {
            return _Influence1.Weight > _Influence2.Weight;
          });

          //  影響度が一番大きい値に加算
          {
            float SumWeight = 0.0f;
            for (vdl::uint i = Constants::kMaxBoneInfluence; i < Size; ++i)
            {
              SumWeight += BoneInfluencePerControlPoint[i].Weight;
            }
            BoneInfluencePerControlPoint[0].Weight += SumWeight;
          }

          Size = Constants::kMaxBoneInfluence;
        }
      }
    }

    const vdl::uint MaterialNum = pNode->GetMaterialCount();
    //  マテリアルのロード
    {
      Materials Materials;

      Materials.resize(MaterialNum > 0 ? MaterialNum : 1);

      for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
      {
        Material& Material = Materials[MaterialCount];

        const fbxsdk::FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(MaterialCount);
        auto FetchMaterialProperty = [&](vdl::CompressionImage* _pCompressionImage, const char* _PropertyName, const char* _FactorName)->bool
        {
          const fbxsdk::FbxProperty Property = pSurfaceMaterial->FindProperty(_PropertyName);
          const fbxsdk::FbxProperty Factor = pSurfaceMaterial->FindProperty(_FactorName);

          if (!Property.IsValid())
          {
            return false;
          }

          const vdl::uint TextureNum = Property.GetSrcObjectCount<fbxsdk::FbxFileTexture>();

          if (TextureNum > 0)
          {
            if (fbxsdk::FbxFileTexture * pFileTexture = Property.GetSrcObject<fbxsdk::FbxFileTexture>())
            {
              const std::string FileName = (FileFormat == "fbx" ? Directory + pFileTexture->GetRelativeFileName() : pFileTexture->GetFileName());
              (*_pCompressionImage) = TextureLoader.LoadFromFile(FileName.c_str());
            }
          }

          return true;
        };

        if (FetchMaterialProperty(&Material.Diffuse, fbxsdk::FbxSurfaceMaterial::sDiffuse, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor))
        {
          const fbxsdk::FbxDouble3 Color = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse).Get<fbxsdk::FbxDouble3>();
          const double f = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuseFactor).Get<fbxsdk::FbxDouble>();

          Material.MaterialColor.Red = static_cast<float>(Color[0] * f);
          Material.MaterialColor.Green = static_cast<float>(Color[1] * f);
          Material.MaterialColor.Blue = static_cast<float>(Color[2] * f);
          Material.MaterialColor.Alpha = 1.0f;
        }
        //FetchMaterialProperty(&Material.Ambient, fbxsdk::FbxSurfaceMaterial::sAmbient, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);
        FetchMaterialProperty(&Material.Specular, fbxsdk::FbxSurfaceMaterial::sSpecular, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
        FetchMaterialProperty(&Material.NormalMap, fbxsdk::FbxSurfaceMaterial::sNormalMap, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
        //FetchMaterialProperty(&Material.Bump, fbxsdk::FbxSurfaceMaterial::sBump, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
      }

      ModelData.Materials.insert(ModelData.Materials.cend(), Materials.begin(), Materials.end());
    }

    const size_t IndexOffset = ModelData.Vertices.size();
    //  頂点データのロード
    {
      vdl::Vertices Vertices;

      const vdl::uint PolygonNum = pMesh->GetPolygonCount();
      Vertices.resize(static_cast<size_t>(PolygonNum) * 3);

      const bool hasNormal = pMesh->GetElementNormalCount() > 0;
      const bool hasTangent = pMesh->GetElementTangentCount() > 0;
      const bool hasUV = pMesh->GetElementUVCount() > 0;

      const fbxsdk::FbxVector4* ControlPoints = pMesh->GetControlPoints();
      fbxsdk::FbxStringList UVNames;
      pMesh->GetUVSetNames(UVNames);

      for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
      {
        for (vdl::uint VertexCount = 0; VertexCount < 3; ++VertexCount)
        {
          const vdl::uint VertexIndex = VertexCount + PolygonCount * 3;

          vdl::Vertex3D& Vertex = Vertices[VertexIndex];

          const int ControlPointIndex = pMesh->GetPolygonVertex(PolygonCount, VertexCount);

          Vertex.Position = Cast(ControlPoints[ControlPointIndex]);
          if (hasNormal)
          {
            fbxsdk::FbxVector4 Normal;
            pMesh->GetPolygonVertexNormal(PolygonCount, VertexCount, Normal);
            Vertex.Normal = Cast(Normal);
          }
          else
          {
            Vertex.Normal = vdl::float3::Forward();
          }
          if (hasTangent)
          {
            const fbxsdk::FbxGeometryElementTangent* pGeometryTangent = pMesh->GetElementTangent(0);
            const fbxsdk::FbxGeometryElement::EMappingMode MappingMode = pGeometryTangent->GetMappingMode();
            const fbxsdk::FbxGeometryElement::EReferenceMode ReferenceMode = pGeometryTangent->GetReferenceMode();

            int TangentIndex = -1;
            if (MappingMode == fbxsdk::FbxGeometryElement::EMappingMode::eByControlPoint)
            {
              switch (ReferenceMode)
              {
              case fbxsdk::FbxGeometryElement::EReferenceMode::eDirect:
                TangentIndex = ControlPointIndex;
                break;
              case fbxsdk::FbxGeometryElement::EReferenceMode::eIndexToDirect:
                TangentIndex = pGeometryTangent->GetIndexArray()[ControlPointIndex];
                break;
              }
            }
            else if (MappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
            {
              switch (ReferenceMode)
              {
              case fbxsdk::FbxGeometryElement::EReferenceMode::eDirect:
                TangentIndex = VertexIndex;
                break;
              case fbxsdk::FbxGeometryElement::EReferenceMode::eIndexToDirect:
                TangentIndex = pGeometryTangent->GetIndexArray()[ControlPointIndex];
                break;
              }
            }
            _ASSERT_EXPR_A(TangentIndex != -1, "Invalid Mode");

            Vertex.Tangent = Cast(pGeometryTangent->GetDirectArray()[TangentIndex]);
          }
          else
          {
            Vertex.Tangent = Vertex.Normal.Cross(vdl::float3::Up());
          }
          if (hasUV)
          {
            fbxsdk::FbxVector2 UV;
            bool UnMappedUV = false;
            pMesh->GetPolygonVertexUV(PolygonCount, VertexCount, UVNames[0], UV, UnMappedUV);

            Vertex.Texcoord = Cast(UV);
            Vertex.Texcoord.y = 1.0f - Vertex.Texcoord.y;
          }
          else
          {
            Vertex.Texcoord = vdl::float2(0.0f, 0.0f);
          }

          const BoneInfluencePerControlPoint& BoneInfluencePerControlPoint = BoneInfluencePerControlPoints[ControlPointIndex];

          const vdl::uint InfluenceNum = vdl::Math::Min(Constants::kMaxBoneInfluence, static_cast<vdl::uint>(BoneInfluencePerControlPoint.size()));
          for (vdl::uint InfluenceIndex = 0; InfluenceIndex < InfluenceNum; ++InfluenceIndex)
          {
            Vertex.BoneWeights[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Weight;
            Vertex.BoneIndices[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Index;
          }
        }
      }

      ModelData.Vertices.insert(ModelData.Vertices.cend(), Vertices.begin(), Vertices.end());
    }

    //  インデックスデータのロード
    {
      vdl::Indices Indices;

      const vdl::uint PolygonNum = pMesh->GetPolygonCount();
      Indices.resize(static_cast<size_t>(PolygonNum) * 3);

      MeshDatas MeshDatas;
      MeshDatas.resize(MaterialNum);

      const fbxsdk::FbxLayerElementArrayTemplate<int>& MaterialIndices = pMesh->GetElementMaterial()->GetIndexArray();
      if (MaterialNum)
      {
        for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
        {
          MeshDatas[MaterialIndices[PolygonCount]].IndexCount += 3;
        }
        const size_t MaterialOffset = ModelData.Materials.size();

        vdl::uint Offset = 0;
        for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
        {
          MeshData& MeshData = MeshDatas[MaterialCount];

          MeshData.IndexStart = Offset;
          Offset += MeshData.IndexCount;
          MeshData.IndexCount = 0;
          MeshData.MaterialIndex = MaterialMap[pNode->GetMaterial(MaterialCount)->GetName()];
          MeshData.GlobalTransform = Cast(pNode->EvaluateGlobalTransform()) * SystemUnitConversion * AxisSystemConversion;
        }
      }

      vdl::uint VertexNum = 0;
      for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
      {
        MeshData& MeshData = MeshDatas[(MaterialNum ? MaterialIndices[PolygonCount] : 0)];
        const vdl::uint OffsetIndex = MeshData.IndexStart + MeshData.IndexCount;

        vdl::IndexType* BaseIndices = &Indices[MeshData.IndexStart + MeshData.IndexCount];

        BaseIndices[(isIndexInverse ? 2 : 0)] = static_cast<vdl::uint>(IndexOffset) + VertexNum++;
        BaseIndices[1] = IndexOffset + VertexNum++;
        BaseIndices[(isIndexInverse ? 0 : 2)] = static_cast<vdl::uint>(IndexOffset) + VertexNum++;

        MeshData.IndexCount += 3;
      }

      ModelData.MeshDatas.insert(ModelData.MeshDatas.cend(), MeshDatas.begin(), MeshDatas.end());

      ModelData.Indices.insert(ModelData.Indices.cend(), Indices.begin(), Indices.end());
    }

    ////  アニメーションのロード
    //{
    //  //fbxsdk::FbxScene* pScene = _pMesh->GetScene();
    //
    //  Animations Animations;
    //
    //  fbxsdk::FbxArray<fbxsdk::FbxString*> AnimationStackNames;
    //  pScene->FillAnimStackNameArray(AnimationStackNames);
    //
    //  const vdl::uint AnimationNum = AnimationStackNames.Size();
    //  Animations.resize(AnimationNum);
    //
    //  const fbxsdk::FbxTime::EMode TimeMode = pScene->GetGlobalSettings().GetTimeMode();
    //  for (vdl::uint AnimationCount = 0; AnimationCount < AnimationNum; ++AnimationCount)
    //  {
    //    fbxsdk::FbxString* AnimationStackName = AnimationStackNames[AnimationCount];
    //    fbxsdk::FbxAnimStack* CurrentAnimationStack = pScene->FindMember<fbxsdk::FbxAnimStack>(AnimationStackName->Buffer());
    //    pScene->SetCurrentAnimationStack(CurrentAnimationStack);
    //
    //    const fbxsdk::FbxTakeInfo* TakeInfo = pScene->GetTakeInfo(AnimationStackName->Buffer());
    //    const fbxsdk::FbxTime StartTime = TakeInfo->mLocalTimeSpan.GetStart();
    //    const fbxsdk::FbxTime EndTime = TakeInfo->mLocalTimeSpan.GetStop();
    //
    //    fbxsdk::FbxTime SamplingStep;
    //    SamplingStep.SetTime(0, 0, 1, 0, 0, TimeMode);
    //    SamplingStep = static_cast<fbxsdk::FbxLongLong>(SamplingStep.Get() * static_cast<double>(Constants::kAnimationSamplingTime));
    //
    //    for (fbxsdk::FbxTime CurrentTime = StartTime; CurrentTime < EndTime; CurrentTime += SamplingStep)
    //    {
    //      Skeletal Skeletal;
    //      FetchBoneMatrices(_pMesh, &Skeletal, CurrentTime);
    //
    //      Animations[AnimationCount].Skeletals.emplace_back(std::move(Skeletal));
    //    }
    //
    //    delete AnimationStackNames[AnimationCount];
    //  }
    //
    //  ModelData.Animations.insert(ModelData.Animations.cend(), Animations.begin(), Animations.end());
    //}
  }

  return ModelData;
}

//ModelData FBXLoader::Load(const char* _FilePath)const
//{
//  const std::string FileFormat = GetFileFormat(_FilePath);
//  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "は対応していません。").c_str());
//
//  ModelData ModelData;
//
//  fbxsdk::FbxScene* pScene;
//  {
//    //  新規シーンを作成
//    pScene = fbxsdk::FbxScene::Create(pManager_, "");
//
//    //  インポーター作成&初期化
//    fbxsdk::FbxImporter* pImporter = fbxsdk::FbxImporter::Create(pManager_, "");
//    bool ImportStatus = pImporter->Initialize(_FilePath, -1, pManager_->GetIOSettings());
//    _ASSERT_EXPR_A(ImportStatus, pImporter->GetStatus().GetErrorString());
//
//    //  シーンをインポート
//    ImportStatus = pImporter->Import(pScene);
//    _ASSERT_EXPR_A(ImportStatus, pImporter->GetStatus().GetErrorString());
//
//    //  ポリゴンの三角化
//    fbxsdk::FbxGeometryConverter GeometryConverter(pManager_);
//    GeometryConverter.Triangulate(pScene, true);
//
//    //  インポーターの破棄
//    pImporter->Destroy();
//  }
//
//  bool isIndexInverse = false;
//  vdl::Matrix AxisSystemConversion = vdl::Matrix::Identity();
//  vdl::Matrix SystemUnitConversion = vdl::Matrix::Identity();
//  {
//    fbxsdk::FbxGlobalSettings& GlobalSettings = pScene->GetGlobalSettings();
//
//    //const fbxsdk::FbxAxisSystem AxisSystem = fbxsdk::FbxAxisSystem::DirectX;
//    //if (AxisSystem != GlobalSettings.GetAxisSystem())
//    //{
//    //  AxisSystem.ConvertScene(pScene);
//    //}
//    //const fbxsdk::FbxSystemUnit SystemUnit = fbxsdk::FbxSystemUnit::m;
//    //if (SystemUnit != GlobalSettings.GetSystemUnit())
//    //{
//    //  SystemUnit.ConvertScene(pScene);
//    //}
//
//    if (fbxsdk::FbxAxisSystem::MayaYUp == GlobalSettings.GetAxisSystem())
//    {
//      isIndexInverse = true;
//      AxisSystemConversion._33 *= -1;
//    }
//    if (fbxsdk::FbxSystemUnit::cm == GlobalSettings.GetSystemUnit())
//    {
//      SystemUnitConversion._11 *= 0.01f;
//      SystemUnitConversion._22 *= 0.01f;
//      SystemUnitConversion._33 *= 0.01f;
//    }
//  }
//
//  const TextureLoader TextureLoader;
//  const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();
//
//  //  マテリアルを持ってないオブジェクト用
//  ModelData.Materials.resize(1);
//
//  const int MeshNum = pScene->GetSrcObjectCount<fbxsdk::FbxMesh>();
//  for (int MeshCount = 0; MeshCount < MeshNum; ++MeshCount)
//  {
//    const fbxsdk::FbxMesh* pMesh = pScene->GetSrcObject<fbxsdk::FbxMesh>(MeshCount);
//    fbxsdk::FbxNode* pNode = pMesh->GetNode();
//
//    BoneInfluencePerControlPoints BoneInfluencePerControlPoints;
//    //  ボーン影響度のロード
//    {
//      const vdl::uint ControlPointNum = pMesh->GetControlPointsCount();
//      BoneInfluencePerControlPoints.resize(ControlPointNum);
//
//      const vdl::uint DeformerNum = pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
//      for (vdl::uint DeformerCount = 0; DeformerCount < DeformerNum; ++DeformerCount)
//      {
//        fbxsdk::FbxSkin* pSkin = static_cast<fbxsdk::FbxSkin*>(pMesh->GetDeformer(DeformerCount, fbxsdk::FbxDeformer::eSkin));
//
//        const vdl::uint ClusterNum = pSkin->GetClusterCount();
//        for (vdl::uint ClusterCount = 0; ClusterCount < ClusterNum; ++ClusterCount)
//        {
//          fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(ClusterCount);
//          const vdl::uint ControlPointIndicesNum = pCluster->GetControlPointIndicesCount();
//          const int* ControlPointIndices = pCluster->GetControlPointIndices();
//          const double* ControlPointWeights = pCluster->GetControlPointWeights();
//
//          for (vdl::uint ControlPointIndicesCount = 0; ControlPointIndicesCount < ControlPointIndicesNum; ++ControlPointIndicesCount)
//          {
//            BoneInfluencePerControlPoint& InfluencePerControlPoint = BoneInfluencePerControlPoints[ControlPointIndices[ControlPointIndicesCount]];
//            BoneInfluence Influence;
//            Influence.Index = ClusterCount;
//            Influence.Weight = static_cast<float>(ControlPointWeights[ControlPointIndicesCount]);
//            InfluencePerControlPoint.emplace_back(std::move(Influence));
//          }
//        }
//      }
//
//      for (auto& BoneInfluencePerControlPoint : BoneInfluencePerControlPoints)
//      {
//        vdl::uint Size = static_cast<vdl::uint>(BoneInfluencePerControlPoint.size());
//
//        if (Size > Constants::kMaxBoneInfluence)
//        {
//          //  影響度順にソート
//          std::sort(BoneInfluencePerControlPoint.begin(), BoneInfluencePerControlPoint.end(), [](const BoneInfluence& _Influence1, const BoneInfluence& _Influence2)->bool {
//            return _Influence1.Weight > _Influence2.Weight;
//          });
//
//          //  影響度が一番大きい値に加算
//          {
//            float SumWeight = 0.0f;
//            for (vdl::uint i = Constants::kMaxBoneInfluence; i < Size; ++i)
//            {
//              SumWeight += BoneInfluencePerControlPoint[i].Weight;
//            }
//            BoneInfluencePerControlPoint[0].Weight += SumWeight;
//          }
//
//          Size = Constants::kMaxBoneInfluence;
//        }
//      }
//    }
//
//    const vdl::uint MaterialNum = pNode->GetMaterialCount();
//    //  マテリアルのロード
//    {
//      Materials Materials;
//
//      Materials.resize(MaterialNum > 0 ? MaterialNum : 1);
//
//      for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
//      {
//        Material& Material = Materials[MaterialCount];
//
//        const fbxsdk::FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(MaterialCount);
//        auto FetchMaterialProperty = [&](vdl::CompressionImage* _pCompressionImage, const char* _PropertyName, const char* _FactorName)->bool
//        {
//          const fbxsdk::FbxProperty Property = pSurfaceMaterial->FindProperty(_PropertyName);
//          const fbxsdk::FbxProperty Factor = pSurfaceMaterial->FindProperty(_FactorName);
//
//          if (!Property.IsValid())
//          {
//            return false;
//          }
//
//          const vdl::uint TextureNum = Property.GetSrcObjectCount<fbxsdk::FbxFileTexture>();
//
//          if (TextureNum > 0)
//          {
//            if (fbxsdk::FbxFileTexture * pFileTexture = Property.GetSrcObject<fbxsdk::FbxFileTexture>())
//            {
//              const std::string FileName = (FileFormat == "fbx" ? Directory + pFileTexture->GetRelativeFileName() : pFileTexture->GetFileName());
//              (*_pCompressionImage) = TextureLoader.LoadFromFile(FileName.c_str());
//            }
//          }
//
//          return true;
//        };
//
//        if (FetchMaterialProperty(&Material.Diffuse, fbxsdk::FbxSurfaceMaterial::sDiffuse, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor))
//        {
//          const fbxsdk::FbxDouble3 Color = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse).Get<fbxsdk::FbxDouble3>();
//          const double f = pSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuseFactor).Get<fbxsdk::FbxDouble>();
//
//          Material.MaterialColor.Red = static_cast<float>(Color[0] * f);
//          Material.MaterialColor.Green = static_cast<float>(Color[1] * f);
//          Material.MaterialColor.Blue = static_cast<float>(Color[2] * f);
//          Material.MaterialColor.Alpha = 1.0f;
//        }
//        //FetchMaterialProperty(&Material.Ambient, fbxsdk::FbxSurfaceMaterial::sAmbient, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);
//        FetchMaterialProperty(&Material.Specular, fbxsdk::FbxSurfaceMaterial::sSpecular, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
//        FetchMaterialProperty(&Material.NormalMap, fbxsdk::FbxSurfaceMaterial::sNormalMap, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
//        //FetchMaterialProperty(&Material.Bump, fbxsdk::FbxSurfaceMaterial::sBump, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
//      }
//
//      ModelData.Materials.insert(ModelData.Materials.cend(), Materials.begin(), Materials.end());
//    }
//
//    const size_t IndexOffset = ModelData.Vertices.size();
//    //  頂点データのロード
//    {
//      Vertices Vertices;
//
//      const vdl::uint PolygonNum = pMesh->GetPolygonCount();
//      Vertices.resize(static_cast<size_t>(PolygonNum) * 3);
//
//      const bool hasNormal = pMesh->GetElementNormalCount() > 0;
//      const bool hasTangent = pMesh->GetElementTangentCount() > 0;
//      const bool hasUV = pMesh->GetElementUVCount() > 0;
//
//      const fbxsdk::FbxVector4* ControlPoints = pMesh->GetControlPoints();
//      fbxsdk::FbxStringList UVNames;
//      pMesh->GetUVSetNames(UVNames);
//
//      for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
//      {
//        for (vdl::uint VertexCount = 0; VertexCount < 3; ++VertexCount)
//        {
//          const vdl::uint VertexIndex = VertexCount + PolygonCount * 3;
//
//          vdl::Vertex3D& Vertex = Vertices[VertexIndex];
//
//          const int ControlPointIndex = pMesh->GetPolygonVertex(PolygonCount, VertexCount);
//
//          Vertex.Position = Cast(ControlPoints[ControlPointIndex]);
//          if (hasNormal)
//          {
//            fbxsdk::FbxVector4 Normal;
//            pMesh->GetPolygonVertexNormal(PolygonCount, VertexCount, Normal);
//            Vertex.Normal = Cast(Normal);
//          }
//          else
//          {
//            Vertex.Normal = vdl::float3::Forward();
//          }
//          if (hasTangent)
//          {
//            const fbxsdk::FbxGeometryElementTangent* pGeometryTangent = pMesh->GetElementTangent(0);
//            const fbxsdk::FbxGeometryElement::EMappingMode MappingMode = pGeometryTangent->GetMappingMode();
//            const fbxsdk::FbxGeometryElement::EReferenceMode ReferenceMode = pGeometryTangent->GetReferenceMode();
//
//            int TangentIndex = -1;
//            if (MappingMode == fbxsdk::FbxGeometryElement::EMappingMode::eByControlPoint)
//            {
//              switch (ReferenceMode)
//              {
//              case fbxsdk::FbxGeometryElement::EReferenceMode::eDirect:
//                TangentIndex = ControlPointIndex;
//                break;
//              case fbxsdk::FbxGeometryElement::EReferenceMode::eIndexToDirect:
//                TangentIndex = pGeometryTangent->GetIndexArray()[ControlPointIndex];
//                break;
//              }
//            }
//            else if (MappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
//            {
//              switch (ReferenceMode)
//              {
//              case fbxsdk::FbxGeometryElement::EReferenceMode::eDirect:
//                TangentIndex = VertexIndex;
//                break;
//              case fbxsdk::FbxGeometryElement::EReferenceMode::eIndexToDirect:
//                TangentIndex = pGeometryTangent->GetIndexArray()[ControlPointIndex];
//                break;
//              }
//            }
//            _ASSERT_EXPR_A(TangentIndex != -1, "Invalid Mode");
//
//            Vertex.Tangent = Cast(pGeometryTangent->GetDirectArray()[TangentIndex]);
//          }
//          else
//          {
//            Vertex.Tangent = Vertex.Normal.Cross(vdl::float3::Up());
//          }
//          if (hasUV)
//          {
//            fbxsdk::FbxVector2 UV;
//            bool UnMappedUV = false;
//            pMesh->GetPolygonVertexUV(PolygonCount, VertexCount, UVNames[0], UV, UnMappedUV);
//
//            Vertex.Texcoord = Cast(UV);
//            Vertex.Texcoord.y = 1.0f - Vertex.Texcoord.y;
//          }
//          else
//          {
//            Vertex.Texcoord = vdl::float2(0.0f, 0.0f);
//          }
//
//          const BoneInfluencePerControlPoint& BoneInfluencePerControlPoint = BoneInfluencePerControlPoints[ControlPointIndex];
//
//          const vdl::uint InfluenceNum = vdl::Math::Min(Constants::kMaxBoneInfluence, static_cast<vdl::uint>(BoneInfluencePerControlPoint.size()));
//          for (vdl::uint InfluenceIndex = 0; InfluenceIndex < InfluenceNum; ++InfluenceIndex)
//          {
//            Vertex.BoneWeights[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Weight;
//            Vertex.BoneIndices[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Index;
//          }
//        }
//      }
//
//      ModelData.Vertices.insert(ModelData.Vertices.cend(), Vertices.begin(), Vertices.end());
//    }
//
//    //  インデックスデータのロード
//    {
//      Indices Indices;
//
//      const vdl::uint PolygonNum = pMesh->GetPolygonCount();
//      Indices.resize(static_cast<size_t>(PolygonNum) * 3);
//
//      MeshDatas MeshDatas;
//      MeshDatas.resize(MaterialNum);
//
//      const fbxsdk::FbxLayerElementArrayTemplate<int>& MaterialIndices = pMesh->GetElementMaterial()->GetIndexArray();
//      if (MaterialNum)
//      {
//        for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
//        {
//          MeshDatas[MaterialIndices[PolygonCount]].IndexCount += 3;
//        }
//        const size_t MaterialOffset = ModelData.Materials.size();
//
//        vdl::uint Offset = 0;
//        for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
//        {
//          MeshData& MeshData = MeshDatas[MaterialCount];
//
//          MeshData.IndexStart = Offset;
//          Offset += MeshData.IndexCount;
//          MeshData.IndexCount = 0;
//          MeshData.MaterialIndex = MaterialOffset + MaterialCount;
//          MeshData.GlobalTransform = Cast(pNode->EvaluateGlobalTransform()) * SystemUnitConversion * AxisSystemConversion;
//        }
//      }
//
//      vdl::uint VertexNum = 0;
//      for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
//      {
//        MeshData& MeshData = MeshDatas[(MaterialNum ? MaterialIndices[PolygonCount] : 0)];
//        const vdl::uint OffsetIndex = MeshData.IndexStart + MeshData.IndexCount;
//
//        vdl::IndexType* BaseIndices = &Indices[MeshData.IndexStart + MeshData.IndexCount];
//
//        BaseIndices[(isIndexInverse ? 2 : 0)] = IndexOffset + VertexNum++;
//        BaseIndices[1] = IndexOffset + VertexNum++;
//        BaseIndices[(isIndexInverse ? 0 : 2)] = IndexOffset + VertexNum++;
//
//        MeshData.IndexCount += 3;
//      }
//
//      ModelData.MeshDatas.insert(ModelData.MeshDatas.cend(), MeshDatas.begin(), MeshDatas.end());
//
//      ModelData.Indices.insert(ModelData.Indices.cend(), Indices.begin(), Indices.end());
//    }
//
//    //  アニメーションのロード
//    {
//      //fbxsdk::FbxScene* pScene = _pMesh->GetScene();
//
//      Animations Animations;
//
//      fbxsdk::FbxArray<fbxsdk::FbxString*> AnimationStackNames;
//      pScene->FillAnimStackNameArray(AnimationStackNames);
//
//      const vdl::uint AnimationNum = AnimationStackNames.Size();
//      Animations.resize(AnimationNum);
//
//      const fbxsdk::FbxTime::EMode TimeMode = pScene->GetGlobalSettings().GetTimeMode();
//      for (vdl::uint AnimationCount = 0; AnimationCount < AnimationNum; ++AnimationCount)
//      {
//        fbxsdk::FbxString* AnimationStackName = AnimationStackNames[AnimationCount];
//        fbxsdk::FbxAnimStack* CurrentAnimationStack = pScene->FindMember<fbxsdk::FbxAnimStack>(AnimationStackName->Buffer());
//        pScene->SetCurrentAnimationStack(CurrentAnimationStack);
//
//        const fbxsdk::FbxTakeInfo* TakeInfo = pScene->GetTakeInfo(AnimationStackName->Buffer());
//        const fbxsdk::FbxTime StartTime = TakeInfo->mLocalTimeSpan.GetStart();
//        const fbxsdk::FbxTime EndTime = TakeInfo->mLocalTimeSpan.GetStop();
//
//        fbxsdk::FbxTime SamplingStep;
//        SamplingStep.SetTime(0, 0, 1, 0, 0, TimeMode);
//        SamplingStep = static_cast<fbxsdk::FbxLongLong>(SamplingStep.Get() * static_cast<double>(Constants::kAnimationSamplingTime));
//
//        for (fbxsdk::FbxTime CurrentTime = StartTime; CurrentTime < EndTime; CurrentTime += SamplingStep)
//        {
//          Skeletal Skeletal;
//          FetchBoneMatrices(_pMesh, &Skeletal, CurrentTime);
//
//          Animations[AnimationCount].Skeletals.emplace_back(std::move(Skeletal));
//        }
//
//        delete AnimationStackNames[AnimationCount];
//      }
//
//      ModelData.Animations.insert(ModelData.Animations.cend(), Animations.begin(), Animations.end());
//    }
//  }
//
//  return ModelData;
//}
