#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class FillMode : uint8_t
  {
    Wireframe = 2,
    Solid = 3
  };

  enum class CullMode : uint8_t
  {
    None = 1,
    Front = 2,
    Back = 3
  };

  class RasterizerState
  {
    enum class PreDefined : uint8_t
    {
      SolidCullNone,
      SolidCullFront,
      SolidCullBack,
      WireframeCullNone,
      WireframeCullFront,
      WireframeCullBack,
      Num
    };
  public:
    using DataType = uint64_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        FillMode FillMode_ : 2;
        CullMode CullMode_ : 2;
        bool FrontCounterClockwise_ : 1;
        int DepthBias_;
        bool ScissorEnable_ : 1;
        bool AntialiasedLineAnable_ : 1;
      };
      DataType DataType_;
    };
#pragma warning(default:4201)
  public:
    constexpr RasterizerState(FillMode _FillMode = FillMode::Solid,
      CullMode _CullMode = CullMode::Back,
      bool _FrontCounterClockwise = false,
      UINT _DepthBias = 0,
      bool _ScissorEnable = false,
      bool _AntialiasedLineEnable = false)
      : FillMode_(_FillMode),
      CullMode_(_CullMode),
      FrontCounterClockwise_(_FrontCounterClockwise),
      DepthBias_(_DepthBias),
      ScissorEnable_(_ScissorEnable),
      AntialiasedLineAnable_(_AntialiasedLineEnable)
    {

    }
  public:
    RasterizerState(PreDefined _PreDefined)
    {
      static constexpr RasterizerState PreDefineds[static_cast<UINT>(PreDefined::Num)] =
      {
        { FillMode::Solid, CullMode::None },
        { FillMode::Solid, CullMode::Front },
        { FillMode::Solid, CullMode::Back },
        { FillMode::Wireframe, CullMode::None },
        { FillMode::Wireframe, CullMode::Front },
        { FillMode::Wireframe, CullMode::Back },
      };

      *this = PreDefineds[static_cast<UINT>(_PreDefined)];
    }
  public:
    bool operator==(const RasterizerState& _RasterizerState)const
    {
      return DataType_ == _RasterizerState.DataType_;
    }
    bool operator!=(const RasterizerState& _RasterizerState)const
    {
      return !((*this) == _RasterizerState);
    }
  public:
    static constexpr PreDefined SolidCullNone = PreDefined::SolidCullNone;
    static constexpr PreDefined SolidCullFront = PreDefined::SolidCullFront;
    static constexpr PreDefined SolidCullBack = PreDefined::SolidCullBack;
    static constexpr PreDefined WireframeCullNone = PreDefined::WireframeCullNone;
    static constexpr PreDefined WireframeCullFront = PreDefined::WireframeCullFront;
    static constexpr PreDefined WireframeCullBack = PreDefined::WireframeCullBack;
    static constexpr PreDefined Default2D = PreDefined::SolidCullNone;
    static constexpr PreDefined Default3D = PreDefined::SolidCullBack;
  };
}
