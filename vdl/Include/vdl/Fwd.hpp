#pragma once
#include "Types.hpp"
#include "Constants.hpp"

#include <array>
#include <vector>

namespace vdl
{
  //-------------------------
  //  Type2.hpp
  //-------------------------
  template<class Type> struct Type2;
  using int2 = Type2<int>;
  using uint2 = Type2<unsigned int>;
  using float2 = Type2<float>;

  //-------------------------
  //  Type3.hpp
  //-------------------------
  template<class Type> struct Type3;
  using int3 = Type3<int>;
  using uint3 = Type3<unsigned int>;
  using float3 = Type3<float>;

  //-------------------------
  //  Type4.hpp
  //-------------------------
  template<class Type> struct Type4;
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
  struct TextureVertex;
  struct StaticMeshVertex;
  struct SkinnedMeshVertex;

  //-------------------------
  //  InputLayout.hpp
  //-------------------------
  enum class InputLayout : uint8_t;

  //-------------------------
  //  Topology.hpp
  //-------------------------
  enum class Topology : uint8_t;

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
  enum class BlendType : uint8_t;
  enum class BlendOpType : uint8_t;
  enum class ColorWriteEnableType : uint8_t;
  struct RenderTextureBlendState;
  struct BlendState;

  //-------------------------
  //  DepthStencilState.hpp
  //-------------------------
  enum class StencilOpType : uint8_t;
  enum class ComparisonFuncType : uint8_t;
  enum class DepthWriteMaskType : uint8_t;
  struct DepthStencilOpState;
  struct DepthStencilState;

  //-------------------------
  //  RasterizerState.hpp
  //-------------------------
  enum class FillModeType : uint8_t;
  enum class CullModeType : uint8_t;
  struct RasterizerState;

  //-------------------------
  //  GraphicsState.hpp
  //-------------------------
  struct GraphicsState;

  //-------------------------
  //  Shader.hpp
  //-------------------------
  class VertexShader;
  class HullShader;
  class DomainShader;
  class GeometryShader;
  class PixelShader;
  class ComputeShader;

  //-------------------------
  //  Image.hpp
  //-------------------------
  class Image;

  //-------------------------
  //  Format.hpp
  //-------------------------
  enum class Format;

  //-------------------------
  //  Texture.hpp
  //-------------------------
  class Texture;
  class RenderTexture;
  using RenderTextures = std::array<RenderTexture, Constants::kMaxRenderTextureNum>;
  class DepthStencilTexture;

  //-------------------------
  //  Sampler.hpp
  //-------------------------
  struct Sampler;

  //-------------------------
  //  ConstantBuffer.hpp
  //-------------------------
  namespace Detail { class ConstantBufferData; }
  template<class T> class ConstantBuffer;

  //-------------------------
  //  Camera.hpp
  //-------------------------
  struct Camera;

  //-------------------------
  //  MotionData.hpp
  //-------------------------
  struct MotionData;
  struct MotionBlendData;
  using MotionBlendDatas = std::vector<MotionBlendData>;

  //-------------------------
  //  MeshData.hpp
  //-------------------------
  struct Material;
  struct StaticMeshData;
  struct Bone;
  struct Skeletal;
  struct Animation;
  struct SkinnedMeshData;

  //-------------------------
  //  Model.hpp
  //-------------------------
  class StaticMesh;
  class SkinnedMesh;
  class Model;

  //-------------------------
  //  Cicle.hpp
  //-------------------------
  namespace Geometry2D { struct Circle; }

  //-------------------------
  //  Box.hpp
  //-------------------------
  namespace Geometry3D { struct AABB; }
  namespace Geometry3D { struct OBB; }
  
  //-------------------------
  //  Sphere.hpp
  //-------------------------
  namespace Geometry3D { struct Sphere; }

  //-------------------------
  //  Capsule.hpp
  //-------------------------
  namespace Geometry3D { struct Capsule; }

  //-------------------------
  //  Button.hpp
  //-------------------------
  namespace Input { enum class InputDevice : uint8_t; }
  namespace Input { class Button; }

  //-------------------------
  //  XInput.hpp
  //-------------------------
  namespace Input { class XInput; }

  //-------------------------
  //  Gamepad.hpp
  //-------------------------
  namespace Input { class Gamepad; }
}
