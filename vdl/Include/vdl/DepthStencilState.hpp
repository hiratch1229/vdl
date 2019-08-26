#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class ComparisonFunc : uint8_t
  {
    Never = 1,
    Less = 2,
    Equal = 3,
    LessEqual = 4,
    Greater = 5,
    NotEqual = 6,
    GreaterEqual = 7,
    Always = 8
  };

  enum class DepthWriteMask : uint8_t
  {
    Zero = 0,
    All = 1
  };

  enum class StencilOp : uint8_t
  {
    Keep = 1,
    Zero = 2,
    Rreplace = 3,
    IncrSat = 4,
    DecrSat = 5,
    Invert = 6,
    Incr = 7,
    Decr = 8
  };

  class DepthStencilOpDesc
  {
    enum class PreDefined : uint8_t
    {
      DefaultFront,
      DefaultBack,
      Num
    };
  public:
    using DataType = uint16_t;
  public:
#pragma warning(disable:4201)
    union
    {
      struct
      {
        StencilOp FailOp_ : 4;
        StencilOp DepthFailOp_ : 4;
        StencilOp PassOp_ : 4;
        ComparisonFunc Func_ : 4;
      };
      DataType DataType_;
    };
#pragma warning(default:4201)
  public:
    constexpr DepthStencilOpDesc(StencilOp _FailOp,
      StencilOp _DepthFailOp,
      StencilOp _PassOp,
      ComparisonFunc _Func)
      : FailOp_(_FailOp),
      DepthFailOp_(_DepthFailOp),
      PassOp_(_PassOp),
      Func_(_Func)
    {

    }
  public:
    DepthStencilOpDesc(PreDefined _PreDefined)
    {
      static constexpr DepthStencilOpDesc PreDefineds[static_cast<UINT>(PreDefined::Num)] =
      {
        { StencilOp::Keep, StencilOp::Incr, StencilOp::Keep, ComparisonFunc::Always },
        { StencilOp::Keep, StencilOp::Decr, StencilOp::Keep, ComparisonFunc::Always }
      };

      *this = PreDefineds[static_cast<UINT>(_PreDefined)];
    }
  public:
    bool operator==(const DepthStencilOpDesc& _DepthStencilOpDesc)const
    {
      return DataType_ == _DepthStencilOpDesc.DataType_;
    }
    bool operator!=(const DepthStencilOpDesc& _DepthStencilOpDesc)const
    {
      return !((*this) == _DepthStencilOpDesc);
    }
  public:
    static constexpr PreDefined DefaultFront = PreDefined::DefaultFront;
    static constexpr PreDefined DefaultBack = PreDefined::DefaultBack;
  };

  class DepthStencilState
  {
    enum class PreDefined : UINT
    {
      Default2D,
      Default3D,
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
        bool DepthEnable_ : 2;
        DepthWriteMask DepthWriteMask_ : 2;
        ComparisonFunc DepthFunc_ : 4;
        bool StencilEnable_ : 2;
        DepthStencilOpDesc FrontFace_;
        DepthStencilOpDesc BackFace_;
      };
      DataType DataType_;
    };
#pragma warning(default:4201)
  public:
    constexpr DepthStencilState(bool _DepthEnable = true,
      DepthWriteMask _DepthWriteMask = DepthWriteMask::All,
      ComparisonFunc _DepthFunc = ComparisonFunc::Less,
      bool _StencilEnable = false,
      const DepthStencilOpDesc& _FrontFace = DepthStencilOpDesc::DefaultFront,
      const DepthStencilOpDesc& _BackFace = DepthStencilOpDesc::DefaultBack)
      : DepthEnable_(_DepthEnable),
      DepthWriteMask_(_DepthWriteMask),
      DepthFunc_(_DepthFunc),
      StencilEnable_(_StencilEnable),
      FrontFace_(_FrontFace),
      BackFace_(_BackFace)
    {

    }
  public:
    DepthStencilState(PreDefined _PreDefined)
    {
      static const DepthStencilState PreDefineds[static_cast<UINT>(PreDefined::Num)] =
      {
        { false, DepthWriteMask::Zero, ComparisonFunc::Always },
        { true, DepthWriteMask::All, ComparisonFunc::Less }
      };

      *this = PreDefineds[static_cast<UINT>(_PreDefined)];
    }
  public:
    bool operator==(const DepthStencilState& _DepthStencilState)const
    {
      return DataType_ == _DepthStencilState.DataType_;
    }
    bool operator!=(const DepthStencilState& _DepthStencilState)const
    {
      return !((*this) == _DepthStencilState);
    }
  public:
    static constexpr PreDefined Default2D = PreDefined::Default2D;
    static constexpr PreDefined Default3D = PreDefined::Default3D;
  };
}
