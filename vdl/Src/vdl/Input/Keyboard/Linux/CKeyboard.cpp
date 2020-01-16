#include "CKeyboard.hpp"

#include <linux/input.h>
#include <unistd.h>

void CKeyboard::Update()
{
  input_event event;
  read(0, &event, );
}
