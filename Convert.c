/**
 * The code in this file is heavily based on SKC-Midi by ValleyBell
 *  https://info.sonicretro.org/SKC-Midi
 */

#include "Convert.h"

#include <string.h>

static unsigned long ReadLE32(const unsigned char *src, size_t pos) {
	unsigned long val = 0;
	val |= src[pos + 0];
	val |= src[pos + 1] << 8;
	val |= src[pos + 2] << 16;
	val |= src[pos + 3] << 24;
	return val;
}

static void WriteBE16(unsigned char *dst, size_t pos, unsigned short val) {
	dst[pos + 0] = (val >> 8) & 0xFF;
	dst[pos + 1] = (val >> 0) & 0xFF;
}

static void WriteBE32(unsigned char *dst, size_t pos, unsigned long val) {
	dst[pos + 0] = (val >> 24) & 0xFF;
	dst[pos + 1] = (val >> 16) & 0xFF;
	dst[pos + 2] = (val >>  8) & 0xFF;
	dst[pos + 3] = (val >>  0) & 0xFF;
}

static size_t WriteMIDIValue(unsigned char *FileData, size_t Pos, unsigned long Value) {
	size_t CurPos, ByteCount;
	unsigned long TempLng;

	TempLng = Value;
	ByteCount = 0;
	do {
		TempLng = TempLng >> 7;
		ByteCount = ByteCount + 1;
	} while (TempLng);

	if (!FileData)
		return ByteCount;

	TempLng = Value;
	CurPos = Pos + (ByteCount - 1);
	FileData[CurPos] = 0 | (TempLng & 0x7F);
	TempLng = TempLng >> 7;

	while (TempLng) {
		CurPos = CurPos - 1;
		FileData[CurPos] = 0x80 | (TempLng & 0x7F);
		TempLng = TempLng >> 7;
	}

	return ByteCount;
}


size_t DecompressData(const void *src, size_t srcSize, void *dst) {
	unsigned long CmpSize, BinSize;
	unsigned char DecmprBuf[0x1000];
	unsigned int DecmprOfs1;
	unsigned int DecmprOfs2;
	unsigned int dwCmd;
	unsigned int wWord;
	unsigned int bCnt;
	size_t SrcPos;
	size_t DstPos;
	const unsigned char *CmprData = (const unsigned char *)src;
	unsigned char *BinData = (unsigned char *)dst;

	CmpSize = ReadLE32(CmprData, 0);
	BinSize = ReadLE32(CmprData, 4);

	if (srcSize > CmpSize + 8)
		srcSize = CmpSize + 8;

	memset(DecmprBuf, 0, 0xFEE);
	DecmprOfs1 = 0xFEE;
	dwCmd = 0x0;

	SrcPos = 8;
	DstPos = 0;
	while (SrcPos < srcSize) {
		dwCmd = dwCmd >> 1;

		if (!(dwCmd & 0x100)) {
			if (SrcPos >= srcSize)
				break;

			dwCmd = 0xFF00 | CmprData[SrcPos];
			SrcPos = SrcPos + 1;
		}

		if (dwCmd & 1) {
			if (SrcPos >= srcSize)
				break;

			if (BinData)
				BinData[DstPos] = CmprData[SrcPos];
			DstPos = DstPos + 1;

			DecmprBuf[DecmprOfs1] = CmprData[SrcPos];
			SrcPos = SrcPos + 1;
			DecmprOfs1 = DecmprOfs1 + 1;
			DecmprOfs1 = DecmprOfs1 & 0xFFF;

			continue;
		}

		if (SrcPos >= srcSize)
			break;

		wWord = CmprData[SrcPos] << 8;
		SrcPos = SrcPos + 1;

		if (SrcPos >= srcSize)
			break;

		wWord = wWord | CmprData[SrcPos];
		SrcPos = SrcPos + 1;

		DecmprOfs2 = (wWord >> 8) + ((wWord & 0xF0) << 4);

		for (bCnt = (wWord & 0xF) + 3; bCnt > 0; bCnt--) {
			unsigned char b = DecmprBuf[DecmprOfs2];
			DecmprOfs2 = DecmprOfs2 + 1;
			DecmprOfs2 = DecmprOfs2 & 0xFFF;

			if (BinData)
				BinData[DstPos] = b;
			DstPos = DstPos + 1;

			DecmprBuf[DecmprOfs1] = b;
			DecmprOfs1 = DecmprOfs1 + 1;
			DecmprOfs1 = DecmprOfs1 & 0xFFF;
		}
	}

	if (BinSize > DstPos)
		return BinSize;

	return DstPos;
}

