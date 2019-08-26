#pragma once
#include "ID.hpp"

namespace vdl
{
  class VertexShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const VertexShader& _VertexShader)const noexcept { return ID_ == _VertexShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const VertexShader& _VertexShader)const noexcept { return ID_ != _VertexShader.ID_; }
  public:
    VertexShader() = default;

    ~VertexShader();

    VertexShader(const char* _FilePath, const char* _EntryPoint);

    VertexShader& operator=(const VertexShader& _VertexShader);
  };

  class HullShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const HullShader& _HullShader)const noexcept { return ID_ == _HullShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const HullShader& _HullShader)const noexcept { return ID_ != _HullShader.ID_; }
  public:
    HullShader() = default;

    ~HullShader();

    HullShader(const char* _FilePath, const char* _EntryPoint);

    HullShader& operator=(const HullShader& _HullShader);
  };

  class DomainShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const DomainShader& _DomainShader)const noexcept { return ID_ == _DomainShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const DomainShader& _DomainShader)const noexcept { return ID_ != _DomainShader.ID_; }
  public:
    DomainShader() = default;

    ~DomainShader();

    DomainShader(const char* _FilePath, const char* _EntryPoint);

    DomainShader& operator=(const DomainShader& _DomainShader);
  };

  class GeometryShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const GeometryShader& _GeometryShader)const noexcept { return ID_ == _GeometryShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const GeometryShader& _GeometryShader)const noexcept { return ID_ != _GeometryShader.ID_; }
  public:
    GeometryShader() = default;

    ~GeometryShader();

    GeometryShader(const char* _FilePath, const char* _EntryPoint);

    GeometryShader& operator=(const GeometryShader& _GeometryShader);
  };

  class PixelShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const PixelShader& _PixelShader)const noexcept { return ID_ == _PixelShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const PixelShader& _PixelShader)const noexcept { return ID_ != _PixelShader.ID_; }
  public:
    PixelShader() = default;

    ~PixelShader();

    PixelShader(const char* _FilePath, const char* _EntryPoint);

    PixelShader& operator=(const PixelShader& _PixelShader);
  };

  class ComputeShader
  {
    ID ID_;
  public:
    [[nodiscard]] ID GetID()const noexcept { return ID_; }
  public:
    [[nodiscard]] constexpr bool operator==(const ComputeShader& _ComputeShader)const noexcept { return ID_ == _ComputeShader.ID_; }
    [[nodiscard]] constexpr bool operator!=(const ComputeShader& _ComputeShader)const noexcept { return ID_ != _ComputeShader.ID_; }
  public:
    ComputeShader() = default;

    ~ComputeShader();

    ComputeShader(const char* _FilePath, const char* _EntryPoint);

    ComputeShader& operator=(const ComputeShader& _ComputeShader);
  };
}
