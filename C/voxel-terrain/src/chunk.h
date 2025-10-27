#ifndef CHUNK_H
#define CHUNK_H 1

#include "block.h"
#include <assert.h>
#include <stdlib.h>

#define CHUNK_HEIGHT 128
#define CHUNK_WIDTH 16
#define CHUNK_BLOCK_INDEX(x, z, y)                                             \
  ((y) + CHUNK_HEIGHT * ((z) + CHUNK_WIDTH * (x)))

struct Chunk {
  int chunkX, chunkZ;
  enum BlockType data[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
};

int generate_chunk(int chunkX, int chunkZ, struct Chunk **out);

struct Vertex {
  float x, y, z;//TODO this could be changed to just 4, 8, 4 bits per block position in chunk
};

struct VertexArray {
  struct Vertex *data;
  size_t length;
  size_t capacity;
};

struct Face {
  unsigned short v1, v2, v3;
};

struct FaceArray {
  struct Face *data;
  size_t length;
  size_t capacity;
};

struct ChunkMesh {
  struct VertexArray vertices;
  struct FaceArray faces;
};

int mesh_chunk(
	const struct Chunk *chunk,
	const struct Chunk *northChunk,
	const struct Chunk *eastChunk,
	const struct Chunk *southChunk,
	const struct Chunk *westChunk,
	struct ChunkMesh **out
);

// Only used for testing purposes(view in blender)
int save_chunk_mesh_to_obj_file(
	const char *filename,
	struct ChunkMesh **meshes,
	size_t mesh_count
);

#endif
