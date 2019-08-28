#pragma once

namespace vdl
{
  //-------------------------
  //  Type.hpp
  //-------------------------
  template<class Type> struct Type2;
  template<class Type> struct Type3;
  template<class Type> struct Type4;

  using int2 = Type2<int>;
  using uint2 = Type2<unsigned int>;
  using float2 = Type2<float>;
  using int3 = Type3<int>;
  using uint3 = Type3<unsigned int>;
  using float3 = Type3<float>;
  using int4 = Type4<int>;
  using uint4 = Type4<unsigned int>;
  using float4 = Type4<float>;

  //-------------------------
  //  Quaternion.hpp
  //-------------------------
  struct Quaternion;

  //-------------------------
  //  Matrix.hpp
  //-------------------------
  struct Matrix;

  //-------------------------
  //  Angle.hpp
  //-------------------------
  class Radian;
  class Degree;

  //-------------------------
  //  Color.hpp
  //-------------------------
  struct Color;
  struct ColorF;

  //-------------------------
  //  Vertex.hpp
  //-------------------------
  struct Vertex2D;
  struct Vertex3D;

  //-------------------------
  //  Image.hpp
  //-------------------------
  class Image;

  //-------------------------
  //  Format.hpp
  //-------------------------
  enum class Format;

  //-------------------------
  //  Scissor.hpp
  //-------------------------
  struct Scissor;

  //-------------------------
  //  Viewport.hpp
  //-------------------------
  struct Viewport;

  //-------------------------
  //  BlendState.hpp
  //-------------------------
  class BlendState;

  //-------------------------
  //  DepthStencilState.hpp
  //-------------------------
  class DepthStencilState;

  //-------------------------
  //  RasterizerState.hpp
  //-------------------------
  class RasterizerState;

  //-------------------------
  //  SamplerState.hpp
  //-------------------------
  class SamplerState;

  //-------------------------
  //  Texture.hpp
  //-------------------------
  class Texture;
  class RenderTexture;
  class DepthStencilTexture;

  //-------------------------
  //  MeshData.hpp
  //-------------------------
  struct StaticMeshData;
  struct SkinnedMeshData;

  //-------------------------
  //  Model.hpp
  //-------------------------
  class StaticMesh;
  class SkinnedMesh;
  class Model;

  //-------------------------
  //  Shader.hpp
  //-------------------------
  class VertexShader;
  class HullShader;
  class DomainShader;
  class GeometryShader;
  class PixelShader;
  class ComputeShader;
}
