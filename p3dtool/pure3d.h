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