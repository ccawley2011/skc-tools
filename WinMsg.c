#include "WinMsg.h"

LPCTSTR GetStringFromTable(HINSTANCE hInstance, UINT uID) {
#ifdef __MINGW32__
	static TCHAR pBuffer[256];
	LoadString(hInstance, uID, pBuffer, 256*sizeof(TCHAR));
#else
	LPCTSTR pBuffer = NULL;
	LoadString(hInstance, uID, (LPTSTR)&pBuffer, 0);
#endif
	return pBuffer;
}

LPTSTR GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId) {
	LPTSTR pBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwMessageId, dwLanguageId, (LPTSTR)&pBuffer, 0, NULL);
	return pBuffer;
}

LPTSTR GetFormattedMessageV(LPCTSTR pMessage, va_list args) {
	LPTSTR pBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING, pMessage, 0, 0, (LPTSTR)&pBuffer, 0, &args);
	return pBuffer;
}

LPTSTR GetFormattedMessage(LPCTSTR pMessage, ...) {
	va_list args = NULL;
	va_start(args, pMessage);

	LPTSTR pBuffer = GetFormattedMessageV(pMessage, args);

	va_end(args);

	return pBuffer;
}

int MessageBoxFromTable(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance) {
	return MessageBox(hWnd, GetStringFromTable(hInstance, uTextID), GetStringFromTable(hInstance, uCaptionID), uType);
}

DWORD MessageBoxFromTableWithError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance) {
	DWORD dw = GetLastError();

	LPCTSTR lpMessage = GetStringFromTable(hInstance, uTextID);
	LPTSTR lpMsgBuf = GetErrorMessage(dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	LPTSTR lpDisplayBuf = GetFormattedMessage(TEXT("%1: %2"), lpMessage, lpMsgBuf);

	MessageBox(hWnd, lpDisplayBuf, GetStringFromTable(hInstance, uCaptionID), uType);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return dw;
}
