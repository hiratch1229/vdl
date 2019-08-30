#include "IRenderer.hpp"

#include "CRenderer.hpp"

IRenderer* IRenderer::Create()
{
  return new CRenderer;
}
