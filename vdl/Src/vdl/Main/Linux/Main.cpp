#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>

#include <iostream>
#include <locale>

void Main();

int main(int, char**)
{
  //  ���{��ɐݒ�
  ::setlocale(LC_CTYPE, "jpn");

  //  ���Ԃŗ�����ݒ�
  ::srand(static_cast<unsigned int>(time(nullptr)));

  try
  {
    //  �G���W���̍쐬
    Engine Engine;

    Main();
  }
  catch (const std::exception & e)
  {
    std::cout << e.what();
  }

  return 0;
}
