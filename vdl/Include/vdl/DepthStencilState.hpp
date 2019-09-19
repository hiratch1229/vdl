#pragma once
#include "Types.hpp"

namespace vdl
{
  enum class StencilOpType : uint8_t
  {
    eKeep,
    eZero,
    eRreplace,
    eIncrSat,
    eDecrSat,
    eInvert,
    eIncr,
    eDecr
  };

  enum class ComparisonFuncType : uint8_t
  {
    eNever,
    eLess,
    eEqual,
    eLessEqual,
    eGreater,
    eNotEqual,
    eGreaterEqual,
    eAlways
  };

  enum class DepthWriteMaskType : uint8_t
  {
    eZero,
    eAll
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
        StencilOpType FailOp : 3;
        StencilOpType DepthFailOp : 3;
        StencilOpType PassOp : 3;
        ComparisonFuncType ComparisonFunc : 3;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr DepthStencilOpDesc(StencilOpType _FailOp, StencilOpType _DepthFailOp, StencilOpType _PassOp, ComparisonFuncType _ComparisonFunc)
      : FailOp(_FailOp), DepthFailOp(_DepthFailOp), PassOp(_PassOp), ComparisonFunc(_ComparisonFunc) {}

    DepthStencilOpDesc(PreDefined _PreDefined)
    {
      static constexpr DepthStencilOpDesc PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { StencilOpType::eKeep, StencilOpType::eIncr, StencilOpType::eKeep, ComparisonFuncType::eAlways },
        { StencilOpType::eKeep, StencilOpType::eDecr, StencilOpType::eKeep, ComparisonFuncType::eAlways }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
    [[nodiscard]] constexpr bool operator==(const DepthStencilOpDesc& _DepthStencilOpDesc)const noexcept { return Data == _DepthStencilOpDesc.Data; }

    [[nodiscard]] constexpr bool operator!=(const DepthStencilOpDesc& _DepthStencilOpDesc)const noexcept { return Data != _DepthStencilOpDesc.Data;; }
  };
  static_assert(sizeof(DepthStencilOpDesc) == sizeof(DepthStencilOpDesc::DataType));

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
        bool DepthEnable : 1;
        DepthWriteMaskType DepthWriteMask : 1;
        ComparisonFuncType DepthFunc : 3;
        bool StencilEnable : 1;
        DepthStencilOpDesc FrontFace;
        DepthStencilOpDesc BackFace;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr DepthStencilState(bool _DepthEnable = true, DepthWriteMaskType _DepthWriteMask = DepthWriteMaskType::eAll, ComparisonFuncType _DepthFunc = ComparisonFuncType::eLess,
      bool _StencilEnable = false, const DepthStencilOpDesc& _FrontFace = DepthStencilOpDesc::kDefaultFront, const DepthStencilOpDesc& _BackFace = DepthStencilOpDesc::kDefaultBack)
      : DepthEnable(_DepthEnable), DepthWriteMask(_DepthWriteMask), DepthFunc(_DepthFunc),
      StencilEnable(_StencilEnable), FrontFace(_FrontFace), BackFace(_BackFace) {}

    DepthStencilState(PreDefined _PreDefined)
    {
      static const DepthStencilState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { false, DepthWriteMaskType::eZero, ComparisonFuncType::eAlways },
        { true, DepthWriteMaskType::eAll, ComparisonFuncType::eLess }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const DepthStencilState& _DepthStencilState)const noexcept { return Data == _DepthStencilState.Data; }

    [[nodiscard]] constexpr bool operator!=(const DepthStencilState& _DepthStencilState)const noexcept { return Data != _DepthStencilState.Data; }
  };
  static_assert(sizeof(DepthStencilState) == sizeof(DepthStencilState::DataType));
}
