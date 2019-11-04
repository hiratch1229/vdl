#include <vdl/Texture.hpp>

#include <vdl/Engine.hpp>
#include <vdl/TextureManager/ITextureManager.hpp>

#include <vdl/Image.hpp>

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

  Texture::Texture(Texture&& _Texture)noexcept
  {
    ID_ = _Texture.ID_;
    _Texture.ID_ = std::nullopt;
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

  Texture& Texture::operator=(Texture&& _Texture)noexcept
  {
    if (ID_)
    {
      Engine::Get<ITextureManager>()->Release(ID_);
    }

    ID_ = _Texture.ID_;
    _Texture.ID_ = std::nullopt;

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
    assert(!isEmpty());

    return Engine::Get<ITextureManager>()->GetTexture(ID_)->GetSize();
  }

  //--------------------------------------------------

  RenderTexture::RenderTexture(const uint2& _TextureSize, FormatType _Format)
  {
    ID_ = Engine::Get<ITextureManager>()->CreateRenderTexture(_TextureSize, _Format);
  }

  FormatType RenderTexture::GetFormat()const
  {
    assert(!isEmpty());

    return Engine::Get<ITextureManager>()->GetTexture(ID_)->GetFormat();
  }

  //--------------------------------------------------

  DepthStencilTexture::DepthStencilTexture(const uint2& _TextureSize, FormatType _Format)
  {
    ID_ = Engine::Get<ITextureManager>()->CreateDepthStencilTexture(_TextureSize, _Format);
  }

  DepthStencilTexture::DepthStencilTexture(const DepthStencilTexture& _DepthStencilTexture)
  {
    if (ID_ = _DepthStencilTexture.ID_)
    {
      Engine::Get<ITextureManager>()->AddRef(ID_);
    }
  }

  DepthStencilTexture::DepthStencilTexture(DepthStencilTexture&& _DepthStencilTexture)noexcept
  {
    ID_ = _DepthStencilTexture.ID_;
    _DepthStencilTexture.ID_ = std::nullopt;
  }

  DepthStencilTexture& DepthStencilTexture::operator=(const DepthStencilTexture& _DepthStencilTexture)
  {
    if (ID_ != _DepthStencilTexture.ID_)
    {
      if (ID_)
      {
        Engine::Get<ITextureManager>()->Release(ID_);
      }
      if (ID_ = _DepthStencilTexture.ID_)
      {
        Engine::Get<ITextureManager>()->AddRef(ID_);
      }
    }

    return *this;
  }

  DepthStencilTexture& DepthStencilTexture::operator=(DepthStencilTexture&& _DepthStencilTexture)noexcept
  {
    if (ID_)
    {
      Engine::Get<ITextureManager>()->Release(ID_);
    }

    ID_ = _DepthStencilTexture.ID_;
    _DepthStencilTexture.ID_ = std::nullopt;

    return *this;
  }

  DepthStencilTexture::~DepthStencilTexture()
  {
    if (ID_)
    {
      Engine::Get<ITextureManager>()->Release(ID_);
    }
  }

  FormatType DepthStencilTexture::GetFormat()const
  {
    assert(!isEmpty());

    return Engine::Get<ITextureManager>()->GetTexture(ID_)->GetFormat();
  }

  vdl::Texture DepthStencilTexture::GetDepthTexture()const
  {
    assert(!isEmpty());

    return static_cast<IDepthStencilTexture*>(Engine::Get<ITextureManager>()->GetTexture(ID_))->GetDepthTexture();
  }

  vdl::Texture DepthStencilTexture::GetStencilTexture()const
  {
    assert(!isEmpty());

    return static_cast<IDepthStencilTexture*>(Engine::Get<ITextureManager>()->GetTexture(ID_))->GetStencilTexture();
  }

  //--------------------------------------------------

  UnorderedAccessTexture::UnorderedAccessTexture(const uint2& _TextureSize, FormatType _Format)
  {
    ID_ = Engine::Get<ITextureManager>()->CreateUnorderedAccessTexture(_TextureSize, _Format);
  }

  FormatType UnorderedAccessTexture::GetFormat()const
  {
    assert(!isEmpty());

    return Engine::Get<ITextureManager>()->GetTexture(ID_)->GetFormat();
  }
}
