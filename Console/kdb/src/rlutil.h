#pragma once
/**
 * File: rlutil.h 
 *
 * About:
 *	Description
 *		This file provides some useful utilities for console mode development with C and C++.
 * About:
 * [UPDATE]: 30/8/2024 - 27/10/2024 - add, modify functions:
 *    - cls (modify)
 *    - SetConsoleOutputANSI (add)
 *	  - SignalHandler and some helper functions (add)
 *    - crustb (add)
 *    - clrln  (add)
 *    - tmstmp, wtmstmp (add)
 *	  - add separate overloads for SetColor (see below) (add|modify)
 *  INDEPENDENT EDITOR - kostenuksoft 
 *  All rights belongs to Tapio Vierros 
 *	About: Copyright
 *	(C) 2010 Tapio Vierros
 */


#ifdef __cplusplus
	/// Common C++ headers
	#include <iostream>
	#include <string>
	#include <sstream>
	#include <chrono>
	/// Namespace forward declarations
	namespace rlutil {
		void locate(int x, int y);
	}
#else
	void locate(int x, int y); // Forward declare for C to avoid warnings
#endif // __cplusplus

#ifndef RLUTIL_INLINE
	#ifdef _MSC_VER
		#define RLUTIL_INLINE __inline
	#else
		#define RLUTIL_INLINE __inline__
	#endif
#endif

#ifdef _WIN32
	#include <windows.h>  // for WinAPI and Sleep()
	#define _NO_OLDNAMES  // for MinGW compatibility
	#include <conio.h>    // for getch() and kbhit()
	#define getch _getch
	#define kbhit _kbhit
#else
	#ifdef __cplusplus
		#include <cstdio> // for getch()
	#else // __cplusplus
		#include <stdio.h> // for getch()
	#endif // __cplusplus
	#include <termios.h> // for getch() and kbhit()
	#include <unistd.h> // for getch(), kbhit() and (u)sleep()
	#include <sys/ioctl.h> // for getkey()
	#include <sys/types.h> // for kbhit()
	#include <sys/time.h> // for kbhit()

