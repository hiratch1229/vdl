#include "FBXLoader.hpp"

#include <vdl/Constants/Constants.hpp>
#include <vdl/TextureManager/Windows/TextureLoader/TextureLoader.hpp>
#include <vdl/Misc/Windows/Misc.hpp>

#include <filesystem>
#include <functional>

namespace
{
  inline vdl::Matrix Cast(const fbxsdk::FbxAMatrix& _M)
  {
    return { static_cast<float>(_M[0][0]), static_cast<float>(_M[0][1]), static_cast<float>(_M[0][2]), static_cast<float>(_M[0][3]),
    static_cast<float>(_M[1][0]), static_cast<float>(_M[1][1]), static_cast<float>(_M[1][2]), static_cast<float>(_M[1][3]),
    static_cast<float>(_M[2][0]), static_cast<float>(_M[2][1]), static_cast<float>(_M[2][2]), static_cast<float>(_M[2][3]),
    static_cast<float>(_M[3][0]), static_cast<float>(_M[3][1]), static_cast<float>(_M[3][2]), static_cast<float>(_M[3][3]) };
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

MeshDatas FBXLoader::Load(const char* _FilePath)const
{
  const std::string FileFormat = GetFileFormat(_FilePath);
  _ASSERT_EXPR_A(CheckSupportFormat(FileFormat), (FileFormat + "は対応していません。").c_str());

  MeshDatas MeshDatas;

  fbxsdk::FbxScene* pScene = ImportScene(_FilePath);

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
      AxisSystemConversion._33 *= -1;
    }
    if (fbxsdk::FbxSystemUnit::cm == GlobalSettings.GetSystemUnit())
    {
      SystemUnitConversion._11 *= 0.01f;
      SystemUnitConversion._22 *= 0.01f;
      SystemUnitConversion._33 *= 0.01f;
    }
  }

  const TextureLoader TexureLoader;
  const std::string Directory = std::filesystem::path(_FilePath).remove_filename().string();

  std::vector<fbxsdk::FbxMesh*> FbxMeshes = GetFbxMeshes(pScene);
  const vdl::uint FbxMeshNum = static_cast<vdl::uint>(FbxMeshes.size());
  MeshDatas.resize(FbxMeshNum);
  for (vdl::uint FbxMeshCount = 0; FbxMeshCount < FbxMeshNum; ++FbxMeshCount)
  {
    fbxsdk::FbxMesh* pFbxMesh = FbxMeshes[FbxMeshCount];
    MeshData& MeshData = MeshDatas[FbxMeshCount];

    BoneInfluencePerControlPoints BoneInfluencePerControlPoints;
    FetchInfluences(pFbxMesh, &BoneInfluencePerControlPoints);

    FetchMaterials(pFbxMesh, &MeshData.Materials, Directory, FileFormat, TexureLoader);
    FetchVertices(pFbxMesh, &MeshData.Vertices, BoneInfluencePerControlPoints);
    FetchIndices(pFbxMesh, &MeshData.Indices, &MeshDatas[FbxMeshCount].Materials);
    FetchAnimations(pFbxMesh, &MeshData.Animations);

    fbxsdk::FbxNode* pNode = pFbxMesh->GetNode();

    MeshData.Name = pNode->GetName();
    MeshData.GlobalTransform = Cast(pNode->EvaluateGlobalTransform()) * SystemUnitConversion * AxisSystemConversion;
  }

  return MeshDatas;
}

//--------------------------------------------------

fbxsdk::FbxScene* FBXLoader::ImportScene(const char* _FilePath)const
{
  //  新規シーンを作成
  fbxsdk::FbxScene* pScene = fbxsdk::FbxScene::Create(pManager_, "");

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

  return pScene;
}

std::vector<fbxsdk::FbxMesh*> FBXLoader::GetFbxMeshes(fbxsdk::FbxScene* _pScene)const
{
  std::vector<fbxsdk::FbxMesh*> Meshes;

  std::function<void(fbxsdk::FbxNode*)> Traverse = [&](fbxsdk::FbxNode* pNode)
  {
    if (pNode)
    {
      if (fbxsdk::FbxNodeAttribute * pFbxNodeAttribute = pNode->GetNodeAttribute())
      {
        switch (pFbxNodeAttribute->GetAttributeType())
        {
        case fbxsdk::FbxNodeAttribute::eMesh:
          Meshes.push_back(pNode->GetMesh());
          break;
        }
      }

      for (int i = 0, ChildCount = pNode->GetChildCount(); i < ChildCount; ++i)
      {
        Traverse(pNode->GetChild(i));
      }
    }
  };

  Traverse(_pScene->GetRootNode());

  return Meshes;
}

