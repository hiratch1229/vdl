#include "ScenePostEffect.hpp"

using namespace vdl;

void ScenePostEffect::Initialize()
{
  House_ = Model("Data/house_obj/house_obj.obj");
  Ground_ = MeshData::Rectangle("Data/GRASSX1/GRASSX1.jpg");

  Camera_ = Camera(float3(0.0f, 5.0f, -15.0f));

}

ScenePostEffect::~ScenePostEffect()
{

}

void ScenePostEffect::Update()
{
  FreeCamera(&Camera_);
  Renderer3D::SetCamera(Camera_);

  Renderer3D::Draw(House_, Matrix::Identity());

  Renderer3D::Draw(Ground_, Matrix::Scale(18.0f) * Matrix::Rotate(Math::ToRadian(90.0f), 0.0f, 0.0f));
}
