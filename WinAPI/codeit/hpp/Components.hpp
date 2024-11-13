#pragma once

extern  
struct HCOMPONENT {
	HMENU hMenu;
	HWND hEdit;
	HFONT hFont;

} *Win32Menu;

extern
struct FCOMPONENT {
	WCHAR szFileName[MAX_PATH] = L"";
} *Win32File;



