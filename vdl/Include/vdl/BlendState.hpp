#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class Blend : uint8_t
  {
    eZero = 1,
    eOne = 2,
    eSrcColor = 3,
    eInvSrcColor = 4,
    eSrcAlpha = 5,
    eInvSrcAlpha = 6,
    eDestAlpha = 7,
    eInvDestAlpha = 8,
    eDestColor = 9,
    eInvDestColor = 10,
    eSrcAlphaSat = 11,
    eBlendFactor = 14,
    eInvBlendFactor = 15,
    eSrc1Color = 16,
    eInvSrc1Color = 17,
    eSrc1Alpha = 18,
    eInvSrc1Alpha = 19
  };

  enum class BlendOperation : uint8_t
  {
    eAdd = 1,
    eSubtract = 2,
    eRevSubtract = 3,
    eMin = 4,
    eMax = 5
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
        bool AlphaToCoverageEnable : 3;
        bool BlendEnable : 3;
        Blend SrcBlend : 5;
        Blend DestBlend : 5;
        BlendOperation BlendOp : 3;
        Blend SrcBlendAlpha : 5;
        Blend DestBlnedAlpha : 5;
        BlendOperation BlendOpAlpha : 3;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    bool operator==(const BlendState& _BlendState)const noexcept { return Data == _BlendState.Data; }
    bool operator!=(const BlendState& _BlendState)const noexcept { return !((*this) == _BlendState); }
  public:
    constexpr BlendState(bool _AlphaToCoverageEnable = false,
      bool _BlendEnable = false,
      Blend _SrcBlend = Blend::eOne,
      Blend _DestBlend = Blend::eZero,
      BlendOperation _BlendOp = BlendOperation::eAdd,
      Blend _SrcBlendAlpha = Blend::eOne,
      Blend _DestBlnedAlpha = Blend::eZero,
      BlendOperation _BlendOpAlpha = BlendOperation::eAdd)
      : AlphaToCoverageEnable(_AlphaToCoverageEnable),
      BlendEnable(_BlendEnable),
      SrcBlend(_SrcBlend),
      DestBlend(_DestBlend),
      BlendOp(_BlendOp),
      SrcBlendAlpha(_SrcBlendAlpha),
      DestBlnedAlpha(_DestBlnedAlpha),
      BlendOpAlpha(_BlendOpAlpha) {}

    BlendState(PreDefined _PreDefined)
    {
      static constexpr BlendState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { false, true, Blend::eSrcAlpha, Blend::eInvSrcAlpha, BlendOperation::eAdd, Blend::eOne, Blend::eInvSrcAlpha, BlendOperation::eAdd },
        { false, true, Blend::eSrcAlpha, Blend::eOne, BlendOperation::eAdd, Blend::eZero, Blend::eOne, BlendOperation::eAdd },
        { false, true, Blend::eSrcAlpha, Blend::eOne, BlendOperation::eSubtract, Blend::eZero, Blend::eOne, BlendOperation::eAdd },
        { false, true, Blend::eSrcAlpha, Blend::eZero, BlendOperation::eAdd, Blend::eOne, Blend::eZero, BlendOperation::eAdd },
        { false, true, Blend::eZero, Blend::eSrcColor, BlendOperation::eAdd, Blend::eDestAlpha, Blend::eZero, BlendOperation::eAdd },
        { false, true, Blend::eOne, Blend::eOne, BlendOperation::eMax, Blend::eOne, Blend::eOne, BlendOperation::eMax },
        { false, true, Blend::eOne, Blend::eOne, BlendOperation::eMin, Blend::eOne, Blend::eOne, BlendOperation::eMin },
        { false, true, Blend::eSrcAlpha, Blend::eInvSrcColor, BlendOperation::eAdd, Blend::eOne, Blend::eInvSrcAlpha, BlendOperation::eAdd }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  };
}
