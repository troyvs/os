#pragma once
#include <stddef.h>
typedef struct {
	void * baseAddress;
	size_t Size;
	unsigned int width;
	unsigned int height;
	unsigned int pixelsperscanline;
}FrameBuffer;