#ifndef PIECE_H_
#define PIECE_H_

#include <cmath>
#include <string>
#include <vector>

using std::string;

class Piece {
 public:
  Piece();
  ~Piece();

  std::vector<std::vector<int>> testAvailableMoves(string board, bool turn, string lastMove, int castling[4]);
  bool move(string move, string board, char curPiece, int width, bool turn, string lastMove, int castling[4]);
  bool testCheck(string board, bool turn, string lastMove, int castling[4]);
  bool getPassant();
  void setPassant(bool pPassant);
  int getValue(char piece);

 private:
  char file;
  char rank;
  bool turn;
  bool passant;
};

#endif  // PIECE_H_