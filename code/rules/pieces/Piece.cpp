#include "./Piece.h"

/**
 * @brief Default constructor for the Piece class.
 *
 * Initializes a new instance of the Piece class with the passant flag set to false.
 */
Piece::Piece() : passant(false) {}

/**
 * Returns the passant status of the piece.
 *
 * @return true if the piece is passant, false otherwise.
 */
bool Piece::getPassant() { return passant; }

/**
 * Sets the passant flag for the piece.
 *
 * @param pPassant The value to set for the passant flag.
 */
void Piece::setPassant(bool pPassant) { passant = pPassant; }

/**
 * Returns the value of a chess piece, Adjusted to validifying insufficient material checks.
 *
 * @param piece The character representing the chess piece.
 * @return The value of the chess piece.
 */
int Piece::getValue(char piece) {
  switch (tolower(piece)) {
    case 'p':
      return 5;
    case 'n':
      return 2;
    case 'b':
      return 3;
    case 'r':
      return 5;
    case 'q':
      return 9;
    default:
      return 0;
  }
}

/**
 * Checks if the current player's king is in check.
 *
 * @param board The current state of the chessboard represented as a string.
 * @param turn The current player's turn. `true` for white, `false` for black.
 * @param lastMove The last move made on the chessboard.
 * @param castling An array representing the availability of castling for both players.
 *                Index 0 and 1 represent white's castling availability, while index 2 and 3 represent black's castling
 * availability. A value of 1 indicates that castling is available, while a value of 0 indicates that castling is not
/**
 * This function tests if the current player's king is in check.
 * It takes the current state of the chessboard, the turn, the last move made, and the castling availability as input.
 * It iterates through the chessboard to find the position of the current player's king.
 * Then, it checks if any opponent's piece can capture the king.
 * If a capture is possible, it simulates the move and checks if the king is still in check.
 * If the king is not in check after the move, it means the current player's king is not in check.
 * The function returns true if the king is in check, and false otherwise.
 */
bool Piece::testCheck(string board, bool turn, string lastMove, int castling[4]) {
  // Find the position of the current player's king
  char curKing = turn == 0 ? 'k' : 'K';
  int width = std::sqrt(board.length());
  string kingPos = "";
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < width; j++) {
      if (board[i * width + j] == curKing) {
        string str1(1, j + '0');
        string str2(1, i + '0');
        kingPos = str1 + str2;
        break;
      }
    }
    if (kingPos != "") break;
  }

  // Check if any opponent's piece can capture the king
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < width; j++) {
      if (board[i * width + j] == ' ') continue;
      if (isupper(board[i * width + j]) != isupper(curKing)) {
        string str0(1, board[i * width + j]);
        string str1(1, j + '0');
        string str2(1, i + '0');
        // Simulate the move and check if the king is still in check
        if (move(str0 + str1 + str2 + "x" + kingPos, board, str0[0], width, !turn, lastMove, castling)) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * This function calculates the available moves for all pieces of the current player.
 * It takes the current state of the chessboard, the turn, the last move made, and the castling availability as input.
 * It iterates through the chessboard to find the current player's pieces.
 * For each piece, it generates all possible moves and checks if the move is valid.
 * If the move is valid, it simulates the move and checks if the king is in check.
 * If the king is not in check after the move, it adds the move to the result vector.
 * The function returns a vector of vectors, where each inner vector represents the possible moves for a piece.
 * The first element of each inner vector is the Move-From location.
 */
std::vector<std::vector<int>> Piece::testAvailableMoves(string board1, bool turn, string lastMove, int castling[4]) {
  std::vector<std::vector<int>> result;
  int width = std::sqrt(board1.length());
  char curKing = turn == 0 ? 'k' : 'K';
  string board = board1;

  char curPiece;
  // Find the current player's pieces and calculate their available moves
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < width; j++) {
      if (board[i * width + j] == ' ' || isupper(board[i * width + j]) != isupper(curKing))
        continue;
      else
        curPiece = board[i * width + j];
      std::vector<int> moves = {i * width + j};

      // Generate all possible moves for the current piece
      for (int t = 0; t < width; t++) {
        for (int l = 0; l < width; l++) {
          if (move(string(1, curPiece) + string(1, j + '0') + string(1, i + '0') + string(1, l + '0') +
                       string(1, t + '0'),
                   board, curPiece, width, turn, lastMove, castling) ||
              move(string(1, curPiece) + string(1, j + '0') + string(1, i + '0') + "x" + string(1, l + '0') +
                       string(1, t + '0'),
                   board, curPiece, width, turn, lastMove, castling)) {
            char temp = board[t * width + l];
            board[i * width + j] = ' ';
            board[t * width + l] = curPiece;
            // Check if the king is in check after the move
            if (!testCheck(board, turn, lastMove, castling)) {
              moves.push_back(t * width + l);
            }
            board[i * width + j] = curPiece;
            board[t * width + l] = temp;
          }
        }
      }

      // Add the moves to the result vector
      if (moves.size() > 1) result.push_back(moves);
    }
  }
  return result;
}

