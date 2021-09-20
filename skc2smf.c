#include "Convert.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <tchar.h>

/* FIXME: MinGW32 is missing this definition */
#ifndef _tcserror
#ifdef _UNICODE
#define _tcserror _wcserror
#else
#define _tcserror strerror
#endif
#endif

#else
#include <sys/stat.h>

typedef char TCHAR;
#define _T(x) (x)

#define _tmain main
#define _ftprintf fprintf
#define _tfopen fopen
#define _tcserror strerror
#endif

#if defined(_WIN32) && !defined(__USE_MINGW_ANSI_STDIO)
#define PRIsizet "Iu"
#else
#define PRIsizet "zu"
#endif

static size_t fsize(FILE *fp) {
#ifdef _WIN32
	return _filelength(_fileno(fp));
#else
	struct stat st;
	fstat(fileno(fp), &st);
	return st.st_size;
#endif
}

int _tmain(int argc, TCHAR **argv) {
	FILE *ifp, *ofp;
	void *in, *decmp, *out;
	size_t isize, dsize, osize;
	const TCHAR *infile = NULL, *outfile = NULL;
	int loopCtrl = 0, totalLoopCnt = 2;

	if (argc < 2) {
		_ftprintf(stderr, _T("Syntax: %s <infile> <outfile>\n"), argv[0]);
		return EXIT_FAILURE;
	}

	infile = argv[1];
	outfile = argv[2];

	ifp = _tfopen(infile, _T("rb"));
	if (ifp) {
		isize = fsize(ifp);
		in = malloc(isize);
		if (!in) {
			_ftprintf(stderr, _T("Out of memory\n"));
			fclose(ifp);
			return EXIT_FAILURE;
		}

		if (fread(in, isize, 1, ifp) != 1) {
			_ftprintf(stderr, _T("Failed to read %") _T(PRIsizet) _T(" bytes from %s\n"), isize, infile);
			fclose(ifp);
			return EXIT_FAILURE;
		}

		fclose(ifp);
	} else {
		_ftprintf(stderr, _T("Failed to open %s for reading: %s\n"), infile, _tcserror(errno));
		return EXIT_FAILURE;
	}

	dsize = DecompressData(in, isize, NULL);
	decmp = malloc(dsize);
	if (!decmp) {
		_ftprintf(stderr, _T("Out of memory\n"));
		free(in);
		return EXIT_FAILURE;
	}
	dsize = DecompressData(in, isize, decmp);
	free(in);

	osize = BIN2MID(decmp, dsize, NULL, loopCtrl, totalLoopCnt);
	out = malloc(osize);
	if (!out) {
		_ftprintf(stderr, _T("Out of memory\n"));
		free(decmp);
		return EXIT_FAILURE;
	}
	osize = BIN2MID(decmp, dsize, out, loopCtrl, totalLoopCnt);
	free(decmp);

	ofp = _tfopen(outfile, _T("wb"));
	if (ofp) {
		fwrite(out, osize, 1, ofp);
		fclose(ofp);
		free(out);
	} else {
		_ftprintf(stderr, _T("Failed to open %s for writing: %s\n"), outfile, _tcserror(errno));
		free(out);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
