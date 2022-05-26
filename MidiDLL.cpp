#include "MidiDLL.hpp"
#include "MidiInterface.hpp"
#include "Convert.h"

MidiDLL::MidiDLL() :
	_module(NULL),
	_midi(NULL),
	_id(0),
	_mode(MIDI_FM) {
}

MidiDLL::~MidiDLL() {
	unload();
}

BOOL MidiDLL::load(HWND hwnd, LPTSTR filename) {
	unload();

	_module = LoadLibrary(filename);
	if (_module == NULL) {
		return FALSE;
	}

	GetMidiInterfaceFunc GetMidiInterface = (GetMidiInterfaceFunc)GetProcAddress(_module, "GetMidiInterface");
	if (!GetMidiInterface) {
		FreeLibrary(_module);
		_module = NULL;
		return FALSE;
	}

	_midi = GetMidiInterface();
	if (!_midi) {
		FreeLibrary(_module);
		_module = NULL;
		return FALSE;
	}

	return _midi->init(hwnd);
}

void MidiDLL::unload() {
	if (_midi) {
		_midi->stopSong();
		_midi = NULL;
	}

	if (_module)  {
		// FIXME: Trying to free the module causes the application to hang up.
		// _midi probably needs to be deinitialised properly first.
		// FreeLibrary(_module);
		_module = NULL;
	}
}


BOOL MidiDLL::loadSong(short id, unsigned int bgmmode) {
	if (!_midi)
		return FALSE;

	return _midi->loadSong(id, bgmmode);
}

BOOL MidiDLL::playSong() {
	if (!_midi)
		return FALSE;

	return _midi->playSong();
}

BOOL MidiDLL::stopSong() {
	if (!_midi)
		return FALSE;

	return _midi->stopSong();
}

BOOL MidiDLL::pauseSong() {
	if (!_midi)
		return FALSE;

	return _midi->pauseSong();
}

BOOL MidiDLL::resumeSong() {
	if (!_midi)
		return FALSE;

	return _midi->resumeSong();
}

BOOL MidiDLL::setTempo(unsigned int pct) {
	if (!_midi)
		return FALSE;

	return _midi->setTempo(pct);
}


BOOL MidiDLL::setTrack(short id) {
	_id = id;

	if (_id > 0)
		return loadSong(_id, _mode);

	return TRUE;
}

BOOL MidiDLL::setMode(unsigned int bgmmode) {
	_mode = bgmmode;

	if (_id > 0)
		return loadSong(_id, _mode);

	return TRUE;
}

BOOL MidiDLL::isReady() {
	return _midi && (_id > 0);
}

BOOL MidiDLL::save(LPCTSTR filename, int loopCtrl, int totalLoopCnt) {
	HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	BOOL bErrorFlag = save(hFile, loopCtrl, totalLoopCnt);

	CloseHandle(hFile);

	return bErrorFlag;
}

BOOL MidiDLL::save(HANDLE hFile, int loopCtrl, int totalLoopCnt) {
	size_t isize, dsize, osize;
	LPVOID in, decmp, out;
	HRSRC resource;
	HGLOBAL handle;
	BOOL bErrorFlag;

	if (!_module)
		return FALSE;

	int resNum = _id + (_mode * TRACK_LAST);

	resource = FindResource(_module, MAKEINTRESOURCE(resNum), _T("CMP"));
	if (!resource)
		return FALSE;

	isize = SizeofResource(_module, resource);
	if (!isize)
		return FALSE;

	handle = LoadResource(_module, resource);
	if (!handle)
		return FALSE;

	in = LockResource(handle);
	if (!in) {
		FreeResource(handle);
		return FALSE;
	}

	bErrorFlag = DecompressData(in, isize, decmp, dsize);
	FreeResource(handle);
	if (!bErrorFlag)
		return FALSE;

	bErrorFlag = BIN2MID(decmp, dsize, out, osize, loopCtrl, totalLoopCnt);
	HeapFree(GetProcessHeap(), 0, decmp);
	if (!bErrorFlag)
		return FALSE;

	bErrorFlag = WriteFile(hFile, out, osize);
	HeapFree(GetProcessHeap(), 0, out);
	return bErrorFlag;
}

BOOL MidiDLL::DecompressData(const void* src, size_t srcSize, void*& dst, size_t& dstSize) {
	unsigned char wspace[DECOMPRESS_WORKSPACE];
	dstSize = ::DecompressData(src, srcSize, NULL, wspace);
	dst = HeapAlloc(GetProcessHeap(), 0, dstSize);
	if (!dst)
		return FALSE;
	dstSize = ::DecompressData(src, srcSize, dst, wspace);
	return TRUE;
}

BOOL MidiDLL::BIN2MID(const void* src, size_t srcSize, void*& dst, size_t& dstSize, int loopCtrl, int totalLoopCnt) {
	dstSize = ::BIN2MID(src, srcSize, NULL, loopCtrl, totalLoopCnt);
	dst = HeapAlloc(GetProcessHeap(), 0, dstSize);
	if (!dst)
		return FALSE;
	dstSize = ::BIN2MID(src, srcSize, dst, loopCtrl, totalLoopCnt);
	return TRUE;
}

BOOL MidiDLL::WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) {
	DWORD dwBytesWritten;
	if (!::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &dwBytesWritten, NULL))
		return FALSE;

	if (dwBytesWritten != nNumberOfBytesToWrite)
		return FALSE;

	return TRUE;
}
