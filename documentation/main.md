# Project Documentation

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Classes](#classes)
- [Notable functions](#notable-functions)
- [Implementation of University concepts](#implementation-of-university-concepts)
- [Video](#video)

## Introduction
This project aims to create a graphic representation of a playable chess game. It further includes various features besides the playable chess game, such as quality of life options like board rotation and visual movement options for selected pieces, and the ability to edit the FEN of the board. This documentation will cover the classes involved in the project, notable functions, and the implementation of various concepts such as recursion, modularity and multithreading.

## Features
As mentioned in the [Introduction](#introduction), the Project entails a graphic visualization of playable chess, using the [Win32-API](https://de.wikipedia.org/wiki/Windows_Application_Programming_Interface) and the [GdiPlus-Library](https://learn.microsoft.com/de-de/windows/win32/gdiplus/-gdiplus-gdi-start). 

In more detail, there are the following features:
- Playable chess via mouse inputs, with the standart [FIDE-Chess-Ruleset](https://www.fide.com/FIDE/handbook/LawsOfChess.pdf)
- Quality of life Options 
    - A setting for having the board turn, depending on which players turn it is
    - A setting for visually showing the movement options for the selected piece
    - An Undo Button
- An option for editing the [FEN](https://de.wikipedia.org/wiki/Forsyth-Edwards-Notation) of the Board
- A Timer for Timed chess

## Classes
The project entails the following classes:
- [Project](#project) 
- [Window](#window)
- [Paint](#paint)
- [Input](#input)
- [Board](#board)
- [Piece](#piece)

### Project
The Project class includes the main function, which creates the main instances of all other classes and manages them, as well as the Timer Thread.

### Window
The Window class creates the Window, using the Win-32 API. All interactions with the Window are caught in the Windows [Window Procedure](#window-procedure), which relays all necessary included information to the [Input class](#input), where the handling of the interactions proceed. The Window class also manages all aspects of the program, which utilize the Window API, and decides, when to call the [Paint](#paint) class.

### Paint
The Paint class handles all actions, which require the gdiplus library, which means, the Paint class is responsible for all Events, realted to anything visual on the Windows GUI surface. All visible elements of the Window are created and drewn on the windows graphics-object, by the Paint classes functions.

### Input
The Input class, as mentioned in the [Window classes description](#window) handles the necessary interactions with the Window, specifically the Mouse-clicks and dragging events are managed by the Input classes functions. The Input class further converts the coordinates, from the mouse clicks into the clicked board squares and relays this more usable information to the [Board class](#board), where the game logic is handeled. 

### Board
The Board class handles the game logic of chess and saves all values connected to it. It utilizes the [piece class](#piece) to complement the movement rules. 

### Piece
The Piece class checks for the movement rules of all the different pieces. It also allocates material values to all pieces and tests positions for checks and available moves, according to movement rules, by using the [move](#move), and  [TestAvailableMoves](#testavailablemoves) functions. It is initialized by the Board class and exclusively called by it as well.

## Notable functions

### WindowProc
The Window Procedure is called everytime a window event (from the Win32 API) occurs, which can for example be a mouseclick. The Window Procedure distinguishes the type of the Event and calls the function, responsible for Handling the Event. 

### DrawingProcedure
The Drawing Procedure is called by the [Window Procedure](#windowproc), if the [WM_PAINT Event](https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint) occurs, which in this case is triggered through the [InvalidateRect Function](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-invalidaterect) which is called by the dragging Event or the Main-function loop. It creates a fake HDC-Object, on whiches graphic-object the different [Paint](#paint)-functions draw the GUI. This fake HDC object is then mirrored onto the Window. This complicated procedure is necessary, since from painting over the last frame, onto the main graphics object, a "flickering"-Effect occurs. By painting first and mirroring the graphics afterwards, the flickering is prevented and the movement on the window appears smooth. The windows graphics are also updated way less, which lowers the strain on the computers GPU.

### MovePiece
The movePiece function in the [Board class](#board) converts the from-/ to information into move-strings, which it compares with the board history to manage castling, en-passant and promotion moves and relays the move to the [move function](#move), where it is checked for correctness. The movePiece function then changes the board according to the move and adds the last board state to the undo list. 

### Move
The move function in the [piece class](#piece) is called by [movePiece](#movepiece) and [TestAvailableMoves](#testavailablemoves) in order to verify a moves correctness. This code is taken directly from the exercise sheets, only modified, to accomodate for the board to be expressed as a 64 character string, instead of a 8x8 2-dimensional character array. It has also been mixed with the castling and the en-passant checks from the bonus sheet. 

### TestAvailableMoves
The testAvailableMoves function in the [piece class](#piece) returns a vector containing all possible moves for the specified player. This is used both for the "Show move options" option and for the game end checks, since no available moves implicates either a stalemate or a checkmate. This function is comprised of a variation of the testCheckmate function, I implemented in the Exercise sheets.

## Implementation of learned concepts

### Recursion
The project utilizes Recursion in the [Move function](#move) to test whether any piece is blocking the path of a queen, rook or bishop, by testing every move with the length of 1 in the direction of the move. 

### Modularity
The project uses modularity, by seperating the same type of tasks into their own classes, as described in the [Classes subsection](#classes). All non-static member variables and functions are also declared in their seperate header files, for easier oversight over the classes as a whole. 

### Errorhandling
For the FEN-input feature, the user has total freedom to input any string into the FEN. This could allow for memory leaks, injections or crashes. Which is why the assertion of FEN-correctness, has to be Exploit-free. Because of this, the setup function in the [board class](#board) especially utilizes Errorhandling, in order to prevent possible damages.

### Multithreading
In order to include the Timer feature, for timed chess games, the project class declares a timer function, which is then wrapped into a seperate thread and let run alongside the main thread. This allows for the timer to be much more accurate, since if it were included in the main game-loop, the unpredictable timecost of the entire Program would influence the timers accuracy. 

## Video
https://cloud.uni-konstanz.de/index.php/s/afnQdw9WPJNFynp 