#include "../hpp/Modules.hpp"
#include "../hpp/Components.hpp"
#include "../hpp/Properties.hpp"

LRESULT CALLBACK WindowsProcedure
(
    CONST HWND hWnd,
    CONST UINT msg,
    CONST WPARAM wp,
    CONST LPARAM lp
)
{
    switch (msg) {
	    case WM_COMMAND:
	        switch (LOWORD(wp)) {
		        case IDM_NEW:
		            SetWindowTextW(Win32Menu->hEdit, L"");
		            Win32File->szFileName[0] = L'\0';
		            SaveFile(hWnd);
		            break;
		        case IDM_OPEN:
		            OpenFile(hWnd);
		            break;

		        case IDM_SAVE:
					case IDC_BTN_SAVE:
		            SaveFile(hWnd);
		            break;
		        case IDM_SAVE_AS:
		            SaveFileAs(hWnd);
		            break;

	            case IDC_BTN_CLOSE:
					case IDC_BTN_EXIT:
		            if (SafeExit(hWnd) == IDYES) {
		                delete Win32Menu;
		                delete Win32File;
		                DestroyWindow(hWnd);
		            }
        			break;

		        case IDM_VIEW_HELP:
					case IDM_WHATS_NEW:
						case IDM_FEEDBACK:
		            MessageBeep(MB_ICONINFORMATION);
		            MessageBoxW(hWnd, L"Скоро! (явно ні)", APP_NAME, MB_OK | MB_ICONINFORMATION);
		            break;
	        }
	        break;
	    case WM_CREATE:
	        AddMenuItems(hWnd);
	        AddControls(hWnd);
	        break;
	    case WM_SIZE:
	        SetWorkField(hWnd);
	        break;
	    case WM_CLOSE:
	        if (SafeExit(hWnd) == IDYES) 
	            DestroyWindow(hWnd);
	        break;
	    case WM_DESTROY:
	        PostQuitMessage(0);
	        break;

	    default:
	        return DefWindowProcW(hWnd, msg, wp, lp);
    }
    return 0;
}

VOID DisplayFile(CONST WCHAR* path)
{
    if (!path || !*path) return;

    FILE* file = NULL;
    if (_wfopen_s(&file, path, L"rb") != 0 || !file) return;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto buffer = std::make_unique<CHAR[]>(size + 1);
    if (buffer) {

        CONST SIZE_T bytesRead = fread(buffer.get(), 1, size, file);
        buffer[bytesRead] = '\0';

        INT wideSize = MultiByteToWideChar
        (
            CP_UTF8,
            0,
            buffer.get(),
            -1,
            NULL,
            0
        );

        CONST auto wideBuffer = std::make_unique<WCHAR[]>(wideSize);
        if (wideBuffer) {
            MultiByteToWideChar(CP_UTF8, 0, buffer.get(), -1, wideBuffer.get(), wideSize);
            SetWindowTextW(Win32Menu->hEdit, wideBuffer.get());
        }
    }

    INT err = fclose(file);
    wcscpy_s(Win32File->szFileName, path);
}

VOID SaveFile(CONST HWND hWnd)
{
    if (Win32File->szFileName[0] == NULL) return;

    FILE* file = NULL;
    if (_wfopen_s(&file, Win32File->szFileName, L"wb") != 0 || !file) {
        MessageBoxW(hWnd, L"Не обрано жодного файлу !", APP_NAME, MB_OK | MB_ICONEXCLAMATION);
        OpenFile(hWnd);
        return;
    }

    CONST INT length = GetWindowTextLengthW(Win32Menu->hEdit);
    CONST auto buffer = std::make_unique<WCHAR[]>(length + 1);

    if (buffer) {

        GetWindowTextW(Win32Menu->hEdit, buffer.get(), length + 1);
        CONST INT utf8Size = WideCharToMultiByte
        (
            CP_UTF8,
            0,
            buffer.get(),
            -1,
            NULL,
            0,
            NULL,
            NULL
        );

        CONST auto utf8Buffer = std::make_unique<CHAR[]>(utf8Size);

        if (utf8Buffer) {
            WideCharToMultiByte
            (
                CP_UTF8,
                0,
                buffer.get(),
                -1,
                utf8Buffer.get(),
                utf8Size,
                NULL,
                NULL
            );
            SIZE_T err = fwrite(utf8Buffer.get(), 1, utf8Size - 1, file);
        }
    }

    INT err = fclose(file);
    MessageBoxW(hWnd, L"Файл успішно збережено!", APP_NAME, MB_OK | MB_ICONINFORMATION);
}

