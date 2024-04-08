#define UNICODE

#include "./window.h"

int mouseCoords[2];
Paint* wPaint;
Input* wInput;
bool clicked;
HWND mainWindow;
HWND hEdit;
HMENU hMenu;

/**
 * @brief This function is responsible for performing the drawing procedure on the specified window.
 *
 * @param hWnd The handle to the window on which the drawing procedure is performed.
 * @param lpPS A pointer to a PAINTSTRUCT structure that contains information about the painting request.
 */
void Window::DrawingProcedure(HWND hWnd, LPPAINTSTRUCT lpPS) {
  RECT rc;
  HDC hdcMem;
  HBITMAP hbmMem, hbmOld;

  BeginPaint(hWnd, lpPS);

  GetClientRect(hWnd, &rc);

  // Create an identical copy of the window's device context
  hdcMem = CreateCompatibleDC(lpPS->hdc);

  hbmMem = CreateCompatibleBitmap(lpPS->hdc, rc.right - rc.left, rc.bottom - rc.top);

  hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

  Gdiplus::Graphics graphics(hdcMem);

  // Draw all the windows components
  wPaint->drawBgd(&graphics);
  wPaint->drawBoard(&graphics);
  wPaint->drawMoveOptions(&graphics);
  wPaint->drawPieces(&graphics);
  wPaint->drawButtons(&graphics);
  wPaint->drawPromotionMenu(&graphics);
  wPaint->drawTimer(&graphics);
  wPaint->drawEndingScreen(&graphics);
  if (clicked) wPaint->drawDraggedPiece(&graphics, mouseCoords[0], mouseCoords[1]);

  BitBlt(lpPS->hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

  SelectObject(hdcMem, hbmOld);
  DeleteObject(hbmMem);
  DeleteDC(hdcMem);

  EndPaint(hWnd, lpPS);
}

/**
 * @brief The return type of the window procedure callback function.
 *
 * The LRESULT type represents the result of processing a window message in the window procedure callback function.
 * It is a signed integer type that can hold either a message-specific value or a pointer to a structure or object.
 * The window procedure callback function should return an LRESULT value that is specific to the message being
 * processed. The DefWindowProc function uses the return value to perform default processing of the message.
 *
 * @param hwnd The handle to the window procedure that received the message.
 * @param uMsg The message identifier.
 * @param wParam Additional message information. The contents of this parameter depend on the value of the uMsg
 * parameter.
 * @param lParam Additional message information. The contents of this parameter depend on the value of the uMsg
 * parameter.
 * @return The result of processing the window message.
 */
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  HDC hdc;
  PAINTSTRUCT ps;

  switch (uMsg) {
    case WM_COMMAND:
      switch (wParam) {
        case 1:
          wPaint->getBoard()->endGame(false, false, false);
          wPaint->getBoard()->newGame(600);
          break;
        case 2:
          displayDialog(hwnd);
          break;
        case 3:
          wPaint->getBoard()->undoMove();
          break;
        case 4:
          DestroyWindow(hwnd);
          break;
      }
      return 0;
    case WM_MOUSEMOVE:
      if (!clicked) {
        return 0;
      } else {
        mouseCoords[0] = GET_X_LPARAM(lParam);
        mouseCoords[1] = GET_Y_LPARAM(lParam);
        InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;
    case WM_ACTIVATE:
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_CREATE:
      AddMenus(hwnd);
      return 0;
    case WM_LBUTTONDOWN:
      clicked = true;
      wInput->handleMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      mouseCoords[0] = GET_X_LPARAM(lParam);
      mouseCoords[1] = GET_Y_LPARAM(lParam);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_LBUTTONUP:
      clicked = false;
      wInput->handleMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_PAINT:
      DrawingProcedure(hwnd, &ps);
      return 0;
    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
    case WM_ERASEBKGND:
      return 1;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/**
 * @brief Constructs a Window object.
 *
 * This constructor initializes a Window object with the given Paint and Input objects.
 * It also initializes GDI+ and sets up the window class and style.
 *
 * @param mPaint A pointer to the Paint object.
 * @param mInput A pointer to the Input object.
 */
Window::Window(Board* mBoard) : h_hInstance(GetModuleHandle(nullptr)) {
  clicked = false;

  // Initializing GDI+
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

  // Initializing window class
  WNDCLASS wndClass = {};
  const wchar_t CLASS_NAME[] = L"Sample Window Class";
  wndClass.lpszClassName = CLASS_NAME;
  wndClass.hInstance = h_hInstance;
  wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClass.lpfnWndProc = WindowProc;

  // Register the window and dialog classes
  RegisterClass(&wndClass);
  registerDialogClass(h_hInstance);

  // Initializing window style
  DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

  // Initializing window size by 80% of the desktop size
  RECT desktop;
  const HWND hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);

  width = fmin(desktop.right * 0.8, 1500);
  height = fmin(desktop.bottom * 0.8, 800);
  wPaint = paint = new Paint(mBoard, width, height);
  wInput = input = new Input(mBoard, width, height);

  RECT rect;
  rect.left = 100;
  rect.top = 100;
  rect.right = rect.left + width;
  rect.bottom = rect.top + height;

  // Create and show the window
  AdjustWindowRect(&rect, style, false);

  mainWindow = h_hWnd = CreateWindowEx(0, CLASS_NAME, L"Chess game", style, rect.left, rect.top, rect.right - rect.left,
                                       rect.bottom - rect.top, nullptr, nullptr, h_hInstance, nullptr);

  ShowWindow(h_hWnd, SW_SHOW);
  ShowWindow(GetConsoleWindow(), SW_HIDE);
}

/**
 * @brief Destructor for the Window class.
 *
 * This destructor is responsible for unregistering the window class.
 *
 * @remarks The destructor assumes that the window class has been previously registered using the `RegisterClass`
 * function.
 */
Window::~Window() {
  const wchar_t* CLASS_NAME = L"Sample Window Class";
  UnregisterClass(CLASS_NAME, h_hInstance);

  // Clean up resources
  DestroyMenu(hMenu);
  delete wPaint;
  delete paint;
}

/**
 * Adds controls to the window.
 *
 * This function creates and adds several controls to the specified window.
 * The controls include two buttons, a static label, and an edit box.
 *
 * @param hDlg The handle to the window to which the controls will be added.
 */
void Window::AddControls(HWND hDlg) {
  CreateWindowW(L"Button", L"OK", WS_VISIBLE | WS_CHILD, 30, 100, 100, 30, hDlg, (HMENU)1, NULL, NULL);
  CreateWindowW(L"Button", L"Cancel", WS_VISIBLE | WS_CHILD, 160, 100, 100, 30, hDlg, (HMENU)2, NULL, NULL);
  CreateWindowW(L"Static", L"Edit Fen:", WS_VISIBLE | WS_CHILD, 30, 10, 230, 30, hDlg, (HMENU)3, NULL, NULL);
  hEdit = CreateWindowW(L"Edit", wPaint->getBoard()->getFen().c_str(),
                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 30, 30, 230, 50, hDlg,
                        (HMENU)4, NULL, NULL);
}

/**
 * @brief Callback function for the dialog window procedure.
 *
 * This function handles the events sent to the dialog window.
 *
 * @param hwnd The handle to the dialog window.
 * @param message The message identifier.
 * @param wParam Additional message information.
 * @param lParam Additional message information.
 * @return The result of the message processing and depends on the message sent.
 */
LRESULT CALLBACK Window::DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      EnableWindow(mainWindow, true);
      DestroyWindow(hwnd);
    case WM_COMMAND:
      switch (wParam) {
        case 1:
          // Set the board's FEN to the text in the edit box
          wchar_t text[71];
          GetWindowTextW(hEdit, text, 72);
          wPaint->getBoard()->setFen(text);
          // Close the dialog window
          EndDialog(hwnd, 0);
          EnableWindow(mainWindow, true);
          DestroyWindow(hwnd);
          break;
        case 2:
          // Close the dialog window
          EndDialog(hwnd, 0);
          EnableWindow(mainWindow, true);
          DestroyWindow(hwnd);
          break;
      }
      break;
    default:
      return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}

/**
 * Registers the dialog class for the window.
 *
 * @param hInstance The handle to the instance of the application.
 */
void Window::registerDialogClass(HINSTANCE hInstance) {
  WNDCLASS dialog = {0};

  dialog.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  dialog.lpfnWndProc = DefDlgProc;
  dialog.hInstance = hInstance;
  dialog.lpszClassName = L"DialogClass";
  dialog.lpfnWndProc = DialogProc;

  RegisterClass(&dialog);
}

/**
 * Displays the FEN editor dialog window.
 *
 * @param hWnd The handle to the parent window.
 */
void Window::displayDialog(HWND hWnd) {
  HWND hDlg = CreateWindowW(L"DialogClass", L"Edit Fen", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 100, 100, 300, 200, hWnd,
                            NULL, NULL, NULL);

  AddControls(hDlg);

  EnableWindow(hWnd, FALSE);
}

/**
 * @brief Adds menus to the specified window.
 *
 * This function creates a menu and adds it to the specified window.
 * The menu contains options such as "New Game", "Download", "Import Fen", and "Exit".
 *
 * @param hWnd The handle to the window to which the menu will be added.
 */
void Window::AddMenus(HWND hWnd) {
  hMenu = CreateMenu();
  HMENU hFileMenu = CreateMenu();

  AppendMenu(hFileMenu, MF_STRING, 1, L"New Game");
  AppendMenu(hFileMenu, MF_STRING, 2, L"Edit Fen");
  AppendMenu(hFileMenu, MF_STRING, 3, L"Undo");
  AppendMenu(hFileMenu, MF_SEPARATOR, -1, NULL);
  AppendMenu(hFileMenu, MF_STRING, 4, L"Exit");

  AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

  SetMenu(hWnd, hMenu);
}

/**
 * Processes the messages in the message queue for the window.
 * This function retrieves and dispatches messages from the message queue until the queue is empty.
 * If a WM_QUIT message is encountered, it shuts down GDI+ and returns false.
 *
 * @return true if the message processing is successful and the window should continue running,
 *         false if a WM_QUIT message is encountered and the window should be closed.
 */
bool Window::ProcessMessages() {
  MSG msg = {};

  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      Gdiplus::GdiplusShutdown(gdiplusToken);
      return false;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return true;
}

/**
 * @brief Returns the Paint object associated with the window.
 *
 * @return A pointer to the Paint object associated with the window.
 */

Paint* Window::getPaint() { return paint; }

/**
 * @brief Returns the handle to the window.
 *
 * @return The handle to the window.
 */
HWND Window::getHWnd() { return h_hWnd; }