/// Function: getch
/// Get character without waiting for Return to be pressed.
/// Windows has this in conio.h
RLUTIL_INLINE int getch(void) {
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

/// Function: kbhit
/// Determines if keyboard has been hit.
/// Windows has this in conio.h
RLUTIL_INLINE int kbhit(void) {
	static struct termios oldt, newt;
	int cnt = 0;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag    &= ~(ICANON | ECHO);
	newt.c_iflag     = 0; // input mode
	newt.c_oflag     = 0; // output mode
	newt.c_cc[VMIN]  = 1; // minimum time to wait
	newt.c_cc[VTIME] = 1; // minimum characters to wait for
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ioctl(0, FIONREAD, &cnt); // Read count
	struct timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 100;
	select(STDIN_FILENO+1, NULL, NULL, NULL, &tv); // A small time delay
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return cnt; // Return number of characters
}
#endif // _WIN32

#ifndef gotoxy
/// Function: gotoxy
/// Same as <rlutil.locate>.
RLUTIL_INLINE void gotoxy(int x, int y) {
	#ifdef __cplusplus
	rlutil::
	#endif
	locate(x,y);
}
#endif // gotoxy

#ifdef __cplusplus
/// Namespace: rlutil
/// In C++ all functions except <getch>, <kbhit> and <gotoxy> are arranged
/// under namespace rlutil. That is because some platforms have them defined
/// outside of rlutil.
namespace rlutil {
#endif

/**
 * Defs: Internal typedefs and macros
 * RLUTIL_STRING_T - String type depending on which one of C or C++ is used
 * RLUTIL_PRINT(str) - Printing macro independent of C/C++
 * UPD: RLUTIL_WSTRING_T - Multibyte string type depending on which one of C or C++ is used
 * UPD: RLUTIL_(W)PRINT((w)st) - Printing macro independent of C/C++
 */

#ifdef __cplusplus
	#ifndef RLUTIL_STRING_T
		typedef std::string RLUTIL_STRING_T;
	#endif // RLUTIL_STRING_T

	#ifndef RLUTIL_WSTRING_T
			typedef std::wstring RLUTIL_WSTRING_T;
	#endif // RLUTIL_WSTRING_T

	inline void RLUTIL_PRINT(CONST RLUTIL_STRING_T& st)    { std::cout << st; }
	inline void RLUTIL_WPRINT(CONST RLUTIL_WSTRING_T& wst) { std::wcout << wst; }

#else // __cplusplus
	#ifndef RLUTIL_STRING_T
		typedef char* RLUTIL_STRING_T;
		
	#endif // RLUTIL_STRING_T
	#ifndef RLUTIL_WSTRING_T
			typedef wchar_t* RLUTIL_WSTRING_T;
	#endif // RLUTIL_WSTRING_T

	#define RLUTIL_PRINT(st) printf("%s", st)
	#define RLUTIL_PRINT(wst) wprintf("%ls", wst)
#endif // __cplusplus

/**
 * Enums: Color codes
 *
 * BLACK - Black
 * BLUE - Blue
 * GREEN - Green
 * CYAN - Cyan
 * RED - Red
 * MAGENTA - Magenta / purple
 * BROWN - Brown / dark yellow
 * GREY - Grey / dark white
 * DARKGREY - Dark grey / light black
 * LIGHTBLUE - Light blue
 * LIGHTGREEN - Light green
 * LIGHTCYAN - Light cyan
 * LIGHTRED - Light red
 * LIGHTMAGENTA - Light magenta / light purple
 * YELLOW - Yellow (bright)
 * WHITE - White (bright)
 */
enum {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	GREY,
	DARKGREY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};

/**
 * Consts: ANSI color strings
 *
 * ANSI_CLS - Clears screen
 * ANSI_BLACK - Black
 * ANSI_RED - Red
 * ANSI_GREEN - Green
 * ANSI_BROWN - Brown / dark yellow
 * ANSI_BLUE - Blue
 * ANSI_MAGENTA - Magenta / purple
 * ANSI_CYAN - Cyan
 * ANSI_GREY - Grey / dark white
 * ANSI_DARKGREY - Dark grey / light black
 * ANSI_LIGHTRED - Light red
 * ANSI_LIGHTGREEN - Light green
 * ANSI_YELLOW - Yellow (bright)
 * ANSI_LIGHTBLUE - Light blue
 * ANSI_LIGHTMAGENTA - Light magenta / light purple
 * ANSI_LIGHTCYAN - Light cyan
 * ANSI_WHITE - White (bright)
 */
const RLUTIL_STRING_T ANSI_CLS = "\033[2J";
const RLUTIL_STRING_T ANSI_BLACK = "\033[22;30m";
const RLUTIL_STRING_T ANSI_RED = "\033[22;31m";
const RLUTIL_STRING_T ANSI_GREEN = "\033[22;32m";
const RLUTIL_STRING_T ANSI_BROWN = "\033[22;33m";
const RLUTIL_STRING_T ANSI_BLUE = "\033[22;34m";
const RLUTIL_STRING_T ANSI_MAGENTA = "\033[22;35m";
const RLUTIL_STRING_T ANSI_CYAN = "\033[22;36m";
const RLUTIL_STRING_T ANSI_GREY = "\033[22;37m";
const RLUTIL_STRING_T ANSI_DARKGREY = "\033[01;30m";
const RLUTIL_STRING_T ANSI_LIGHTRED = "\033[01;31m";
const RLUTIL_STRING_T ANSI_LIGHTGREEN = "\033[01;32m";
const RLUTIL_STRING_T ANSI_YELLOW = "\033[01;33m";
const RLUTIL_STRING_T ANSI_LIGHTBLUE = "\033[01;34m";
const RLUTIL_STRING_T ANSI_LIGHTMAGENTA = "\033[01;35m";
const RLUTIL_STRING_T ANSI_LIGHTCYAN = "\033[01;36m";
const RLUTIL_STRING_T ANSI_WHITE = "\033[01;37m";

/**
 * Consts: Key codes for keyhit()
 *
 * KEY_ESCAPE  - Escape
 * KEY_ENTER   - Enter
 * KEY_SPACE   - Space
 * KEY_INSERT  - Insert
 * KEY_HOME    - Home
 * KEY_END     - End
 * KEY_DELETE  - Delete
 * KEY_PGUP    - PageUp
 * KEY_PGDOWN  - PageDown
 * KEY_UP      - Up arrow
 * KEY_DOWN    - Down arrow
 * KEY_LEFT    - Left arrow
 * KEY_RIGHT   - Right arrow
 * KEY_F1      - F1
 * KEY_F2      - F2
 * KEY_F3      - F3
 * KEY_F4      - F4
 * KEY_F5      - F5
 * KEY_F6      - F6
 * KEY_F7      - F7
 * KEY_F8      - F8
 * KEY_F9      - F9
 * KEY_F10     - F10
 * KEY_F11     - F11
 * KEY_F12     - F12
 * KEY_NUMDEL  - Numpad del
 * KEY_NUMPAD0 - Numpad 0
 * KEY_NUMPAD1 - Numpad 1
 * KEY_NUMPAD2 - Numpad 2
 * KEY_NUMPAD3 - Numpad 3
 * KEY_NUMPAD4 - Numpad 4
 * KEY_NUMPAD5 - Numpad 5
 * KEY_NUMPAD6 - Numpad 6
 * KEY_NUMPAD7 - Numpad 7
 * KEY_NUMPAD8 - Numpad 8
 * KEY_NUMPAD9 - Numpad 9
 */
const int KEY_ESCAPE  = 0;
const int KEY_ENTER   = 1;
const int KEY_SPACE   = 32;

const int KEY_INSERT  = 2;
const int KEY_HOME    = 3;
const int KEY_PGUP    = 4;
const int KEY_DELETE  = 5;
const int KEY_END     = 6;
const int KEY_PGDOWN  = 7;

const int KEY_UP      = 14;
const int KEY_DOWN    = 15;
const int KEY_LEFT    = 16;
const int KEY_RIGHT   = 17;

const int KEY_F1      = 18;
const int KEY_F2      = 19;
const int KEY_F3      = 20;
const int KEY_F4      = 21;
const int KEY_F5      = 22;
const int KEY_F6      = 23;
const int KEY_F7      = 24;
const int KEY_F8      = 25;
const int KEY_F9      = 26;
const int KEY_F10     = 27;
const int KEY_F11     = 28;
const int KEY_F12     = 29;

const int KEY_NUMDEL  = 30;
const int KEY_NUMPAD0 = 31;
const int KEY_NUMPAD1 = 127;
const int KEY_NUMPAD2 = 128;
const int KEY_NUMPAD3 = 129;
const int KEY_NUMPAD4 = 130;
const int KEY_NUMPAD5 = 131;
const int KEY_NUMPAD6 = 132;
const int KEY_NUMPAD7 = 133;
const int KEY_NUMPAD8 = 134;
const int KEY_NUMPAD9 = 135;

/// Function: msleep
/// Waits given number of milliseconds before continuing.
RLUTIL_INLINE void msleep(unsigned int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	// usleep argument must be under 1 000 000
	if (ms > 1000) sleep(ms / 1000000);
	usleep((ms % 1000000) * 1000);
#endif
}

/// Function: getkey
/// Reads a key press (blocking) and returns a key code.
///
/// See <Key codes for keyhit()>
///
/// Note:
/// Only Arrows, Esc, Enter and Space are currently working properly.
RLUTIL_INLINE int getkey() {
	#ifndef _WIN32
	int cnt = kbhit(); // for ANSI escapes processing
	#endif
	int k = getch();
	switch(k) {
		case 0: {
			int kk;
			switch (kk = getch()) {
				case 71: return KEY_NUMPAD7;
				case 72: return KEY_NUMPAD8;
				case 73: return KEY_NUMPAD9;
				case 75: return KEY_NUMPAD4;
				case 77: return KEY_NUMPAD6;
				case 79: return KEY_NUMPAD1;
				case 80: return KEY_NUMPAD4;
				case 81: return KEY_NUMPAD3;
				case 82: return KEY_NUMPAD0;
				case 83: return KEY_NUMDEL;
				default: return kk-59+KEY_F1; // Function keys
			}}
		case 224: {
			int kk;
			switch (kk = getch()) {
				case 71: return KEY_HOME;
				case 72: return KEY_UP;
				case 73: return KEY_PGUP;
				case 75: return KEY_LEFT;
				case 77: return KEY_RIGHT;
				case 79: return KEY_END;
				case 80: return KEY_DOWN;
				case 81: return KEY_PGDOWN;
				case 82: return KEY_INSERT;
				case 83: return KEY_DELETE;
				default: return kk-123+KEY_F1; // Function keys
			}}
		case 13: return KEY_ENTER;
#ifdef _WIN32
		case 27: return KEY_ESCAPE;
#else // _WIN32
		case 155: // single-character CSI
		case 27: {
			// Process ANSI escape sequences
			if (cnt >= 3 && getch() == '[') {
				switch (k = getch()) {
					case 'A': return KEY_UP;
					case 'B': return KEY_DOWN;
					case 'C': return KEY_RIGHT;
					case 'D': return KEY_LEFT;
				}
			} else return KEY_ESCAPE;
		}
#endif // _WIN32
		default: return k;
	}
}

/// Function: nb_getch
/// Non-blocking getch(). Returns 0 if no key was pressed.
RLUTIL_INLINE int nb_getch() {
	if (kbhit()) return getch();

	return 0;
}

///Function: SetConsoleOutputANSI
///Output ANSI color sequence.	 
RLUTIL_INLINE void SetConsoleOutputANSI() {
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}


/// Function: getANSIColor
/// Return ANSI color escape sequence for specified number 0-15.
///
/// See <Color Codes>
RLUTIL_INLINE RLUTIL_STRING_T getANSIColor(const int c) {
	switch (c) {
		case 0 : return ANSI_BLACK;
		case 1 : return ANSI_BLUE; // non-ANSI
		case 2 : return ANSI_GREEN;
		case 3 : return ANSI_CYAN; // non-ANSI
		case 4 : return ANSI_RED; // non-ANSI
		case 5 : return ANSI_MAGENTA;
		case 6 : return ANSI_BROWN;
		case 7 : return ANSI_GREY;
		case 8 : return ANSI_DARKGREY;
		case 9 : return ANSI_LIGHTBLUE; // non-ANSI
		case 10: return ANSI_LIGHTGREEN;
		case 11: return ANSI_LIGHTCYAN; // non-ANSI;
		case 12: return ANSI_LIGHTRED; // non-ANSI;
		case 13: return ANSI_LIGHTMAGENTA;
		case 14: return ANSI_YELLOW; // non-ANSI
		case 15: return ANSI_WHITE;
		default: return "";
	}
}

/// Function: setColor
/// Change color specified by number (Windows / QBasic colors).
///
/// See <Color Codes>
RLUTIL_INLINE void setColor(int textColor) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD) textColor);
#else
	RLUTIL_PRINT(getANSIColor(c));
