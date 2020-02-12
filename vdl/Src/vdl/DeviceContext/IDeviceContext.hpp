#pragma once
#include <vdl/Fwd.hpp>
#include <vdl/Types.hpp>
#include <vdl/Matrix.hpp>

#include <vdl/Platform/Platform.hpp>
#include <vdl/Buffer/Buffer.hpp>

class BaseGraphicsCommandList;
class ComputeCommandList;

class IDeviceContext
{
public:
  static IDeviceContext* Create();

  IDeviceContext() = default;

  virtual ~IDeviceContext() = default;

  [[nodiscard]] virtual PlatformFlags GetPlatform()const = 0;

  virtual void Initialize() = 0;

  virtual void SetRenderTextures(const vdl::RenderTextures& _RenderTextures, const vdl::DepthStencilTexture& _DepthStencilTexture) = 0;

  virtual void ClearRenderTexture(const vdl::RenderTexture& _RenderTexture, const vdl::Color4F& _ClearColor) = 0;

  virtual void ClearDepthStencilTexture(const vdl::DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, vdl::uint _ClearStencil) = 0;

  virtual void ClearUnorderedAccessTexture(const vdl::UnorderedAccessTexture& _UnorderedAccessTexture, const vdl::Color4F& _ClearColor) = 0;

  virtual void Execute(const BaseGraphicsCommandList& _GraphicsCommandList) = 0;

  virtual void Execute(const ComputeCommandList& _ComputeCommandList) = 0;

  virtual void Flush() = 0;

  virtual void Present() = 0;

  virtual void ChangeWindowMode() = 0;

  virtual void ScreenShot() = 0;

  virtual const vdl::RenderTexture& GetRenderTexture()const = 0;

  virtual const vdl::DepthStencilTexture& GetDepthStencilTexture()const = 0;
};
