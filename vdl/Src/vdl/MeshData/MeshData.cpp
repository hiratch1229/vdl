#include <vdl/MeshData.hpp>

#include <vdl/Angle.hpp>
#include <vdl/MathConstants.hpp>

namespace vdl
{
  MeshData MeshData::Rectangle(const Texture& _Diffuse)
  {
    MeshData MeshData;
    {
      MeshData.Name = "Rectangle";

      MeshData.Vertices.resize(4);
      {
        MeshData.Vertices[0] = { { -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[1] = { { +0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[2] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[3] = { { +0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
      }

      MeshData.Indices.resize(6);
      {
        MeshData.Indices[0] = 0;
        MeshData.Indices[1] = 1;
        MeshData.Indices[2] = 2;
        MeshData.Indices[3] = 1;
        MeshData.Indices[4] = 3;
        MeshData.Indices[5] = 2;
      }

      MeshData.Materials.resize(1);
      {
        MeshData.Materials[0].Diffuse = _Diffuse;
        MeshData.Materials[0].IndexStart = 0;
        MeshData.Materials[0].IndexCount = 6;
      }
    }

    return MeshData;
  }

  MeshData MeshData::Box(const Texture& _Diffuse)
  {
    MeshData MeshData;
    {
      MeshData.Name = "Box";

      MeshData.Vertices.resize(24);
      {
        //  手前
        MeshData.Vertices[0] = { { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[1] = { { +0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[2] = { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[3] = { { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  奥
        MeshData.Vertices[4] = { { +0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[5] = { { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[6] = { { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[7] = { { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, +1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  下
        MeshData.Vertices[8] = { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[9] = { { +0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[10] = { { -0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[11] = { { +0.5f, -0.5f, +0.5f }, { 0.0f, -1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  上
        MeshData.Vertices[12] = { { -0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[13] = { { +0.5f, +0.5f, +0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[14] = { { -0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[15] = { { +0.5f, +0.5f, -0.5f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

        //  左
        MeshData.Vertices[16] = { { -0.5f, +0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[17] = { { -0.5f, +0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[18] = { { -0.5f, -0.5f, +0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[19] = { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };

        //  右
        MeshData.Vertices[20] = { { +0.5f, +0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 0.0f } };
        MeshData.Vertices[21] = { { +0.5f, +0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 0.0f } };
        MeshData.Vertices[22] = { { +0.5f, -0.5f, -0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 0.0f, 1.0f } };
        MeshData.Vertices[23] = { { +0.5f, -0.5f, +0.5f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, +1.0f }, { 1.0f, 1.0f } };
      }

      MeshData.Indices.resize(36);
      {
        MeshData.Indices[0] = 0;
        MeshData.Indices[1] = 1;
        MeshData.Indices[2] = 2;
        MeshData.Indices[3] = 1;
        MeshData.Indices[4] = 3;
        MeshData.Indices[5] = 2;
        MeshData.Indices[6] = 4;
        MeshData.Indices[7] = 5;
        MeshData.Indices[8] = 6;
        MeshData.Indices[9] = 5;
        MeshData.Indices[10] = 7;
        MeshData.Indices[11] = 6;
        MeshData.Indices[12] = 8;
        MeshData.Indices[13] = 9;
        MeshData.Indices[14] = 10;
        MeshData.Indices[15] = 9;
        MeshData.Indices[16] = 11;
        MeshData.Indices[17] = 10;
        MeshData.Indices[18] = 12;
        MeshData.Indices[19] = 13;
        MeshData.Indices[20] = 14;
        MeshData.Indices[21] = 13;
        MeshData.Indices[22] = 15;
        MeshData.Indices[23] = 14;
        MeshData.Indices[24] = 16;
        MeshData.Indices[25] = 17;
        MeshData.Indices[26] = 18;
        MeshData.Indices[27] = 17;
        MeshData.Indices[28] = 19;
        MeshData.Indices[29] = 18;
        MeshData.Indices[30] = 20;
        MeshData.Indices[31] = 21;
        MeshData.Indices[32] = 22;
        MeshData.Indices[33] = 21;
        MeshData.Indices[34] = 23;
        MeshData.Indices[35] = 22;
      }

      MeshData.Materials.resize(1);
      {
        MeshData.Materials[0].Diffuse = _Diffuse;
        MeshData.Materials[0].IndexStart = 0;
        MeshData.Materials[0].IndexCount = 36;
      }
    }

    return MeshData;
  }

  MeshData MeshData::Sphere(uint _SliceCount, uint _StackCount, const Texture& _Diffuse)
  {
    //  半径
    constexpr float kRadius = 0.5f;

    MeshData MeshData;
    {
      MeshData.Name = "Sphere";

      const size_t VertexNum = 1 + (_StackCount + 1) * (_SliceCount + 1) + 1;
      MeshData.Vertices.resize(VertexNum);
      {
        //  一番上の点
        MeshData.Vertices[0] = { { 0.0f, +kRadius, 0.0f }, { 0.0f, +1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };

        const Radian PhiStep = Math::kPI / _StackCount;
        const Radian ThetaStep = Math::kTwoPI / _SliceCount;

        for (uint i = 0; i <= _StackCount; ++i)
        {
          const Radian Phi = PhiStep * i;

          for (uint j = 0; j <= _SliceCount; ++j)
          {
            Vertex3D& Vertex = MeshData.Vertices[1 + _SliceCount * i + j];

            const Radian Theta = ThetaStep * j;

            Vertex.Position = float3(kRadius * sinf(Phi) * cosf(Theta), kRadius * cosf(Phi), kRadius * sinf(Phi) * sinf(Theta));
            Vertex.Tangent = float3(-kRadius * sinf(Phi) * sinf(Theta), 0.0f, kRadius * sinf(Phi) * cosf(Theta)).Normalize();
            Vertex.Normal = Vertex.Position.Normalize();
            Vertex.Texcoord = float2(Theta / Math::kTwoPI, Phi / Math::kPI);
          }
        }

        //  一番下の点
        MeshData.Vertices[VertexNum - 1] = { { 0.0f, -kRadius, 0.0f }, { 0.0f, -1.0f, 0.0f }, { +1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
      }

      const size_t IndexNum = _SliceCount + (_StackCount - 1) * _SliceCount + _SliceCount;
      MeshData.Indices.resize(IndexNum);
      {
        //  一番上の点周辺
        for (uint i = 0; i < _SliceCount; ++i)
        {
          const uint BaseIndex = i * 3;
          MeshData.Indices[BaseIndex + 0] = 0;
          MeshData.Indices[BaseIndex + 1] = i + 2;
          MeshData.Indices[BaseIndex + 2] = i + 1;
        }

        uint BaseIndex = 1;
        const uint RingVertexCount = _SliceCount + 1;
        //  中心部分
        {
          for (uint i = 0; i < _StackCount - 2; ++i)
          {
            for (uint j = 0; j < _SliceCount; ++j)
            {
              const uint StartIndex = _SliceCount * 3 + (_SliceCount * i + j) * 6;

              MeshData.Indices[StartIndex + 0] = BaseIndex + i * RingVertexCount + j;
              MeshData.Indices[StartIndex + 1] = BaseIndex + i * RingVertexCount + j + 1;
              MeshData.Indices[StartIndex + 2] = BaseIndex + (i + 1) * RingVertexCount + j;
              MeshData.Indices[StartIndex + 4] = BaseIndex + (i + 1) * RingVertexCount + j;
              MeshData.Indices[StartIndex + 5] = BaseIndex + i * RingVertexCount + j + 1;
              MeshData.Indices[StartIndex + 6] = BaseIndex + (i + 1) * RingVertexCount + j + 1;
            }
          }
        }

        //  一番下の点周辺
        {
          const uint SouthPoleIndex = static_cast<uint>(VertexNum) - 1;
          BaseIndex = SouthPoleIndex - RingVertexCount;

          const uint StartIndex = _SliceCount * 3 + (_StackCount - 2) * _SliceCount * 6;
          for (uint i = 0; i < _SliceCount; ++i)
          {
            MeshData.Indices[StartIndex + i * 3 + 0] = SouthPoleIndex;
            MeshData.Indices[StartIndex + i * 3 + 1] = BaseIndex + i;
            MeshData.Indices[StartIndex + i * 3 + 2] = BaseIndex + i + 1;
          }
        }
      }

      MeshData.Materials.resize(1);
      {
        MeshData.Materials[0].Diffuse = _Diffuse;
        MeshData.Materials[0].IndexStart = 0;
        MeshData.Materials[0].IndexCount = static_cast<vdl::IndexType>(IndexNum);
      }
    }

    return MeshData;
  }

  //MeshData MeshData::Capsule(uint _SliceCount, uint _StackCount, float _Height, const Texture& _Diffuse)
  //{
  //  MeshData MeshData;
  //  {
  //    MeshData.Name = "Capsule";
  //
  //    MeshData.Vertices.resize();
  //
  //    MeshData.Indices.resize();
  //
  //    MeshData.Materials.resize(1);
  //    {
  //      MeshData.Materials[0].Diffuse.Texture = _Diffuse;
  //      MeshData.Materials[0].Diffuse.Color = Palette::White;
  //      MeshData.Materials[0].IndexStart = 0;
  //      MeshData.Materials[0].IndexCount = ;
  //    }
  //  }
  //
  //  return MeshData;
  //}
}