#endif
}
/// Function: setColor
/// Change colors text/background specified by number (Windows / QBasic colors).
///
/// See <Color Codes>
inline void setColor(int textColor, int bgColor) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD) (bgColor << 4 |  textColor));
#else
	RLUTIL_PRINT(getANSIColor(c));
#endif
}

/// Function: setBgColor
/// Change  background colors specified by number (Windows / QBasic colors).
///
/// See <Color Codes>
inline void setBgColor(int bgColor) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)(bgColor << 4));
#else
	RLUTIL_PRINT(getANSIColor(c));
#endif
}


/// Function: cls
/// Clears screen and moves cursor home.
RLUTIL_INLINE void cls() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)

	const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	constexpr COORD topLeft = { 0, 0 };
	std::cout.flush();

	if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;
	DWORD written;
	FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);
	FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);
	SetConsoleCursorPosition(hOut, topLeft);

#else
	RLUTIL_PRINT("\033[2J\033[H");
#endif
}

/// Function: curstb 
/// Moves cursor to the left bottom
RLUTIL_INLINE void curstb() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	COORD coord;
	coord.X = 0;
	coord.Y = csbi.srWindow.Bottom-1;
	SetConsoleCursorPosition(hConsole, coord);
}

/// Function: tmpstmp
/// Returns formated timestamp 
RLUTIL_INLINE RLUTIL_STRING_T tmstmp() {

	const auto now = std::chrono::system_clock::now();
	const time_t t_c = std::chrono::system_clock::to_time_t(now);

	char secure_buffer[64];
	auto ec_t = ctime_s(secure_buffer, sizeof(secure_buffer), &t_c);

	if (ec_t == 0) return { secure_buffer };
	return {};
}

