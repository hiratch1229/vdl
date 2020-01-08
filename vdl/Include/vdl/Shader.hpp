#pragma once
#include "Fwd.hpp"
#include "Resource.hpp"

namespace vdl
{
  class VertexShader : public Resource
  {
  public:
    VertexShader() = default;

    VertexShader(const char* _FilePath, InputLayoutType _InputLayout, const char* _EntryPoint = "main");

    VertexShader(const char* _Source, uint _DataSize, InputLayoutType _InputLayout, const char* _EntryPoint = "main");

    VertexShader(const VertexShader& _VertexShader);

    VertexShader(VertexShader&& _VertexShader)noexcept;

    ~VertexShader();
  public:
    VertexShader& operator=(const VertexShader& _VertexShader);

    VertexShader& operator=(VertexShader&& _VertexShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const VertexShader& _VertexShader)const noexcept { return ID_ == _VertexShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const VertexShader& _VertexShader)const noexcept { return ID_ != _VertexShader.ID_; }
  public:
    [[nodiscard]] InputLayoutType GetInputLayout()const;
  };

  class HullShader : public Resource
  {
  public:
    HullShader() = default;

    HullShader(const char* _FilePath, const char* _EntryPoint = "main");

    HullShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    HullShader(const HullShader& _HullShader);

    HullShader(HullShader&& _HullShader)noexcept;

    ~HullShader();
  public:
    HullShader& operator=(const HullShader& _HullShader);

    HullShader& operator=(HullShader&& _HullShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const HullShader& _HullShader)const noexcept { return ID_ == _HullShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const HullShader& _HullShader)const noexcept { return ID_ != _HullShader.ID_; }
  };

  class DomainShader : public Resource
  {
  public:
    DomainShader() = default;

    DomainShader(const char* _FilePath, const char* _EntryPoint = "main");

    DomainShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    DomainShader(const DomainShader& _DomainShader);

    DomainShader(DomainShader&& _DomainShader)noexcept;

    ~DomainShader();
  public:
    DomainShader& operator=(const DomainShader& _DomainShader);

    DomainShader& operator=(DomainShader&& _DomainShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const DomainShader& _DomainShader)const noexcept { return ID_ == _DomainShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const DomainShader& _DomainShader)const noexcept { return ID_ != _DomainShader.ID_; }
  };

  class GeometryShader : public Resource
  {
  public:
    GeometryShader() = default;

    GeometryShader(const char* _FilePath, const char* _EntryPoint = "main");

    GeometryShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    GeometryShader(const GeometryShader& _GeometryShader);

    GeometryShader(GeometryShader&& _GeometryShader)noexcept;

    ~GeometryShader();
  public:
    GeometryShader& operator=(const GeometryShader& _GeometryShader);

    GeometryShader& operator=(GeometryShader&& _GeometryShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const GeometryShader& _GeometryShader)const noexcept { return ID_ == _GeometryShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const GeometryShader& _GeometryShader)const noexcept { return ID_ != _GeometryShader.ID_; }
  };

  class PixelShader : public Resource
  {
  public:
    PixelShader() = default;

    PixelShader(const char* _FilePath, const char* _EntryPoint = "main");

    PixelShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    PixelShader(const PixelShader& _PixelShader);

    PixelShader(PixelShader&& _PixelShader)noexcept;

    ~PixelShader();
  public:
    PixelShader& operator=(const PixelShader& _PixelShader);

    PixelShader& operator=(PixelShader&& _PixelShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const PixelShader& _PixelShader)const noexcept { return ID_ == _PixelShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const PixelShader& _PixelShader)const noexcept { return ID_ != _PixelShader.ID_; }
  };

  class ComputeShader : public Resource
  {
  public:
    ComputeShader() = default;

    ComputeShader(const char* _FilePath, const char* _EntryPoint = "main");

    ComputeShader(const char* _Source, uint _DataSize, const char* _EntryPoint = "main");

    ComputeShader(const ComputeShader& _ComputeShader);

    ComputeShader(ComputeShader&& _ComputeShader)noexcept;

    ~ComputeShader();
  public:
    ComputeShader& operator=(const ComputeShader& _ComputeShader);

    ComputeShader& operator=(ComputeShader&& _ComputeShader)noexcept;

    [[nodiscard]] constexpr bool operator==(const ComputeShader& _ComputeShader)const noexcept { return ID_ == _ComputeShader.ID_; }

    [[nodiscard]] constexpr bool operator!=(const ComputeShader& _ComputeShader)const noexcept { return ID_ != _ComputeShader.ID_; }
  };

  struct GraphicsShaders
  {
    VertexShader VertexShader;
    HullShader HullShader;
    DomainShader DomainShader;
    GeometryShader GeometryShader;
    PixelShader PixelShader;
  public:
    [[nodiscard]] bool operator==(const GraphicsShaders& _GraphicsShaders)const noexcept {
      return VertexShader == _GraphicsShaders.VertexShader && HullShader == _GraphicsShaders.HullShader
        && DomainShader == _GraphicsShaders.DomainShader && GeometryShader == _GraphicsShaders.GeometryShader && PixelShader == _GraphicsShaders.PixelShader;
    }

    [[nodiscard]] bool operator!=(const GraphicsShaders& _GraphicsShaders)const noexcept {
      return VertexShader != _GraphicsShaders.VertexShader || HullShader != _GraphicsShaders.HullShader
        || DomainShader != _GraphicsShaders.DomainShader || GeometryShader != _GraphicsShaders.GeometryShader || PixelShader != _GraphicsShaders.PixelShader;
    }
  };
}
