#pragma once
#include "Types.hpp"
#include "Constants.hpp"

#include <array>
#include <vector>
#include <optional>
#include <variant>

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
  //  Half.hpp
  //-------------------------
  class half;

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
  using IndexType = uint32_t;
  using Vertices = std::vector<Vertex3D>;
  using Indices = std::vector<IndexType>;

  //-------------------------
  //  InputLayout.hpp
  //-------------------------
  enum class InputLayoutType : uint8_t;

  //-------------------------
  //  Topology.hpp
  //-------------------------
  enum class TopologyType : uint8_t;

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
  //  GraphicsStates.hpp
  //-------------------------
  struct GraphicsStates;

  //-------------------------
  //  Shader.hpp
  //-------------------------
  class VertexShader;
  class HullShader;
  class DomainShader;
  class GeometryShader;
  class PixelShader;
  class ComputeShader;
  struct GraphicsShaders;

  //-------------------------
  //  Image.hpp
  //-------------------------
  class Image;
  class CompressionImage;

  //-------------------------
  //  Format.hpp
  //-------------------------
  enum class FormatType : uint8_t;

  //-------------------------
  //  Texture.hpp
  //-------------------------
  class Texture;
  class RenderTexture;
  using RenderTextures = std::array<RenderTexture, Constants::kMaxRenderTextureNum>;
  class DepthStencilTexture;
  class UnorderedAccessTexture;
  struct OutputManager;

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
  //  UnorderedAccessBuffer.hpp
  //-------------------------
  namespace Detail { class UnorderedAccessBufferData; }
  template<class T> class UnorderedAccessBuffer;

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
  //  ModelData.hpp
  //-------------------------
  struct Material;
  using Materials = std::vector<Material>;
  struct Bone;
  struct Skeletal;
  struct Animation;
  using Animations = std::vector<Animation>;
  struct MeshData;
  using MeshDatas = std::vector<MeshData>;
  struct ModelData;

  //-------------------------
  //  Model.hpp
  //-------------------------
  class Mesh;
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
  namespace Input { enum class InputDeviceType : uint8_t; }
  namespace Input { class Button; }

  //-------------------------
  //  XInput.hpp
  //-------------------------
  namespace Input { class XInput; }

  //-------------------------
  //  Gamepad.hpp
  //-------------------------
  namespace Input { class Gamepad; }

  //-------------------------
  //-------------------------
  using ID = std::optional<uint>;
  using ShaderResource = std::variant<Texture, Detail::UnorderedAccessBufferData>;
  using UnorderedAccessObject = std::variant<UnorderedAccessTexture, Detail::UnorderedAccessBufferData>;
}
