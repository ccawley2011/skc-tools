#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#include "MidiDLL.hpp"
#include "MidiInterface.hpp"
#include "WinMsg.h"
#include "resource.h"

BOOL OpenFileDialog(HWND hWnd, LPTSTR lpstrFile, DWORD nMaxFile, LPCTSTR lpstrFilter) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = lpstrFile;
	ofn.lpstrFile[0] = 0;
	ofn.nMaxFile = nMaxFile;
	ofn.lpstrFilter = lpstrFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return GetOpenFileName(&ofn);
}

void SetButtonState(HWND hDlg, BOOL play, BOOL pause, BOOL resume, BOOL stop, BOOL tempo) {
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_PLAY), play);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_PAUSE), pause);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_RESUME), resume);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_STOP), stop);
	EnableWindow(GetDlgItem(hDlg, IDC_SLIDER_TEMPO), tempo);
}

INT_PTR InitDialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
	HWND hComboBox = GetDlgItem(hDlg, IDC_COMBO_TRACKS);
	for (UINT uID = IDS_TRACK_TITLE_S3; uID <= IDS_TRACK_STAFF_SK; uID++) {
		SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)GetStringFromTable(hInstance, uID));
	}

	HWND hSlider = GetDlgItem(hDlg, IDC_SLIDER_TEMPO);
	SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 200));
	SendMessage(hSlider, TBM_SETPOS, TRUE, 100);

	CheckRadioButton(hDlg, IDC_RADIO_MIDI_FM, IDC_RADIO_MIDI_GM, IDC_RADIO_MIDI_FM);

	return FALSE;
}

