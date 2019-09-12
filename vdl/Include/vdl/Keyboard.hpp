#pragma once
#include "Button.hpp"

namespace vdl::Input
{
  namespace Keyboard
  {
    //  Backspace �L�[
    constexpr Button KeyBackspace{ InputDevice::eKeyboard, 0x08 };
    //  Tab �L�[
    constexpr Button KeyTab{ InputDevice::eKeyboard, 0x09 };
    //  Enter �L�[
    constexpr Button KeyEnter{ InputDevice::eKeyboard, 0x0D };
    //  Shift �L�[
    constexpr Button KeyShift{ InputDevice::eKeyboard, 0x10 };
    //  Ctrl �L�[
    constexpr Button KeyControl{ InputDevice::eKeyboard, 0x11 };
    //  Alt �L�[
    constexpr Button KeyAlt{ InputDevice::eKeyboard, 0x12 };
    //  Pause �L�[
    constexpr Button KeyPause{ InputDevice::eKeyboard, 0x13 };
    //  Cpas Lock �L�[
    constexpr Button KeyCapsLock{ InputDevice::eKeyboard, 0x14 };
    //  Escape �L�[
    constexpr Button KeyEscape{ InputDevice::eKeyboard, 0x1B };
    //  �X�y�[�X �L�[
    constexpr Button KeySpace{ InputDevice::eKeyboard, 0x20 };
    //  PageUp �L�[
    constexpr Button KeyPageUp{ InputDevice::eKeyboard, 0x21 };
    //  PageDown �L�[
    constexpr Button KeyPageDown{ InputDevice::eKeyboard, 0x22 };
    //  End �L�[
    constexpr Button KeyEnd{ InputDevice::eKeyboard, 0x23 };
    //  Home �L�[
    constexpr Button KeyHome{ InputDevice::eKeyboard, 0x24 };
    //  �� �L�[
    constexpr Button KeyLeft{ InputDevice::eKeyboard, 0x25 };
    //  �� �L�[
    constexpr Button KeyUp{ InputDevice::eKeyboard, 0x26 };
    //  �� �L�[
    constexpr Button KeyRight{ InputDevice::eKeyboard, 0x27 };
    //  �� �L�[
    constexpr Button KeyDown{ InputDevice::eKeyboard, 0x28 };
    //  PrintScreen �L�[
    constexpr Button KeyPrintScreen{ InputDevice::eKeyboard, 0x2C };
    //  Insert �L�[
    constexpr Button KeyInsert{ InputDevice::eKeyboard, 0x2D };
    //  Delete �L�[
    constexpr Button KeyDelete{ InputDevice::eKeyboard, 0x2E };
    //  0 �L�[
    constexpr Button Key0{ InputDevice::eKeyboard, 0x30 };
    //  1 �L�[
    constexpr Button Key1{ InputDevice::eKeyboard, 0x31 };
    //  2 �L�[
    constexpr Button Key2{ InputDevice::eKeyboard, 0x32 };
    //  3 �L�[
    constexpr Button Key3{ InputDevice::eKeyboard, 0x33 };
    //  4 �L�[
    constexpr Button Key4{ InputDevice::eKeyboard, 0x34 };
    //  5 �L�[
    constexpr Button Key5{ InputDevice::eKeyboard, 0x35 };
    //  6 �L�[
    constexpr Button Key6{ InputDevice::eKeyboard, 0x36 };
    //  7 �L�[
    constexpr Button Key7{ InputDevice::eKeyboard, 0x37 };
    //  8 �L�[
    constexpr Button Key8{ InputDevice::eKeyboard, 0x38 };
    //  9 �L�[
    constexpr Button Key9{ InputDevice::eKeyboard, 0x39 };
    //  A �L�[
    constexpr Button KeyA{ InputDevice::eKeyboard, 0x41 };
    //  B �L�[
    constexpr Button KeyB{ InputDevice::eKeyboard, 0x42 };
    //  C �L�[
    constexpr Button KeyC{ InputDevice::eKeyboard, 0x43 };
    //  D �L�[
    constexpr Button KeyD{ InputDevice::eKeyboard, 0x44 };
    //  E �L�[
    constexpr Button KeyE{ InputDevice::eKeyboard, 0x45 };
    //  F �L�[
    constexpr Button KeyF{ InputDevice::eKeyboard, 0x46 };
    //  G �L�[
    constexpr Button KeyG{ InputDevice::eKeyboard, 0x47 };
    //  H �L�[
    constexpr Button KeyH{ InputDevice::eKeyboard, 0x48 };
    //  I �L�[
    constexpr Button KeyI{ InputDevice::eKeyboard, 0x49 };
    //  J �L�[
    constexpr Button KeyJ{ InputDevice::eKeyboard, 0x4A };
    //  K �L�[
    constexpr Button KeyK{ InputDevice::eKeyboard, 0x4B };
    //  L �L�[
    constexpr Button KeyL{ InputDevice::eKeyboard, 0x4C };
    //  M �L�[
    constexpr Button KeyM{ InputDevice::eKeyboard, 0x4D };
    //  N �L�[
    constexpr Button KeyN{ InputDevice::eKeyboard, 0x4E };
    //  O �L�[
    constexpr Button KeyO{ InputDevice::eKeyboard, 0x4F };
    //  P �L�[
    constexpr Button KeyP{ InputDevice::eKeyboard, 0x50 };
    //  Q �L�[
    constexpr Button KeyQ{ InputDevice::eKeyboard, 0x51 };
    //  R �L�[
    constexpr Button KeyR{ InputDevice::eKeyboard, 0x52 };
    //  S �L�[
    constexpr Button KeyS{ InputDevice::eKeyboard, 0x53 };
    //  T �L�[
    constexpr Button KeyT{ InputDevice::eKeyboard, 0x54 };
    //  U �L�[
    constexpr Button KeyU{ InputDevice::eKeyboard, 0x55 };
    //  V �L�[
    constexpr Button KeyV{ InputDevice::eKeyboard, 0x56 };
    //  W �L�[
    constexpr Button KeyW{ InputDevice::eKeyboard, 0x57 };
    //  X �L�[
    constexpr Button KeyX{ InputDevice::eKeyboard, 0x58 };
    //  Y �L�[
    constexpr Button KeyY{ InputDevice::eKeyboard, 0x59 };
    //  Z �L�[
    constexpr Button KeyZ{ InputDevice::eKeyboard, 0x5A };
    //  �� Windows �L�[
    constexpr Button KeyLeftWindows{ InputDevice::eKeyboard, 0x5B };
    //  �E Windows �L�[
    constexpr Button KeyRightWindows{ InputDevice::eKeyboard, 0x5C };
    //  �e���L�[ 0 �L�[
    constexpr Button KeyNumPad0{ InputDevice::eKeyboard, 0x60 };
    //  �e���L�[ 1 �L�[
    constexpr Button KeyNumPad1{ InputDevice::eKeyboard, 0x61 };
    //  �e���L�[ 2 �L�[
    constexpr Button KeyNumPad2{ InputDevice::eKeyboard, 0x62 };
    //  �e���L�[ 3 �L�[
    constexpr Button KeyNumPad3{ InputDevice::eKeyboard, 0x63 };
    //  �e���L�[ 4 �L�[
    constexpr Button KeyNumPad4{ InputDevice::eKeyboard, 0x64 };
    //  �e���L�[ 5 �L�[
    constexpr Button KeyNumPad5{ InputDevice::eKeyboard, 0x65 };
    //  �e���L�[ 6 �L�[
    constexpr Button KeyNumPad6{ InputDevice::eKeyboard, 0x66 };
    //  �e���L�[ 7 �L�[
    constexpr Button KeyNumPad7{ InputDevice::eKeyboard, 0x67 };
    //  �e���L�[ 8 �L�[
    constexpr Button KeyNumPad8{ InputDevice::eKeyboard, 0x68 };
    //  �e���L�[ 9 �L�[
    constexpr Button KeyNumPad9{ InputDevice::eKeyboard, 0x69 };
    //  �e���L�[ * �L�[
    constexpr Button KeyNumPadMultiply{ InputDevice::eKeyboard, 0x6A };
    //  �e���L�[ + �L�[
    constexpr Button KeyNumPadAdd{ InputDevice::eKeyboard, 0x6B };
    //  �e���L�[ Enter �L�[
    constexpr Button KeyNumPadEnter{ InputDevice::eKeyboard, 0x6C };
    //  �e���L�[ - �L�[
    constexpr Button KeyNumPadSubtract{ InputDevice::eKeyboard, 0x6D };
    //  �e���L�[ . �L�[
    constexpr Button KeyNumPadDecimal{ InputDevice::eKeyboard, 0x6E };
    //  �e���L�[ / �L�[
    constexpr Button KeyNumPadDivide{ InputDevice::eKeyboard, 0x6F };
    //  F1 �L�[
    constexpr Button KeyF1{ InputDevice::eKeyboard, 0x70 };
    //  F2 �L�[
    constexpr Button KeyF2{ InputDevice::eKeyboard, 0x71 };
    //  F3 �L�[
    constexpr Button KeyF3{ InputDevice::eKeyboard, 0x72 };
    //  F4 �L�[
    constexpr Button KeyF4{ InputDevice::eKeyboard, 0x73 };
    //  F5 �L�[
    constexpr Button KeyF5{ InputDevice::eKeyboard, 0x74 };
    //  F6 �L�[
    constexpr Button KeyF6{ InputDevice::eKeyboard, 0x75 };
    //  F7 �L�[
    constexpr Button KeyF7{ InputDevice::eKeyboard, 0x76 };
    //  F8 �L�[
    constexpr Button KeyF8{ InputDevice::eKeyboard, 0x77 };
    //  F9 �L�[
    constexpr Button KeyF9{ InputDevice::eKeyboard, 0x78 };
    //  F10 �L�[
    constexpr Button KeyF10{ InputDevice::eKeyboard, 0x79 };
    //  F11 �L�[
    constexpr Button KeyF11{ InputDevice::eKeyboard, 0x7A };
    //  F12 �L�[
    constexpr Button KeyF12{ InputDevice::eKeyboard, 0x7B };
    //  F13 �L�[
    constexpr Button KeyF13{ InputDevice::eKeyboard, 0x7C };
    //  F14 �L�[
    constexpr Button KeyF14{ InputDevice::eKeyboard, 0x7D };
    //  F15 �L�[
    constexpr Button KeyF15{ InputDevice::eKeyboard, 0x7E };
    //  F16 �L�[
    constexpr Button KeyF16{ InputDevice::eKeyboard, 0x7F };
    //  F17 �L�[
    constexpr Button KeyF17{ InputDevice::eKeyboard, 0x80 };
    //  F18 �L�[
    constexpr Button KeyF18{ InputDevice::eKeyboard, 0x81 };
    //  F19 �L�[
    constexpr Button KeyF19{ InputDevice::eKeyboard, 0x82 };
    //  F20 �L�[
    constexpr Button KeyF20{ InputDevice::eKeyboard, 0x83 };
    //  F21 �L�[
    constexpr Button KeyF21{ InputDevice::eKeyboard, 0x84 };
    //  F22 �L�[
    constexpr Button KeyF22{ InputDevice::eKeyboard, 0x85 };
    //  F23 �L�[
    constexpr Button KeyF23{ InputDevice::eKeyboard, 0x86 };
    //  F24 �L�[
    constexpr Button KeyF24{ InputDevice::eKeyboard, 0x87 };
    //  NumLock �L�[
    constexpr Button KeyNumLock{ InputDevice::eKeyboard, 0x90 };
    //  �� Shift �L�[
    constexpr Button KeyLeftShift{ InputDevice::eKeyboard, 0xA0 };
    //  �E Shift �L�[
    constexpr Button KeyRightShift{ InputDevice::eKeyboard, 0xA1 };
    //  �� Control �L�[
    constexpr Button KeyLeftControl{ InputDevice::eKeyboard, 0xA2 };
    //  �E Control �L�[
    constexpr Button KeyRightControl{ InputDevice::eKeyboard, 0xA3 };
    //  �� Alt �L�[
    constexpr Button KeyLeftAlt{ InputDevice::eKeyboard, 0xA4 };
    //  �E Alt �L�[
    constexpr Button KeyRightAlt{ InputDevice::eKeyboard, 0xA5 };
  }

