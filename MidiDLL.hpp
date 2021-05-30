#ifndef MIDIDLL_HPP
#define MIDIDLL_HPP

#include <tchar.h>
#include <windows.h>

class MidiInterface;

class MidiDLL {
public:
	MidiDLL();
	~MidiDLL();

	BOOL load(HWND hwnd, LPTSTR filename);
	void unload();

	BOOL loadSong(short id, unsigned int bgmmode);
	BOOL playSong();
	BOOL stopSong();
	BOOL pauseSong();
	BOOL resumeSong();
	BOOL setTempo(unsigned int pct);

	BOOL setTrack(short id);
	BOOL setMode(unsigned int bgmmode);
	BOOL isReady();

	BOOL save(LPCTSTR filename, int loopCtrl = 0, int totalLoopCnt = 2);
	BOOL save(HANDLE hFile, int loopCtrl = 0, int totalLoopCnt = 2);

private:
	HMODULE _module;
	MidiInterface *_midi;
	short _id;
	unsigned int _mode;

	BOOL DecompressData(const void* src, size_t srcSize, void* &dst, size_t &dstSize);
	BOOL BIN2MID(const void* src, size_t srcSize, void* &dst, size_t &dstSize, int loopCtrl, int totalLoopCnt);
	BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
};

#endif
