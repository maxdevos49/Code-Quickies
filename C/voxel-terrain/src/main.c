#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// enum TileType {
// 	AIR_TILE,
// 	GRASS_BLOCK_TOP_TILE,
// 	GRASS_BLOCK_SIDE_TILE,
// 	DIRT_TILE,
// TILE_COUNT,
// };
// struct Tile {
// 	float u, v;
// };

// struct Tile Tiles[TILE_TYPE_COUNT];

enum BlockType { AIR_BLOCK = 0, DIRT_BLOCK, GRASS_BLOCK, BLOCK_TYPE_COUNT };

struct Block {
  enum BlockType type;
  bool isSolid;
};

struct Block AirBlock = {
    .type = AIR_BLOCK,
    .isSolid = false,
};

struct Block GrassBlock = {
    .type = GRASS_BLOCK,
    .isSolid = true,
};

struct Block DirtBlock = {
    .type = DIRT_BLOCK,
    .isSolid = true,
};

struct Block Blocks[BLOCK_TYPE_COUNT] = {0};

void init_block_registry(void) {
  Blocks[AIR_BLOCK] = AirBlock;
  Blocks[GRASS_BLOCK] = GrassBlock;
  Blocks[DIRT_BLOCK] = DirtBlock;
}

#define CHUNK_HEIGHT 128
#define CHUNK_WIDTH 16
#define CHUNK_BLOCK_INDEX(x, z, y)                                             \
  ((y) * CHUNK_WIDTH * CHUNK_WIDTH + (z) * CHUNK_WIDTH + (x))

struct Chunk {
  int chunkX, chunkY;
  enum BlockType data[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
};

int generate_chunk(int chunkX, int chunkY, struct Chunk **out) {
  assert(out != NULL);

  struct Chunk *chunk = malloc(sizeof(struct Chunk));
  if (chunk == NULL) {
    perror("Failed to allocate chunk");
    return -1;
  }

  chunk->chunkX = chunkX;
  chunk->chunkY = chunkY;

  // Generate a stair stepping chunk for testing purposes
  for (int y = 0; y < CHUNK_HEIGHT; y++) {
    for (int z = 0; z < CHUNK_WIDTH; z++) {
      for (int x = 0; x < CHUNK_WIDTH; x++) {
        int index = CHUNK_BLOCK_INDEX(x, z, y);
        if (y == x + z) {
          chunk->data[index] = GRASS_BLOCK;
        } else if (y < x + z) {
          chunk->data[index] = DIRT_BLOCK;
        } else {
          chunk->data[index] = AIR_BLOCK;
        }
      }
    }
  }

  *out = chunk;
  return 0;
}

struct Vertex {
  float x, y, z;
};

struct VertexArray {
  struct Vertex *data;
  size_t length;
  size_t capacity;
};

int push_back_vertex(struct VertexArray *array, const struct Vertex *value) {
  assert(array != NULL && value != NULL);

  if (array->length >= array->capacity) {
    size_t new_capacity = (array->capacity == 0) ? 4 : array->capacity * 2;
    struct Vertex *new_data =
        realloc(array->data, new_capacity * sizeof(struct Vertex));
    if (new_data == NULL) {
      perror("Failed to expand vertex array");
      return -1;
    }

    array->data = new_data;
    new_data = NULL;
    array->capacity = new_capacity;
  }

  array->data[array->length] = *value;
  array->length++;

  return 0;
}

struct Face {
  int v1, v2, v3;
};

struct FaceArray {
  struct Face *data;
  size_t length;
  size_t capacity;
};

int push_back_face(struct FaceArray *array, struct Face *value) {
  assert(array != NULL && value != NULL);

  if (array->length >= array->capacity) {
    size_t new_capacity = (array->capacity == 0) ? 4 : array->capacity * 2;
    struct Face *new_data =
        realloc(array->data, new_capacity * sizeof(struct Face));
    if (new_data == NULL) {
      perror("Failed to expand vertex array");
      return -1;
    }

    array->data = new_data;
    new_data = NULL;
    array->capacity = new_capacity;
  }

  array->data[array->length] = *value;
  array->length++;

  return 0;
}

struct ChunkMesh {
  struct VertexArray vertices;
  struct FaceArray faces;
};

void add_face(struct ChunkMesh *mesh, float x, float y, float z, float dx1,
              float dy1, float dz1, float dx2, float dy2, float dz2) {
  int base = mesh->vertices.length;
  struct Vertex v0 = {x, y, z};
  struct Vertex v1 = {x + dx1, y + dy1, z + dz1};
  struct Vertex v2 = {x + dx2, y + dy2, z + dz2};
  struct Vertex v3 = {x + dx1 + dx2, y + dy1 + dy2, z + dz1 + dz2};

  push_back_vertex(&mesh->vertices, &v0);
  push_back_vertex(&mesh->vertices, &v1);
  push_back_vertex(&mesh->vertices, &v2);
  push_back_vertex(&mesh->vertices, &v3);

  struct Face f1 = {base, base + 1, base + 2};
  struct Face f2 = {base + 1, base + 3, base + 2};
  push_back_face(&mesh->faces, &f1);
  push_back_face(&mesh->faces, &f2);
}

struct BlockCoords {
  int x;
  int y;
  int z;
};

static inline struct BlockCoords chunk_block_coords(size_t index) {
  struct BlockCoords c;

  c.y = index / (CHUNK_WIDTH * CHUNK_WIDTH);
  size_t rem = index % (CHUNK_WIDTH * CHUNK_WIDTH);

  c.z = rem / CHUNK_WIDTH;
  c.x = rem % CHUNK_WIDTH;

  return c;
}

int mesh_chunk(const struct Chunk *chunk, const struct Chunk *northChunk,
               const struct Chunk *eastChunk, const struct Chunk *southChunk,
               const struct Chunk *westChunk, struct ChunkMesh **out) {
  assert(chunk != NULL && out != NULL);

  struct ChunkMesh *mesh = malloc(sizeof(struct ChunkMesh));
  if (mesh == NULL) {
    perror("Failed to mesh chunk");
    return -1;
  }

  mesh->vertices = (struct VertexArray){0};
  mesh->faces = (struct FaceArray){0};

  int offsetX = chunk->chunkX * CHUNK_WIDTH;
  int offsetZ = chunk->chunkY * CHUNK_WIDTH;

  for (int i = 0; i < CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT; i++) {
    enum BlockType type = chunk->data[i];

    if (type == AIR_BLOCK) {
      continue;
    }

    struct BlockCoords pos = chunk_block_coords(i);

    // +X (Right/East)
    if ((pos.x + 1 < CHUNK_WIDTH &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x + 1, pos.z, pos.y)] ==
             AIR_BLOCK) ||
        (eastChunk != NULL && pos.x + 1 == CHUNK_WIDTH &&
         eastChunk->data[CHUNK_BLOCK_INDEX(0, pos.z, pos.y)] == AIR_BLOCK)) {
      add_face(mesh, pos.x + 1 + offsetX, pos.y, pos.z + offsetZ, 0, 1, 0, 0, 0,
               1);
    }