/// Function: wtmpstmp
/// Returns formated international timestamp according to locale provided
/// defaul locale - english
RLUTIL_INLINE RLUTIL_WSTRING_T wtmstmp(const wchar_t* locale = L"en")
{
	const auto my_last_locale = _wsetlocale(LC_ALL, nullptr);

	_wsetlocale(LC_ALL, locale);

	const auto now = std::chrono::system_clock::now();
	const time_t t_c = std::chrono::system_clock::to_time_t(now);

	wchar_t secure_buffer[64];
	auto ec_t = wcsftime(secure_buffer,
		sizeof(secure_buffer) / sizeof(wchar_t), L"%A, %c", localtime(&t_c));

	_wsetlocale(LC_ALL, my_last_locale);

	if (ec_t) return secure_buffer;

	return {};
}

/// Function: locate
/// Sets the cursor position to 1-based x,y.
RLUTIL_INLINE void locate(int x, int y) {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)

	COORD coord;
	coord.X = (SHORT)x - 1;
	coord.Y = (SHORT)y - 1; // Windows uses 0-based coordinates
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

#else // _WIN32 || USE_ANSI
#ifdef __cplusplus
	std::ostringstream oss;
	oss << "\033[" << y << ";" << x << "H";
	RLUTIL_PRINT(oss.str());
