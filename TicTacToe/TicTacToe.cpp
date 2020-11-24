//P2549004 Oskar Kojder 
//TicTacToe Game
//Refrence page: docs.microsoft.com/en-us/windows/console/reading-input-buffer-events

//Include all necessary libraries
#include "pch.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <windows.h>

using namespace std;


//Declare Global Variables
bool player = 0;
bool playing = true;
int board[3][3];
int movecount = 0;
HANDLE hStdin;
DWORD fdwSaveOldMode;
COORD destCoord;
HANDLE hStdout;

//Declare functions
VOID PrepareBoard();
VOID ErrorExit(LPCSTR);
VOID KeyEventProc(KEY_EVENT_RECORD);
VOID Move(char);
VOID PlaceMark();
bool CheckPlace();
bool CheckWin();



int main()
{
	start:
	PrepareBoard();

	//Get standard output handle
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	//Change position of the cursor to start of the board
	destCoord.X = 3;
	destCoord.Y = 3;
	SetConsoleCursorPosition(hStdout, destCoord);


	//Declare buffor variables
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	

	// Get the standard input handle. 
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	
	//ErrorExit
	if (hStdin == INVALID_HANDLE_VALUE)
		ErrorExit("GetStdHandle");

	// Loop to read and handle the next X input events

	while (playing)
	{
		//Check if user can make a move
		if (movecount > 8) {
			break;
		}
		// Wait for the events
		if (!ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			128,         // size of read buffer 
			&cNumRead)) // number of records read 
			ErrorExit("ReadConsoleInput");

		// Dispatch the events to the appropriate handle 

		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: // keyboard input 
				KeyEventProc(irInBuf[i].Event.KeyEvent);
				break;
			default:
				ErrorExit("Unknown event type");
				break;
			}
		}
	}
	//If the board is full wait for 2s then clear screen and start game again
	Sleep(2000);
	system("CLS");
	goto start;
}

VOID PrepareBoard() {
	//Draw a board
	cout << R"(

      |     |     
   -  |  -  |  -  
 _____|_____|_____
      |     |     
   -  |  -  |  -  
 _____|_____|_____
      |     |     
   -  |  -  |  -  
      |     |     
TIC TAC TOE GAME
Controls: Arrow keys to move, Enter to Place mark, E to exit 
Player 1 - O | Player 2 - X
)";
	//Fill the board array with empty fields (value 0)
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			board[i][j] = 0;
		}
	}
	//set loop variable and reset movecount
	playing = true;
	movecount = 0;
}

//In case of invalid input print the message on screen and exit program
VOID ErrorExit(LPCSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);
	SetConsoleMode(hStdin, fdwSaveOldMode);
	ExitProcess(0);
}

//Handle keys Input
VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
	//Left arrow
	if (ker.bKeyDown && ker.wVirtualKeyCode == VK_LEFT) {
		Move('l');
	}
	//Right arrow
	if (ker.bKeyDown && ker.wVirtualKeyCode == VK_RIGHT) {
		Move('r');
	}
	//Up arrow
	if (ker.bKeyDown && ker.wVirtualKeyCode == VK_UP) {
		Move('u');
	}
	//Down arrow
	if (ker.bKeyDown && ker.wVirtualKeyCode == VK_DOWN) {
		Move('d');
	}
	//Enter key
	if (ker.bKeyDown && ker.wVirtualKeyCode == VK_RETURN) {
		PlaceMark();
	}
	//'E' Key
	if (ker.bKeyDown && ker.wVirtualKeyCode == 0x45) {
		system("CLS");
		SetConsoleMode(hStdin, fdwSaveOldMode);
		ExitProcess(0);
	}
}

//Movement around the board
VOID Move(char direction) {
	//Calculate cursor position to left
	if (direction == 'l' && destCoord.X > 3) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		destCoord.X -= 6;
		SetConsoleCursorPosition(hStdout, destCoord);
	}
	//Calculate cursor position to up
	if (direction == 'u' && destCoord.Y > 3) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		destCoord.Y -= 3;
		SetConsoleCursorPosition(hStdout, destCoord);
	}
	//Calculate cursor position to down
	if (direction == 'd' && destCoord.Y < 9) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		destCoord.Y += 3;
		SetConsoleCursorPosition(hStdout, destCoord);
	}
	//Calculate cursor position to right
	if (direction == 'r' && destCoord.X < 15) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		destCoord.X += 6;
		SetConsoleCursorPosition(hStdout, destCoord);
	}
}

//Placing a mark
VOID PlaceMark() {
	//Get cursor coordinates
	int saveX = destCoord.X;
	int saveY = destCoord.Y;
	if (CheckPlace()) {
		//Place Mark of actually playing player then change the player and increase movecount
		if (player) {
			puts("X");
			destCoord.X = saveX;
			destCoord.Y = saveY;
			SetConsoleCursorPosition(hStdout, destCoord);
			player = false;
			movecount++;
		}
		else {
			puts("O");
			destCoord.X = saveX;
			destCoord.Y = saveY;
			SetConsoleCursorPosition(hStdout, destCoord);
			player = true;
			movecount++;
		}
		//After Move check if they can still play
		CheckWin();
	}
	
}

//Check if player can put mark on cursor position and change the board table 
bool CheckPlace() {
	//translate cursor position to position in the board array
	int x;
	if (destCoord.X == 3)
		x = 0;
	else
		x = (destCoord.X - 3)/6;
	int y = destCoord.Y/4;
	//Check if this position is empty and modify values inside the board array
	if (board[x][y] == 0) {
		if(player)
			board[x][y]= 1;
		else
			board[x][y] = 2;
		return true;
	}
		
	else {
		return false;
	}
}
//Check for win 
bool CheckWin() {
	int horiz=0, vert=0, cross=0, playernum;
	//assign appropriate number 
	if (player)
		playernum = 2;
	else
		playernum = 1;

	//Check for vertical and horizontal lines
	for (int i = 0; i < 3; i++) {
		if (board[i][0] == playernum && board[i][1] == playernum && board[i][2] == playernum)
			horiz++;
		if (board[0][i] == playernum && board[1][i] == playernum && board[2][i] == playernum)
			vert++;		
	}
	//Check other possibilities 
	if (board[0][0] == playernum && board[1][1] == playernum && board[2][2] == playernum)
		cross++;
	if (board[0][2] == playernum && board[1][1] == playernum && board[2][0] == playernum)
		cross++;
	//if one of the following was true one of the player won
	if (cross > 0 || horiz > 0 || vert > 0) {
		cout << "WON";
		playing = false;
		//Move cursor bellow the board and print which player won
		destCoord.X = 0;
		destCoord.Y = 15;
		SetConsoleCursorPosition(hStdout, destCoord);
		SetConsoleMode(hStdin, fdwSaveOldMode);
		if (player)
			cout << "Player 1 won";
		else
			cout << "Player 2 won";
		return true;
	}
	else {
		//Check for stalemate
		if (movecount > 8) {
			destCoord.X = 0;
			destCoord.Y = 15;
			SetConsoleCursorPosition(hStdout, destCoord);
			SetConsoleMode(hStdin, fdwSaveOldMode);
			cout << "Stalemate";
		}
		return false;
	}
		
}