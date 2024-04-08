#ifndef PAINT_H_
#define PAINT_H_

#include <Windows.h>
#include <gdiplus.h>

#include <vector>

#include "../rules/board/Board.h"

class Paint {
 public:
  Paint();
  Paint(Board* pBoard, int mWidth, int mHeight);
  ~Paint();

  Board* getBoard();
  void drawBgd(Gdiplus::Graphics* graphics);
  void drawBoard(Gdiplus::Graphics* graphics);
  void drawPieces(Gdiplus::Graphics* graphics);
  void drawButtons(Gdiplus::Graphics* graphics);
  void drawDraggedPiece(Gdiplus::Graphics* graphics, int x, int y);
  void drawPromotionMenu(Gdiplus::Graphics* graphics);
  void drawMoveOptions(Gdiplus::Graphics* graphics);
  void drawTimer(Gdiplus::Graphics* graphics);
  void drawEndingScreen(Gdiplus::Graphics* graphics);
  void setDimensions(int pWidth, int pHeight);

 private:
  Gdiplus::Pen* pen;
  Gdiplus::SolidBrush brush;
  Gdiplus::Font* endingFont;
  Gdiplus::Font* timerFont;
  Gdiplus::PointF pointF;
  Gdiplus::StringFormat stringFormat;
  Board* board;
  std::wstring path;
  int width;
  int height;
};

#endif  // PAINT_H_