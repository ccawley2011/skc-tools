#ifndef WINMSG_H
#define WINMSG_H

#include <stdarg.h>
#include <tchar.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

LPTSTR GetStringFromTable(HINSTANCE hInstance, UINT uID);
LPTSTR GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId);
LPTSTR GetFormattedMessageV(LPCTSTR pMessage, va_list args);
LPTSTR GetFormattedMessage(LPCTSTR pMessage, ...);
void MessageBoxFromTable(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance);
DWORD MessageBoxFromTableWithError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance);
DWORD MessageBoxFromTableWithCommDlgError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance, DWORD dwError);
LPCTSTR GetCommDlgErrorMessage(DWORD dwError);

#ifdef __cplusplus
}
#endif

#endif
