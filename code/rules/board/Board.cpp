#include "./Board.h"

#include <iostream>
#include <stdexcept>

std::string protoBoard = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

/**
 * @brief Constructs a Board object with the specified width and height.
 *
 * @param pWidth The width of the board.
 * @param pHeight The height of the board.
 *
 * @throws std::runtime_error if the width or height is negative or if they are not equal.
 */
Board::Board(int pWidth, int pHeight)
    : width(pWidth), height(pHeight), style{255, 168, 139, 103}, maxTime(600.0), castling{0, 0, 0, 0} {
  // Initialize the board dimensions and setup the board
  if (pWidth < 0 || pHeight < 0)
    throw std::runtime_error("Width and height must be positive");
  else if (pWidth != pHeight)
    throw std::runtime_error("Width and height must be equal");
  setup(protoBoard);

  // Initialize the standart board state
  piece = new Piece();
  selectedPiece = -1;
  turn = true;
  rotate = true;
  showMoves = true;
  promoting = false;
  passant = false;
  drawCounter = 0;
  lastMove = "";
  gameStarted = false;
  gameEnded = L"";
  time[0] = maxTime;
  time[1] = maxTime;
  fen = protoBoard;
}

/**
 * @brief Destructor for the Board class.
 *
 * This destructor is responsible for deleting the dynamically allocated piece object.
 *
 */
Board::~Board() {}

/**
 * @brief Sets up the board based on the given FEN (Forsyth-Edwards Notation) string.
 *
 * @param fen The FEN string representing the board configuration.
 * @return True if the board setup is successful, false otherwise.
 *
 * @details The function parses the FEN string and sets up the board accordingly. The FEN string
 * should only contain valid characters for chess pieces ('K', 'Q', 'R', 'B', 'N', 'P', 'k', 'q', 'r', 'b', 'n', 'p'),
 * digits ('1' to '8'), slashes ('/'), and the active color indicator ('w' or 'b').
 *
 * The function iterates through each character in the FEN string and constructs the board representation.
 * If any invalid characters are found in the FEN string, an exception of type std::runtime_error is thrown.
 * If the FEN string does not match the dimensions of the board, another exception is thrown.
 *
 * If an exception is caught during the setup process, the function prints the error message to the standard error
 * stream and restores the previous board configuration. The function then returns false to indicate a failed setup.
 *
 * After a successful setup, the visualBoard member variable is updated to match the new board configuration.
 *
 * @see https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
 */
bool Board::setup(std::string fen) {
  board = "";

  int count = 0;
  try {
    for (int i = 0; i < fen.length(); i++) {
      if (fen.find_first_not_of("KQRBNPkqrbnp12345678/-w") != std::string::npos)
        throw std::runtime_error("Invalid Characters in FEN");
      if (fen[i] == '/') {
        if (count != width) throw std::runtime_error("FEN does not match board dimensions");
        count = 0;
        continue;
      }
      if (fen[i] > '0' && fen[i] <= '0' + width) {
        for (int j = 0; j < fen[i] - '0'; j++) {
          count++;
          board += " ";
        }
      } else {
        count++;
        board += fen[i];
      }
    }
    if (board.length() != width * height) throw std::runtime_error("FEN ending does not match board dimensions");
  } catch (std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    board = visualBoard;
    return false;
  }
  visualBoard = board;
  return true;
}

/**
 * Calculates the material value of the board for a given player's turn.
 *
 * @param turn A boolean value indicating the player's turn. True for white, false for black.
 * @return The material value of the board for the specified player's turn.
 */
int Board::material(bool turn) {
  int material = 0;
  for (int i = 0; i < board.length(); i++) {
    if ((isupper(board[i]) == 0) == turn) {
      material += piece->getValue(board[i]);
    }
  }
  return material;
}

/**
 * Initializes the necessary values to prepare moving a chess piece.
 *
 * @param fromX The x-coordinate of the piece to move.
 * @param fromY The y-coordinate of the piece to move.
 */
