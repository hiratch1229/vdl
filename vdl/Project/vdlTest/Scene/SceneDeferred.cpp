#include "SceneDeferred.hpp"

using namespace vdl;

namespace
{
  float QuadraticFunction(float _Time, float _TotalTime, float _Min, float _Max)
  {
    return (1.0f - (_Time / (_TotalTime / 2.0f) - 1.0f) * (_Time / (_TotalTime / 2.0f) - 1.0f)) * (_Max - _Min) + _Min;
  }
}

void SceneDeferred::Initialize()
{
  Rectangle_ = MeshData::Rectangle("Data/test1_albedo.png", "Data/test1_normal.png");
  Sphere_ = MeshData::Sphere(12, 12, "Data/earthmap.jpg", "Data/earthnormal.jpg");
  Camera_ = Camera(float3(0.0f, 5.0f, -15.0f));

  for (auto& Data : Datas_)
  {
    Data.Color = ColorF(Random::Range(-kRectangleHalfScale, kRectangleHalfScale), Random::Range(-kRectangleHalfScale, kRectangleHalfScale), Random::Range(-kRectangleHalfScale, kRectangleHalfScale));
    Data.MinRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MinRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.x = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.MaxRange.z = Random::Range(-kRectangleHalfScale, kRectangleHalfScale);
    Data.Timer = 0.0f;
    Data.Time = Random::Range(5.0f, 15.0f);
  }
}

SceneDeferred::~SceneDeferred()
{

}

void SceneDeferred::Update()
{
  FreeCamera(&Camera_);

  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(Rectangle_, Matrix::Scale(kRectangleScale) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f));

  const float DeltaTime = System::GetDeltaTime();
  for (auto& Data : Datas_)
  {
    Data.Timer += DeltaTime;
    while (Data.Timer > Data.Time)
    {
      Data.Timer -= Data.Time;
    }

    Renderer3D::Draw(Sphere_, Matrix::Translate(float3(QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.x, Data.MaxRange.x), 0.5f, QuadraticFunction(Data.Timer, Data.Time, Data.MinRange.z, Data.MaxRange.z))));
  }
}