/**
 * This function checks the validity of a move.
 * It takes the move in algebraic notation, the current state of the chessboard, the type of the piece to be moved,
 * the width of the chessboard, the turn, the last move made, and the castling availability as input.
 * It checks if the move string format is correct and if it is the correct player's turn to move.
 * Then, it calculates the Move-From and Move-To coordinates based on the move string.
 * It checks if the move is valid for the specific piece type.
 * If the move is a capture, it checks if the captured piece is of the opposite color.
 * If the move is a pawn double move, it checks if it is a valid en passant capture.
 * If the move is a castling move, it checks if the castling is available and if the path is clear.
 * The function returns true if the move is valid, and false otherwise.
 */
bool Piece::move(string move, string board, char curPiece, int width, bool turn, string lastMove, int castling[4]) {
  // Check for the correct move string format
  if (move.find_first_not_of("KQRBNPkqrbnp") != 1) return false;
  if ((isupper(move[0]) > 0) != turn) return false;
  int fromCord = (move[2] - '0') * width + (move[1] - '0');
  int l = (move[3] == 'x');
  int toCord = (move[4 + l] - '0') * width + (move[3 + l] - '0');

  // Handle special pawn move: en passant
  if (tolower(move[0]) == 'p' && tolower(lastMove[0]) == 'p' && abs(lastMove[2] - lastMove[4]) == 2 &&
      move[2] == lastMove[4] && abs(move[2] - move[4]) == 1) {
    if (turn && move[3] == lastMove[3] && move[4] == lastMove[4] - 1 &&
        (move[1] == lastMove[3] - 1 || move[1] == lastMove[3] + 1)) {
      passant = true;
      return true;
    } else if (!turn && move[3] == lastMove[3] && move[4] == lastMove[4] + 1 &&
               (move[1] == lastMove[3] - 1 || move[1] == lastMove[3] + 1)) {
      passant = true;
      return true;
    }
  }

  // Check the secondary validity of the move format for taking moves
  if (move.length() == 6 || move.length() == 8) {
    if (move[3] != 'x') return false;
    if (curPiece != move[0]) return false;
    if ((tolower(move[0]) != 'k' && isupper(curPiece) == isupper(board[toCord])) || board[toCord] == ' ') return false;
  } else {
    if (curPiece != move[0] || (board[toCord] != ' ')) {
      return false;
    }
  }

  // Check for special move: promotion
  if (move.size() > 6 &&
      (move[move.length() - 2] != '=' || move.find_last_of("RrNnBbQq") != move.length() - 1 ||
       isupper(move[move.length() - 1]) != isupper(move[0]) || (move[4 + l] != '0' && move[4 + l] != '7'))) {
    return false;
  }

  // copy the move string to a new string without the 'x' character for recursive checks
  string move1 = "";
  for (long unsigned int i = 0; i < move.length(); i++) {
    if (move[i] != 'x') move1.push_back(move[i]);
  }

  switch (tolower(move[0])) {
    case 'p': {
      // Check for movement direction
      if ((isupper(move[0]) == 0) != (move[2] - move[4 + l] < 0)) {
        return false;
      }

      if (l) {
        if (abs(move[1] - move[4]) == 1) {
          // Check for correct take
          if (abs(move[2] - move[5]) != 1) return false;

          if (board[toCord] == ' ' || isupper(board[toCord]) == isupper(move[0])) return false;

        } else {
          return false;
        }
      } else if (abs(move[2] - move[4]) == 1) {
        // Check for unmarked capture
        if (abs(move[1] - move[3]) != 0) return false;

      } else if (abs(move[2] - move[4]) == 2) {
        // Check if pawn is on starting position
        if (move[2] != '1' && move[2] != '6') return false;
        // Check if pawn is blocked
        if (board[toCord] != ' ') return false;
        if (abs(move[1] - move[3]) != 0) return false;
        if (isupper(move[0]) != 0) {
          if (board[toCord + width] != ' ') return false;
        } else {
          if (board[toCord - width] != ' ') return false;
        }
      } else {
        return false;
      }
      break;
    }
    case 'r': {
      if (move[1] != move[3 + l] && move[2] != move[4 + l]) return false;
      if ((abs(move[1] - move[3 + l]) > 1 || abs(move[2] - move[4 + l]) > 1)) {
        // Check if the path is clear by recursively checking the move to every square between the two points, within
        // the rooks movement rules
        if (move[1] > move[3 + l])
          move1[3]++;
        else if (move[1] < move[3 + l])
          move1[3]--;
        else if (move[2] > move[4 + l])
          move1[4]++;
        else if (move[2] < move[4 + l])
          move1[4]--;
        if (!Piece::move(move1, board, curPiece, width, turn, lastMove, castling)) return false;
      }
      break;
    }
    case 'n': {
      if (!(abs(move[1] - move[3 + l]) == 2 && abs(move[2] - move[4 + l]) == 1) &&
          !(abs(move[1] - move[3 + l]) == 1 && abs(move[2] - move[4 + l]) == 2)) {
        return false;
      }
      break;
    }
    case 'b': {
      if (abs(move[1] - move[3 + l]) != abs(move[2] - move[4 + l])) return false;
      if ((abs(move[1] - move[3 + l]) > 1 || abs(move[2] - move[4 + l]) > 1)) {
        // Check if the path is clear by recursively checking the move to every square between the two points, within
        // the bishops movement rules
        if (move[1] > move[3 + l]) move1[3]++;
        if (move[1] < move[3 + l]) move1[3]--;
        if (move[2] > move[4 + l]) move1[4]++;
        if (move[2] < move[4 + l]) move1[4]--;

        if (!Piece::move(move1, board, curPiece, width, turn, lastMove, castling)) return false;
      }
      break;
    }
    case 'q': {
      if (move[1] != move[3 + l] && move[2] != move[4 + l] && abs(move[1] - move[3 + l]) != abs(move[2] - move[4 + l]))
        return false;

      if ((abs(move[1] - move[3 + l]) > 1 || abs(move[2] - move[4 + l]) > 1)) {
        // Check if the path is clear by recursively checking the move to every square between the two points, within
        // the bishops and rooks movement rules, to simulate the queen's movement
        if (move[1] > move[3 + l]) move1[3]++;
        if (move[1] < move[3 + l]) move1[3]--;
        if (move[2] > move[4 + l]) move1[4]++;
        if (move[2] < move[4 + l]) move1[4]--;

        if (!Piece::move(move1, board, curPiece, width, turn, lastMove, castling)) return false;
      }
      break;
    }
    case 'k': {
      if (move.length() == 6 && isupper(move[0]) == isupper(board[toCord]) && board[toCord] != (turn == 0 ? 'r' : 'R'))
        return false;
      if (abs(move[1] - move[3 + l]) > 1 || abs(move[2] - move[4 + l]) > 1) {
        // Seperate check for castling moves
        if ((castling[1] == 0 && move.compare("K47x77") == 0 && board[63] == 'R' && board[62] == ' ' &&
             board[61] == ' ') ||
            (move.compare("K47x07") == 0 && board[56] == 'R' && castling[0] == 0 && board[57] == ' ' &&
             board[58] == ' ' && board[59] == ' ') ||
            (move.compare("k40x70") == 0 && board[7] == 'r' && castling[3] == 0 && board[6] == ' ' &&
             board[5] == ' ') ||
            (move.compare("k40x00") == 0 && board[0] == 'r' && castling[2] == 0 && board[1] == ' ' && board[2] == ' ' &&
             board[3] == ' ')) {
          return true;
        }
        return false;
        break;
      } else if (isupper(move[0]) == isupper(board[toCord])) {
        return false;
      }
    }
  }
  return true;
}
