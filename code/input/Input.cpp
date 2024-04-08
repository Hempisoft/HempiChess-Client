#include "./Input.h"

/**
 * @brief Constructs an Input object.
 *
 * @param mBoard Pointer to the Board object.
 * @param mWidth The width of the input.
 * @param mHeight The height of the input.
 */
Input::Input(Board* mBoard, int mWidth, int mHeight) : board(mBoard), width(mWidth), height(mHeight) {
  clicked = false;
  draggingFigure = false;
  time[0] = 0;
  time[1] = 0;
  maxTime = 300;
  test = false;
}

/**
 * Handles the mouse down event.
 *
 * @param x The x-coordinate of the mouse click.
 * @param y The y-coordinate of the mouse click.
 * @return True if the event was handled successfully, false otherwise.
 */
bool Input::handleMouseDown(int x, int y) {
  int xPos = (height - 100) / 2;
  int yPos = 50;

  // Deseslect current piece, if the user clicks outside the board
  if (!(x > xPos && y > yPos && x < xPos + width && y < yPos + width)) {
    board->setSelectedPiece(-1);
    return false;
  }
  clicked = true;
  // Construct the square, on which was clicked
  square[0] = abs((board->doesRotate() ? board->getTurn() ? 0 : board->getWidth() - 1 : 0) -
                  (int)floor(x - (height - 100) / 2) / ((height - 100) / 8));
  square[1] = abs((board->doesRotate() ? board->getTurn() ? 0 : board->getHeight() - 1 : 0) -
                  (int)floor((y - 50) / ((height - 100) / 8)));

  board->beginMovePiece(square[0], square[1]);

  return true;
}

/**
 * Handles the mouse up event.
 *
 * @param x The x-coordinate of the mouse position.
 * @param y The y-coordinate of the mouse position.
 * @return Returns `false` if the event is handled, `true` otherwise.
 */
bool Input::handleMouseUp(int x, int y) {
  int xPos = (height - 100) / 2;
  int yPos = 50;
  if (board->getEndMessage() != L"") {
    if (x > width / 2 - 105 && x < width / 2 + 55 && y > height / 2 + 30 && y < height / 2 + 70) {
      board->newGame(maxTime);
    }
    return false;
  }

  // Deseslect current piece, if the user clicks outside the board and check for a click on the buttons
  if (!(x > xPos && y > yPos && x < xPos + width && y < yPos + width)) {
    if (x > 86 && x < 164 && y > 100 && y < 140) {
      board->setDoesRotate(!board->doesRotate());
    }
    if (x > 86 && x < 164 && y > 220 && y < 260) {
      board->setShowMoves(!board->doesShowMoves());
    }
    if (x > 45 && x < 205 && y > 290 && y < 330) {
      board->undoMove();
    }
    clicked = false;
    board->setSelectedPiece(-1);
    board->setIsPromoting(false);
    return false;
  }

  clicked = false;
  // Construct the square, on which was clicked
  int square1[2];
  square1[0] =
      abs((board->doesRotate() ? board->getTurn() ? 0 : board->getWidth() - 1 : 0) - (int)floor(x - xPos) / (xPos / 4));
  square1[1] = abs((board->doesRotate() ? board->getTurn() ? 0 : board->getHeight() - 1 : 0) -
                   (int)floor((y - yPos) / (xPos / 4)));

  // Check if the move is valid and execute it
  if (!board->isPromoting()) {
    if (board->movePiece(square[0], square[1], square1[0], square1[1], ' ') && !board->gameStarted) {
      board->gameStarted = true;
    }
  } else {
    board->movePiece(square[0], square[1], square1[0], square1[1],
                     square1[0] == board->getSelectedPiece() % board->getWidth()
                         ? square1[1] == 0                               ? 'Q'
                           : square1[1] == (board->doesRotate() ? 7 : 4) ? 'q'
                           : square1[1] == 1                             ? 'R'
                           : square1[1] == (board->doesRotate() ? 6 : 5) ? 'r'
                           : square1[1] == 2                             ? 'B'
                           : square1[1] == (board->doesRotate() ? 5 : 6) ? 'b'
                           : square1[1] == 3                             ? 'N'
                           : square1[1] == (board->doesRotate() ? 4 : 7) ? 'n'
                                                                         : ' '
                         : ' ');
    board->setIsPromoting(false);
  }

  return true;
}

/**
 * @brief Sets the dimensions of the input.
 *
 * This function sets the width and height of the input.
 *
 * @param mWidth The width of the input.
 * @param mHeight The height of the input.
 */
void Input::setDimensions(int mWidth, int mHeight) {
  width = mWidth;
  height = mHeight;
}

/**
 * Check if a figure is currently being dragged.
 *
 * @return true if a figure is being dragged, false otherwise.
 */
bool Input::isDraggingFigure() { return draggingFigure; }