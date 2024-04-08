#include <cmath>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "./gui/Paint.h"
#include "./gui/Window.h"
#include "./input/Input.h"
#include "./rules/board/Board.h"

// Initialize the timer function
void timer(Board* mBoard) {
  double time[2] = {mBoard->getMaxTime(), mBoard->getMaxTime()};
  std::vector<double> undoTimes;
  undoTimes.push_back(mBoard->getMaxTime());
  int moveCount = mBoard->getMoveCount();

  // Start the timer loop
  while (time[mBoard->getTurn()] > 0.0 && mBoard->gameStarted) {
    if (moveCount < mBoard->getMoveCount()) {
      undoTimes.push_back(time[mBoard->getTurn()]);
    } else if (moveCount > mBoard->getMoveCount() && !undoTimes.empty()) {
      time[!mBoard->getTurn()] = undoTimes[undoTimes.size() - 1];
      undoTimes.pop_back();
    }
    moveCount = mBoard->getMoveCount();
    time[mBoard->getTurn()] -= 0.1;
    mBoard->setTime(time);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  // End the game by timeout
  if (mBoard->gameStarted) mBoard->endGame(false, true, false);
};

int main() {
  // Initialize the board
  Board* mBoard = new Board(8, 8);
  mBoard->setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

  // Initialize the window
  Window* mWindow = new Window(mBoard);
  std::thread timerThread;

  // Window loop
  bool running = true;
  while (running) {
    // Handle the timer thread
    if (mBoard->gameStarted && !timerThread.joinable()) {
      timerThread = std::thread(timer, mBoard);
    } else if (!mBoard->gameStarted && timerThread.joinable()) {
      timerThread.join();
    }

    // Redraw the window in order to update the timer
    InvalidateRect(mWindow->getHWnd(), NULL, TRUE);

    if (!mWindow->ProcessMessages()) running = false;
    Sleep(50);
  }

  delete mWindow;
  delete mBoard;
  return 0;
}