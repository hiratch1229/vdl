#include <vdl/Renderer.hpp>
#include <vdl/Renderer2D.hpp>
#include <vdl/Renderer3D.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Renderer/IRenderer.hpp>

#include <vdl/Window.hpp>

#include <assert.h>

namespace
{
  inline bool CheckSupportMultiRenderTextures(const vdl::RenderTextures& _RenderTextures)
  {
    const vdl::uint2 TexureSize = (_RenderTextures[0].isEmpty() ? vdl::Window::GetWindowSize() : _RenderTextures[0].GetSize());

    for (size_t RenderTextureCount = 1; RenderTextureCount < vdl::Constants::kMaxRenderTextureNum; ++RenderTextureCount)
    {
      const vdl::RenderTexture& RenderTexture = _RenderTextures[RenderTextureCount];

      //  テクスチャが設定されていないなら終了
      if (RenderTexture.isEmpty())
      {
        return true;
      }

      //  サイズが最初のレンダーターゲットと違うならエラー
      if (RenderTexture.GetSize() != TexureSize)
      {
        return false;
      }
    }

    return true;
  }
}

namespace vdl
{
  namespace Renderer
  {
    void SetRenderTextures(const RenderTextures& _RenderTextures, const DepthStencilTexture& _DepthStencilTexture)
    {
      assert(CheckSupportMultiRenderTextures(_RenderTextures));

      Engine::Get<IRenderer>()->SetRenderTextures(_RenderTextures, _DepthStencilTexture);
    }

    void Clear(const RenderTexture& _RenderTexure, const ColorF& _ClearColor)
    {
      Engine::Get<IRenderer>()->Clear(_RenderTexure, _ClearColor);
    }

    void Clear(const DepthStencilTexture& _DepthStencilTexture, float _ClearDepth, uint _ClearStencil)
    {
      Engine::Get<IRenderer>()->Clear(_DepthStencilTexture, _ClearDepth, _ClearStencil);
    }

    void Clear(const UnorderedAccessTexture& _UnorderedAccessTexture, const ColorF& _ClearColor)
    {
      assert(!_UnorderedAccessTexture.isEmpty());

      Engine::Get<IRenderer>()->Clear(_UnorderedAccessTexture, _ClearColor);
    }