#else // __cplusplus
	char buf[32];
	sprintf(buf, "\033[%d;%df", y, x);
	RLUTIL_PRINT(buf);
#endif // __cplusplus
#endif // _WIN32 || USE_ANSI
}

/// Function: hidecursor
/// Hides the cursor.
RLUTIL_INLINE void hidecursor() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsoleOutput;
	CONSOLE_CURSOR_INFO structCursorInfo;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hConsoleOutput, &structCursorInfo); // Get current cursor size
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);
#else // _WIN32 || USE_ANSI
	RLUTIL_PRINT("\033[?25l");
#endif // _WIN32 || USE_ANSI
}

/// Function: showcursor
/// Shows the cursor.
RLUTIL_INLINE void showcursor() {
#if defined(_WIN32) && !defined(RLUTIL_USE_ANSI)
	HANDLE hConsoleOutput;
	CONSOLE_CURSOR_INFO structCursorInfo;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hConsoleOutput, &structCursorInfo); // Get current cursor size
	structCursorInfo.bVisible = TRUE;
	SetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);
#else // _WIN32 || USE_ANSI
	RLUTIL_PRINT("\033[?25h");
#endif // _WIN32 || USE_ANSI
}

/// Function: trows
/// Get the number of rows in the terminal window or -1 on error.
RLUTIL_INLINE int trows() {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;

	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1; // Window height
	// return csbi.dwSize.Y; // Buffer height
#else
#ifdef TIOCGSIZE
	struct ttysize ts;
	ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
	return ts.ts_lines;
#elif defined(TIOCGWINSZ)
	struct winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	return ts.ws_row;
#else // TIOCGSIZE
	return -1;
#endif // TIOCGSIZE
#endif // _WIN32
}

/// Function: tcols
/// Get the number of columns in the terminal window or -1 on error.
RLUTIL_INLINE int tcols() {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;

	return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Window width
	// return csbi.dwSize.X; // Buffer width
#else
#ifdef TIOCGSIZE
	struct ttysize ts;
	ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
	return ts.ts_cols;
#elif defined(TIOCGWINSZ)
	struct winsize ts;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
	return ts.ws_col;
#else // TIOCGSIZE
	return -1;
#endif // TIOCGSIZE
#endif // _WIN32
}

/// Function: anykey
/// Waits until a key is pressed.
RLUTIL_INLINE void anykey() {
	getch();
}

/// Function : SignalHandler
/// Sets 2 types of signals CTRL-C, CTRL-BREAK
RLUTIL_INLINE BOOL WINAPI SignalHandler(CONST DWORD fdwCtrlType) {
	const wchar_t* MESSAGE;

	switch (fdwCtrlType) {
	case CTRL_C_EVENT:        
		MESSAGE = L"[CTRL + C]";
		break;

	case CTRL_BREAK_EVENT:   
		MESSAGE = L"[CTRL + BREAK]";
		break;
	default:
		return FALSE;
	}

	Sleep(40);

	setColor(LIGHTRED);
	wprintf(L"\n\n<?!> Виявлено системний сигнал: %s <?!>\n", MESSAGE);
	wprintf(L"<?!> Не рекомендується переривати роботу програми таким чином <?!>\n");
	setColor(WHITE);

	return TRUE;  
}
/// Function : SetupSingalHandler
/// Helper function to set up the handler
RLUTIL_INLINE BOOL SetupSignalHandler() {
	if (!SetConsoleCtrlHandler(SignalHandler, TRUE)) {
		DWORD error = GetLastError();
		wchar_t errorMsg[256];
		FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			error,
			0,
			errorMsg,
			256,
			nullptr
		);

		setColor(LIGHTRED);
		wprintf(L"Помилка встановлення обробника сигналів: %ls\n", errorMsg);
		setColor(WHITE);
		return FALSE;
	}
	return TRUE;
}
/// Function : RemoveSignalHandler
/// Optional: Helper function to remove the handler
RLUTIL_INLINE BOOL RemoveSignalHandler() {
	return SetConsoleCtrlHandler(SignalHandler, FALSE);
}


