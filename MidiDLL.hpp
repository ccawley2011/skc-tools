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

private:
	HMODULE _module;
	MidiInterface *_midi;
	short _id;
	unsigned int _mode;
};

#endif
