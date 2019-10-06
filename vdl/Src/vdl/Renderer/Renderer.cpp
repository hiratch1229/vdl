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

      //  �e�N�X�`�����ݒ肳��Ă��Ȃ��Ȃ�I��
      if (RenderTexture.isEmpty())
      {
        return true;
      }

      //  �T�C�Y���ŏ��̃����_�[�^�[�Q�b�g�ƈႤ�Ȃ�G���[
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
      Engine::Get<IRenderer>()->SetTopology(_Topology, RenderType::eNone);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor, RenderType::eNone);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport, RenderType::eNone);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, RenderType::eNone);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, RenderType::eNone);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, RenderType::eNone);
    }

    void SetVertexShader(const VertexShader& _VertexShader)
    {
      assert(!_VertexShader.isEmpty());

      Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, RenderType::eNone);
    }

    void SetHullShader(const HullShader& _HullShader)
    {
      Engine::Get<IRenderer>()->SetHullShader(_HullShader, RenderType::eNone);
    }

    void SetDomainShader(const DomainShader& _DomainShader)
    {
      Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, RenderType::eNone);
    }

    void SetGeometryShader(const GeometryShader& _GeometryShader)
    {
      Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, RenderType::eNone);
    }

    void SetPixelShader(const PixelShader& _PixelShader)
    {
      assert(!_PixelShader.isEmpty());

      Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, RenderType::eNone);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, RenderType::eNone);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, RenderType::eNone);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, RenderType::eNone);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, RenderType::eNone);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, RenderType::eNone);
    }

    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, RenderType::eNone);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, RenderType::eNone);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, RenderType::eNone);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, RenderType::eNone);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      if (_SamplerNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, RenderType::eNone);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, RenderType::eNone);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, RenderType::eNone);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, RenderType::eNone);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, RenderType::eNone);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      if (_BufferNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, RenderType::eNone);
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
      Engine::Get<IRenderer>()->SetTopology(_Topology, RenderType::eTexture);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor, RenderType::eTexture);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport, RenderType::eTexture);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, RenderType::eTexture);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, RenderType::eTexture);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, RenderType::eTexture);
    }

    void SetVertexShader(const VertexShader& _VertexShader)
    {
      assert(!_VertexShader.isEmpty());

      Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, RenderType::eTexture);
    }

    void SetHullShader(const HullShader& _HullShader)
    {
      Engine::Get<IRenderer>()->SetHullShader(_HullShader, RenderType::eTexture);
    }

    void SetDomainShader(const DomainShader& _DomainShader)
    {
      Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, RenderType::eTexture);
    }

    void SetGeometryShader(const GeometryShader& _GeometryShader)
    {
      Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, RenderType::eTexture);
    }

    void SetPixelShader(const PixelShader& _PixelShader)
    {
      assert(!_PixelShader.isEmpty());

      Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, RenderType::eTexture);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, RenderType::eTexture);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, RenderType::eTexture);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, RenderType::eTexture);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, RenderType::eTexture);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, RenderType::eTexture);
    }
       
    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, RenderType::eTexture);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, RenderType::eTexture);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, RenderType::eTexture);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, RenderType::eTexture);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, RenderType::eTexture);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, RenderType::eTexture);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, RenderType::eTexture);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, RenderType::eTexture);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, RenderType::eTexture);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, RenderType::eTexture);
    }

    void Draw(const Texture& _Texture, const float2& _DestLeftTop, const float2& _DestSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color)
    {
      assert(!_Texture.isEmpty());

      Engine::Get<IRenderer>()->Draw(_Texture, _DestLeftTop, _DestSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
    }
  }

  namespace Renderer3D
  {
    void SetCamera(const Camera& _Camera)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetCamera(_Camera, RenderType::eStaticMesh);
      pRenderer->SetCamera(_Camera, RenderType::eSkinnedMesh);
    }

    void SetScissor(const Scissor& _Scissor)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetScissor(_Scissor, RenderType::eStaticMesh);
      pRenderer->SetScissor(_Scissor, RenderType::eSkinnedMesh);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetViewport(_Viewport, RenderType::eStaticMesh);
      pRenderer->SetViewport(_Viewport, RenderType::eSkinnedMesh);
    }

    void SetBlendState(const BlendState& _BlendState)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetBlendState(_BlendState, RenderType::eStaticMesh);
      pRenderer->SetBlendState(_BlendState, RenderType::eSkinnedMesh);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetDepthStencilState(_DepthStencilState, RenderType::eStaticMesh);
      pRenderer->SetDepthStencilState(_DepthStencilState, RenderType::eSkinnedMesh);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetRasterizerState(_RasterizerState, RenderType::eStaticMesh);
      pRenderer->SetRasterizerState(_RasterizerState, RenderType::eSkinnedMesh);
    }

    void SetVertexStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, RenderType::eTexture);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, RenderType::eStaticMesh);
      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eVertexShader, RenderType::eSkinnedMesh);
    }

    void SetHullStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, RenderType::eTexture);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, RenderType::eStaticMesh);
      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eHullShader, RenderType::eSkinnedMesh);
    }

    void SetDomainStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, RenderType::eTexture);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, RenderType::eStaticMesh);
      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eDomainShader, RenderType::eSkinnedMesh);
    }

    void SetGeometryStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, RenderType::eTexture);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, RenderType::eStaticMesh);
      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::eGeometryShader, RenderType::eSkinnedMesh);
    }

    void SetPixelStageShaderResources(uint _StartSlot, uint _ShaderResourceNum, const ShaderResource _ShaderResources[])
    {
      assert(_StartSlot + _ShaderResourceNum - 1 < Constants::kMaxShaderResourceNum);

      if (_ShaderResourceNum == 0)
      {
        return;
      }

      Engine::Get<IRenderer>()->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, RenderType::eTexture);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, RenderType::eStaticMesh);
      pRenderer->SetShaderResources(_StartSlot, _ShaderResourceNum, _ShaderResources, ShaderType::ePixelShader, RenderType::eSkinnedMesh);
    }

    void SetVertexStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, RenderType::eStaticMesh);
      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eVertexShader, RenderType::eSkinnedMesh);
    }

    void SetHullStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, RenderType::eStaticMesh);
      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eHullShader, RenderType::eSkinnedMesh);
    }

    void SetDomainStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eDomainShader, RenderType::eSkinnedMesh);
    }

    void SetGeometryStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, RenderType::eStaticMesh);
      Engine::Get<IRenderer>()->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::eGeometryShader, RenderType::eSkinnedMesh);
    }

    void SetPixelStageSamplers(uint _StartSlot, uint _SamplerNum, const Sampler _Sampler[])
    {
      assert(_StartSlot + _SamplerNum - 1 < Constants::kMaxSamplerNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, RenderType::eStaticMesh);
      pRenderer->SetSamplers(_StartSlot, _SamplerNum, _Sampler, ShaderType::ePixelShader, RenderType::eSkinnedMesh);
    }

    void SetVertexStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(0 < _StartSlot && _StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, RenderType::eStaticMesh);
      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eVertexShader, RenderType::eSkinnedMesh);
    }

    void SetHullStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, RenderType::eStaticMesh);
      Engine::Get<IRenderer>()->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eHullShader, RenderType::eSkinnedMesh);
    }

    void SetDomainStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, RenderType::eStaticMesh);
      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eDomainShader, RenderType::eSkinnedMesh);
    }

    void SetGeometryStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, RenderType::eStaticMesh);
      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::eGeometryShader, RenderType::eSkinnedMesh);
    }

    void SetPixelStageConstantBuffers(uint _StartSlot, uint _BufferNum, const Detail::ConstantBufferData _ConstantBuffers[])
    {
      assert(_StartSlot + _BufferNum - 1 < Constants::kMaxConstantBufferNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, RenderType::eStaticMesh);
      pRenderer->SetConstantBuffers(_StartSlot, _BufferNum, _ConstantBuffers, ShaderType::ePixelShader, RenderType::eSkinnedMesh);
    }

    void Draw(const StaticMesh& _StaticMesh, const Matrix& _World, const ColorF& _Color)
    {
      assert(!_StaticMesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_StaticMesh, _World, _Color);
    }

    void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color)
    {
      assert(!_SkinnedMesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_SkinnedMesh, _World, { _MotionData }, _Color);
    }

    void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionBlendDatas& _MotionBlendDatas, const ColorF& _Color)
    {
      assert(!_SkinnedMesh.isEmpty());

      Engine::Get<IRenderer>()->Draw(_SkinnedMesh, _World, _MotionBlendDatas, _Color);
    }

    namespace RendererStaticMesh
    {
      void SetTopology(TopologyType _Topology)
      {
        Engine::Get<IRenderer>()->SetTopology(_Topology, RenderType::eStaticMesh);
      }

      void SetVertexShader(const VertexShader& _VertexShader)
      {
        assert(!_VertexShader.isEmpty());

        Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, RenderType::eStaticMesh);
      }

      void SetHullShader(const HullShader& _HullShader)
      {
        Engine::Get<IRenderer>()->SetHullShader(_HullShader, RenderType::eStaticMesh);
      }

      void SetDomainShader(const DomainShader& _DomainShader)
      {
        Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, RenderType::eStaticMesh);
      }

      void SetGeometryShader(const GeometryShader& _GeometryShader)
      {
        Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, RenderType::eStaticMesh);
      }

      void SetPixelShader(const PixelShader& _PixelShader)
      {
        assert(!_PixelShader.isEmpty());

        Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, RenderType::eStaticMesh);
      }
    }

    namespace RendererSkinnedMesh
    {
      void SetTopology(TopologyType _Topology)
      {
        Engine::Get<IRenderer>()->SetTopology(_Topology, RenderType::eSkinnedMesh);
      }

      void SetVertexShader(const VertexShader& _VertexShader)
      {
        assert(!_VertexShader.isEmpty());

        Engine::Get<IRenderer>()->SetVertexShader(_VertexShader, RenderType::eSkinnedMesh);
      }

      void SetHullShader(const HullShader& _HullShader)
      {
        Engine::Get<IRenderer>()->SetHullShader(_HullShader, RenderType::eSkinnedMesh);
      }

      void SetDomainShader(const DomainShader& _DomainShader)
      {
        Engine::Get<IRenderer>()->SetDomainShader(_DomainShader, RenderType::eSkinnedMesh);
      }

      void SetGeometryShader(const GeometryShader& _GeometryShader)
      {
        Engine::Get<IRenderer>()->SetGeometryShader(_GeometryShader, RenderType::eSkinnedMesh);
      }

      void SetPixelShader(const PixelShader& _PixelShader)
      {
        assert(!_PixelShader.isEmpty());

        Engine::Get<IRenderer>()->SetPixelShader(_PixelShader, RenderType::eSkinnedMesh);
      }
    }
  }
}
