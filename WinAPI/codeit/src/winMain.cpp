#include "../hpp/pch.hpp"
#include "../hpp/Components.hpp"
#include "../hpp/Modules.hpp"
#include "../hpp/Properties.hpp"

HCOMPONENT* Win32Menu = nullptr;
FCOMPONENT* Win32File = nullptr;

INT WINAPI wWinMain
(
    HINSTANCE hInst,
    HINSTANCE obsolete_win16,
    LPWSTR    argp,
    INT       argc
)
{

    if (wcscmp(argp, L"-db") == 0)
        ShowWindow(GetConsoleWindow(), SW_SHOW);
    
    else ShowWindow(GetConsoleWindow(), SW_HIDE);
    
    WNDCLASSW wc = { };
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"CodeItClass";
    wc.lpfnWndProc = WindowsProcedure;
    wc.hIcon = (HICON) LoadImage
	(
        NULL, 
        L"ico/codeit.ico", 
        IMAGE_ICON,
        0, 
        0,
        LR_LOADFROMFILE
    );

    Win32Menu = new HCOMPONENT;
    Win32File = new FCOMPONENT;

    if (!RegisterClassW(&wc)) {
        printf("Fatal error experienced during class registration.");
        Sleep(400);
	    return EXIT_FAILURE;
    }

    CONST INT screenWidth  = GetSystemMetrics(SM_CXSCREEN),
              screenHeight = GetSystemMetrics(SM_CYSCREEN),
              winX         = (screenWidth - WINDOW_WIDTH) / 2,
              winY         = (screenHeight - WINDOW_HEIGHT) / 2;

	HWND hwnd = CreateWindowW
	(
        wc.lpszClassName,
        APP_NAME,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        winX, winY,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInst, NULL
    );

    if (!hwnd) {
        printf("Fatal error experienced during window initialization.");
        Sleep(400);
        return EXIT_FAILURE;
    }

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (INT) msg.wParam;
}



