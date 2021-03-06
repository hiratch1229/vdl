#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>

#include <iostream>
#include <locale>

void Main();

int main(int, char**)
{
  //  日本語に設定
  ::setlocale(LC_CTYPE, "jpn");

  //  時間で乱数を設定
  ::srand(static_cast<unsigned int>(time(nullptr)));

  try
  {
    //  エンジンの作成
    Engine Engine;

    Main();
  }
  catch (const std::exception & e)
  {
    std::cout << e.what();
  }

  return 0;
}