    // -X (Left/West)
    if ((pos.x - 1 >= 0 &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x - 1, pos.z, pos.y)] ==
             AIR_BLOCK) ||
        (westChunk != NULL && pos.x - 1 == -1 &&
         westChunk->data[CHUNK_BLOCK_INDEX(CHUNK_WIDTH - 1, pos.z, pos.y)] ==
             AIR_BLOCK)) {
      add_face(mesh, pos.x + offsetX, pos.y, pos.z + offsetZ, 0, 1, 0, 0, 0, 1);
    }

    // +Z (Back/North)
    if ((pos.z + 1 < CHUNK_WIDTH &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x, pos.z + 1, pos.y)] ==
             AIR_BLOCK) ||
        (pos.z + 1 == CHUNK_WIDTH && northChunk != NULL &&
         northChunk->data[CHUNK_BLOCK_INDEX(pos.x, 0, pos.y)] == AIR_BLOCK)) {
      add_face(mesh, pos.x, pos.y, pos.z + 1 + offsetZ, 0, 1, 0, 1, 0, 0);
    }

    // -Z (Front face/South)
    if ((pos.z - 1 >= 0 &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x, pos.z - 1, pos.y)] ==
             AIR_BLOCK) ||
        (southChunk != NULL && pos.z - 1 == -1 &&
         southChunk->data[CHUNK_BLOCK_INDEX(pos.x, CHUNK_WIDTH - 1, pos.y)] ==
             AIR_BLOCK)) {
      add_face(mesh, pos.x + offsetX, pos.y, pos.z + offsetZ, 0, 1, 0, 1, 0, 0);
    }

    // +Y (Top face)
    if ((pos.y + 1 < CHUNK_HEIGHT &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x, pos.z, pos.y + 1)] ==
             AIR_BLOCK) ||
        pos.y + 1 == CHUNK_HEIGHT) {
      add_face(mesh, pos.x + offsetX, pos.y + 1, pos.z + offsetZ, 1, 0, 0, 0, 0,
               1);
    }

    // -Y (Bottom face)
    if ((pos.y - 1 >= 0 &&
         chunk->data[CHUNK_BLOCK_INDEX(pos.x, pos.z, pos.y - 1)] ==
             AIR_BLOCK) ||
        pos.y - 1 == -1) {
      add_face(mesh, pos.x + offsetX, pos.y, pos.z + offsetZ, 1, 0, 0, 0, 0, 1);
    }
  }

  *out = mesh;
  return 0;
}