void Board::beginMovePiece(int fromX, int fromY) {
  if (gameEnded != L"") return;
  if (promoting) return;
  if (fromX < 0 || fromY < 0 || fromX >= width || fromY >= height) {
    selectedPiece = -1;
    return;
  };
  if (visualBoard != board) visualBoard = board;
  if (board[fromY * width + fromX] != ' ') {
    selectedPiece = fromX + fromY * width;
  }
  curPiece = board[selectedPiece];
  visualBoard = visualBoard.substr(0, fromY * width + fromX) + " " + visualBoard.substr(fromY * width + fromX + 1);
}

/**
 * Moves a chess piece on the board.
 *
 * @param fromX The x-coordinate of the piece's current position.
 * @param fromY The y-coordinate of the piece's current position.
 * @param toX The x-coordinate of the piece's target position.
/**
 * Moves a piece on the board from one position to another.
 *
 * @param fromX The x-coordinate of the piece's current position.
 * @param fromY The y-coordinate of the piece's current position.
 * @param toX The x-coordinate of the piece's target position.
 * @param toY The y-coordinate of the piece's target position.
 * @param promotionPiece The piece to promote to, if applicable.
 * @return True if the move was successful, false otherwise.
 */
bool Board::movePiece(int fromX, int fromY, int toX, int toY, char promotionPiece) {
  // Check if the game has already ended
  if (gameEnded != L"") return false;

  // Check if the coordinates are within the board boundaries
  if (fromX < 0 || fromY < 0 || fromX >= width || fromY >= height || toX < 0 || toY < 0 || toX >= width ||
      toY >= height) {
    visualBoard = board;
    selectedPiece = -1;
    promoting = false;
    return false;
  }

  // Calculate the position of the source and target squares
  int posFrom = (fromY * width + fromX);
  if (selectedPiece != -1) {
    posFrom = selectedPiece;
    fromY = ceil(posFrom / width);
    fromX = posFrom % width;
  }
  int posTo = toY * width + toX;

  // Check if the move is a selection of the same piece without promotion
  if (posFrom == posTo && !promoting) {
    visualBoard = board;
    selectedPiece = posFrom < board.length() ? posFrom : -1;
    return false;
  }

  // Check if the move is a pawn promotion
  if ((toY == 0 || toY == 7) && (curPiece == 'P' || curPiece == 'p') && !promoting) {
    tempMove = curPiece + std::to_string(fromX) + std::to_string(fromY) + (visualBoard[posTo] != ' ' ? "x" : "") +
               std::to_string(toX) + std::to_string(toY) + "=";
    promoting = true;
    selectedPiece = posFrom;
    return false;
  }

  // Reset en passant flag
  piece->setPassant(false);

  // Move the piece and check if the move is valid
  if (!piece->move(promotionPiece != ' '
                       ? (tempMove + promotionPiece)
                       : (curPiece + std::to_string(fromX) + std::to_string(fromY) +
                          (visualBoard[posTo] != ' ' ? "x" : "") + std::to_string(toX) + std::to_string(toY)),
                   visualBoard, curPiece, width, turn, lastMove, castling)) {
    visualBoard = visualBoard.substr(0, posFrom) + curPiece + visualBoard.substr(posFrom + 1);
    return false;
  }

  // Update en passant flag
  if (piece->getPassant()) {
    passant = true;
  }

  // Update position if promoting
  if (promoting) {
    posFrom = tempMove[1] - '0' + (tempMove[2] - '0') * width;
    posTo = tempMove[tempMove[3] == 'x' ? 4 : 3] - '0' + (tempMove[tempMove[3] == 'x' ? 5 : 4] - '0') * width;
  }

  // Save the current board state for undo
  undo.push_back(board);

  // Handle en passant capture
  if (passant) {
    visualBoard[lastMove[3] - '0' + (lastMove[4] - '0') * width] = ' ';
    passant = false;
  }

  // Update last move
  lastMove = curPiece + std::to_string(fromX) + std::to_string(fromY) + std::to_string(toX) + std::to_string(toY);

  // Update castling availability
  if (curPiece == 'K' && (castling[0] == 0 || castling[1] == 0)) {
    castling[0] = undo.size();
    castling[1] = undo.size();
  } else if (curPiece == 'k' && (castling[2] == 0 || castling[3] == 0)) {
    castling[2] = undo.size();
    castling[3] = undo.size();
  } else if (tolower(curPiece) == 'r') {
    if (fromX == 0 || fromX == 7) {
      if (castling[(turn ? 0 : 2) + (fromX == 0 ? 0 : 1)] == 0)
        castling[(turn ? 0 : 2) + (fromX == 0 ? 0 : 1)] = undo.size();
    }
  }

  // Save the last move for undo
  undoMoves.push_back(lastMove);

  // Update the visual board after the move
  if (posFrom < posTo)
    visualBoard = visualBoard.substr(0, posFrom) + " " + visualBoard.substr(posFrom + 1, posTo - posFrom - 1) +
                  (promotionPiece != ' ' ? promotionPiece : curPiece) + visualBoard.substr(posTo + 1);
  else if (posFrom > posTo)
    visualBoard = visualBoard.substr(0, posTo) + (promotionPiece != ' ' ? promotionPiece : curPiece) +
                  visualBoard.substr(posTo + 1, posFrom - posTo - 1) + " " + visualBoard.substr(posFrom + 1);

  std::vector<std::vector<int>> moves = piece->testAvailableMoves(board, !turn, lastMove, castling);
  // Unecessarily long check if castling is obstructed by checks.

  if (lastMove.compare("K4777") == 0 || lastMove.compare("K4707") == 0 || lastMove.compare("k4070") == 0 ||
      lastMove.compare("k4000") == 0) {
    if (piece->testCheck(board, turn, lastMove, castling)) {
      undoMove();
      drawCounter++;
      turn = !turn;
      return false;
    }
    for (int i = 0; i < moves.size(); i++) {
      if ((lastMove.compare("K4777") == 0 && (std::find(moves[i].begin(), moves[i].end(), 62) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 61) != moves[i].end())) ||
          (lastMove.compare("K4707") == 0 && (std::find(moves[i].begin(), moves[i].end(), 59) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 58) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 57) != moves[i].end())) ||
          (lastMove.compare("k4070") == 0 && (std::find(moves[i].begin(), moves[i].end(), 6) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 5) != moves[i].end())) ||
          (lastMove.compare("K4000") == 0 && (std::find(moves[i].begin(), moves[i].end(), 3) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 2) != moves[i].end() ||
                                              std::find(moves[i].begin(), moves[i].end(), 1) != moves[i].end()))) {
        undoMove();
        drawCounter++;
        turn = !turn;
        return false;
      }
    }
    if (lastMove.compare("K4777") == 0)
      visualBoard = visualBoard.substr(0, 60) + " RK ";
    else if (lastMove.compare("K4707") == 0)
      visualBoard = visualBoard.substr(0, 56) + "  KR " + visualBoard.substr(61);
    else if (lastMove.compare("k4070") == 0)
      visualBoard = visualBoard.substr(0, 4) + " kr " + visualBoard.substr(8);
    else if (lastMove.compare("k4000") == 0)
      visualBoard = "  kr " + visualBoard.substr(5);
  }

  // Test if the move puts the current player's king in check
  moves = piece->testAvailableMoves(board, !turn, lastMove, castling);
  if (piece->testCheck(visualBoard, turn, lastMove, castling)) {
    undoMove();
    drawCounter++;
    turn = !turn;
    return false;
  }

  // Update the board state
  board = visualBoard;

  // Check for end game conditions
  if (material(!turn) + material(turn) < 5) {
    endGame(false, false, false);
    return false;
  }

  // Check for draw by repetition
  for (int i = 0; i < undo.size(); i++) {
    for (int j = 0; j < undo.size(); j++) {
      for (int l = 0; l < undo.size(); l++) {
        if (undo[i] == undo[j] && undo[i] == undo[l] && i != j && i != l && j != l) {
          endGame(true, false, false);
          return false;
        }
      }
    }
  }

  // Test available moves for the opponent
  moves = piece->testAvailableMoves(board, !turn, lastMove, castling);

  // Check for draw by insufficient material or 50-move rule
  if (moves.size() == 0 || drawCounter >= 50) {
    endGame(false, false, false);
    return false;
  }

  // Update game state
  promoting = false;
  drawCounter++;
  turn = !turn;

  return true;
}