size_t BIN2MID(const void *src, size_t BinSize, void *dst, int LoopCtrl, int TotalLoopCnt) {
	unsigned long BinCommands, LoopCmds, TempLng;
	size_t BinPos, MidPos, LoopPos, TempInt;
	const char *TempStr;
	int LoopCnt;
	const unsigned char *BinFile = (const unsigned char *)src;
	unsigned char *MidFile = (unsigned char *)dst;

	TempLng = ReadLE32(BinFile, 0);
	BinCommands = ReadLE32(BinFile, 4);
	if (0x10 + BinCommands * 8 > BinSize)
		return 0;

	if (MidFile) {
		/* Write MThd */
		WriteBE32(MidFile, 0x00, 0x4D546864);
		WriteBE32(MidFile, 0x04, 6);
		WriteBE16(MidFile, 0x08, 0);
		WriteBE16(MidFile, 0x0A, 1);
		WriteBE16(MidFile, 0x0C, (unsigned short)TempLng);

		/* Write MTrk */
		WriteBE32(MidFile, 0x0E, 0x4D54726B);
		WriteBE32(MidFile, 0x12, 0);
	}
	MidPos = 0x16;

	BinPos = 0x10;
	LoopCnt = 0;
	while (BinPos < BinSize) {
		if (BinCommands == 0)
			break;

		TempLng = ReadLE32(BinFile, BinPos);
		if (TempLng > 0x0FFFFFFF)
			return 0;

		MidPos = MidPos + WriteMIDIValue(MidFile, MidPos, TempLng);
		BinPos = BinPos + 4;

		switch (BinFile[BinPos + 3]) {
		case 0:
			/* Normal Event */
			if (MidFile)
				memcpy(&MidFile[MidPos], &BinFile[BinPos], 3);
			switch (BinFile[BinPos + 0] & 0xF0) {
			case 0x80:
			case 0x90:
			case 0xA0:
			case 0xB0:
			case 0xE0:
				MidPos = MidPos + 3;
				break;
			case 0xC0:
			case 0xD0:
				MidPos = MidPos + 2;
				break;
			}
			break;
		case 1:
			/* Tempo Event */
			if (MidFile) {
				MidFile[MidPos + 0] = 0xFF;
				MidFile[MidPos + 1] = 0x51;
				MidFile[MidPos + 2] = 3;
				MidFile[MidPos + 3] = BinFile[BinPos + 2];   /* convert Little Endian to Big Endian */
				MidFile[MidPos + 4] = BinFile[BinPos + 1];
				MidFile[MidPos + 5] = BinFile[BinPos + 0];
			}
			MidPos = MidPos + 6;
			break;
		case 2:
			/* Loop Start/End */
			if (LoopCnt < 2) {
				if (LoopCtrl) {
					/* write Controller 111 */
					if (MidFile) {
						MidFile[MidPos + 0] = 0xB0;
						MidFile[MidPos + 1] = 0x6F;
						MidFile[MidPos + 2] = LoopCnt;
					}
					MidPos = MidPos + 3;
				} else {
					/* write Winamp-compatible Marker Event */
					TempStr = (LoopCnt & 1) ? "loopEnd" : "loopStart";
					TempInt = strlen(TempStr);
					if (MidFile) {
						MidFile[MidPos + 0] = 0xFF;
						MidFile[MidPos + 1] = 6;
						MidFile[MidPos + 2] = (unsigned char)TempInt;
						memcpy(&MidFile[MidPos + 3], TempStr, TempInt);
					}
					MidPos = MidPos + 3 + TempInt;
				}
			} else {
				/* write empty Meta Event */
				if (MidFile) {
					MidFile[MidPos + 0] = 0xFF;
					MidFile[MidPos + 1] = 0;
					MidFile[MidPos + 2] = 0;
				}
				MidPos = MidPos + 3;
			}
			if (LoopCnt == 0) {
				LoopPos = BinPos;
				LoopCmds = BinCommands;
			} else if (LoopCnt < TotalLoopCnt) {
				BinPos = LoopPos;
				BinCommands = LoopCmds;
			}
			LoopCnt = LoopCnt + 1;
			break;
		}
		BinPos = BinPos + 4;
		BinCommands = BinCommands - 1;
	}

	/* Finalize track */
	if (MidFile) {
		MidFile[MidPos + 0] = 0;
		MidFile[MidPos + 1] = 0xFF;
		MidFile[MidPos + 2] = 0x2F;
		MidFile[MidPos + 3] = 0;
	}
	MidPos = MidPos + 4;

	if (MidFile)
		WriteBE32(MidFile, 0x12, (unsigned long)MidPos - 0x16);

	return MidPos;
}
