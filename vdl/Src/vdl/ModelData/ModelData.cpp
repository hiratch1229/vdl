#include <vdl/ModelData.hpp>

#include <vdl/Angle.hpp>
#include <vdl/MathConstants.hpp>

namespace vdl
{
  StaticModelData StaticModelData::Rectangle(const Texture& _Diffuse, const Texture& _NormalMap)
  {
    StaticModelData ModelData;
    {
      ModelData.Vertices.resize(4);
      {
        ModelData.Vertices[0] = { { -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[1] = { { +0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[3] = { { +0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
      }

      ModelData.Indices.resize(6);
      {
        ModelData.Indices[0] = 0;
        ModelData.Indices[1] = 1;
        ModelData.Indices[2] = 2;
        ModelData.Indices[3] = 1;
        ModelData.Indices[4] = 3;
        ModelData.Indices[5] = 2;
      }

      ModelData.MeshDatas.resize(1);
      {
        ModelData.MeshDatas[0].IndexCount = static_cast<uint>(ModelData.Indices.size());
        ModelData.MeshDatas[0].Material.Diffuse = _Diffuse;;
        ModelData.MeshDatas[0].Material.MaterialColor = Palette::White;
        ModelData.MeshDatas[0].Material.NormalMap = _NormalMap;
      }
    }

    return ModelData;
  }

  StaticModelData StaticModelData::Box(const Texture& _Diffuse, const Texture& _NormalMap)
  {
    StaticModelData ModelData;
    {
      ModelData.Vertices.resize(24);
      {
        //  手前
        ModelData.Vertices[0] = { { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[1] = { { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[2] = { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[3] = { { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  奥
        ModelData.Vertices[4] = { { +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[5] = { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[6] = { { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[7] = { { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  下
        ModelData.Vertices[8] = { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[9] = { { +0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[10] = { { -0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[11] = { { +0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  上
        ModelData.Vertices[12] = { { -0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[13] = { { +0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[14] = { { -0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[15] = { { +0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  左
        ModelData.Vertices[16] = { { -0.5f, +0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[17] = { { -0.5f, +0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[18] = { { -0.5f, -0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[19] = { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };

        //  右
        ModelData.Vertices[20] = { { +0.5f, +0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } };
        ModelData.Vertices[21] = { { +0.5f, +0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } };
        ModelData.Vertices[22] = { { +0.5f, -0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } };
        ModelData.Vertices[23] = { { +0.5f, -0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } };
      }

      ModelData.Indices.resize(36);
      {
        ModelData.Indices[0] = 0;
        ModelData.Indices[1] = 1;
        ModelData.Indices[2] = 2;
        ModelData.Indices[3] = 1;
        ModelData.Indices[4] = 3;
        ModelData.Indices[5] = 2;
        ModelData.Indices[6] = 4;
        ModelData.Indices[7] = 5;
        ModelData.Indices[8] = 6;
        ModelData.Indices[9] = 5;
        ModelData.Indices[10] = 7;
        ModelData.Indices[11] = 6;
        ModelData.Indices[12] = 8;
        ModelData.Indices[13] = 9;
        ModelData.Indices[14] = 10;
        ModelData.Indices[15] = 9;
        ModelData.Indices[16] = 11;
        ModelData.Indices[17] = 10;
        ModelData.Indices[18] = 12;
        ModelData.Indices[19] = 13;
        ModelData.Indices[20] = 14;
        ModelData.Indices[21] = 13;
        ModelData.Indices[22] = 15;
        ModelData.Indices[23] = 14;
        ModelData.Indices[24] = 16;
        ModelData.Indices[25] = 17;
        ModelData.Indices[26] = 18;
        ModelData.Indices[27] = 17;
        ModelData.Indices[28] = 19;
        ModelData.Indices[29] = 18;
        ModelData.Indices[30] = 20;
        ModelData.Indices[31] = 21;
        ModelData.Indices[32] = 22;
        ModelData.Indices[33] = 21;
        ModelData.Indices[34] = 23;
        ModelData.Indices[35] = 22;
      }

      ModelData.MeshDatas.resize(1);
      {
        ModelData.MeshDatas[0].IndexCount = static_cast<uint>(ModelData.Indices.size());
        ModelData.MeshDatas[0].Material.Diffuse = _Diffuse;;
        ModelData.MeshDatas[0].Material.MaterialColor = Palette::White;
        ModelData.MeshDatas[0].Material.NormalMap = _NormalMap;
      }
    }

    return ModelData;
  }

  StaticModelData StaticModelData::Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse, const Texture& _NormalMap)
  {
    //  半径
    constexpr float kRadius = 0.5f;

    StaticModelData ModelData;
    {
      //  頂点情報
      for (uint i = 0; i <= _StackCount; ++i)
      {
        const float V = i / static_cast<float>(_StackCount);
        const float Phi = V * Math::kPI;

        for (uint j = 0; j <= _SliceCount; ++j)
        {
          const float U = j / static_cast<float>(_SliceCount);
          const float Theta = U * Math::kTwoPI;

          VertexStaticMesh& Vertex = ModelData.Vertices.emplace_back();
          {
            Vertex.Position = float3(kRadius * sinf(Phi) * cosf(Theta), kRadius * cosf(Phi), kRadius * sinf(Phi) * sinf(Theta));
            Vertex.Tangent = float3(-kRadius * sinf(Phi) * sinf(Theta), 0.0f, kRadius * sinf(Phi) * cosf(Theta)).Normalize();
            Vertex.Normal = Vertex.Position.Normalize();
            Vertex.Texcoord = float2(U, V);
          }
        }
      }

      //  インデックス情報
      for (uint i = 0; i < _SliceCount * _StackCount + _SliceCount; ++i)
      {
        ModelData.Indices.emplace_back(i);
        ModelData.Indices.emplace_back(i + _SliceCount + 1);
        ModelData.Indices.emplace_back(i + _SliceCount);

        ModelData.Indices.emplace_back(i + _SliceCount + 1);
        ModelData.Indices.emplace_back(i);
        ModelData.Indices.emplace_back(i + 1);
      }

      ModelData.MeshDatas.resize(1);
      {
        ModelData.MeshDatas[0].IndexCount = static_cast<uint>(ModelData.Indices.size());
        ModelData.MeshDatas[0].Material.Diffuse = _Diffuse;;
        ModelData.MeshDatas[0].Material.MaterialColor = Palette::White;
        ModelData.MeshDatas[0].Material.NormalMap = _NormalMap;
      }
    }

    return ModelData;
  }
}