/**
 * Undo the last move made on the board.
 * If there are no moves to undo, the function returns immediately.
 */
void Board::undoMove() {
  if (undo.empty() || undoMoves.empty()) return;
  board = undo[undo.size() - 1];
  undoMoves.pop_back();
  lastMove = undoMoves.size() > 0 ? undoMoves[undoMoves.size() - 1] : "";
  undo.pop_back();
  if (undo.empty()) {
    newGame(maxTime);
    return;
  }
  for (int i = 0; i < 4; i++) {
    if (castling[i] >= undo.size()) castling[i] = 0;
  }
  visualBoard = board;
  turn = !turn;
  drawCounter--;
  promoting = false;
  passant = false;
}

/**
 * Ends the game and displays the result.
 *
 * @param repetition Indicates if the game ended due to repetition.
 * @param timeOut Indicates if the game ended due to timeout.
 */
void Board::endGame(bool repetition, bool timeOut, bool resignation) {
  // Check for end game conditions
  if (repetition) {
    gameEnded = L"Draw by repetition!";
  } else if (timeOut) {
    gameEnded = (material(!turn) > 4 ? (turn ? L"Black wins" : L"White wins") : L"Draw by insufficient \n material");
    gameEnded += L" by Timeout!";
  } else if (drawCounter >= 50) {
    gameEnded = L"Draw by 50-move rule!";
  } else if (material(true) + material(false) < 5) {
    gameEnded = L"Draw by insufficient \n material!";
  } else if (piece->testCheck(visualBoard, !turn, lastMove, castling)) {
    gameEnded = (turn ? L"White" : L"Black");
    gameEnded += L" wins \n by Checkmate!";
  } else {
    gameEnded = L"Draw by stalemate!";
  }

  // Reset game state
  undo = std::vector<std::string>();
  undoMoves = std::vector<std::string>();
  promoting = false;
  turn = true;
  gameStarted = false;
}

