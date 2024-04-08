#ifndef BOARD_H_
#define BOARD_H_

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

#include "../pieces/Piece.h"

class Board {
 public:
  Board(int pWidth, int pHeight);
  ~Board();

  std::vector<std::vector<int>> testAvailableMoves();
  std::string getBoard();
  std::string getVisualBoard();
  std::wstring getFen();
  std::wstring getEndMessage();
  void undoMove();
  void setTime(double* pTime);
  void setStyle(int pStyle[4]);
  void setFen(wchar_t fen[72]);
  void setMaxTime(double pMaxTime);
  void setDoesRotate(bool pRotate);
  void setShowMoves(bool pShowMoves);
  void setIsPromoting(bool pPromoting);
  void setSelectedPiece(int pSelectedPiece);
  void beginMovePiece(int fromX, int fromY);
  void newGame(double maxTimeT);
  void endGame(bool rep, bool timeOut, bool resignation);
  bool movePiece(int fromX, int fromY, int toX, int toY, char PromotionPiece);
  bool getTurn();
  bool doesRotate();
  bool isPromoting();
  bool doesShowMoves();
  bool setup(std::string fen);
  bool gameStarted;
  int getWidth();
  int getHeight();
  int getMoveCount();
  int getSelectedPiece();
  int material(bool turn);
  int style[4];
  double getMaxTime();
  std::atomic<double>* getTime();

 private:
  Piece* piece;
  std::vector<std::string> undo;
  std::vector<std::string> undoMoves;
  std::string visualBoard;
  std::string tempMove;
  std::string lastMove;
  std::string board;
  std::string fen;
  std::wstring gameEnded;
  char curPiece;
  bool rotate;
  bool turn;
  bool showMoves;
  bool promoting;
  bool passant;
  int castling[4];
  int width;
  int height;
  int selectedPiece;
  int drawCounter;
  double maxTime;
  std::atomic<double> time[2];
};

#endif  // BOARD_H_