#ifndef INPUT_H_
#define INPUT_H_

#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

#include "../rules/board/Board.h"

class Input {
 public:
  Input(Board* mBoard, int mWidth, int mHeight);
  Input(const Input&) = delete;
  Input& operator=(const Input&) = delete;
  ~Input();

  bool handleMouseDown(int x, int y);
  bool handleMouseUp(int x, int y);
  bool isDraggingFigure();
  void setDimensions(int mWidth, int mHeight);
  double* getTime();

 private:
  Board* board;
  std::thread timer;
  std::vector<std::string> buttons;
  std::atomic<double> time[2];
  std::atomic<double> maxTime;
  bool clicked;
  bool draggingFigure;
  bool test;
  int width;
  int height;
  int square[2];
};

#endif  // INPUT_H_