#include <vdl/Texture.hpp>

#include <vdl/Engine.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

namespace vdl
{
  Texture::Texture(const char* _FilePath, bool _isSerialize)
  {
    ID_ = Engine::Get<ITextureManager>()->Load(_FilePath, _isSerialize);
  }

  Texture::Texture(const Image& _Image)
  {
    ID_ = Engine::Get<ITextureManager>()->Load(_Image);
  }

  Texture::Texture(const Color& _Color)
  {
    Image Image;
    {
      Image.Resize(1);
      Image.Buffer()[0] = _Color;
    }

    ID_ = Engine::Get<ITextureManager>()->Load(Image);
  }

  Texture::Texture(const Texture& _Texture)
  {
    if (ID_ = _Texture.ID_)
    {
      Engine::Get<ITextureManager>()->AddRef(ID_);
    }
  }

  Texture& Texture::operator=(const Texture& _Texture)
  {
    if (ID_ != _Texture.ID_)
    {
      if (ID_)
      {
        Engine::Get<ITextureManager>()->Release(ID_);
      }
      if (ID_ = _Texture.ID_)
      {
        Engine::Get<ITextureManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  Texture::~Texture()
  {
    if (ID_)
    {
      Engine::Get<ITextureManager>()->Release(ID_);
    }
  }

  uint2 Texture::GetSize()const
  {
    return Engine::Get<ITextureManager>()->GetTexture(ID_)->GetSize();
  }

  //--------------------------------------------------

  RenderTexture::RenderTexture(const uint2& _TextureSize, Format _Format)
  {
    ID_ = Engine::Get<ITextureManager>()->CreateRenderTexture(_TextureSize, _Format);
  }

  //--------------------------------------------------

  DepthStencilTexture::DepthStencilTexture(const uint2& _TextureSize, Format _Format)
  {
    ID_ = Engine::Get<ITextureManager>()->CreateDepthStencilTexture(_TextureSize, _Format);
  }
}