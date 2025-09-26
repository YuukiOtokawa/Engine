#include "SystemLog.h"

#include <windows.h>
#include <iostream>

#include <iomanip>

void SystemLog::Log(int line, std::string log)
{
	COORD pos;
	pos.X = 0;
	pos.Y = line;

	auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(handle, pos);

	std::cout << std::left << std::setw(100) << log << std::endl;
}

void SystemLog::ResetLog()
{
	auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD charsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD consoleSize;

	if (!GetConsoleScreenBufferInfo(handle, &csbi)) {
		return;
	}
	SetConsoleCursorPosition(handle, { 0,0 });

	consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

	if (!FillConsoleOutputCharacter(handle, ' ', consoleSize, { 0,0 }, &charsWritten)) {
		return;
	}

	if (!FillConsoleOutputAttribute(handle, csbi.wAttributes, consoleSize, { 0,0 }, &charsWritten)) {
		return;
	}

	SetConsoleCursorPosition(handle, { 0,0 });//カーソルを先頭に戻す
}