int output_meshes_obj(const char *filename, struct ChunkMesh **meshes,
                      size_t mesh_count) {
  if (!filename || !meshes || mesh_count == 0)
    return -1;

  FILE *fp = fopen(filename, "w");
  if (!fp)
    return -2;

  size_t vertex_offset = 0;

  for (size_t m = 0; m < mesh_count; m++) {
    const struct ChunkMesh *mesh = meshes[m];

    // Skip empty meshes
    if ((!mesh->vertices.data || mesh->vertices.length == 0) &&
        (!mesh->faces.data || mesh->faces.length == 0))
      continue;

    fprintf(fp, "o chunk_%zu\n", m); // separate each mesh as an object/group

    // Write vertices
    for (size_t i = 0; i < mesh->vertices.length; i++) {
      const struct Vertex *v = &mesh->vertices.data[i];
      if (fprintf(fp, "v %f %f %f\n", v->x, v->y, v->z) < 0) {
        fclose(fp);
        return -3;
      }
    }

    // Write faces (OBJ uses 1-based indexing, adjusted by vertex_offset)
    for (size_t i = 0; i < mesh->faces.length; i++) {
      const struct Face *f = &mesh->faces.data[i];
      if (fprintf(fp, "f %zu %zu %zu\n", f->v1 + 1 + vertex_offset,
                  f->v2 + 1 + vertex_offset, f->v3 + 1 + vertex_offset) < 0) {
        fclose(fp);
        return -3;
      }
    }

    // Update vertex offset for next mesh
    vertex_offset += mesh->vertices.length;
  }

  fclose(fp);
  return 0;
}

int main(void) {
  init_block_registry();

  //
  // Generate 3x3 world
  //

  struct Chunk *chunk = NULL;
  if (generate_chunk(0, 0, &chunk) < 0) {
    fprintf(stderr, "Failed to generate chunk!\n");
    return EXIT_FAILURE;
  }

  struct Chunk *northChunk = NULL;
  if (generate_chunk(0, 1, &northChunk) < 0) {
    fprintf(stderr, "Failed to generate north chunk!\n");
    return EXIT_FAILURE;
  }

  struct Chunk *eastChunk = NULL;
  if (generate_chunk(1, 0, &eastChunk) < 0) {
    fprintf(stderr, "Failed to generate east chunk!\n");
    return EXIT_FAILURE;
  }

  struct Chunk *southChunk = NULL;
  if (generate_chunk(0, -1, &southChunk) < 0) {
    fprintf(stderr, "Failed to generate south chunk!\n");
    return EXIT_FAILURE;
  }

  struct Chunk *westChunk = NULL;
  if (generate_chunk(-1, 0, &westChunk) < 0) {
    fprintf(stderr, "Failed to generate west chunk!\n");
    return EXIT_FAILURE;
  }

  //   double meshChunkStart = get_time_ms();

  struct ChunkMesh *mesh = NULL;
  if (mesh_chunk(chunk, northChunk, eastChunk, southChunk, westChunk, &mesh) <
      0) {
    fprintf(stderr, "Failed to mesh chunk!\n");
    return EXIT_FAILURE;
  }

  struct ChunkMesh *northMesh = NULL;
  if (mesh_chunk(northChunk, NULL, NULL, chunk, NULL, &northMesh) < 0) {
    fprintf(stderr, "Failed to mesh north chunk!\n");
    return EXIT_FAILURE;
  }

  struct ChunkMesh *eastMesh = NULL;
  if (mesh_chunk(eastChunk, NULL, NULL, NULL, chunk, &eastMesh) < 0) {
    fprintf(stderr, "Failed to mesh east chunk!\n");
    return EXIT_FAILURE;
  }

  struct ChunkMesh *southMesh = NULL;
  if (mesh_chunk(southChunk, chunk, NULL, NULL, NULL, &southMesh) < 0) {
    fprintf(stderr, "Failed to mesh south chunk!\n");
    return EXIT_FAILURE;
  }

  struct ChunkMesh *westMesh = NULL;
  if (mesh_chunk(westChunk, NULL, chunk, NULL, NULL, &westMesh) < 0) {
    fprintf(stderr, "Failed to mesh west chunk!\n");
    return EXIT_FAILURE;
  }

  //   double meshChunkEnd = get_time_ms();

  //   printf("[Benchmark] mesh_chunk took %.3f ms\n",
  // 		 meshChunkEnd - meshChunkStart);

  //   printf("[Benchmark] Vertices: %zu, Faces: %zu\n", mesh->vertices.length,
  // 		 mesh->faces.length);

  //   double vertexKB = (mesh->vertices.length * sizeof(struct Vertex)) /
  //   1024.0; double indicesKB = (mesh->faces.length * sizeof(struct Face)) /
  //   1024.0; double totalKB = vertexKB + indicesKB;

  //   printf("[Benchmark] GPU memory: %.2f KB (vertices), %.2f KB (indices),
  //   %.2f "
  // 		 "KB total\n",
  // 		 vertexKB, indicesKB, totalKB);

  struct ChunkMesh *meshes[5];
  meshes[0] = mesh;
  meshes[1] = northMesh;
  meshes[2] = eastMesh;
  meshes[3] = southMesh;
  meshes[4] = westMesh;

  // Output obj for testing purposes(Open in blender)
  if (output_meshes_obj("test.obj", meshes, 5) < 0) {
    fprintf(stderr, "Failed to output chunk mesh!\n");
    return EXIT_FAILURE;
  }

  free(chunk);
  chunk = NULL;

  return EXIT_SUCCESS;
}