/**
 * Starts a new game with the specified FEN (Forsyth-Edwards Notation) string and maximum time.
 *
 * @param fen The FEN string representing the initial position of the game.
 * @param maxTimeT The maximum time allowed for each move in the game.
 */
void Board::newGame(double maxTimeT) {
  if (gameStarted) endGame(false, false, true);
  if (!setup(fen)) setup(protoBoard);
  for (int i = 0; i < 4; i++) castling[i] = 0;
  maxTime = maxTime;
  time[0] = maxTime;
  time[1] = maxTime;
  gameEnded = L"";
}

/**
 * @brief Setters of the Board class.
 *
 * */
void Board::setStyle(int pStyle[4]) {
  for (int i = 0; i < 4; i++) style[i] = pStyle[i];
}

std::vector<std::vector<int>> Board::testAvailableMoves() {
  return piece->testAvailableMoves(board, turn, lastMove, castling);
};

void Board::setDoesRotate(bool pRotate) { rotate = pRotate; }

void Board::setShowMoves(bool pShowMoves) { showMoves = pShowMoves; }

void Board::setSelectedPiece(int pSelectedPiece) { selectedPiece = pSelectedPiece; }

void Board::setIsPromoting(bool pPromoting) { promoting = pPromoting; }

void Board::setMaxTime(double pMaxTime) { maxTime = pMaxTime; }

void Board::setTime(double* pTime) {
  time[0] = pTime[0];
  time[1] = pTime[1];
}

void Board::setFen(wchar_t pFen[72]) {
  fen = "";
  std::wstring temp(pFen);
  for (int i = 0; i < temp.length(); i++) fen += pFen[i];
  setup(fen);
}

/**
 * @brief Getters of the Board class.
 *
 * */
std::string Board::getBoard() { return board; }

int Board::getSelectedPiece() { return selectedPiece; }

std::string Board::getVisualBoard() { return visualBoard; }

bool Board::doesRotate() { return rotate; }

bool Board::doesShowMoves() { return showMoves; }

bool Board::getTurn() { return turn; }

int Board::getWidth() { return width; }

int Board::getHeight() { return height; }

bool Board::isPromoting() { return promoting; }

double Board::getMaxTime() { return maxTime; }

int Board::getMoveCount() { return undo.size(); }

std::atomic<double>* Board::getTime() { return time; }

std::wstring Board::getEndMessage() { return gameEnded; }

std::wstring Board::getFen() { return std::wstring(fen.begin(), fen.end()); }