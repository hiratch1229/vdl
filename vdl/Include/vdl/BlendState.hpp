#pragma once
#include "Fwd.hpp"
#include "Constants.hpp"

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

  struct RenderTextureBlendState
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
    static constexpr PreDefined kAlpha = PreDefined::eAlpha;
    static constexpr PreDefined kAdd = PreDefined::eAdd;
    static constexpr PreDefined kSubtract = PreDefined::eSubtract;
    static constexpr PreDefined kReplace = PreDefined::eReplace;
    static constexpr PreDefined kMultiply = PreDefined::eMultiply;
    static constexpr PreDefined kLighten = PreDefined::eLighten;
    static constexpr PreDefined kDraken = PreDefined::eDraken;
    static constexpr PreDefined kScreen = PreDefined::eScreen;
    static constexpr PreDefined kDefault = PreDefined::eAlpha;
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
    constexpr RenderTextureBlendState(bool _BlendEnable = false, BlendType _SrcBlend = BlendType::eOne, BlendType _DestBlend = BlendType::eZero, BlendOpType _BlendOp = BlendOpType::eAdd,
      BlendType _SrcBlendAlpha = BlendType::eOne, BlendType _DestBlendAlpha = BlendType::eZero, BlendOpType _BlendOpAlpha = BlendOpType::eAdd, ColorWriteEnableType _RenderTargetWriteMask = ColorWriteEnableType::eAll)noexcept
      : BlendEnable(_BlendEnable), SrcBlend(_SrcBlend), DestBlend(_DestBlend), BlendOp(_BlendOp),
      SrcBlendAlpha(_SrcBlendAlpha), DestBlendAlpha(_DestBlendAlpha), BlendOpAlpha(_BlendOpAlpha), RenderTargetWriteMask(_RenderTargetWriteMask) {}

    RenderTextureBlendState(PreDefined _PreDefined)noexcept
    {
      static constexpr RenderTextureBlendState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
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
    [[nodiscard]] constexpr bool operator==(const RenderTextureBlendState& _RenderTextureBlendState)const noexcept { return Data == _RenderTextureBlendState.Data; }

    [[nodiscard]] constexpr bool operator!=(const RenderTextureBlendState& _RenderTextureBlendState)const noexcept { return Data != _RenderTextureBlendState.Data; }
  };
  static_assert(sizeof(RenderTextureBlendState) == sizeof(RenderTextureBlendState::DataType));

  struct BlendState
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
    static constexpr PreDefined kAlpha = PreDefined::eAlpha;
    static constexpr PreDefined kAdd = PreDefined::eAdd;
    static constexpr PreDefined kSubtract = PreDefined::eSubtract;
    static constexpr PreDefined kReplace = PreDefined::eReplace;
    static constexpr PreDefined kMultiply = PreDefined::eMultiply;
    static constexpr PreDefined kLighten = PreDefined::eLighten;
    static constexpr PreDefined kDraken = PreDefined::eDraken;
    static constexpr PreDefined kScreen = PreDefined::eScreen;
    static constexpr PreDefined kDefault = PreDefined::eAlpha;
  public:
    bool AlphaToCoverageEnable : 1;
    bool IndependentBlendEnable : 1;
    std::array<RenderTextureBlendState, Constants::kMaxRenderTextureNum> RenderTexture;
  public:
    constexpr BlendState(bool _AlphaToCoverageEnable = false, bool _IndependentBlendEnable = false,
      const RenderTextureBlendState& _RenderTexture0 = RenderTextureBlendState::kDefault, const RenderTextureBlendState& _RenderTexture1 = RenderTextureBlendState::kDefault,
      const RenderTextureBlendState& _RenderTexture2 = RenderTextureBlendState::kDefault, const RenderTextureBlendState& _RenderTexture3 = RenderTextureBlendState::kDefault,
      const RenderTextureBlendState& _RenderTexture4 = RenderTextureBlendState::kDefault, const RenderTextureBlendState& _RenderTexture5 = RenderTextureBlendState::kDefault,
      const RenderTextureBlendState& _RenderTexture6 = RenderTextureBlendState::kDefault, const RenderTextureBlendState& _RenderTexture7 = RenderTextureBlendState::kDefault)noexcept
      : AlphaToCoverageEnable(_AlphaToCoverageEnable), IndependentBlendEnable(_IndependentBlendEnable),
      RenderTexture({ _RenderTexture0, _RenderTexture1, _RenderTexture2, _RenderTexture3, _RenderTexture4, _RenderTexture5, _RenderTexture6, _RenderTexture7 }) {}

    BlendState(PreDefined _PreDefined)noexcept
    {
      static const BlendState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { false, false, RenderTextureBlendState::kAlpha },
        { false, false, RenderTextureBlendState::kAdd },
        { false, false, RenderTextureBlendState::kSubtract },
        { false, false, RenderTextureBlendState::kReplace },
        { false, false, RenderTextureBlendState::kMultiply },
        { false, false, RenderTextureBlendState::kLighten },
        { false, false, RenderTextureBlendState::kDraken },
        { false, false, RenderTextureBlendState::kScreen } };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const BlendState& _BlendState)const noexcept { return AlphaToCoverageEnable == _BlendState.AlphaToCoverageEnable && IndependentBlendEnable == _BlendState.IndependentBlendEnable && RenderTexture == _BlendState.RenderTexture; }

    [[nodiscard]] constexpr bool operator!=(const BlendState& _BlendState)const noexcept { return AlphaToCoverageEnable != _BlendState.AlphaToCoverageEnable || IndependentBlendEnable != _BlendState.IndependentBlendEnable || RenderTexture != _BlendState.RenderTexture; }
  };
}
