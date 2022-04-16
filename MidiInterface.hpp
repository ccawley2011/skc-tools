#ifndef MIDIINTERFACE_H
#define MIDIINTERFACE_H

#include <windows.h>

/**
 * Based on code from skc-mod-loader: https://github.com/sonicretro/skc-mod-loader/blob/master/SKCModLoader/MidiInterface.h
 */

/**
 * The following message is posted by MIDIOUT.DLL to the window handle passed to MidiInterface::init()
 * whenever a non-looping track comes to an end.
 */
#define WM_USER_MIDIEND 0x464

 /**
  * This enum is used to switch between "FM Synthesizer" and "General MIDI" modes.
  */
enum {
	MIDI_FM = 0,
	MIDI_GM = 1
};

/**
 * This enum contains a list of songs to be played, as used by MidiInterface::loadSong(). The value matches the number
 * in the resources. Note that this starts from 1, whereas the in-game sound test starts from 0.
 */
enum {
	TRACK_TITLE_S3         = 1,
	TRACK_AIZ1             = 2,
	TRACK_AIZ2             = 3,
	TRACK_HCZ1             = 4,
	TRACK_HCZ2             = 5,
	TRACK_MGZ1             = 6,
	TRACK_MGZ2             = 7,
	TRACK_CNZ1             = 8,
	TRACK_CNZ2             = 9,
	TRACK_FBZ1             = 10,
	TRACK_FBZ2             = 11,
	TRACK_ICZ1             = 12,
	TRACK_ICZ2             = 13,
	TRACK_LBZ1             = 14,
	TRACK_LBZ2             = 15,
	TRACK_MHZ1             = 16,
	TRACK_MHZ2             = 17,
	TRACK_SZ1              = 18,
	TRACK_SZ2              = 19,
	TRACK_LRZ1             = 20,
	TRACK_LRZ2             = 21,
	TRACK_SSZ              = 22,
	TRACK_DEZ1             = 23,
	TRACK_DEZ2             = 24,
	TRACK_MINIBOSS         = 25,
	TRACK_BOSS             = 26,
	TRACK_DDZ              = 27,
	TRACK_BONUS_ORBS       = 28,
	TRACK_SPECIAL_STAGE    = 29,
	TRACK_BONUS_SLOTS      = 30,
	TRACK_BONUS_GUMBALL    = 31,
	TRACK_KNUCKLES_S3      = 32,
	TRACK_ALZ              = 33,
	TRACK_BPZ              = 34,
	TRACK_DPZ              = 35,
	TRACK_CGZ              = 36,
	TRACK_EMZ              = 37,
	TRACK_GAMEOVER         = 38,
	TRACK_CONTINUE         = 39,
	TRACK_ACTCLEAR         = 40,
	TRACK_1UP_S3           = 41,
	TRACK_EMERALD          = 42,
	TRACK_INVINCIBILITY_S3 = 43,
	TRACK_COMPETITION      = 44,
	TRACK_UNUSED           = 45,
	TRACK_DATA_SELECT      = 46,
	TRACK_FINAL_BOSS       = 47,
	TRACK_DROWNING         = 48,
	TRACK_ALLCLEAR_S3      = 49,
	TRACK_STAFF_S3         = 50,
	TRACK_KNUCKLES_SK      = 51,
	TRACK_TITLE_SK         = 52,
	TRACK_1UP_SK           = 53,
	TRACK_INVINCIBILITY_SK = 54,
	TRACK_ALLCLEAR_SK      = 55,
	TRACK_STAFF_SK         = 56,

	TRACK_LAST = TRACK_STAFF_SK
};

#ifdef __WATCOMC__
#pragma aux my_thiscall "_*" \
        __parm __routine [ecx] \
        __value __struct __struct __caller [] \
        __modify [eax ecx edx];

#define THISCALL __declspec(__pragma("my_thiscall"))
#else
#define THISCALL
#endif

class MidiInterface {
public:
	MidiInterface(HMODULE moduleHandle);
	~MidiInterface();

	virtual THISCALL BOOL init(HWND hwnd) = 0;
	virtual THISCALL BOOL loadSong(short id, unsigned int bgmmode) = 0;
	virtual THISCALL BOOL playSong() = 0;
	virtual THISCALL BOOL stopSong() = 0;
	virtual THISCALL BOOL pauseSong() = 0;
	virtual THISCALL BOOL resumeSong() = 0;
	virtual THISCALL BOOL setTempo(unsigned int pct) = 0;
};

typedef MidiInterface *(__stdcall *GetMidiInterfaceFunc)();

#endif
