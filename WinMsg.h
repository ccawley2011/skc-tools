#ifndef WINMSG_H
#define WINMSG_H

#include <stdarg.h>
#include <tchar.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

LPCTSTR GetStringFromTable(HINSTANCE hInstance, UINT uID);
LPTSTR GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId);
LPTSTR GetFormattedMessageV(LPCTSTR pMessage, va_list args);
LPTSTR GetFormattedMessage(LPCTSTR pMessage, ...);
int MessageBoxFromTable(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance);
DWORD MessageBoxFromTableWithError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance);

#ifdef __cplusplus
}
#endif

#endif
