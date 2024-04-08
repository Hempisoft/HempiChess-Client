#define UNICODE

#include "./Paint.h"

#include <filesystem>

/**
 * @brief Constructs a new Paint object.
 *
 * This constructor initializes a Paint object with the specified Board pointer.
 *
 * @param pBoard A pointer to the Board object.
 * @param mWidth The width of the paint area.
 * @param mHeight The height of the paint area.
 */
Paint::Paint(Board* pBoard, int mWidth, int mHeight)
    : width(mWidth), height(mHeight), board(pBoard), brush(Gdiplus::Color(255, 255, 255, 255)) {
  path = L".//graphics//";
  endingFont = new Gdiplus::Font(new Gdiplus::FontFamily(L"Arial"), 16, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
  timerFont = new Gdiplus::Font(new Gdiplus::FontFamily(L"Arial"), 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
  pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
  stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
  stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
}

/**
 * @brief Destroys the Paint object.
 *
 * This destructor releases any resources held by the Paint object.
 */
Paint::~Paint() { delete board; }

/**
 * Draws the background of the window.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object used for drawing.
 */
void Paint::drawBgd(Gdiplus::Graphics* graphics) {
  brush.SetColor(Gdiplus::Color(255, 38, 38, 38));
  graphics->FillRectangle(&brush, 0, 0, width, height);
}

/**
 * Draws the ending screen on the specified graphics object.
 * The ending screen consists of a background image, a title image, showing the result, and a text message for further
 * details.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which to draw the ending screen.
 */
void Paint::drawEndingScreen(Gdiplus::Graphics* graphics) {
  int result = board->getEndMessage().substr(0, 5) == L"White"   ? 1
               : board->getEndMessage().substr(0, 5) == L"Black" ? 2
                                                                 : 0;
  if (board->getEndMessage() == L"") return;
  int x = width / 2 - 125;
  int y = height / 2 - 80;

  Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(
      (path + L"EndingScreens//" + (result == 0 ? L"Draw" : ((result == 1 ? L"White" : L"Black"))) + L"WinPieces.png")
          .c_str());
  graphics->DrawImage(bmp, x, y - 68, 200, 68);

  bmp = new Gdiplus::Bitmap((path + L"EndingScreens//StandartBgd.png").c_str());
  graphics->DrawImage(bmp, x, y, 200, 160);
  bmp = new Gdiplus::Bitmap(
      (path + L"EndingScreens//" + (result == 0 ? L"Draw" : ((result == 1 ? L"White" : L"Black"))) + L"Win.png")
          .c_str());
  graphics->DrawImage(bmp, x + 20, y + 10, 160, 40);
  bmp = new Gdiplus::Bitmap((path + L"Buttons//NewGameBlack.png").c_str());
  graphics->DrawImage(bmp, x + 20, y + 110, 160, 40);

  pointF.X = x + 100;
  pointF.Y = y + 72;
  graphics->DrawString(board->getEndMessage().c_str(), -1, endingFont, pointF, &stringFormat, &brush);
}

/**
 * Draws the timer on the graphics object.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which to draw the timer.
 */
void Paint::drawTimer(Gdiplus::Graphics* graphics) {
  bool color = (board->doesRotate() ? board->getTurn() ? 0 : 1 : 0);
  brush.SetColor(Gdiplus::Color(255, color * 255, color * 255, color * 255));
  pointF.X = width - 200;
  pointF.Y = 100;
  double time[2] = {round((board->getTime()[0].load()) * 10) / 10, round((board->getTime()[1].load()) * 10) / 10};
  double visTime[2] = {time[board->doesRotate() ? !board->getTurn() : 0],
                       time[board->doesRotate() ? board->getTurn() : 1]};
  graphics->FillRectangle(&brush, width - 300, 60, 200, 80);
  brush.SetColor(Gdiplus::Color(255, !color * 255, !color * 255, !color * 255));
  graphics->DrawString((std::to_wstring(static_cast<int>(floor(visTime[0] / 60))) + L":" +
                        std::to_wstring((static_cast<int>(floor(visTime[0]))) % 60) + L"." +
                        std::to_wstring(static_cast<int>(round(visTime[0] * 10)) % 10))
                           .c_str(),
                       -1, timerFont, pointF, &stringFormat, &brush);
  pointF.Y = height - 100;
  graphics->FillRectangle(&brush, width - 300, height - 140, 200, 80);
  brush.SetColor(Gdiplus::Color(255, color * 255, color * 255, color * 255));
  graphics->DrawString((std::to_wstring(static_cast<int>(floor(visTime[1] / 60))) + L":" +
                        std::to_wstring((static_cast<int>(floor(visTime[1]))) % 60) + L"." +
                        std::to_wstring(static_cast<int>(round(visTime[1] * 10)) % 10))
                           .c_str(),
                       -1, timerFont, pointF, &stringFormat, &brush);
}

/**
 * Draws the promotion menu on the graphics object.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which to draw the promotion menu.
 */
void Paint::drawPromotionMenu(Gdiplus::Graphics* graphics) {
  if (!board->isPromoting() || board->getSelectedPiece() == -1) return;
  brush.SetColor(Gdiplus::Color(255, 255, 255, 255));
  // Initialize the location variables
  int piece = board->getSelectedPiece();
  int squareWidth = height - 100;
  int x =
      squareWidth / 2 +
      (abs((board->doesRotate() ? board->getTurn() ? 0 : board->getWidth() - 1 : 0) - (piece % board->getWidth()))) *
          squareWidth / board->getWidth();
  int y = 50 + (floor(piece / board->getWidth()) == 1 ? 0
                : board->doesRotate()                 ? 0
                                                      : (board->getHeight() - 4)) *
                   squareWidth / board->getHeight();
  squareWidth = (height - 100) / board->getWidth();
  graphics->FillRectangle(&brush, x, y, squareWidth, 4 * squareWidth);
  for (int i = 0; i < 4; i++) {
    graphics->DrawRectangle(pen, x, y + i * squareWidth, squareWidth, squareWidth);
  }
  // Construct the path to the piece image
  std::wstring tempPath = path + L"Pieces//";
  if (isupper(board->getBoard()[piece]))
    tempPath += L"w";
  else
    tempPath += L"b";

  // Draw the promotion pieces
  Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap((tempPath + L"q.png").c_str());
  graphics->DrawImage(bmp, x, y, squareWidth, squareWidth);
  bmp = new Gdiplus::Bitmap((tempPath + L"r.png").c_str());
  graphics->DrawImage(bmp, x, y + squareWidth, squareWidth, squareWidth);
  bmp = new Gdiplus::Bitmap((tempPath + L"b.png").c_str());
  graphics->DrawImage(bmp, x, y + 2 * squareWidth, squareWidth, squareWidth);
  bmp = new Gdiplus::Bitmap((tempPath + L"n.png").c_str());
  graphics->DrawImage(bmp, x, y + 3 * squareWidth, squareWidth, squareWidth);
}

/**
 * Draws the piece currently being dragged on the graphics object.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object.
 * @param x The x-coordinate of the dragged piece.
 * @param y The y-coordinate of the dragged piece.
 */
void Paint::drawDraggedPiece(Gdiplus::Graphics* graphics, int x, int y) {
  if (board->getSelectedPiece() == -1) return;
  // Construct the path to the piece image
  std::wstring tempPath = path + L"Pieces//";
  if (isupper(board->getBoard()[board->getSelectedPiece()]))
    tempPath += L"w";
  else
    tempPath += L"b";

  tempPath += std::wstring(1, board->getBoard()[board->getSelectedPiece()]);
  tempPath += L".png";

  Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(tempPath.c_str());
  graphics->DrawImage(bmp, x - 45, y - 45, 90, 90);
}

/**
 * Draws the move options on the graphics object.
 * If the board does not show moves or no piece is selected, the function returns early.
 * The move options are determined based on the selected piece and the available moves on the board.
 * The move options are filled rectangles on the graphics object, represented by a solid brush.
 * The position and size of each rectangle is calculated based on the board's properties and the moves vector.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which the move options will be drawn.
 */
void Paint::drawMoveOptions(Gdiplus::Graphics* graphics) {
  if (!board->doesShowMoves()) return;
  std::vector<int> moves;
  if (board->getSelectedPiece() != -1) {
    int invert = board->getWidth() * board->getHeight() - 1;
    int j;
    brush.SetColor(Gdiplus::Color(120, 219, 2, 2));
    for (int i = 0; i < board->testAvailableMoves().size(); i++) {
      j = board->doesRotate() ? board->getTurn() ? i : (invert - i) : i;
      if (board->testAvailableMoves()[i][0] == board->getSelectedPiece()) {
        moves = board->testAvailableMoves()[i];
        break;
      }
    }
    int width = height - 100;
    int x = width / 2;
    int y = 50;
    for (int i = 0; i < moves.size(); i++) {
      graphics->FillRectangle(&brush,
                              x + (abs((board->doesRotate() ? board->getTurn() ? 0 : board->getWidth() - 1 : 0) -
                                       (moves[i] % board->getWidth()))) *
                                      width / board->getWidth(),
                              y + (abs((board->doesRotate() ? board->getTurn() ? 0 : board->getHeight() - 1 : 0) -
                                       ceil(moves[i] / board->getWidth()))) *
                                      width / board->getHeight(),
                              width / board->getWidth(), width / board->getHeight());
    }
  }
}

/**
 * Draws the game board on the specified graphics object.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which to draw the board.
 */
void Paint::drawBoard(Gdiplus::Graphics* graphics) {
  brush.SetColor(Gdiplus::Color(board->style[0], board->style[1], board->style[2], board->style[3]));
  // Initialize the location variables
  int x = (height - 100) / 2;
  int y = 50;
  int width = height - 100;
  int w = board->getWidth();
  graphics->FillRectangle(&brush, x, y, width, width);
  brush.SetColor(Gdiplus::Color(123, 255, 255, 255));
  for (int i = 0; i < board->getWidth(); i++) {
    for (int j = 0; j < board->getHeight(); j++) {
      if ((i + j) % 2 == 0)
        graphics->FillRectangle(&brush, x + i * width / board->getWidth(), y + j * width / board->getHeight(),
                                width / board->getWidth(), width / board->getHeight());
    }
  }
}

/**
 * Draws the pieces on the graphics object.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which the pieces will be drawn.
 */
void Paint::drawPieces(Gdiplus::Graphics* graphics) {
  // Initialize the location variables
  int x = (height - 100) / 2;
  int y = 50;
  int width = height - 100;
  int invert = board->getWidth() * board->getHeight() - 1;
  int i;
  for (int j = 0; j < board->getVisualBoard().length(); j++) {
    i = board->doesRotate() ? board->getTurn() ? j : (invert - j) : j;
    if (board->getVisualBoard()[i] == ' ') continue;

    // Construct the path to the piece image
    std::wstring tempPath = path + L"Pieces//";
    if (isupper(board->getVisualBoard()[i]))
      tempPath += L"w";
    else
      tempPath += L"b";

    tempPath += std::wstring(1, board->getVisualBoard()[i]);
    tempPath += L".png";

    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(tempPath.c_str());
    graphics->DrawImage(bmp, x + (j % board->getWidth()) * width / board->getWidth(),
                        y + (j / board->getWidth()) * width / board->getHeight(), width / board->getWidth(),
                        width / board->getHeight());
  }
}

/**
 * @brief Draws the buttons on the graphics object.
 *
 * This function is responsible for drawing the buttons on the specified graphics object.
 * It takes the width and height of the window as parameters to calculate the position of the buttons.
 * The buttons are drawn using the specified graphics object and the corresponding bitmaps.
 *
 * @param graphics A pointer to the Gdiplus::Graphics object on which the buttons will be drawn.
 */
void Paint::drawButtons(Gdiplus::Graphics* graphics) {
  int x = 50;
  int y = 50;
  int width = 150;
  std::wstring tempPath = path + L"Buttons//";

  Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap((tempPath + L"TurnBoardBlackBig.png").c_str());
  graphics->DrawImage(bmp, x, y, width, 40);
  bmp = new Gdiplus::Bitmap(
      (tempPath + (board->doesRotate() ? L"SwitchStandartOn.png" : L"SwitchStandartOff.png")).c_str());
  graphics->DrawImage(bmp, x + width / 2 - 39, y + 50, 78, 40);

  bmp = new Gdiplus::Bitmap((tempPath + L"MoveOptionsBlack.png").c_str());
  graphics->DrawImage(bmp, x - 25, y + 120, 200, 40);
  bmp = new Gdiplus::Bitmap(
      (tempPath + (board->doesShowMoves() ? L"SwitchStandartOn.png" : L"SwitchStandartOff.png")).c_str());
  graphics->DrawImage(bmp, x + width / 2 - 39, y + 170, 78, 40);

  bmp = new Gdiplus::Bitmap((tempPath + L"UndoMoveBlack.png").c_str());
  graphics->DrawImage(bmp, x - 5, y + 240, 160, 40);
}

/**
 * @brief Get the board object.
 *
 * @return Board* A pointer to the board object.
 */
Board* Paint::getBoard() { return board; }

/**
 * @brief Set the dimensions of the paint area.
 *
 * This function is responsible for setting the dimensions of the paint area.
 * It takes the width and height of the paint area as parameters and sets the corresponding member variables.
 *
 * @param pWidth The width of the paint area.
 * @param pHeight The height of the paint area.
 */
void Paint::setDimensions(int pWidth, int pHeight) {
  width = pWidth;
  height = pHeight;
}