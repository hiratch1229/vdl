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
  }

  namespace Renderer2D
  {
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

    void SetVertexStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      Engine::Get<IRenderer>()->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eVertexShader, RenderType::eTexture);
    }

    void SetHullStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      Engine::Get<IRenderer>()->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eHullShader, RenderType::eTexture);
    }

    void SetDomainStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      Engine::Get<IRenderer>()->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eDomainShader, RenderType::eTexture);
    }

    void SetGeometryStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      Engine::Get<IRenderer>()->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eGeometryShader, RenderType::eTexture);
    }

    void SetPixelStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(0 < _StartSlot && _StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      Engine::Get<IRenderer>()->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::ePixelShader, RenderType::eTexture);
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

  namespace RendererTexture
  {
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

    void SetVertexStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eVertexShader, RenderType::eStaticMesh);
      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eVertexShader, RenderType::eSkinnedMesh);
    }

    void SetHullStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eHullShader, RenderType::eStaticMesh);
      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eHullShader, RenderType::eSkinnedMesh);
    }

    void SetDomainStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eDomainShader, RenderType::eStaticMesh);
      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eDomainShader, RenderType::eSkinnedMesh);
    }

    void SetGeometryStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(_StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eGeometryShader, RenderType::eStaticMesh);
      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::eGeometryShader, RenderType::eSkinnedMesh);
    }

    void SetPixelStageTextures(uint _StartSlot, uint _TextureNum, const Texture _Textures[])
    {
      assert(0 < _StartSlot && _StartSlot + _TextureNum - 1 < Constants::kMaxTextureNum);

      IRenderer* pRenderer = Engine::Get<IRenderer>();

      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::ePixelShader, RenderType::eStaticMesh);
      pRenderer->SetTextures(_StartSlot, _TextureNum, _Textures, ShaderType::ePixelShader, RenderType::eSkinnedMesh);
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
  }

  namespace RendererStaticMesh
  {
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
