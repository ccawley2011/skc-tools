#include "MidiDLL.hpp"
#include "MidiInterface.hpp"

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
