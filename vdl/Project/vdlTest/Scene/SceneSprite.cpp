#include "SceneSprite.hpp"

using namespace vdl;

void SceneSprite::Initialize()
{
  Texture_ = Texture("Data/SAO�w�i.png");
}

SceneSprite::~SceneSprite()
{

}

void SceneSprite::Update()
{
  int Size = static_cast<int>(Datas_.size());

  ImGui::Begin("Sprite");
  {
    //  ���l���ύX���ꂽ��
    if (ImGui::InputInt("Size", &Size))
    {
      if (Size < 0)
      {
        Size = static_cast<int>(Datas_.size());
      }
      else
      {
        Datas_.resize(Size);
      }
    }

    //  �c���[�\���őS�Ă̏����o��
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Once);
    for (int i = 0; i < Size; ++i)
    {
      Data& Data = Datas_[i];

      if (ImGui::TreeNode((std::string("Data[") + std::to_string(i) + "]").c_str()))
      {
        ImGui::InputFloat2("Pos", &Data.Pos.x);
        ImGui::InputFloat2("Size", &Data.Size.x);
        ImGui::InputFloat2("SrcPos", &Data.SrcPos.x);
        ImGui::InputFloat2("SrcSize", &Data.SrcSize.x);
        ImGui::InputFloat("Angle", reinterpret_cast<float*>(&Data.Angle));
        ImGui::ColorEdit4("Color", &Data.Color.Red);
        ImGui::TreePop();
      }
    }
  }
  ImGui::End();

  //  �`��
  for (auto& Data : Datas_)
  {
    Renderer2D::Draw(Texture_, Data.Pos, Data.Size, Data.SrcPos, Data.SrcSize, Data.Angle, Data.Color);
  }
}
