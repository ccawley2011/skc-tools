#ifndef CONVERT_H
#define CONVERT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DECOMPRESS_WORKSPACE 0x1000

size_t DecompressData(const void *src, size_t srcSize, void *dst, void *wspace);
size_t BIN2MID(const void *src, size_t srcSize, void *dst, int loopCtrl, int totalLoopCnt);

#ifdef __cplusplus
}
#endif

#endif
