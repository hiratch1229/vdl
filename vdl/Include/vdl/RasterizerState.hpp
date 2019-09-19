#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class FillMode : uint8_t
  {
    eWireframe,
    eSolid
  };

  enum class CullMode : uint8_t
  {
    eNone,
    eFront,
    eBack
  };

  struct RasterizerState
  {
  private:
    enum class PreDefined : uint8_t
    {
      eSolidCullNone,
      eSolidCullFront,
      eSolidCullBack,
      eWireframeCullNone,
      eWireframeCullFront,
      eWireframeCullBack,

      Num
    };
  public:
    static constexpr PreDefined kSolidCullNone = PreDefined::eSolidCullNone;
    static constexpr PreDefined kSolidCullFront = PreDefined::eSolidCullFront;
    static constexpr PreDefined kSolidCullBack = PreDefined::eSolidCullBack;
    static constexpr PreDefined kWireframeCullNone = PreDefined::eWireframeCullNone;
    static constexpr PreDefined kWireframeCullFront = PreDefined::eWireframeCullFront;
    static constexpr PreDefined kWireframeCullBack = PreDefined::eWireframeCullBack;
    static constexpr PreDefined kDefault2D = PreDefined::eSolidCullNone;
    static constexpr PreDefined kDefault3D = PreDefined::eSolidCullBack;
  public:
    using DataType = uint64_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        FillMode Filling : 1;
        CullMode Culling : 2;
        bool FrontCounterClockwise : 1;
        int DepthBias;
        bool ScissorEnable : 1;
        bool AntialiasedLineAnable : 1;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr RasterizerState(FillMode _FillMode = FillMode::eSolid, CullMode _CullMode = CullMode::eBack, bool _FrontCounterClockwise = false,
      int _DepthBias = 0, bool _ScissorEnable = false, bool _AntialiasedLineEnable = false)
      : Filling(_FillMode), Culling(_CullMode), FrontCounterClockwise(_FrontCounterClockwise),
      DepthBias(_DepthBias), ScissorEnable(_ScissorEnable), AntialiasedLineAnable(_AntialiasedLineEnable) {}

    RasterizerState(PreDefined _PreDefined)
    {
      static constexpr RasterizerState PreDefineds[static_cast<uint>(PreDefined::Num)] =
      {
        { FillMode::eSolid, CullMode::eNone },
        { FillMode::eSolid, CullMode::eFront },
        { FillMode::eSolid, CullMode::eBack },
        { FillMode::eWireframe, CullMode::eNone },
        { FillMode::eWireframe, CullMode::eFront },
        { FillMode::eWireframe, CullMode::eBack },
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const RasterizerState& _RasterizerState)const noexcept { return Data == _RasterizerState.Data; }

    [[nodiscard]] constexpr bool operator!=(const RasterizerState& _RasterizerState)const noexcept { return Data != _RasterizerState.Data; }
  };
  static_assert(sizeof(RasterizerState) == sizeof(RasterizerState::DataType));
}
