#include "SceneDeferred.hpp"

using namespace vdl;

void SceneDeferred::Initialize()
{
  //Model_ = Model("Data/sibenik/sibenik.obj");
  Model_ = Model("Data/sponza/sponza.obj");
  Camera_ = Camera(float3(0.0f, 0.0f, -15.0f));
}

SceneDeferred::~SceneDeferred()
{

}

void SceneDeferred::Update()
{
  FreeCamera(&Camera_);

  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(Model_, Matrix::Identity());
}
