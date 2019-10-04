#pragma once
#include "Fwd.hpp"

namespace vdl
{
  enum class FillModeType : uint8_t
  {
    eWireframe,
    eSolid
  };

  enum class CullModeType : uint8_t
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
        FillModeType FillMode : 2;
        CullModeType CullMode : 2;
        bool FrontCounterClockwise : 1;
        bool DepthClipEnable : 1;
        bool ScissorEnable : 1;
        bool AntialiasedLineEnable : 1;
        int DepthBias;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr RasterizerState(FillModeType _FillMode = FillModeType::eSolid, CullModeType _CullMode = CullModeType::eBack, bool _FrontCounterClockwise = false,
      bool _DepthClipEnable = true, int _DepthBias = 0, bool _ScissorEnable = false, bool _AntialiasedLineEnable = false)
      : FillMode(_FillMode), CullMode(_CullMode), FrontCounterClockwise(_FrontCounterClockwise), DepthClipEnable(_DepthClipEnable),
      ScissorEnable(_ScissorEnable), AntialiasedLineEnable(_AntialiasedLineEnable), DepthBias(_DepthBias) {}

    RasterizerState(PreDefined _PreDefined)
    {
      static constexpr RasterizerState PreDefineds[static_cast<uint>(PreDefined::Num)] =
      {
        { FillModeType::eSolid, CullModeType::eNone },
        { FillModeType::eSolid, CullModeType::eFront },
        { FillModeType::eSolid, CullModeType::eBack },
        { FillModeType::eWireframe, CullModeType::eNone },
        { FillModeType::eWireframe, CullModeType::eFront },
        { FillModeType::eWireframe, CullModeType::eBack },
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const RasterizerState& _RasterizerState)const noexcept { return Data == _RasterizerState.Data; }

    [[nodiscard]] constexpr bool operator!=(const RasterizerState& _RasterizerState)const noexcept { return Data != _RasterizerState.Data; }
  };
  static_assert(sizeof(RasterizerState) == sizeof(RasterizerState::DataType));
}
