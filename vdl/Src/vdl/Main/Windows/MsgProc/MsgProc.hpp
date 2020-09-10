#pragma once
#include <vdl/pch/Windows/pch.hpp>

//  メインウィンドウ用
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//  サブウィンドウ用
LRESULT CALLBACK SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
