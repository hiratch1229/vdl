#pragma once
#include "Button.hpp"

namespace vdl::Input
{
  namespace Keyboard
  {
    //  Backspace キー
    constexpr Button KeyBackspace{ InputDeviceType::eKeyboard, 0x08 };
    //  Tab キー
    constexpr Button KeyTab{ InputDeviceType::eKeyboard, 0x09 };
    //  Enter キー
    constexpr Button KeyEnter{ InputDeviceType::eKeyboard, 0x0D };
    //  Shift キー
    constexpr Button KeyShift{ InputDeviceType::eKeyboard, 0x10 };
    //  Ctrl キー
    constexpr Button KeyControl{ InputDeviceType::eKeyboard, 0x11 };
    //  Alt キー
    constexpr Button KeyAlt{ InputDeviceType::eKeyboard, 0x12 };
    //  Pause キー
    constexpr Button KeyPause{ InputDeviceType::eKeyboard, 0x13 };
    //  Cpas Lock キー
    constexpr Button KeyCapsLock{ InputDeviceType::eKeyboard, 0x14 };
    //  Escape キー
    constexpr Button KeyEscape{ InputDeviceType::eKeyboard, 0x1B };
    //  スペース キー
    constexpr Button KeySpace{ InputDeviceType::eKeyboard, 0x20 };
    //  PageUp キー
    constexpr Button KeyPageUp{ InputDeviceType::eKeyboard, 0x21 };
    //  PageDown キー
    constexpr Button KeyPageDown{ InputDeviceType::eKeyboard, 0x22 };
    //  End キー
    constexpr Button KeyEnd{ InputDeviceType::eKeyboard, 0x23 };
    //  Home キー
    constexpr Button KeyHome{ InputDeviceType::eKeyboard, 0x24 };
    //  ← キー
    constexpr Button KeyLeft{ InputDeviceType::eKeyboard, 0x25 };
    //  ↑ キー
    constexpr Button KeyUp{ InputDeviceType::eKeyboard, 0x26 };
    //  → キー
    constexpr Button KeyRight{ InputDeviceType::eKeyboard, 0x27 };
    //  ↓ キー
    constexpr Button KeyDown{ InputDeviceType::eKeyboard, 0x28 };
    //  PrintScreen キー
    constexpr Button KeyPrintScreen{ InputDeviceType::eKeyboard, 0x2C };
    //  Insert キー
    constexpr Button KeyInsert{ InputDeviceType::eKeyboard, 0x2D };
    //  Delete キー
    constexpr Button KeyDelete{ InputDeviceType::eKeyboard, 0x2E };
    //  0 キー
    constexpr Button Key0{ InputDeviceType::eKeyboard, 0x30 };
    //  1 キー
    constexpr Button Key1{ InputDeviceType::eKeyboard, 0x31 };
    //  2 キー
    constexpr Button Key2{ InputDeviceType::eKeyboard, 0x32 };
    //  3 キー
    constexpr Button Key3{ InputDeviceType::eKeyboard, 0x33 };
    //  4 キー
    constexpr Button Key4{ InputDeviceType::eKeyboard, 0x34 };
    //  5 キー
    constexpr Button Key5{ InputDeviceType::eKeyboard, 0x35 };
    //  6 キー
    constexpr Button Key6{ InputDeviceType::eKeyboard, 0x36 };
    //  7 キー
    constexpr Button Key7{ InputDeviceType::eKeyboard, 0x37 };
    //  8 キー
    constexpr Button Key8{ InputDeviceType::eKeyboard, 0x38 };
    //  9 キー
    constexpr Button Key9{ InputDeviceType::eKeyboard, 0x39 };
    //  A キー
    constexpr Button KeyA{ InputDeviceType::eKeyboard, 0x41 };
    //  B キー
    constexpr Button KeyB{ InputDeviceType::eKeyboard, 0x42 };
    //  C キー
    constexpr Button KeyC{ InputDeviceType::eKeyboard, 0x43 };
    //  D キー
    constexpr Button KeyD{ InputDeviceType::eKeyboard, 0x44 };
    //  E キー
    constexpr Button KeyE{ InputDeviceType::eKeyboard, 0x45 };
    //  F キー
    constexpr Button KeyF{ InputDeviceType::eKeyboard, 0x46 };
    //  G キー
    constexpr Button KeyG{ InputDeviceType::eKeyboard, 0x47 };
    //  H キー
    constexpr Button KeyH{ InputDeviceType::eKeyboard, 0x48 };
    //  I キー
    constexpr Button KeyI{ InputDeviceType::eKeyboard, 0x49 };
    //  J キー
    constexpr Button KeyJ{ InputDeviceType::eKeyboard, 0x4A };
    //  K キー
    constexpr Button KeyK{ InputDeviceType::eKeyboard, 0x4B };
    //  L キー
    constexpr Button KeyL{ InputDeviceType::eKeyboard, 0x4C };
    //  M キー
    constexpr Button KeyM{ InputDeviceType::eKeyboard, 0x4D };
    //  N キー
    constexpr Button KeyN{ InputDeviceType::eKeyboard, 0x4E };
    //  O キー
    constexpr Button KeyO{ InputDeviceType::eKeyboard, 0x4F };
    //  P キー
    constexpr Button KeyP{ InputDeviceType::eKeyboard, 0x50 };
    //  Q キー
    constexpr Button KeyQ{ InputDeviceType::eKeyboard, 0x51 };
    //  R キー
    constexpr Button KeyR{ InputDeviceType::eKeyboard, 0x52 };
    //  S キー
    constexpr Button KeyS{ InputDeviceType::eKeyboard, 0x53 };
    //  T キー
    constexpr Button KeyT{ InputDeviceType::eKeyboard, 0x54 };
    //  U キー
    constexpr Button KeyU{ InputDeviceType::eKeyboard, 0x55 };
    //  V キー
    constexpr Button KeyV{ InputDeviceType::eKeyboard, 0x56 };
    //  W キー
    constexpr Button KeyW{ InputDeviceType::eKeyboard, 0x57 };
    //  X キー
    constexpr Button KeyX{ InputDeviceType::eKeyboard, 0x58 };
    //  Y キー
    constexpr Button KeyY{ InputDeviceType::eKeyboard, 0x59 };
    //  Z キー
    constexpr Button KeyZ{ InputDeviceType::eKeyboard, 0x5A };
    //  左 Windows キー
    constexpr Button KeyLeftWindows{ InputDeviceType::eKeyboard, 0x5B };
    //  右 Windows キー
    constexpr Button KeyRightWindows{ InputDeviceType::eKeyboard, 0x5C };
    //  テンキー 0 キー
    constexpr Button KeyNumPad0{ InputDeviceType::eKeyboard, 0x60 };
    //  テンキー 1 キー
    constexpr Button KeyNumPad1{ InputDeviceType::eKeyboard, 0x61 };
    //  テンキー 2 キー
    constexpr Button KeyNumPad2{ InputDeviceType::eKeyboard, 0x62 };
    //  テンキー 3 キー
    constexpr Button KeyNumPad3{ InputDeviceType::eKeyboard, 0x63 };
    //  テンキー 4 キー
    constexpr Button KeyNumPad4{ InputDeviceType::eKeyboard, 0x64 };
    //  テンキー 5 キー
    constexpr Button KeyNumPad5{ InputDeviceType::eKeyboard, 0x65 };
    //  テンキー 6 キー
    constexpr Button KeyNumPad6{ InputDeviceType::eKeyboard, 0x66 };
    //  テンキー 7 キー
    constexpr Button KeyNumPad7{ InputDeviceType::eKeyboard, 0x67 };
    //  テンキー 8 キー
    constexpr Button KeyNumPad8{ InputDeviceType::eKeyboard, 0x68 };
    //  テンキー 9 キー
    constexpr Button KeyNumPad9{ InputDeviceType::eKeyboard, 0x69 };
    //  テンキー * キー
    constexpr Button KeyNumPadMultiply{ InputDeviceType::eKeyboard, 0x6A };
    //  テンキー + キー
    constexpr Button KeyNumPadAdd{ InputDeviceType::eKeyboard, 0x6B };
    //  テンキー Enter キー
    constexpr Button KeyNumPadEnter{ InputDeviceType::eKeyboard, 0x6C };
    //  テンキー - キー
    constexpr Button KeyNumPadSubtract{ InputDeviceType::eKeyboard, 0x6D };
    //  テンキー . キー
    constexpr Button KeyNumPadDecimal{ InputDeviceType::eKeyboard, 0x6E };
    //  テンキー / キー
    constexpr Button KeyNumPadDivide{ InputDeviceType::eKeyboard, 0x6F };
    //  F1 キー
    constexpr Button KeyF1{ InputDeviceType::eKeyboard, 0x70 };
    //  F2 キー
    constexpr Button KeyF2{ InputDeviceType::eKeyboard, 0x71 };
    //  F3 キー
    constexpr Button KeyF3{ InputDeviceType::eKeyboard, 0x72 };
    //  F4 キー
    constexpr Button KeyF4{ InputDeviceType::eKeyboard, 0x73 };
    //  F5 キー
    constexpr Button KeyF5{ InputDeviceType::eKeyboard, 0x74 };
    //  F6 キー
    constexpr Button KeyF6{ InputDeviceType::eKeyboard, 0x75 };
    //  F7 キー
    constexpr Button KeyF7{ InputDeviceType::eKeyboard, 0x76 };
    //  F8 キー
    constexpr Button KeyF8{ InputDeviceType::eKeyboard, 0x77 };
    //  F9 キー
    constexpr Button KeyF9{ InputDeviceType::eKeyboard, 0x78 };
    //  F10 キー
    constexpr Button KeyF10{ InputDeviceType::eKeyboard, 0x79 };
    //  F11 キー
    constexpr Button KeyF11{ InputDeviceType::eKeyboard, 0x7A };
    //  F12 キー
    constexpr Button KeyF12{ InputDeviceType::eKeyboard, 0x7B };
    //  F13 キー
    constexpr Button KeyF13{ InputDeviceType::eKeyboard, 0x7C };
    //  F14 キー
    constexpr Button KeyF14{ InputDeviceType::eKeyboard, 0x7D };
    //  F15 キー
    constexpr Button KeyF15{ InputDeviceType::eKeyboard, 0x7E };
    //  F16 キー
    constexpr Button KeyF16{ InputDeviceType::eKeyboard, 0x7F };
    //  F17 キー
    constexpr Button KeyF17{ InputDeviceType::eKeyboard, 0x80 };
    //  F18 キー
    constexpr Button KeyF18{ InputDeviceType::eKeyboard, 0x81 };
    //  F19 キー
    constexpr Button KeyF19{ InputDeviceType::eKeyboard, 0x82 };
    //  F20 キー
    constexpr Button KeyF20{ InputDeviceType::eKeyboard, 0x83 };
    //  F21 キー
    constexpr Button KeyF21{ InputDeviceType::eKeyboard, 0x84 };
    //  F22 キー
    constexpr Button KeyF22{ InputDeviceType::eKeyboard, 0x85 };
    //  F23 キー
    constexpr Button KeyF23{ InputDeviceType::eKeyboard, 0x86 };
    //  F24 キー
    constexpr Button KeyF24{ InputDeviceType::eKeyboard, 0x87 };
    //  NumLock キー
    constexpr Button KeyNumLock{ InputDeviceType::eKeyboard, 0x90 };
    //  左 Shift キー
    constexpr Button KeyLeftShift{ InputDeviceType::eKeyboard, 0xA0 };
    //  右 Shift キー
    constexpr Button KeyRightShift{ InputDeviceType::eKeyboard, 0xA1 };
    //  左 Control キー
    constexpr Button KeyLeftControl{ InputDeviceType::eKeyboard, 0xA2 };
    //  右 Control キー
    constexpr Button KeyRightControl{ InputDeviceType::eKeyboard, 0xA3 };
    //  左 Alt キー
    constexpr Button KeyLeftAlt{ InputDeviceType::eKeyboard, 0xA4 };
    //  右 Alt キー
    constexpr Button KeyRightAlt{ InputDeviceType::eKeyboard, 0xA5 };
  }

  //  何か押されているならtrueを返す
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

  //  何か押された瞬間ならtrueを返す
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

  //  何か離された瞬間ならtrueを返す
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

  //  何か押されていないならtrueを返す
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
