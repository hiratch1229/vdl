#pragma once
#include "Types.hpp"

#include <array>

namespace vdl
{
  enum class BlendType : uint8_t
  {
    eZero,
    eOne,
    eSrcColor,
    eInvSrcColor,
    eSrcAlpha,
    eInvSrcAlpha,
    eDestAlpha,
    eInvDestAlpha,
    eDestColor,
    eInvDestColor,
    eSrcAlphaSat,
    eBlendFactor,
    eInvBlendFactor,
    eSrc1Color,
    eInvSrc1Color,
    eSrc1Alpha,
    eInvSrc1Alpha
  };

  enum class BlendOpType : uint8_t
  {
    eAdd,
    eSubtract,
    eRevSubtract,
    eMin,
    eMax
  };

  enum class ColorWriteEnableType : uint8_t
  {
    eRed,
    eGreen,
    eBlue,
    eAlpha,
    eAll
  };

  struct RenderTargetBlendState
  {
  private:
    enum class PreDefined : uint8_t
    {
      eAlpha,
      eAdd,
      eSubtract,
      eReplace,
      eMultiply,
      eLighten,
      eDraken,
      eScreen,

      eNum
    };
  public:
    static constexpr PreDefined kDefault = PreDefined::eAlpha;
    static constexpr PreDefined kAdd = PreDefined::eAdd;
    static constexpr PreDefined kSubtract = PreDefined::eSubtract;
    static constexpr PreDefined kReplace = PreDefined::eReplace;
    static constexpr PreDefined kMultiply = PreDefined::eMultiply;
    static constexpr PreDefined kLighten = PreDefined::eLighten;
    static constexpr PreDefined kDraken = PreDefined::eDraken;
    static constexpr PreDefined kScreen = PreDefined::eScreen;
  public:
    using DataType = uint32_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        bool BlendEnable : 1;
        BlendType SrcBlend : 5;
        BlendOpType BlendOp : 3;
        BlendType DestBlend : 5;
        BlendOpType BlendOpAlpha : 3;
        BlendType SrcBlendAlpha : 5;
        BlendType DestBlendAlpha : 5;
        ColorWriteEnableType RenderTargetWriteMask : 3;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr RenderTargetBlendState(bool _BlendEnable = false, BlendType _SrcBlend = BlendType::eOne, BlendType _DestBlend = BlendType::eZero, BlendOpType _BlendOp = BlendOpType::eAdd,
      BlendType _SrcBlendAlpha = BlendType::eOne, BlendType _DestBlendAlpha = BlendType::eZero, BlendOpType _BlendOpAlpha = BlendOpType::eAdd, ColorWriteEnableType _RenderTargetWriteMask = ColorWriteEnableType::eAll)
      : BlendEnable(_BlendEnable), SrcBlend(_SrcBlend), DestBlend(_DestBlend), BlendOp(_BlendOp),
      SrcBlendAlpha(_SrcBlendAlpha), DestBlendAlpha(_DestBlendAlpha), BlendOpAlpha(_BlendOpAlpha), RenderTargetWriteMask(_RenderTargetWriteMask) {}

    RenderTargetBlendState(PreDefined _PreDefined)
    {
      static constexpr RenderTargetBlendState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { true, BlendType::eSrcAlpha, BlendType::eInvSrcAlpha, BlendOpType::eAdd, BlendType::eOne, BlendType::eInvSrcAlpha, BlendOpType::eAdd },
        { true, BlendType::eSrcAlpha, BlendType::eOne, BlendOpType::eAdd, BlendType::eZero, BlendType::eOne, BlendOpType::eAdd },
        { true, BlendType::eSrcAlpha, BlendType::eOne, BlendOpType::eSubtract, BlendType::eZero, BlendType::eOne, BlendOpType::eAdd },
        { true, BlendType::eSrcAlpha, BlendType::eZero, BlendOpType::eAdd, BlendType::eOne, BlendType::eZero, BlendOpType::eAdd },
        { true, BlendType::eZero, BlendType::eSrcColor, BlendOpType::eAdd, BlendType::eDestAlpha, BlendType::eZero, BlendOpType::eAdd },
        { true, BlendType::eOne, BlendType::eOne, BlendOpType::eMax, BlendType::eOne, BlendType::eOne, BlendOpType::eMax },
        { true, BlendType::eOne, BlendType::eOne, BlendOpType::eMin, BlendType::eOne, BlendType::eOne, BlendOpType::eMin },
        { true, BlendType::eSrcAlpha, BlendType::eInvSrcColor, BlendOpType::eAdd, BlendType::eOne, BlendType::eInvSrcAlpha, BlendOpType::eAdd }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const RenderTargetBlendState& _RenderTargetBlendState)const noexcept { return Data == _RenderTargetBlendState.Data; }

    [[nodiscard]] constexpr bool operator!=(const RenderTargetBlendState& _RenderTargetBlendState)const noexcept { return Data != _RenderTargetBlendState.Data; }
  };
  static_assert(sizeof(RenderTargetBlendState) == sizeof(RenderTargetBlendState::DataType));

  struct BlendState
  {
    bool AlphaToCoverageEnable : 1;
    bool IndependentBlendEnable : 1;
    std::array<RenderTargetBlendState, 8> RenderTarget;
  public:
    constexpr BlendState(bool _AlphaToCoverageEnable = false, bool _IndependentBlendEnable = false,
      const RenderTargetBlendState& _RenderTarget0 = RenderTargetBlendState::kDefault, const RenderTargetBlendState& _RenderTarget1 = RenderTargetBlendState::kDefault,
      const RenderTargetBlendState& _RenderTarget2 = RenderTargetBlendState::kDefault, const RenderTargetBlendState& _RenderTarget3 = RenderTargetBlendState::kDefault,
      const RenderTargetBlendState& _RenderTarget4 = RenderTargetBlendState::kDefault, const RenderTargetBlendState& _RenderTarget5 = RenderTargetBlendState::kDefault,
      const RenderTargetBlendState& _RenderTarget6 = RenderTargetBlendState::kDefault, const RenderTargetBlendState& _RenderTarget7 = RenderTargetBlendState::kDefault)
      : AlphaToCoverageEnable(_AlphaToCoverageEnable), IndependentBlendEnable(_IndependentBlendEnable),
      RenderTarget({ _RenderTarget0, _RenderTarget1, _RenderTarget2, _RenderTarget3, _RenderTarget4, _RenderTarget5, _RenderTarget6, _RenderTarget7 }) {}
  public:
    [[nodiscard]] constexpr bool operator==(const BlendState& _BlendState)const noexcept { return AlphaToCoverageEnable == _BlendState.AlphaToCoverageEnable && IndependentBlendEnable == _BlendState.IndependentBlendEnable && RenderTarget == _BlendState.RenderTarget; }

    [[nodiscard]] constexpr bool operator!=(const BlendState& _BlendState)const noexcept { return AlphaToCoverageEnable != _BlendState.AlphaToCoverageEnable || IndependentBlendEnable != _BlendState.IndependentBlendEnable || RenderTarget != _BlendState.RenderTarget; }
  };
}