INT_PTR CommandEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
	MidiDLL *dll = (MidiDLL*)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	switch (LOWORD(wParam)) {
	case IDC_BUTTON_PLAY:
		if (!dll->playSong())
			MessageBoxFromTable(hDlg, IDS_PLAY_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		else
			SetButtonState(hDlg, TRUE, TRUE, FALSE, TRUE, TRUE);
		return TRUE;

	case IDC_BUTTON_PAUSE:
		if (!dll->pauseSong())
			MessageBoxFromTable(hDlg, IDS_PAUSE_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		else
			SetButtonState(hDlg, TRUE, FALSE, TRUE, TRUE, TRUE);
		return TRUE;

	case IDC_BUTTON_RESUME:
		if (!dll->resumeSong())
			MessageBoxFromTable(hDlg, IDS_RESUME_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		else
			SetButtonState(hDlg, TRUE, TRUE, FALSE, TRUE, TRUE);
		return TRUE;

	case IDC_BUTTON_STOP:
		if (!dll->stopSong())
			MessageBoxFromTable(hDlg, IDS_STOP_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		else
			SetButtonState(hDlg, TRUE, FALSE, FALSE, FALSE, TRUE);
		return TRUE;

	case IDC_COMBO_TRACKS: {
		if (HIWORD(wParam) != CBN_SELCHANGE)
			break;

		int index = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
		if (index != CB_ERR && !dll->setTrack(index + 1)) {
			MessageBoxFromTable(hDlg, IDS_PLAY_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		}
		else {
			SetButtonState(hDlg, dll->isReady(), FALSE, FALSE, FALSE, TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TEMPO), TBM_SETPOS, TRUE, 100);
			SendMessage(GetDlgItem(hDlg, IDC_LABEL_PCT), WM_SETTEXT, 0, (LPARAM)TEXT("100%"));
		}
		return TRUE;
	}

	case IDC_RADIO_MIDI_FM:
		if (!dll->setMode(MIDI_FM)) {
			MessageBoxFromTable(hDlg, IDS_FM_SWITCH_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		}
		else {
			SetButtonState(hDlg, dll->isReady(), FALSE, FALSE, FALSE, TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TEMPO), TBM_SETPOS, TRUE, 100);
			SendMessage(GetDlgItem(hDlg, IDC_LABEL_PCT), WM_SETTEXT, 0, (LPARAM)TEXT("100%"));
		}
		return TRUE;

	case IDC_RADIO_MIDI_GM:
		if (!dll->setMode(MIDI_GM)) {
			MessageBoxFromTable(hDlg, IDS_GM_SWITCH_FAILED, IDS_TITLE, MB_ICONWARNING, hInstance);
		}
		else {
			SetButtonState(hDlg, dll->isReady(), FALSE, FALSE, FALSE, TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_SLIDER_TEMPO), TBM_SETPOS, TRUE, 100);
			SendMessage(GetDlgItem(hDlg, IDC_LABEL_PCT), WM_SETTEXT, 0, (LPARAM)TEXT("100%"));
		}
		return TRUE;
	}

	return FALSE;
}

INT_PTR HScrollEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	MidiDLL *dll = (MidiDLL*)GetWindowLongPtr(hDlg, GWLP_USERDATA);

	int pct = 100;
	switch (LOWORD(wParam)) {
	case TB_THUMBTRACK:
		pct = HIWORD(wParam);
		break;
	case TB_PAGEUP:
	case TB_PAGEDOWN:
	case TB_LINEUP:
	case TB_LINEDOWN:
	case TB_TOP:
	case TB_BOTTOM:
		pct = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
		break;
	default:
		return FALSE;
	}

	if (dll->setTempo(pct)) {
		LPTSTR pLabel = GetFormattedMessage(TEXT("%1!u!%%"), pct);
		if (pLabel) {
			SendMessage(GetDlgItem(hDlg, IDC_LABEL_PCT), WM_SETTEXT, 0, (LPARAM)pLabel);
			LocalFree(pLabel);
		}
	}

	return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		return InitDialogEvent(hDlg, uMsg, wParam, lParam);

	case WM_COMMAND:
		return CommandEvent(hDlg, uMsg, wParam, lParam);

	case WM_HSCROLL:
		return HScrollEvent(hDlg, uMsg, wParam, lParam);

	case WM_USER_MIDIEND:
		SetButtonState(hDlg, TRUE, FALSE, FALSE, FALSE, TRUE);
		return TRUE;

	case WM_CLOSE:
		DestroyWindow(hDlg);
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ERASEBKGND: {
		HBRUSH hbr = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 100, 100, NULL);
		SetViewportExtEx(hdc, rc.right, rc.bottom, NULL);
		FillRect(hdc, &rc, hbr);
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL CreateMainWindow(HINSTANCE hInstance, HWND *hWnd, HWND *hDlg) {
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.lpszClassName = TEXT("SoundTest");
	wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
	if (!RegisterClass(&wc))
		return FALSE;

	*hWnd = CreateWindow(TEXT("SoundTest"), GetStringFromTable(hInstance, IDS_TITLE),
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		(HWND)NULL, (HMENU)NULL, hInstance, (LPVOID)NULL);
	if (!*hWnd)
		return FALSE;

	*hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_FORMVIEW), *hWnd, DialogProc);
	if (!*hDlg)
		return FALSE;

	RECT wndRect, dlgRect;
	GetWindowRect(*hWnd, &wndRect);
	GetClientRect(*hDlg, &dlgRect);
	AdjustWindowRect(&dlgRect, WS_OVERLAPPEDWINDOW, FALSE);
	SetWindowPos(*hWnd, HWND_TOP, wndRect.left, wndRect.top,
		dlgRect.right - dlgRect.left, dlgRect.bottom - dlgRect.top, 0);

	return TRUE;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd) {
	HWND hWnd, hDlg;
	if (!CreateMainWindow(hInstance, &hWnd, &hDlg))
		return MessageBoxFromTableWithError(hDlg, IDS_INIT_ERROR, IDS_TITLE, MB_ICONERROR, hInstance);

	MidiDLL dll;
	SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)&dll);

	if (lpCmdLine && lpCmdLine[0]) {
		// TODO: Do this properly!
		if (!dll.load(hDlg, lpCmdLine)) {
			return MessageBoxFromTableWithError(hDlg, IDS_LOAD_ERROR, IDS_TITLE, MB_ICONERROR, hInstance);
		}
	} else {
		TCHAR dllName[260];
		if (!OpenFileDialog(hDlg, dllName, sizeof(dllName), TEXT("Dynamic Link Library(*.dll)\0*.dll\0")))
			return 0;

		if (!dll.load(hDlg, dllName)) {
			return MessageBoxFromTableWithError(hDlg, IDS_LOAD_ERROR, IDS_TITLE, MB_ICONERROR, hInstance);
		}
	}

	ShowWindow(hWnd, nShowCmd);
	ShowWindow(hDlg, nShowCmd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
