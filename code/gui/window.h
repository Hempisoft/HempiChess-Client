#ifndef WINDOW_H_
#define WINDOW_H_

#include <Windows.h>
#include <Windowsx.h>
#include <gdiplus.h>

#include <cmath>
#include <vector>

#include "../input/Input.h"
#include "./Paint.h"

class Window {
 public:
  Window(Board* mBoard);
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window();

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  static void DrawingProcedure(HWND hWnd, LPPAINTSTRUCT lpPS);
  static void displayDialog(HWND hWnd);
  static void AddControls(HWND hDlg);
  static void AddMenus(HWND hWnd);
  void registerDialogClass(HINSTANCE hInstance);
  bool ProcessMessages();
  Paint* getPaint();
  HWND getHWnd();

 private:
  HINSTANCE h_hInstance;
  HWND h_hWnd;
  ULONG_PTR gdiplusToken;
  Paint* paint;
  Input* input;
  int width;
  int height;
};

#endif  // WINDOW_H_