#undef UNICODE
#include <windows.h>
#include "resource.h"
#include <fileapi.h>
#include <Lmcons.h>
#include "Robot.h"



#pragma warning (disable:4996)

#define WM_MESSAGESENT WM_USER


BOOL CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);


static bool terminateListener = false;
static HWND mainWnd = NULL;
static COLORREF color = 0x00FF0000;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)MainWndProc);

	return 0;
}


bool CheckIfRobotChecked() {
	HWND check = GetDlgItem(::mainWnd, IDC_ROBOT);

	UINT t = SendMessage(check, BM_GETCHECK, 0, 0);

	return t == BST_CHECKED;
}


void SetTextColor(char* color) {

	if (strcmp("Black", color) ==0) {
		::color = 0xFFFFFFFF;
		return;
	}
	else if (strcmp("Red", color)==0) {
		::color = 0x000000FF;
		return;
	}
	else if (strcmp("Blue", color)==0) {
		::color = 0x00FF0000;
		return;
	}
}


DWORD WINAPI ListenerThread(void* param) {

	HANDLE mailSlotHandle = CreateMailslot("\\\\.\\mailslot\\Shouter", 0, MAILSLOT_WAIT_FOREVER, NULL);

	Robot robot;

	while (!terminateListener) {

		DWORD  nextSize = NULL;

		bool getMailInfo = GetMailslotInfo(mailSlotHandle,NULL,&nextSize,NULL,NULL);

		if (nextSize != MAILSLOT_NO_MESSAGE) {

			bool isRobotChecked = CheckIfRobotChecked();


			char* buffer = new char[(int)nextSize];

			LPDWORD readedBytes=NULL;

			bool readFile = ReadFile(mailSlotHandle, buffer, (DWORD)nextSize, readedBytes, NULL);

			PostMessage((HWND)mainWnd, WM_MESSAGESENT, NULL, (LPARAM)buffer);

			if (isRobotChecked) {
				

				RobotoAnswer ans = {};
				

				bool haveAnswer =robot.GetRobotAnswer(ans,buffer);

				if (haveAnswer) {
					
					if (ans.haveAnswer) {
						PostMessage((HWND)mainWnd, WM_MESSAGESENT, NULL, (LPARAM)ans.answer);
					}
					else {
						SetTextColor(ans.action);
					}
					
				}				

			}
			
		}

		Sleep(100);
	}

	CloseHandle(mailSlotHandle);

	return 0;
}




void HandleShout() {
	DWORD size = 16;
	char text[250] = "";
	char computerName[16] = "";
	char name[50] = "";
	char output[300] = "";


	GetDlgItemText(::mainWnd, IDC_MYSCREAM, text, 250);
	GetDlgItemText(::mainWnd, IDC_MYNAME, name, 50);
	GetComputerName(computerName, &size);

	sprintf(output,"%s from %s shout:%s",name,computerName,text);

	HANDLE msgFileHandle = CreateFile("\\\\*\\mailslot\\Shouter", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD rt = NULL;
	WriteFile(msgFileHandle, output, 300, (LPDWORD)rt, NULL);

	SetDlgItemText(::mainWnd, IDC_MYSCREAM, "");
	CloseHandle(msgFileHandle);
}


void HandleReceivedMessage(char* text,int param) {


	HWND hub = GetDlgItem(::mainWnd, IDC_HUBHUB);

	int length = GetWindowTextLength(hub);

	SetFocus(hub);

	SendMessage(hub, EM_SETSEL, (WPARAM)length, (LPARAM)length);

	if (length != 0) {
		SendMessage(hub, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
	}

	
	SendMessage(hub, EM_REPLACESEL, FALSE, (LPARAM)text);


}


void Initialize(HWND hWnd) {
	
	char username[UNLEN + 1];

	DWORD username_len = UNLEN + 1;

	GetUserName(username, &username_len);

	SetDlgItemText(hWnd, IDC_MYNAME, username);

	::mainWnd = hWnd;

	CloseHandle(CreateThread(NULL, 0, ListenerThread, NULL, 0, NULL));

}



BOOL CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_INITDIALOG:		

		Initialize(hWnd);

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_EXIT:
			DestroyWindow(hWnd);
			return TRUE;
		case IDC_SHOUT:
			HandleShout();
			return TRUE;
		
		}
		return FALSE;
	case WM_DESTROY:

		PostQuitMessage(0);
		return TRUE;
	case WM_MESSAGESENT:
		
		if (lParam != NULL) {
			HandleReceivedMessage((char*)lParam,(int)wParam);
		}

			return TRUE;
	case WM_CTLCOLORSTATIC:

		if ((HWND)lParam == GetDlgItem(hWnd, IDC_HUBHUB)) {
			SetBkMode((HDC)wParam, TRANSPARENT);

			SetTextColor((HDC)wParam, color);
			return (BOOL)GetSysColorBrush(COLOR_MENU);
		}

		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return TRUE;
	}
	return FALSE;
}