  //  ����������Ă���Ȃ�true��Ԃ�
  inline bool isPressAnyButtonOnKeyboard()
  {
    return isPressOr(Keyboard::KeyBackspace, Keyboard::KeyTab, Keyboard::KeyEnter, Keyboard::KeyShift, Keyboard::KeyControl, Keyboard::KeyAlt, Keyboard::KeyPause, Keyboard::KeyCapsLock, Keyboard::KeyEscape, Keyboard::KeySpace,
      Keyboard::KeyPageUp, Keyboard::KeyPageDown, Keyboard::KeyEnd, Keyboard::KeyHome, Keyboard::KeyLeft, Keyboard::KeyUp, Keyboard::KeyRight, Keyboard::KeyDown, Keyboard::KeyPrintScreen, Keyboard::KeyInsert, Keyboard::KeyDelete,
      Keyboard::Key0, Keyboard::Key1, Keyboard::Key2, Keyboard::Key3, Keyboard::Key4, Keyboard::Key5, Keyboard::Key6, Keyboard::Key7, Keyboard::Key8, Keyboard::Key9,
      Keyboard::KeyA, Keyboard::KeyB, Keyboard::KeyC, Keyboard::KeyD, Keyboard::KeyE, Keyboard::KeyF, Keyboard::KeyG, Keyboard::KeyH, Keyboard::KeyI, Keyboard::KeyJ, Keyboard::KeyK, Keyboard::KeyL, Keyboard::KeyM,
      Keyboard::KeyN, Keyboard::KeyO, Keyboard::KeyP, Keyboard::KeyQ, Keyboard::KeyR, Keyboard::KeyS, Keyboard::KeyT, Keyboard::KeyU, Keyboard::KeyV, Keyboard::KeyW, Keyboard::KeyX, Keyboard::KeyY, Keyboard::KeyZ,
      Keyboard::KeyLeftWindows, Keyboard::KeyRightWindows, Keyboard::KeyNumPad0, Keyboard::KeyNumPad1, Keyboard::KeyNumPad2, Keyboard::KeyNumPad3, Keyboard::KeyNumPad4, Keyboard::KeyNumPad5, Keyboard::KeyNumPad6, Keyboard::KeyNumPad7, Keyboard::KeyNumPad8, Keyboard::KeyNumPad9,
      Keyboard::KeyNumPadMultiply, Keyboard::KeyNumPadAdd, Keyboard::KeyNumPadEnter, Keyboard::KeyNumPadSubtract, Keyboard::KeyNumPadDecimal, Keyboard::KeyNumPadDivide,
      Keyboard::KeyF1, Keyboard::KeyF2, Keyboard::KeyF3, Keyboard::KeyF4, Keyboard::KeyF5, Keyboard::KeyF6, Keyboard::KeyF7, Keyboard::KeyF8, Keyboard::KeyF9, Keyboard::KeyF10, Keyboard::KeyF11, Keyboard::KeyF12,
      Keyboard::KeyF13, Keyboard::KeyF14, Keyboard::KeyF15, Keyboard::KeyF16, Keyboard::KeyF17, Keyboard::KeyF18, Keyboard::KeyF19, Keyboard::KeyF20, Keyboard::KeyF21, Keyboard::KeyF22, Keyboard::KeyF23, Keyboard::KeyF24,
      Keyboard::KeyNumLock, Keyboard::KeyLeftShift, Keyboard::KeyRightShift, Keyboard::KeyLeftControl, Keyboard::KeyRightControl, Keyboard::KeyLeftAlt, Keyboard::KeyRightAlt);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isPressedAnyButtonOnKeyboard()
  {
    return isPressedOr(Keyboard::KeyBackspace, Keyboard::KeyTab, Keyboard::KeyEnter, Keyboard::KeyShift, Keyboard::KeyControl, Keyboard::KeyAlt, Keyboard::KeyPause, Keyboard::KeyCapsLock, Keyboard::KeyEscape, Keyboard::KeySpace,
      Keyboard::KeyPageUp, Keyboard::KeyPageDown, Keyboard::KeyEnd, Keyboard::KeyHome, Keyboard::KeyLeft, Keyboard::KeyUp, Keyboard::KeyRight, Keyboard::KeyDown, Keyboard::KeyPrintScreen, Keyboard::KeyInsert, Keyboard::KeyDelete,
      Keyboard::Key0, Keyboard::Key1, Keyboard::Key2, Keyboard::Key3, Keyboard::Key4, Keyboard::Key5, Keyboard::Key6, Keyboard::Key7, Keyboard::Key8, Keyboard::Key9,
      Keyboard::KeyA, Keyboard::KeyB, Keyboard::KeyC, Keyboard::KeyD, Keyboard::KeyE, Keyboard::KeyF, Keyboard::KeyG, Keyboard::KeyH, Keyboard::KeyI, Keyboard::KeyJ, Keyboard::KeyK, Keyboard::KeyL, Keyboard::KeyM,
      Keyboard::KeyN, Keyboard::KeyO, Keyboard::KeyP, Keyboard::KeyQ, Keyboard::KeyR, Keyboard::KeyS, Keyboard::KeyT, Keyboard::KeyU, Keyboard::KeyV, Keyboard::KeyW, Keyboard::KeyX, Keyboard::KeyY, Keyboard::KeyZ,
      Keyboard::KeyLeftWindows, Keyboard::KeyRightWindows, Keyboard::KeyNumPad0, Keyboard::KeyNumPad1, Keyboard::KeyNumPad2, Keyboard::KeyNumPad3, Keyboard::KeyNumPad4, Keyboard::KeyNumPad5, Keyboard::KeyNumPad6, Keyboard::KeyNumPad7, Keyboard::KeyNumPad8, Keyboard::KeyNumPad9,
      Keyboard::KeyNumPadMultiply, Keyboard::KeyNumPadAdd, Keyboard::KeyNumPadEnter, Keyboard::KeyNumPadSubtract, Keyboard::KeyNumPadDecimal, Keyboard::KeyNumPadDivide,
      Keyboard::KeyF1, Keyboard::KeyF2, Keyboard::KeyF3, Keyboard::KeyF4, Keyboard::KeyF5, Keyboard::KeyF6, Keyboard::KeyF7, Keyboard::KeyF8, Keyboard::KeyF9, Keyboard::KeyF10, Keyboard::KeyF11, Keyboard::KeyF12,
      Keyboard::KeyF13, Keyboard::KeyF14, Keyboard::KeyF15, Keyboard::KeyF16, Keyboard::KeyF17, Keyboard::KeyF18, Keyboard::KeyF19, Keyboard::KeyF20, Keyboard::KeyF21, Keyboard::KeyF22, Keyboard::KeyF23, Keyboard::KeyF24,
      Keyboard::KeyNumLock, Keyboard::KeyLeftShift, Keyboard::KeyRightShift, Keyboard::KeyLeftControl, Keyboard::KeyRightControl, Keyboard::KeyLeftAlt, Keyboard::KeyRightAlt);
  }

  //  ���������ꂽ�u�ԂȂ�true��Ԃ�
  inline bool isReleasedAnyButtonOnKeyboard()
  {
    return isReleasedOr(Keyboard::KeyBackspace, Keyboard::KeyTab, Keyboard::KeyEnter, Keyboard::KeyShift, Keyboard::KeyControl, Keyboard::KeyAlt, Keyboard::KeyPause, Keyboard::KeyCapsLock, Keyboard::KeyEscape, Keyboard::KeySpace,
      Keyboard::KeyPageUp, Keyboard::KeyPageDown, Keyboard::KeyEnd, Keyboard::KeyHome, Keyboard::KeyLeft, Keyboard::KeyUp, Keyboard::KeyRight, Keyboard::KeyDown, Keyboard::KeyPrintScreen, Keyboard::KeyInsert, Keyboard::KeyDelete,
      Keyboard::Key0, Keyboard::Key1, Keyboard::Key2, Keyboard::Key3, Keyboard::Key4, Keyboard::Key5, Keyboard::Key6, Keyboard::Key7, Keyboard::Key8, Keyboard::Key9,
      Keyboard::KeyA, Keyboard::KeyB, Keyboard::KeyC, Keyboard::KeyD, Keyboard::KeyE, Keyboard::KeyF, Keyboard::KeyG, Keyboard::KeyH, Keyboard::KeyI, Keyboard::KeyJ, Keyboard::KeyK, Keyboard::KeyL, Keyboard::KeyM,
      Keyboard::KeyN, Keyboard::KeyO, Keyboard::KeyP, Keyboard::KeyQ, Keyboard::KeyR, Keyboard::KeyS, Keyboard::KeyT, Keyboard::KeyU, Keyboard::KeyV, Keyboard::KeyW, Keyboard::KeyX, Keyboard::KeyY, Keyboard::KeyZ,
      Keyboard::KeyLeftWindows, Keyboard::KeyRightWindows, Keyboard::KeyNumPad0, Keyboard::KeyNumPad1, Keyboard::KeyNumPad2, Keyboard::KeyNumPad3, Keyboard::KeyNumPad4, Keyboard::KeyNumPad5, Keyboard::KeyNumPad6, Keyboard::KeyNumPad7, Keyboard::KeyNumPad8, Keyboard::KeyNumPad9,
      Keyboard::KeyNumPadMultiply, Keyboard::KeyNumPadAdd, Keyboard::KeyNumPadEnter, Keyboard::KeyNumPadSubtract, Keyboard::KeyNumPadDecimal, Keyboard::KeyNumPadDivide,
      Keyboard::KeyF1, Keyboard::KeyF2, Keyboard::KeyF3, Keyboard::KeyF4, Keyboard::KeyF5, Keyboard::KeyF6, Keyboard::KeyF7, Keyboard::KeyF8, Keyboard::KeyF9, Keyboard::KeyF10, Keyboard::KeyF11, Keyboard::KeyF12,
      Keyboard::KeyF13, Keyboard::KeyF14, Keyboard::KeyF15, Keyboard::KeyF16, Keyboard::KeyF17, Keyboard::KeyF18, Keyboard::KeyF19, Keyboard::KeyF20, Keyboard::KeyF21, Keyboard::KeyF22, Keyboard::KeyF23, Keyboard::KeyF24,
      Keyboard::KeyNumLock, Keyboard::KeyLeftShift, Keyboard::KeyRightShift, Keyboard::KeyLeftControl, Keyboard::KeyRightControl, Keyboard::KeyLeftAlt, Keyboard::KeyRightAlt);
  }

  //  ����������Ă��Ȃ��Ȃ�true��Ԃ�
  inline bool isReleaseAnyButtonOnKeyboard()
  {
    return isReleaseOr(Keyboard::KeyBackspace, Keyboard::KeyTab, Keyboard::KeyEnter, Keyboard::KeyShift, Keyboard::KeyControl, Keyboard::KeyAlt, Keyboard::KeyPause, Keyboard::KeyCapsLock, Keyboard::KeyEscape, Keyboard::KeySpace,
      Keyboard::KeyPageUp, Keyboard::KeyPageDown, Keyboard::KeyEnd, Keyboard::KeyHome, Keyboard::KeyLeft, Keyboard::KeyUp, Keyboard::KeyRight, Keyboard::KeyDown, Keyboard::KeyPrintScreen, Keyboard::KeyInsert, Keyboard::KeyDelete,
      Keyboard::Key0, Keyboard::Key1, Keyboard::Key2, Keyboard::Key3, Keyboard::Key4, Keyboard::Key5, Keyboard::Key6, Keyboard::Key7, Keyboard::Key8, Keyboard::Key9,
      Keyboard::KeyA, Keyboard::KeyB, Keyboard::KeyC, Keyboard::KeyD, Keyboard::KeyE, Keyboard::KeyF, Keyboard::KeyG, Keyboard::KeyH, Keyboard::KeyI, Keyboard::KeyJ, Keyboard::KeyK, Keyboard::KeyL, Keyboard::KeyM,
      Keyboard::KeyN, Keyboard::KeyO, Keyboard::KeyP, Keyboard::KeyQ, Keyboard::KeyR, Keyboard::KeyS, Keyboard::KeyT, Keyboard::KeyU, Keyboard::KeyV, Keyboard::KeyW, Keyboard::KeyX, Keyboard::KeyY, Keyboard::KeyZ,
      Keyboard::KeyLeftWindows, Keyboard::KeyRightWindows, Keyboard::KeyNumPad0, Keyboard::KeyNumPad1, Keyboard::KeyNumPad2, Keyboard::KeyNumPad3, Keyboard::KeyNumPad4, Keyboard::KeyNumPad5, Keyboard::KeyNumPad6, Keyboard::KeyNumPad7, Keyboard::KeyNumPad8, Keyboard::KeyNumPad9,
      Keyboard::KeyNumPadMultiply, Keyboard::KeyNumPadAdd, Keyboard::KeyNumPadEnter, Keyboard::KeyNumPadSubtract, Keyboard::KeyNumPadDecimal, Keyboard::KeyNumPadDivide,
      Keyboard::KeyF1, Keyboard::KeyF2, Keyboard::KeyF3, Keyboard::KeyF4, Keyboard::KeyF5, Keyboard::KeyF6, Keyboard::KeyF7, Keyboard::KeyF8, Keyboard::KeyF9, Keyboard::KeyF10, Keyboard::KeyF11, Keyboard::KeyF12,
      Keyboard::KeyF13, Keyboard::KeyF14, Keyboard::KeyF15, Keyboard::KeyF16, Keyboard::KeyF17, Keyboard::KeyF18, Keyboard::KeyF19, Keyboard::KeyF20, Keyboard::KeyF21, Keyboard::KeyF22, Keyboard::KeyF23, Keyboard::KeyF24,
      Keyboard::KeyNumLock, Keyboard::KeyLeftShift, Keyboard::KeyRightShift, Keyboard::KeyLeftControl, Keyboard::KeyRightControl, Keyboard::KeyLeftAlt, Keyboard::KeyRightAlt);
  }
}
