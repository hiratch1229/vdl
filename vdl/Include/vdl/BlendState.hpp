#pragma once
#include "Types.hpp"

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
        bool AlphaToCoverageEnable : 1;
        bool BlendEnable : 1;
        BlendType SrcBlend : 5;
        BlendType DestBlend : 5;
        BlendOpType BlendOp : 3;
        BlendType SrcBlendAlpha : 5;
        BlendType DestBlnedAlpha : 5;
        BlendOpType BlendOpAlpha : 3;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr BlendState(bool _AlphaToCoverageEnable = false, bool _BlendEnable = false, BlendType _SrcBlend = BlendType::eOne, BlendType _DestBlend = BlendType::eZero,
      BlendOpType _BlendOp = BlendOpType::eAdd, BlendType _SrcBlendAlpha = BlendType::eOne, BlendType _DestBlnedAlpha = BlendType::eZero, BlendOpType _BlendOpAlpha = BlendOpType::eAdd)
      : AlphaToCoverageEnable(_AlphaToCoverageEnable), BlendEnable(_BlendEnable), SrcBlend(_SrcBlend), DestBlend(_DestBlend),
      BlendOp(_BlendOp), SrcBlendAlpha(_SrcBlendAlpha), DestBlnedAlpha(_DestBlnedAlpha), BlendOpAlpha(_BlendOpAlpha) {}

    BlendState(PreDefined _PreDefined)
    {
      static constexpr BlendState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { false, true, BlendType::eSrcAlpha, BlendType::eInvSrcAlpha, BlendOpType::eAdd, BlendType::eOne, BlendType::eInvSrcAlpha, BlendOpType::eAdd },
        { false, true, BlendType::eSrcAlpha, BlendType::eOne, BlendOpType::eAdd, BlendType::eZero, BlendType::eOne, BlendOpType::eAdd },
        { false, true, BlendType::eSrcAlpha, BlendType::eOne, BlendOpType::eSubtract, BlendType::eZero, BlendType::eOne, BlendOpType::eAdd },
        { false, true, BlendType::eSrcAlpha, BlendType::eZero, BlendOpType::eAdd, BlendType::eOne, BlendType::eZero, BlendOpType::eAdd },
        { false, true, BlendType::eZero, BlendType::eSrcColor, BlendOpType::eAdd, BlendType::eDestAlpha, BlendType::eZero, BlendOpType::eAdd },
        { false, true, BlendType::eOne, BlendType::eOne, BlendOpType::eMax, BlendType::eOne, BlendType::eOne, BlendOpType::eMax },
        { false, true, BlendType::eOne, BlendType::eOne, BlendOpType::eMin, BlendType::eOne, BlendType::eOne, BlendOpType::eMin },
        { false, true, BlendType::eSrcAlpha, BlendType::eInvSrcColor, BlendOpType::eAdd, BlendType::eOne, BlendType::eInvSrcAlpha, BlendOpType::eAdd }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const BlendState& _BlendState)const noexcept { return Data == _BlendState.Data; }

    [[nodiscard]] constexpr bool operator!=(const BlendState& _BlendState)const noexcept { return Data != _BlendState.Data; }
  };
  static_assert(sizeof(BlendState) == sizeof(BlendState::DataType));
}