BOOL SaveFileAs(CONST HWND hWnd)
{
    OPENFILENAMEW ofn = { };
    WCHAR szFile[MAX_PATH] = L"";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) {
        wcscpy_s(Win32File->szFileName, ofn.lpstrFile);
        SaveFile(hWnd);
        return TRUE;
    }

    return FALSE;
}

VOID OpenFile(CONST HWND hWnd)
{
    OPENFILENAMEW ofn = { };
    WCHAR szFile[MAX_PATH] = L"";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter =
        L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        DisplayFile(ofn.lpstrFile);
       // return;
    }
   // MessageBoxW(hWnd, L"Не вдалось відчинити файл.", APP_NAME, MB_OK | MB_ICONERROR);

}

VOID AddMenuItems(CONST HWND hWnd)
{
    Win32Menu->hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenuW(Win32Menu->hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Файл");
    AppendMenuW(hFileMenu, MF_STRING, IDM_NEW, L"Новий файл");
    AppendMenuW(hFileMenu, MF_STRING, IDM_OPEN, L"Відчинити файл");
    AppendMenuW(hFileMenu, MF_STRING, IDM_SAVE, L"Зберегти");
    AppendMenuW(hFileMenu, MF_STRING, IDM_SAVE_AS, L"Зберегти як..");

    AppendMenuW(Win32Menu->hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Довідка");
    AppendMenuW(hHelpMenu, MF_STRING, IDM_VIEW_HELP, L"Переглянути довідку");
    AppendMenuW(hHelpMenu, MF_STRING, IDM_WHATS_NEW, L"Що нового ?");
    AppendMenuW(hHelpMenu, MF_STRING, IDM_FEEDBACK, L"Зворотній зв'язок");

    SetMenu(hWnd, Win32Menu->hMenu);
}

VOID AddControls(CONST HWND hWnd)
{
    CreateWindowW
    (
        L"BUTTON", L"Save",
        WS_VISIBLE | WS_CHILD,
        EDIT_MARGIN, 325,
        BUTTON_WIDTH + 100, BUTTON_HEIGHT,
        hWnd, (HMENU)IDC_BTN_SAVE,
        NULL, NULL
    );

    CreateWindowW
    (
        L"BUTTON", L"Exit",
        WS_VISIBLE | WS_CHILD,
        EDIT_MARGIN + BUTTON_WIDTH + EDIT_MARGIN, 320,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hWnd, (HMENU)IDC_BTN_EXIT,
        NULL, NULL
    );


    Win32Menu->hEdit = CreateWindowExW
    (
        WS_EX_CLIENTEDGE,
        L"EDIT", L"",
        WS_VISIBLE | WS_CHILD |
        WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        EDIT_MARGIN, EDIT_MARGIN,
        650, 300,
        hWnd, NULL,
        NULL, NULL
    );

    Win32Menu->hFont = CreateFontW
    (
        16, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Consolas"
    );

    SendMessage(Win32Menu->hEdit, WM_SETFONT, (WPARAM)Win32Menu->hFont, TRUE);
}

BOOL SetWorkField(CONST HWND hWnd) {

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    BOOL FLAG_1 =
        SetWindowPos(Win32Menu->hEdit, NULL,
            EDIT_MARGIN,
            EDIT_MARGIN,
            rcClient.right - 2 * EDIT_MARGIN,
            rcClient.bottom - EDIT_MARGIN - BUTTON_HEIGHT - EDIT_MARGIN,
            SWP_NOZORDER);

    HWND hSaveBtn = GetDlgItem(hWnd, IDC_BTN_SAVE);
    HWND hExitBtn = GetDlgItem(hWnd, IDC_BTN_EXIT);

    BOOL FLAG_2 =
        SetWindowPos(
            hSaveBtn, NULL,
            EDIT_MARGIN,
            rcClient.bottom - BUTTON_HEIGHT - EDIT_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            SWP_NOZORDER);

    BOOL FLAG_3 =
        SetWindowPos(hExitBtn, NULL,
            EDIT_MARGIN + BUTTON_WIDTH + EDIT_MARGIN,
            rcClient.bottom - BUTTON_HEIGHT - EDIT_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            SWP_NOZORDER);

    return FLAG_1 && FLAG_2 && FLAG_3;
}

INT SafeExit(CONST HWND hWnd) {
    MessageBeep(MB_OKCANCEL);
    return MessageBoxW
	(
        hWnd,
        L"Точно бажаєте вийти (exit) ?", 
        APP_NAME, 
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2
    );
}
