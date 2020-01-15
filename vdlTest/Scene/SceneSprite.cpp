#include "SceneSprite.hpp"

using namespace vdl;

void SceneSprite::Initialize()
{
  Texture_ = Texture("Data/asphalt/asphalt.jpg");
}

void SceneSprite::Update()
{
  int Size = static_cast<int>(Datas_.size());

  GUI::Begin("Sprite");
  {
    //  ���l���ύX���ꂽ��
    if (GUI::Input("Size", &Size))
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
    for (int i = 0; i < Size; ++i)
    {
      Data& Data = Datas_[i];

      if (GUI::TreeNode((std::string("Data[") + std::to_string(i) + "]").c_str()))
      {
        GUI::Input("Pos", &Data.Pos);
        GUI::Input("Size", &Data.Size);
        GUI::Input("SrcPos", &Data.SrcPos);
        GUI::Input("SrcSize", &Data.SrcSize);
        GUI::Input("Angle", &Data.Angle);
        GUI::ColorEdit("Color", &Data.Color);
        GUI::TreePop();
      }
    }
  }
  GUI::End();

  //  �`��
  for (auto& Data : Datas_)
  {
    Renderer2D::Draw(Texture_, Data.Pos, Data.Size, Data.SrcPos, Data.SrcSize, Data.Angle, Data.Color);
  }
}
