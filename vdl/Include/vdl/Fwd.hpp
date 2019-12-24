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
  struct Color3;
  struct Color3F;
  struct Color4;
  struct Color4F;

  //-------------------------
  //  Vertex.hpp
  //-------------------------
  struct Vertex2D;
  struct VertexStaticMesh;
  struct VertexSkinnedMesh;
  using IndexType = uint32_t;
  using VertexStaticMeshs = std::vector<VertexStaticMesh>;
  using VertexSkinnedMeshs = std::vector<VertexSkinnedMesh>;
  using Indices = std::vector<IndexType>;
  struct Instance2D;
  struct InstanceStaticMesh;
  struct InstanceSkinnedMesh;

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
  class CubeTexture;
  struct OutputManager;

  //-------------------------
  //  Sampler.hpp
  //-------------------------
  struct Sampler;

  //-------------------------
  //  ConstantBuffer.hpp
  //-------------------------
  namespace Detail
  {
    class ConstantBufferData;
  }
  template<class T> class ConstantBuffer;

  //-------------------------
  //  UnorderedAccessBuffer.hpp
  //-------------------------
  namespace Detail
  {
    class UnorderedAccessBufferData;
  }
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
  struct StaticMeshData;
  using StaticMeshDatas = std::vector<StaticMeshData>;
  struct StaticModelData;
  struct Bone;
  struct Skeletal;
  struct Animation;
  using Animations = std::vector<Animation>;
  struct SkinnedMeshData;
  using SkinnedMeshDatas = std::vector<SkinnedMeshData>;
  struct SkinnedModelData;

  //-------------------------
  //  Model.hpp
  //-------------------------
  class StaticMesh;
  class SkinnedMesh;
  class Model;

  //-------------------------
  //  Cicle.hpp
  //-------------------------
  namespace Geometry2D
  {
    struct Circle;
  }

  //-------------------------
  //  Box.hpp
  //-------------------------
  namespace Geometry3D
  {
    struct AABB;
    struct OBB;
  }

  //-------------------------
  //  Sphere.hpp
  //-------------------------
  namespace Geometry3D
  {
    struct Sphere;
  }

  //-------------------------
  //  Capsule.hpp
  //-------------------------
  namespace Geometry3D
  {
    struct Capsule;
  }

  //-------------------------
  //  Button.hpp
  //-------------------------
  namespace Input
  {
    enum class InputDeviceType : uint8_t;
    class Button;
  }

  //-------------------------
  //  XInput.hpp
  //-------------------------
  namespace Input
  {
    class XInput;
  }

  //-------------------------
  //  Gamepad.hpp
  //-------------------------
  namespace Input
  {
    class Gamepad;
  }

  //-------------------------
  //  Flag.hpp
  //-------------------------
  template<class FlagType, class DataType> class Flags;

  //-------------------------
  //  System.hpp
  //-------------------------
  namespace System
  {
    enum class DefaultActionFlag : uint8_t;
    using DefaultActionFlags = Flags<DefaultActionFlag, uint8_t>;
  }

  //-------------------------
  //  GUI.hpp
  //-------------------------
  namespace GUI
  {
    enum class WindowFlag : uint;
    using WindowFlags = Flags<WindowFlag, uint>;
    enum class InputTextFlag : uint;
    using InputTextFlags = Flags<InputTextFlag, uint>;
    enum class TreeNodeFlag : uint16_t;
    using TreeNodeFlags = Flags<TreeNodeFlag, uint16_t>;
    enum class SelectableFlag : uint8_t;
    using SelectableFlags = Flags<SelectableFlag, uint8_t>;
    enum class ComboFlag : uint8_t;
    using ComboFlags = Flags<ComboFlag, uint8_t>;
    enum class TabBarFlag : uint8_t;
    using TabBarFlags = Flags<TabBarFlag, uint8_t>;
    enum class TabItemFlag : uint8_t;
    using TabItemFlags = Flags<TabItemFlag, uint8_t>;
    enum class FocusedFlag : uint8_t;
    using FocusedFlags = Flags<FocusedFlag, uint8_t>;
    enum class HoveredFlag : uint8_t;
    using HoveredFlags = Flags<HoveredFlag, uint8_t>;
    enum class ColorEditFlag : uint;
    using ColorEditFlags = Flags<ColorEditFlag, uint>;
  }

  //-------------------------
  //-------------------------
  using ID = std::optional<uint>;
  using ShaderResource = std::variant<Texture, CubeTexture, Detail::UnorderedAccessBufferData>;
  using UnorderedAccessObject = std::variant<UnorderedAccessTexture, Detail::UnorderedAccessBufferData>;
}
