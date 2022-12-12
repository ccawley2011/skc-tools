#include "WinMsg.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

LPTSTR GetStringFromTable(HINSTANCE hInstance, UINT uID) {
	TCHAR pLocalBuffer[256];
	LPTSTR pBuffer = NULL;
	LPTSTR pCopy;
	int len;

	len = LoadString(hInstance, uID, (LPTSTR)&pBuffer, 0);
	if (len <= 0) {
		/* Calling LoadString with cchBufferMax == 0 doesn't always work properly when
		 * using the ANSI APIs, so a fixed size buffer is used as a fallback.
		 */
		len = LoadString(hInstance, uID, pLocalBuffer, ARRAYSIZE(pLocalBuffer));
		if (len <= 0) {
			return NULL;
		}
		pBuffer = pLocalBuffer;
	}

	pCopy = (LPTSTR)LocalAlloc(LMEM_FIXED, (len + 1) * sizeof(TCHAR));
	if (!pCopy) {
		return NULL;
	}

	CopyMemory(pCopy, pBuffer, len * sizeof(TCHAR));
	pCopy[len] = 0;

	return pCopy;
}

LPTSTR GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId) {
	LPTSTR pBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwMessageId, dwLanguageId, (LPTSTR)&pBuffer, 0, NULL);
	return pBuffer;
}

LPTSTR GetFormattedMessageV(LPCTSTR pMessage, va_list args) {
	LPTSTR pBuffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING, pMessage, 0, 0, (LPTSTR)&pBuffer, 0, (va_list *)&args);
	return pBuffer;
}

LPTSTR GetFormattedMessage(LPCTSTR pMessage, ...) {
	LPTSTR pBuffer;
	va_list args;

	va_start(args, pMessage);
	pBuffer = GetFormattedMessageV(pMessage, args);
	va_end(args);

	return pBuffer;
}

void MessageBoxFromTable(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance) {
	LPTSTR lpMessage = GetStringFromTable(hInstance, uTextID);
	LPTSTR lpCaption = GetStringFromTable(hInstance, uCaptionID);

	MessageBox(hWnd, lpMessage, lpCaption, uType);

	LocalFree(lpMessage);
	LocalFree(lpCaption);
}

DWORD MessageBoxFromTableWithError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance) {
	DWORD dw = GetLastError();

	LPTSTR lpMessage = GetStringFromTable(hInstance, uTextID);
	LPTSTR lpCaption = GetStringFromTable(hInstance, uCaptionID);
	LPTSTR lpMsgBuf = GetErrorMessage(dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	LPTSTR lpDisplayBuf = GetFormattedMessage(TEXT("%1: %2"), lpMessage, lpMsgBuf);

	MessageBox(hWnd, lpDisplayBuf, lpCaption, uType);

	LocalFree(lpMessage);
	LocalFree(lpCaption);
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return dw;
}

DWORD MessageBoxFromTableWithCommDlgError(HWND hWnd, UINT uTextID, UINT uCaptionID, UINT uType, HINSTANCE hInstance, DWORD dwError) {
	LPTSTR lpMessage = GetStringFromTable(hInstance, uTextID);
	LPTSTR lpCaption = GetStringFromTable(hInstance, uCaptionID);
	LPCTSTR lpErrorString = GetCommDlgErrorMessage(dwError);
	LPTSTR lpDisplayBuf = GetFormattedMessage(TEXT("%1: %2"), lpMessage, lpErrorString);

	MessageBox(hWnd, lpDisplayBuf, lpCaption, uType);

	LocalFree(lpMessage);
	LocalFree(lpCaption);
	LocalFree(lpDisplayBuf);

	return dwError;
}

LPCTSTR GetCommDlgErrorMessage(DWORD dwError) {
	switch (dwError) {
#define X(err) case err: return _T(#err)
	X(CDERR_DIALOGFAILURE);
	X(CDERR_FINDRESFAILURE);
	X(CDERR_INITIALIZATION);
	X(CDERR_LOADRESFAILURE);
	X(CDERR_LOADSTRFAILURE);
	X(CDERR_LOCKRESFAILURE);
	X(CDERR_MEMALLOCFAILURE);
	X(CDERR_MEMLOCKFAILURE);
	X(CDERR_NOHINSTANCE);
	X(CDERR_NOHOOK);
	X(CDERR_NOTEMPLATE);
	X(CDERR_REGISTERMSGFAIL);
	X(CDERR_STRUCTSIZE);
	X(PDERR_CREATEICFAILURE);
	X(PDERR_DEFAULTDIFFERENT);
	X(PDERR_DNDMMISMATCH);
	X(PDERR_GETDEVMODEFAIL);
	X(PDERR_INITFAILURE);
	X(PDERR_LOADDRVFAILURE);
	X(PDERR_NODEFAULTPRN);
	X(PDERR_NODEVICES);
	X(PDERR_PARSEFAILURE);
	X(PDERR_PRINTERNOTFOUND);
	X(PDERR_RETDEFFAILURE);
	X(PDERR_SETUPFAILURE);
	X(CFERR_MAXLESSTHANMIN);
	X(CFERR_NOFONTS);
	X(FNERR_BUFFERTOOSMALL);
	X(FNERR_INVALIDFILENAME);
	X(FNERR_SUBCLASSFAILURE);
	X(FRERR_BUFFERLENGTHZERO);
#undef X
	default: return _T("unknown error code");
	}
}
