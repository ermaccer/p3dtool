#pragma once

struct p3d_header {
	int header;
	int version;
	int fileSize;
};

struct p3d_chunk {
	int id;
	int dataSize;
	int size;
};

enum p3d_chunks {
	CHUNK_TEXTURE = 0x19000,
	CHUNK_IMAGE = 0x19001,
	CHUNK_IMAGEDATA = 0x19002,
};

struct p3d_texture_header {
	int unk;
	int x;
	int y;
	int bits;
	int alphaDepth;
	int mipMaps;
	int type;
	int usage;
	int priority;
};
struct p3d_image_header {
	int unk;
	int x;
	int y;
	int bits;
	int pal;
	int alpha;
	int format;
};