void FBXLoader::FetchMaterials(fbxsdk::FbxMesh* _pMesh, Materials* _pMaterials, const std::string& _FileDirectory, const std::string& _FileFormat, const TextureLoader& _TextureLoader)const
{
  assert(_pMesh && _pMaterials);

  fbxsdk::FbxNode* pNode = _pMesh->GetNode();

  const vdl::uint MaterialNum = pNode->GetMaterialCount();
  _pMaterials->resize(MaterialNum > 0 ? MaterialNum : 1);

  for (vdl::uint MaterialCount = 0; MaterialCount < MaterialNum; ++MaterialCount)
  {
    Material& Material = (*_pMaterials)[MaterialCount];

    const fbxsdk::FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(MaterialCount);
    auto FetchMaterialProperty = [&](Material::Property* _pProperty, const char* _PropertyName, const char* _FactorName)->void
    {
      const fbxsdk::FbxProperty Property = pSurfaceMaterial->FindProperty(_PropertyName);
      const fbxsdk::FbxProperty Factor = pSurfaceMaterial->FindProperty(_FactorName);

      if (Property.IsValid() && Factor.IsValid())
      {
        const fbxsdk::FbxDouble3 Color = Property.Get<fbxsdk::FbxDouble3>();
        const double f = Factor.Get<fbxsdk::FbxDouble>();

        _pProperty->Color.Red = static_cast<float>(Color[0] * f);
        _pProperty->Color.Green = static_cast<float>(Color[1] * f);
        _pProperty->Color.Blue = static_cast<float>(Color[2] * f);
        _pProperty->Color.Alpha = 1.0f;
      }

      if (Property.IsValid())
      {
        const vdl::uint TextureNum = Property.GetSrcObjectCount<fbxsdk::FbxFileTexture>();

        if (TextureNum > 0)
        {
          if (fbxsdk::FbxFileTexture * pFileTexture = Property.GetSrcObject<fbxsdk::FbxFileTexture>())
          {
            const std::string FileName = (_FileFormat == "fbx" ? _FileDirectory + pFileTexture->GetRelativeFileName() : pFileTexture->GetFileName());
            _pProperty->CompressionImage = _TextureLoader.LoadFromFile(FileName.c_str());
          }
        }
      }
    };

    FetchMaterialProperty(&Material.Diffuse, fbxsdk::FbxSurfaceMaterial::sDiffuse, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor);
    FetchMaterialProperty(&Material.Ambient, fbxsdk::FbxSurfaceMaterial::sAmbient, fbxsdk::FbxSurfaceMaterial::sAmbientFactor);
    FetchMaterialProperty(&Material.Specular, fbxsdk::FbxSurfaceMaterial::sSpecular, fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
    FetchMaterialProperty(&Material.NormalMap, fbxsdk::FbxSurfaceMaterial::sNormalMap, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
    FetchMaterialProperty(&Material.Bump, fbxsdk::FbxSurfaceMaterial::sBump, fbxsdk::FbxSurfaceMaterial::sBumpFactor);
  }
}

void FBXLoader::FetchInfluences(fbxsdk::FbxMesh* _pMesh, BoneInfluencePerControlPoints* _pInfluences)const
{
  const vdl::uint ControlPointNum = _pMesh->GetControlPointsCount();
  _pInfluences->resize(ControlPointNum);

  const vdl::uint DeformerNum = _pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
  for (vdl::uint DeformerCount = 0; DeformerCount < DeformerNum; ++DeformerCount)
  {
    fbxsdk::FbxSkin* pSkin = static_cast<fbxsdk::FbxSkin*>(_pMesh->GetDeformer(DeformerCount, fbxsdk::FbxDeformer::eSkin));

    const vdl::uint ClusterNum = pSkin->GetClusterCount();
    for (vdl::uint ClusterCount = 0; ClusterCount < ClusterNum; ++ClusterCount)
    {
      fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(ClusterCount);
      const vdl::uint ControlPointIndicesNum = pCluster->GetControlPointIndicesCount();
      const int* ControlPointIndices = pCluster->GetControlPointIndices();
      const double* ControlPointWeights = pCluster->GetControlPointWeights();

      for (vdl::uint ControlPointIndicesCount = 0; ControlPointIndicesCount < ControlPointIndicesNum; ++ControlPointIndicesCount)
      {
        BoneInfluencePerControlPoint& InfluencePerControlPoint = (*_pInfluences)[ControlPointIndices[ControlPointIndicesCount]];
        BoneInfluence Influence;
        Influence.Index = ClusterCount;
        Influence.Weight = static_cast<float>(ControlPointWeights[ControlPointIndicesCount]);
        InfluencePerControlPoint.emplace_back(std::move(Influence));
      }
    }
  }
}

void FBXLoader::FetchVertices(fbxsdk::FbxMesh* _pMesh, Vertices* _pVertices, const BoneInfluencePerControlPoints& _BoneInfluencePerControlPoints)const
{
  assert(_pMesh && _pVertices);

  const vdl::uint PolygonNum = _pMesh->GetPolygonCount();
  _pVertices->resize(PolygonNum * 3);

  const bool hasNormal = _pMesh->GetElementNormalCount() > 0;
  const bool hasTangent = _pMesh->GetElementTangentCount() > 0;
  const bool hasUV = _pMesh->GetElementUVCount() > 0;

  const fbxsdk::FbxVector4* ControlPoints = _pMesh->GetControlPoints();
  fbxsdk::FbxStringList UVNames;
  _pMesh->GetUVSetNames(UVNames);

  for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
  {
    for (vdl::uint VertexCount = 0; VertexCount < 3; ++VertexCount)
    {
      const int VertexIndex = VertexCount + PolygonCount * 3;

      vdl::SkinnedMeshVertex& Vertex = (*_pVertices)[VertexIndex];

      const int ControlPointIndex = _pMesh->GetPolygonVertex(PolygonCount, VertexCount);

      Vertex.Position.x = static_cast<float>(ControlPoints[ControlPointIndex][0]);
      Vertex.Position.y = static_cast<float>(ControlPoints[ControlPointIndex][1]);
      Vertex.Position.z = static_cast<float>(ControlPoints[ControlPointIndex][2]);

      BoneInfluencePerControlPoint BoneInfluencePerControlPoint = _BoneInfluencePerControlPoints[ControlPointIndex];

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

      for (vdl::uint InfluenceIndex = 0; InfluenceIndex < Size; ++InfluenceIndex)
      {
        Vertex.BoneWeights[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Weight;
        Vertex.BoneIndices[InfluenceIndex] = BoneInfluencePerControlPoint[InfluenceIndex].Index;
      }

      if (hasNormal)
      {
        fbxsdk::FbxVector4 Normal;
        _pMesh->GetPolygonVertexNormal(PolygonCount, VertexCount, Normal);

        Vertex.Normal.x = static_cast<float>(Normal[0]);
        Vertex.Normal.y = static_cast<float>(Normal[1]);
        Vertex.Normal.z = static_cast<float>(Normal[2]);
      }

      if (hasTangent)
      {
        const fbxsdk::FbxGeometryElementTangent* pGeometryTangent = _pMesh->GetElementTangent(0);
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

        const auto& Directs = pGeometryTangent->GetDirectArray();

        Vertex.Tangent.x = static_cast<float>(Directs[TangentIndex][0]);
        Vertex.Tangent.y = static_cast<float>(Directs[TangentIndex][1]);
        Vertex.Tangent.z = static_cast<float>(Directs[TangentIndex][2]);
      }
      else
      {
        Vertex.Tangent = Vertex.Normal.Cross(vdl::float3::Up());
      }

      if (hasUV)
      {
        fbxsdk::FbxVector2 UV;
        bool UnMappedUV = false;
        _pMesh->GetPolygonVertexUV(PolygonCount, VertexCount, UVNames[0], UV, UnMappedUV);

        Vertex.Texcoord.x = static_cast<float>(UV[0]);
        Vertex.Texcoord.y = 1.0f - static_cast<float>(UV[1]);
      }
    }
  }
}

void FBXLoader::FetchIndices(fbxsdk::FbxMesh* _pMesh, Indices* _pIndices, Materials* _pMaterials)const
{
  assert(_pMesh && _pIndices && _pMaterials);

  const vdl::uint PolygonNum = _pMesh->GetPolygonCount();
  _pIndices->resize(PolygonNum * 3);

  const fbxsdk::FbxLayerElementArrayTemplate<int>& MaterialIndices = _pMesh->GetElementMaterial()->GetIndexArray();
  const bool hasMaterial = _pMesh->GetNode()->GetMaterialCount() > 0;

  if (hasMaterial)
  {
    for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
    {
      (*_pMaterials)[MaterialIndices[PolygonCount]].IndexCount += 3;
    }
  }

  vdl::uint Offset = 0;
  for (auto& Material : (*_pMaterials))
  {
    Material.IndexStart = Offset;
    Offset += Material.IndexCount;
    Material.IndexCount = 0;
  }

  vdl::uint VertexNum = 0;
  for (vdl::uint PolygonCount = 0; PolygonCount < PolygonNum; ++PolygonCount)
  {
    Material& Material = (*_pMaterials)[(hasMaterial ? MaterialIndices[PolygonCount] : 0)];
    const vdl::uint OffsetIndex = Material.IndexStart + Material.IndexCount;

    for (vdl::uint VertexCount = 0; VertexCount < 3; ++VertexCount)
    {
      (*_pIndices)[OffsetIndex + VertexCount] = VertexNum++;
    }
    Material.IndexCount += 3;
  }
}

void FBXLoader::FetchAnimations(fbxsdk::FbxMesh* _pMesh, Animations* _pAnimations)const
{
  assert(_pMesh && _pAnimations);

  fbxsdk::FbxScene* pScene = _pMesh->GetScene();

  fbxsdk::FbxArray<fbxsdk::FbxString*> AnimationStackNames;
  pScene->FillAnimStackNameArray(AnimationStackNames);

  const vdl::uint AnimationNum = AnimationStackNames.Size();
  _pAnimations->resize(AnimationNum);

  const fbxsdk::FbxTime::EMode TimeMode = pScene->GetGlobalSettings().GetTimeMode();
  for (vdl::uint AnimationCount = 0; AnimationCount < AnimationNum; ++AnimationCount)
  {
    fbxsdk::FbxString* AnimationStackName = AnimationStackNames[AnimationCount];
    fbxsdk::FbxAnimStack* CurrentAnimationStack = pScene->FindMember<fbxsdk::FbxAnimStack>(AnimationStackName->Buffer());
    pScene->SetCurrentAnimationStack(CurrentAnimationStack);

    const fbxsdk::FbxTakeInfo* TakeInfo = pScene->GetTakeInfo(AnimationStackName->Buffer());
    const fbxsdk::FbxTime StartTime = TakeInfo->mLocalTimeSpan.GetStart();
    const fbxsdk::FbxTime EndTime = TakeInfo->mLocalTimeSpan.GetStop();

    fbxsdk::FbxTime SamplingStep;
    SamplingStep.SetTime(0, 0, 1, 0, 0, TimeMode);
    SamplingStep = static_cast<fbxsdk::FbxLongLong>(SamplingStep.Get() * kAnimationSamplingTime);

    for (fbxsdk::FbxTime CurrentTime = StartTime; CurrentTime < EndTime; CurrentTime += SamplingStep)
    {
      Skeletal Skeletal;
      FetchBoneMatrices(_pMesh, &Skeletal, CurrentTime);

      (*_pAnimations)[AnimationCount].Skeletals.emplace_back(std::move(Skeletal));
    }

    delete AnimationStackNames[AnimationCount];
  }
}

void FBXLoader::FetchBoneMatrices(fbxsdk::FbxMesh* _pMesh, Skeletal* _pSkeletal, fbxsdk::FbxTime _Time)const
{
  fbxsdk::FbxNode* pNode = _pMesh->GetNode();

  const vdl::uint DeformerNum = _pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
  for (vdl::uint DeformerCount = 0; DeformerCount < DeformerNum; ++DeformerCount)
  {
    fbxsdk::FbxSkin* pSkin = static_cast<fbxsdk::FbxSkin*>(_pMesh->GetDeformer(DeformerCount, fbxsdk::FbxDeformer::eSkin));

    const vdl::uint ClusterNum = pSkin->GetClusterCount();
    _pSkeletal->resize(ClusterNum);

    for (vdl::uint ClusterCount = 0; ClusterCount < ClusterNum; ++ClusterCount)
    {
      fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(ClusterCount);
      fbxsdk::FbxNode* pLink = pCluster->GetLink();

      Bone& Bone = (*_pSkeletal)[ClusterCount];
      fbxsdk::FbxAMatrix ReferenceGlobalInitPosition;
      pCluster->GetTransformMatrix(ReferenceGlobalInitPosition);

      fbxsdk::FbxAMatrix ClusterGlobalInitPosition;
      pCluster->GetTransformLinkMatrix(ClusterGlobalInitPosition);

      FbxAMatrix ClusterGlobalCurrentPosition;
      ClusterGlobalCurrentPosition = pLink->EvaluateGlobalTransform(_Time);

      FbxAMatrix ReferenceGlobalCurrentPosition;
      ReferenceGlobalCurrentPosition = pNode->EvaluateGlobalTransform(_Time);

      Bone.Name = pLink->GetName();
      Bone.Offset = Cast(ClusterGlobalInitPosition.Inverse() * ReferenceGlobalInitPosition);
      Bone.Pose = Cast(ReferenceGlobalCurrentPosition.Inverse() * ClusterGlobalCurrentPosition);
    }
  }
}
