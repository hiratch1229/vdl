#include <vdl/Renderer.hpp>
#include <vdl/RendererTexure.hpp>
#include <vdl/RendererStaticMesh.hpp>
#include <vdl/RendererSkinnedMesh.hpp>

#include <vdl/Engine.hpp>
#include <vdl/Renderer/IRenderer.hpp>

namespace vdl
{
  namespace Renderer
  {
    void SetScissor(const Scissor& _Scissor)
    {
      Engine::Get<IRenderer>()->SetScissor(_Scissor);
    }

    void SetViewport(const Viewport& _Viewport)
    {
      Engine::Get<IRenderer>()->SetViewport(_Viewport);
    }

    void SetRenderTextures(const RenderTexture& _RenderTexture, const DepthStencilTexture& _DepthStencilTexture)
    {
      Engine::Get<IRenderer>()->SetRenderTextures(_RenderTexture, _DepthStencilTexture);
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

  namespace RendererTexture
  {
    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, RenderType::eSprite);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, RenderType::eSprite);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, RenderType::eSprite);
    }

    void Draw(const Texture& _Texture, const float2& _DstLeftTop, const float2& _DstSize, const float2& _SrcLeftPos, const float2& _SrcSize, const Degree& _Angle, const ColorF& _Color)
    {
      Engine::Get<IRenderer>()->Draw(_Texture, _DstLeftTop, _DstSize, _SrcLeftPos, _SrcSize, _Angle, _Color);
    }
  }

  namespace RendererStaticMesh
  {
    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, RenderType::eStaticMesh);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, RenderType::eStaticMesh);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, RenderType::eStaticMesh);
    }

    void Draw(const StaticMesh& _StaticMesh, const Matrix& _World, const ColorF& _Color)
    {
      Engine::Get<IRenderer>()->Draw(_StaticMesh, _World, _Color);
    }
  }

  namespace RendererSkinnedMesh
  {
    void SetBlendState(const BlendState& _BlendState)
    {
      Engine::Get<IRenderer>()->SetBlendState(_BlendState, RenderType::eSkinnedMesh);
    }

    void SetDepthStencilState(const DepthStencilState& _DepthStencilState)
    {
      Engine::Get<IRenderer>()->SetDepthStencilState(_DepthStencilState, RenderType::eSkinnedMesh);
    }

    void SetRasterizerState(const RasterizerState& _RasterizerState)
    {
      Engine::Get<IRenderer>()->SetRasterizerState(_RasterizerState, RenderType::eSkinnedMesh);
    }

    void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionData& _MotionData, const ColorF& _Color)
    {
      Engine::Get<IRenderer>()->Draw(_SkinnedMesh, _World, { _MotionData }, _Color);
    }

    void Draw(const SkinnedMesh& _SkinnedMesh, const Matrix& _World, const MotionBlendDatas& _MotionBlendDatas, const ColorF& _Color)
    {
      Engine::Get<IRenderer>()->Draw(_SkinnedMesh, _World, _MotionBlendDatas, _Color);
    }
  }
}