// TODO1: ...
RLUTIL_INLINE RLUTIL_WSTRING_T MultiByteToWideString(CONST RLUTIL_STRING_T& STR) {
	if (STR.empty())  return {};

	CONST INT SIZE_REQUIRED = MultiByteToWideChar(
		CP_UTF8,               
		0,                     
		STR.c_str(),           
		static_cast<int>(STR.length()),     
		nullptr,               
		0                      
	);

	if (SIZE_REQUIRED <= 0) return {};


	RLUTIL_WSTRING_T MyWString(SIZE_REQUIRED, 0);
	CONST INT COPIED = MultiByteToWideChar(
		CP_UTF8,               
		0,                    
		STR.c_str(),         
		static_cast<int>(STR.length()),    
		MyWString.data(),          
		SIZE_REQUIRED           
	);

	if (COPIED <= 0) return {};
	return MyWString;
}

// TODO2: ...
RLUTIL_INLINE RLUTIL_STRING_T WideToMultiByteString(CONST RLUTIL_WSTRING_T& WSTR) {
	if (WSTR.empty())  return {};

	CONST INT SIZE_REQUIRED = WideCharToMultiByte(
		CP_UTF8,                
		0,                      
		WSTR.c_str(),
		static_cast<int>(WSTR.length()),
		nullptr,             
		0,                    
		nullptr,               
		nullptr               
	);

	if (SIZE_REQUIRED <= 0) { return {}; }
	
	RLUTIL_STRING_T MyString(SIZE_REQUIRED, 0);

	CONST INT COPIED = WideCharToMultiByte(
		CP_UTF8,               
		0,                    
		WSTR.c_str(),
		static_cast<int>(WSTR.length()),
		MyString.data(),
		SIZE_REQUIRED,
		nullptr,              
		nullptr            
	);

	if (COPIED <= 0) return {};
	return MyString;
}

// TODO3: ...
RLUTIL_INLINE BOOL SetConsoleIcon(CONST WCHAR* iconPath) {
	
	HWND hWnd = GetConsoleWindow();
	if (!hWnd) {
		printf("Couldn`t get console handle.\n");
		return false;
	}

	HICON hIcon = (HICON) LoadImageW(
		NULL,                   // hInstance must be NULL for LR_LOADFROMFILE
		iconPath,              // Icon path
		IMAGE_ICON,            // Type of image
		0,                     // Use actual width
		0,                     // Use actual height
		LR_LOADFROMFILE |      // Load from file
		LR_DEFAULTSIZE         // Default size if width/height are 0
	);

	if (!hIcon) {
		DWORD error = GetLastError();
		printf("Failed to load icon. Error code: %lu \n", error);
		return FALSE;
	}

	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	return TRUE;
}

#ifndef min
/// Function: min
/// Returns the lesser of the two arguments.
#ifdef __cplusplus
template <class T> const T& min(const T& a, const T& b) { return a < b ? a : b; }
#else
#define min(a,b) (((a)<(b))?(a):(b))
#endif // __cplusplus
#endif // min

#ifndef max
/// Function: max
/// Returns the greater of the two arguments.
#ifdef __cplusplus
template <class T> const T& max ( const T& a, const T& b ) { return b < a ? a : b; }
#else
#define max(a,b) (((b)<(a))?(a):(b))
#endif // __cplusplus
#endif // max

// Classes are here at the end so that documentation is pretty.

#ifdef __cplusplus
/// Class: CursorHider
/// RAII OOP wrapper for <rlutil.hidecursor>.
/// Hides the cursor and shows it again
/// when the object goes out of scope.
struct CursorHider {
	CursorHider() { hidecursor(); }
	~CursorHider() { showcursor(); }
};

} // namespace rlutil
#endif
