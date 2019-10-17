#pragma once
#include "Scene.hpp"

class SceneGPUParticle : public IScene
{
  static constexpr vdl::uint kBufferNum = 2500000;
  static constexpr vdl::uint kThreadNum = 1024;
  static constexpr vdl::uint kThreadGroupNum = kBufferNum / kThreadNum + (kBufferNum % kThreadNum == 0 ? 0 : 1);
private:
  struct CSConstantBufferData
  {
    vdl::float2 MousePosition;
    float DeltaTime;
    float Unused;
  };
  using CSConstantBuffer = vdl::ConstantBuffer<CSConstantBufferData>;
  struct GSConstantBufferData
  {
    vdl::float2 PointHalfSize;
    vdl::float2 Unused;
  };
  using GSConstantBuffer = vdl::ConstantBuffer<GSConstantBufferData>;
  struct Particle
  {
    vdl::float2 Position;
    vdl::float2 Velocity;
    vdl::ColorF Color;
  };
  using UnorderedAccessBuffer = vdl::UnorderedAccessBuffer<Particle>;
private:
  CSConstantBuffer CSConstantBuffer_;
  GSConstantBuffer GSConstantBuffer_;
  UnorderedAccessBuffer UnorderedAccessBuffer_;
public:
  SceneGPUParticle() = default;

  ~SceneGPUParticle();

  void Initialize()override;

  void Update()override;

  SceneType GetType()const override { return SceneType::eGPUParticle; }
};
