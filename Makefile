includes = -lgdiplus -lgdi32

output: Project.o Window.o Input.o Paint.o Board.o Piece.o
	g++ Project.o Window.o Input.o Paint.o Board.o Piece.o $(includes) -o chess

Project.o: ./code/Project.cpp
	g++ -c ./code/Project.cpp

Window.o: ./code/gui/Window.cpp ./code/gui/Window.h
	g++ -c ./code/gui/Window.cpp

Input.o: ./code/input/Input.cpp ./code/input/Input.h
	g++ -c ./code/input/Input.cpp

Paint.o: ./code/gui/Paint.cpp ./code/gui/Paint.h
	g++ -c ./code/gui/Paint.cpp

Board.o: ./code/rules/board/Board.cpp ./code/rules/board/Board.h
	g++ -c ./code/rules/board/Board.cpp

Piece.o: ./code/rules/pieces/Piece.cpp ./code/rules/pieces/Piece.h
	g++ -c ./code/rules/pieces/Piece.cpp

clean:
	del *.o chess.exe
	
#use rm instead of del for different OS