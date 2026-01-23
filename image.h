#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include "general.h"
#include "string.h"

struct Image {
	u32  channels;
	bool has_alpha_channel;
	u32  width;
	u32  height;
	u8 data[];

	u64 GetNumPixels() {
		return width * height;
	}

	u64 GetSize() {
		return GetNumPixels() * channels;
	}
};

static Image* AllocImage(u64 data_size);
static Image* LoadImage(String path);

#endif // IMAGE_H_INCLUDED