    void SetTopology(TopologyType _Topology)
    {
      Engine::Get<IRenderer>()->SetTopology(_Topology, InputLayoutType::eNone);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor, InputLayoutType::eNone);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport, InputLayoutType::eNone);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, InputLayoutType::eNone);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, InputLayoutType::eNone);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, InputLayoutType::eNone);
    }

    void SetVertexShader(const VertexShader& _VertexShader)
    {
      assert(!_VertexShader.isEmpty() && _VertexShader.GetInputLayout() == InputLayoutType::eNone);

      Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, InputLayoutType::eNone);
    }

    void SetHullShader(const HullShader& _HullShader)
    {
      Engine::Get<IRenderer>()->SetHullShader(_HullShader, InputLayoutType::eNone);
    }

    void SetDomainShader(const DomainShader& _DomainShader)
    {
      Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, InputLayoutType::eNone);
    }

    void SetGeometryShader(const GeometryShader& _GeometryShader)
    {
      Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, InputLayoutType::eNone);
    }

    void SetPixelShader(const PixelShader& _PixelShader)
    {
      assert(!_PixelShader.isEmpty());

      Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, InputLayoutType::eNone);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, InputLayoutType::eNone);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, InputLayoutType::eNone);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, InputLayoutType::eNone);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, InputLayoutType::eNone);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, InputLayoutType::eNone);
    }

    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, InputLayoutType::eNone);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, InputLayoutType::eNone);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, InputLayoutType::eNone);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, InputLayoutType::eNone);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, InputLayoutType::eNone);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, InputLayoutType::eNone);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, InputLayoutType::eNone);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, InputLayoutType::eNone);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, InputLayoutType::eNone);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, InputLayoutType::eNone);
    }

    void Draw(uint _VertexCount)
    {
      Engine::Get<IRenderer>()->Draw(_VertexCount);
    }
  }

  namespace Renderer2D
  {
    void SetTopology(TopologyType _Topology)
    {
      Engine::Get<IRenderer>()->SetTopology(_Topology, InputLayoutType::eTexture);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor, InputLayoutType::eTexture);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport, InputLayoutType::eTexture);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, InputLayoutType::eTexture);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, InputLayoutType::eTexture);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, InputLayoutType::eTexture);
    }

    void SetVertexShader(const VertexShader& _VertexShader)
    {
      assert(!_VertexShader.isEmpty() && _VertexShader.GetInputLayout() == InputLayoutType::eTexture);

      Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, InputLayoutType::eTexture);
    }

    void SetHullShader(const HullShader& _HullShader)
    {
      Engine::Get<IRenderer>()->SetHullShader(_HullShader, InputLayoutType::eTexture);
    }

    void SetDomainShader(const DomainShader& _DomainShader)
    {
      Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, InputLayoutType::eTexture);
    }

    void SetGeometryShader(const GeometryShader& _GeometryShader)
    {
      Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, InputLayoutType::eTexture);
    }

    void SetPixelShader(const PixelShader& _PixelShader)
    {
      assert(!_PixelShader.isEmpty());

      Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, InputLayoutType::eTexture);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, InputLayoutType::eTexture);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, InputLayoutType::eTexture);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, InputLayoutType::eTexture);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, InputLayoutType::eTexture);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, InputLayoutType::eTexture);
    }

    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, InputLayoutType::eTexture);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, InputLayoutType::eTexture);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, InputLayoutType::eTexture);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, InputLayoutType::eTexture);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, InputLayoutType::eTexture);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, InputLayoutType::eTexture);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, InputLayoutType::eTexture);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, InputLayoutType::eTexture);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, InputLayoutType::eTexture);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, InputLayoutType::eTexture);
    }

    void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color)
    {
      assert(!_Texture.isEmpty());

      Engine::Get<IRenderer>()->Draw(_Texture, _DestLeftTop, _DestSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
    }
  }

  namespace Renderer3D
  {
    Matrix GetView()
    {
      return Engine::Get<IRenderer>()->GetView();
    }

    Matrix GetProjection()
    {
      return Engine::Get<IRenderer>()->GetProjection();
    }

    void SetCamera(const Camera& _Camera)
    {
      Engine::Get<IRenderer>()->SetCamera(_Camera);
    }

    void SetTopology(TopologyType _Topology)
    {
      Engine::Get<IRenderer>()->SetTopology(_Topology, InputLayoutType::eMesh);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor, InputLayoutType::eMesh);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport, InputLayoutType::eMesh);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, InputLayoutType::eMesh);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, InputLayoutType::eMesh);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, InputLayoutType::eMesh);
    }

    void SetVertexShader(const VertexShader& _VertexShader)
    {
      assert(!_VertexShader.isEmpty() && _VertexShader.GetInputLayout() == InputLayoutType::eMesh);

      Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, InputLayoutType::eMesh);
    }

    void SetHullShader(const HullShader& _HullShader)
    {
      Engine::Get<IRenderer>()->SetHullShader(_HullShader, InputLayoutType::eMesh);
    }

    void SetDomainShader(const DomainShader& _DomainShader)
    {
      Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, InputLayoutType::eMesh);
    }

    void SetGeometryShader(const GeometryShader& _GeometryShader)
    {
      Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, InputLayoutType::eMesh);
    }

    void SetPixelShader(const PixelShader& _PixelShader)
    {
      assert(!_PixelShader.isEmpty());

      Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, InputLayoutType::eMesh);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, InputLayoutType::eMesh);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, InputLayoutType::eMesh);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, InputLayoutType::eMesh);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, InputLayoutType::eMesh);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(1 < _StartSlot && _StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, InputLayoutType::eMesh);
    }

    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, InputLayoutType::eMesh);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, InputLayoutType::eMesh);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, InputLayoutType::eMesh);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, InputLayoutType::eMesh);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, InputLayoutType::eMesh);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(0 < _StartSlot && _StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, InputLayoutType::eMesh);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, InputLayoutType::eMesh);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, InputLayoutType::eMesh);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, InputLayoutType::eMesh);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, InputLayoutType::eMesh);
    }

    void Draw(const Mesh& _Mesh, const Matrix& _World, const ColorF& _Color)
    {
      assert(!_Mesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_Mesh, _World, {}, _Color);
    }

    void Draw(const Mesh& _Mesh, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color)
    {
      assert(!_Mesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_Mesh, _World, { _MotionData }, _Color);
    }

    void Draw(const Mesh& _Mesh, const Matrix& _World, const MotionBlendDatas& _MotionBlendDatas, const ColorF& _Color)
    {
      assert(!_Mesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_Mesh, _World, _MotionBlendDatas, _Color);
    }
  }
}
