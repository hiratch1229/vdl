#include "CRenderer.hpp"

#include <vdl/Engine.hpp>
#include <vdl/Device/IDevice.hpp>
#include <vdl/DeviceContext/IDeviceContext.hpp>

#include <vdl/Constants/Constants.hpp>

void CRenderer::Initialize()
{
  IDevice* pDevice = Engine::Get<IDevice>();
  pDeviceContext_ = Engine::Get<IDeviceContext>();

  IBuffer* pInstanceBuffer2D = pInstanceBuffer2D_.get();
  pDevice->CreateInstanceBuffer(&pInstanceBuffer2D, sizeof(Instance2D), sizeof(Instance2D) * Constants::kMax2DBatchNum);

  IBuffer* pInstanceBuffer3D = pInstanceBuffer3D_.get();
  pDevice->CreateInstanceBuffer(&pInstanceBuffer3D, sizeof(Instance3D), sizeof(Instance3D) * Constants::kMax3DBatchNum);
}

void CRenderer::Draw(const vdl::Texture& _Texture, const vdl::float2& _DstLeftTop, const vdl::float2& _DstSize, const vdl::float2& _SrcLeftPos, const vdl::float2& _SrcSize, const vdl::Radian& _Angle, const vdl::ColorF& _Color)
{

}

void CRenderer::Draw(const vdl::StaticMesh& _StaticMesh, const vdl::Matrix& _World, const vdl::ColorF& _Color)
{

}

void CRenderer::Draw(const vdl::SkinnedMesh& _SkinnedMesh, const vdl::Matrix& _World, const std::vector<vdl::MotionBlendData>& _MotionBlendDatas, const vdl::ColorF& _Color)
{

}

void CRenderer::Flush()
{

}

//--------------------------------------------------

void CRenderer::FlushSprite()
{

}

void CRenderer::FlushStaticMesh()
{

}

void CRenderer::FlushSkinnedMesh()
{

}
