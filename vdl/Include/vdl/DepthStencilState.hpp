#pragma once
#include "Fwd.hpp"

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

  struct DepthStencilOpState
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
        StencilOpType StencilFailOp : 3;
        StencilOpType StencilDepthFailOp : 3;
        StencilOpType StencilPassOp : 3;
        ComparisonFuncType StencilFunc : 3;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    DepthStencilOpState() = default;

    constexpr DepthStencilOpState(StencilOpType _StencilFailOp, StencilOpType _StencilDepthFailOp, StencilOpType _StencilPassOp, ComparisonFuncType _StencilFunc)noexcept
      : StencilFailOp(_StencilFailOp), StencilDepthFailOp(_StencilDepthFailOp), StencilPassOp(_StencilPassOp), StencilFunc(_StencilFunc) {}

    DepthStencilOpState(PreDefined _PreDefined)noexcept
    {
      static constexpr DepthStencilOpState PreDefineds[static_cast<uint>(PreDefined::eNum)] =
      {
        { StencilOpType::eKeep, StencilOpType::eIncr, StencilOpType::eKeep, ComparisonFuncType::eAlways },
        { StencilOpType::eKeep, StencilOpType::eDecr, StencilOpType::eKeep, ComparisonFuncType::eAlways }
      };

      *this = PreDefineds[static_cast<uint>(_PreDefined)];
    }
  public:
    [[nodiscard]] constexpr bool operator==(const DepthStencilOpState& _DepthStencilOpState)const noexcept { return Data == _DepthStencilOpState.Data; }

    [[nodiscard]] constexpr bool operator!=(const DepthStencilOpState& _DepthStencilOpState)const noexcept { return Data != _DepthStencilOpState.Data;; }
  };
  static_assert(sizeof(DepthStencilOpState) == sizeof(DepthStencilOpState::DataType));

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
        uint8_t StencilReadMask;
        uint8_t StencilWriteMask;
        DepthStencilOpState FrontFace;
        DepthStencilOpState BackFace;
      };
      DataType Data;
    };
#pragma warning(default:4201)
  public:
    constexpr DepthStencilState(bool _DepthEnable = true, DepthWriteMaskType _DepthWriteMask = DepthWriteMaskType::eAll, ComparisonFuncType _DepthFunc = ComparisonFuncType::eLess,
      bool _StencilEnable = false, uint8_t _StencilReadMask = 0xFF, uint8_t _StencilWriteMask = 0xFF,
      const DepthStencilOpState & _FrontFace = DepthStencilOpState::kDefaultFront, const DepthStencilOpState & _BackFace = DepthStencilOpState::kDefaultBack)noexcept
      : DepthEnable(_DepthEnable), DepthWriteMask(_DepthWriteMask), DepthFunc(_DepthFunc),
      StencilEnable(_StencilEnable), StencilReadMask(_StencilReadMask), StencilWriteMask(_StencilWriteMask),
      FrontFace(_FrontFace), BackFace(_BackFace) {}

    DepthStencilState(PreDefined _PreDefined)noexcept
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
