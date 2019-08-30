#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class ComparisonFunc : uint8_t
  {
    eNever = 1,
    eLess = 2,
    eEqual = 3,
    eLessEqual = 4,
    eGreater = 5,
    eNotEqual = 6,
    eGreaterEqual = 7,
    eAlways = 8
  };

  enum class WriteMask : uint8_t
  {
    eZero = 0,
    eAll = 1
  };

  enum class StencilOp : uint8_t
  {
    eKeep = 1,
    eZero = 2,
    eRreplace = 3,
    eIncrSat = 4,
    eDecrSat = 5,
    eInvert = 6,
    eIncr = 7,
    eDecr = 8
  };

  struct DepthStencilOpDesc
  {
  private:
    enum class PreDefined : uint8_t
    {
      eDefaultFront,
      eDefaultBack,

      eNum
    };
  public:
    static constexpr PreDefined kDefaultFront = PreDefined::eDefaultFront;
    static constexpr PreDefined kDefaultBack = PreDefined::eDefaultBack;
  public:
    using DataType = uint16_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        StencilOp FailOp : 4;
        StencilOp DepthFailOp : 4;
        StencilOp PassOp : 4;
        ComparisonFunc Func : 4;
      };
      DataType Data;
    };
#pragma warning(default:4201)
    bool operator==(const DepthStencilOpDesc& _DepthStencilOpDesc)const noexcept { return Data == _DepthStencilOpDesc.Data; }
    bool operator!=(const DepthStencilOpDesc& _DepthStencilOpDesc)const noexcept { return !((*this) == _DepthStencilOpDesc); }
  public:
    constexpr DepthStencilOpDesc(StencilOp _FailOp,
      StencilOp _DepthFailOp,
      StencilOp _PassOp,
      ComparisonFunc _Func)
      : FailOp(_FailOp),
      DepthFailOp(_DepthFailOp),
      PassOp(_PassOp),
      Func(_Func) {}

    DepthStencilOpDesc(PreDefined _PreDefined)
    {
      static constexpr DepthStencilOpDesc PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { StencilOp::eKeep, StencilOp::eIncr, StencilOp::eKeep, ComparisonFunc::eAlways },
        { StencilOp::eKeep, StencilOp::eDecr, StencilOp::eKeep, ComparisonFunc::eAlways }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  };

  struct DepthStencilState
  {
  private:
    enum class PreDefined : uint8_t
    {
      eDefault2D,
      eDefault3D,
      
      eNum
    };
  public:
    static constexpr PreDefined kDefault2D = PreDefined::eDefault2D;
    static constexpr PreDefined kDefault3D = PreDefined::eDefault3D;
  public:
    using DataType = uint64_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        bool DepthEnable : 2;
        WriteMask DepthWriteMask : 2;
        ComparisonFunc DepthFunc : 4;
        bool StencilEnable : 2;
        DepthStencilOpDesc FrontFace;
        DepthStencilOpDesc BackFace;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    bool operator==(const DepthStencilState& _DepthStencilState)const noexcept { return Data == _DepthStencilState.Data; }
    bool operator!=(const DepthStencilState& _DepthStencilState)const noexcept { return !((*this) == _DepthStencilState); }
  public:
    constexpr DepthStencilState(bool _DepthEnable = true,
      WriteMask _DepthWriteMask = WriteMask::eAll,
      ComparisonFunc _DepthFunc = ComparisonFunc::eLess,
      bool _StencilEnable = false,
      const DepthStencilOpDesc& _FrontFace = DepthStencilOpDesc::kDefaultFront,
      const DepthStencilOpDesc& _BackFace = DepthStencilOpDesc::kDefaultBack)
      : DepthEnable(_DepthEnable),
      DepthWriteMask(_DepthWriteMask),
      DepthFunc(_DepthFunc),
      StencilEnable(_StencilEnable),
      FrontFace(_FrontFace),
      BackFace(_BackFace){}

    DepthStencilState(PreDefined _PreDefined)
    {
      static const DepthStencilState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { false, WriteMask::eZero, ComparisonFunc::eAlways },
        { true, WriteMask::eAll, ComparisonFunc::eLess }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  };
}
