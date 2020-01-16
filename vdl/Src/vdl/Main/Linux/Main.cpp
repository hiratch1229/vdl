#include <vdl/Engine.hpp>
#include <vdl/System/ISystem.hpp>

#include <iostream>
#include <locale>

void Main();

int main(int, char**)
{
  //  “ú–{Œê‚Éİ’è
  ::setlocale(LC_CTYPE, "jpn");

  //  ŠÔ‚Å—”‚ğİ’è
  ::srand(static_cast<unsigned int>(time(nullptr)));

  try
  {
    //  ƒGƒ“ƒWƒ“‚Ìì¬
    Engine Engine;

    Main();
  }
  catch (const std::exception & e)
  {
    std::cout << e.what();
  }

  return 0;
}
