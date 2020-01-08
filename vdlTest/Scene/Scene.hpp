#pragma once
#include <vdl.hpp>

namespace GUIHelper
{
  constexpr vdl::uint2 kInformationWindowPos = vdl::uint2(0, 0);
  constexpr vdl::uint2 kInformationWindowSize = vdl::uint2(225, 130);
  constexpr vdl::uint2 kSceneWindowSize = vdl::uint2(0, vdl::Constants::kDefaultWindowSize.y - kInformationWindowSize.y);
  constexpr vdl::uint2 kSceneWindowPos = vdl::uint2(0, kInformationWindowPos.y + kInformationWindowSize.y);
  constexpr vdl::uint2 kGBufferDisplaySize = vdl::Constants::kDefaultWindowSize / 5;
  constexpr vdl::uint2 kRenderingFlowWindowSize = vdl::uint2(300, vdl::Constants::kDefaultWindowSize.y);
  constexpr vdl::uint2 kRenderingFlowWindowPos = vdl::uint2(vdl::Constants::kDefaultWindowSize.x - kRenderingFlowWindowSize.x, 0);

  inline void DrawRenderTexture(const char* _Label, const vdl::RenderTexture& _Texture, const vdl::float2& _DisplaySize)
  {
    if (vdl::GUI::CollapsingHeader(_Label))
    {
      vdl::GUI::Text(std::string("Format:" + std::to_string(_Texture.GetFormat())).c_str());
      vdl::GUI::Text(std::string("TextureSize:" + std::to_string(_Texture.GetSize())).c_str());
      vdl::GUI::Image(_Texture, _DisplaySize);
    }
  }

  inline void DrawDepthTexture(const char* _Label, const vdl::DepthStencilTexture& _Texture, const vdl::float2& _DisplaySize)
  {
    if (vdl::GUI::CollapsingHeader(_Label))
    {
      vdl::GUI::Text(std::string("Format:" + std::to_string(_Texture.GetFormat())).c_str());
      const vdl::Texture& DepthTexture = _Texture.GetDepthTexture();
      vdl::GUI::Text(std::string("TextureSize:" + std::to_string(DepthTexture.GetSize())).c_str());
      vdl::GUI::Image(DepthTexture, _DisplaySize);
    }
  }

  inline void DrawStencilTexture(const char* _Label, const vdl::DepthStencilTexture& _Texture, const vdl::float2& _DisplaySize)
  {
    if (vdl::GUI::CollapsingHeader(_Label))
    {
      vdl::GUI::Text(std::string("Format:" + std::to_string(_Texture.GetFormat())).c_str());
      const vdl::Texture& StencilTexture = _Texture.GetStencilTexture();
      vdl::GUI::Text(std::string("TextureSize:" + std::to_string(StencilTexture.GetSize())).c_str());
      vdl::GUI::Image(StencilTexture, _DisplaySize);
    }
  }

  inline void DrawDepthStencilTexture(const char* _Label, const vdl::DepthStencilTexture& _Texture, const vdl::float2& _DisplaySize)
  {
    if (vdl::GUI::CollapsingHeader(_Label))
    {
      vdl::GUI::Text(std::string("Format:" + std::to_string(_Texture.GetFormat())).c_str());
      const vdl::Texture& DepthTexture = _Texture.GetDepthTexture();
      vdl::GUI::Text(std::string("TextureSize:" + std::to_string(DepthTexture.GetSize())).c_str());
      vdl::GUI::Image(DepthTexture, _DisplaySize);

      const vdl::Texture& StencilTexture = _Texture.GetStencilTexture();
      if (!StencilTexture.isEmpty())
      {
        vdl::GUI::Image(StencilTexture, _DisplaySize);
      }
    }
  }

  inline void DrawUnorderedAccessTexture(const char* _Label, const vdl::UnorderedAccessTexture& _Texture, const vdl::float2& _DisplaySize)
  {
    if (vdl::GUI::CollapsingHeader(_Label))
    {
      vdl::GUI::Text(std::string("Format:" + std::to_string(_Texture.GetFormat())).c_str());
      vdl::GUI::Text(std::string("TextureSize:" + std::to_string(_Texture.GetSize())).c_str());
      vdl::GUI::Image(_Texture, _DisplaySize);
    }
  }
}

class IScene
{
public:
  IScene() = default;

  virtual ~IScene() = default;

  virtual void Initialize() = 0;

  virtual void Update() = 0;
};
