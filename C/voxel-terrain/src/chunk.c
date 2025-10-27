#include "chunk.h"
#include <cglm/cglm.h>
#include <cglm/noise.h>
#include <stdio.h>
#include <stdlib.h>

int generate_chunk(int chunkX, int chunkZ, struct Chunk **out) {
	assert(out != NULL);

	struct Chunk *chunk = malloc(sizeof(struct Chunk));
	if (chunk == NULL) {
		perror("Failed to allocate chunk");
		return -1;
	}

	chunk->chunkX = chunkX;
	chunk->chunkZ = chunkZ;

	float scale = 1.0f / 10.0f;
	float maxTerrainHeight = 60.0f;

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_WIDTH; z++) {

		float n = glm_perlin_vec2( (vec2) {
				((chunkX * (float)CHUNK_WIDTH) + x + 0.5f) * scale,
				((chunkZ * (float)CHUNK_WIDTH) + z + 0.5f) * scale
		});

		int height = (int)((n * 0.5f + 0.5f) * maxTerrainHeight);

		for (int y = 0; y < CHUNK_HEIGHT; y++) {
			int index = CHUNK_BLOCK_INDEX(x, z, y);
			if (y > height) {
				chunk->data[index] = AIR_BLOCK;
			} else if (y == height) {
				chunk->data[index] = GRASS_BLOCK;
			} else if (y < height) {
				chunk->data[index] = DIRT_BLOCK;
			}
		}
		}
	}

	*out = chunk;
	return 0;
}

int push_back_vertex(struct VertexArray *array, const struct Vertex *value) {
	assert(array != NULL && value != NULL);

	if (array->length >= array->capacity) {
		size_t new_capacity = (array->capacity == 0) ? 4 : array->capacity * 2;
		struct Vertex *new_data = realloc(array->data, new_capacity * sizeof(struct Vertex));
		if (new_data == NULL) {
			return -1;
		}

		array->data = new_data;
		array->capacity = new_capacity;
	}

	array->data[array->length] = *value;
	array->length++;

	return 0;
}

int push_back_face(struct FaceArray *array, struct Face *value) {
	assert(array != NULL && value != NULL);

	if (array->length >= array->capacity) {
		size_t new_capacity = (array->capacity == 0) ? 4 : array->capacity * 2;
		struct Face *new_data = realloc(array->data, new_capacity * sizeof(struct Face));
		if (new_data == NULL) {
			return -1;
		}

		array->data = new_data;
		array->capacity = new_capacity;
	}

	array->data[array->length] = *value;
	array->length++;

	return 0;
}

void add_face(
	struct ChunkMesh *mesh,
	float originX, float originY, float originZ,
	float dx1, float dy1, float dz1,
	float dx2, float dy2, float dz2
) {
	int base = mesh->vertices.length;

	struct Vertex v0 = {originX, originY, originZ};

	struct Vertex v1 = {originX + dx1, originY + dy1, originZ + dz1};
	struct Vertex v2 = {originX + dx2, originY + dy2, originZ + dz2};
	struct Vertex v3 = {originX + dx1 + dx2, originY + dy1 + dy2, originZ + dz1 + dz2};

	push_back_vertex(&mesh->vertices, &v0);
	push_back_vertex(&mesh->vertices, &v1);
	push_back_vertex(&mesh->vertices, &v2);
	push_back_vertex(&mesh->vertices, &v3);

	struct Face f1 = {base, base + 1, base + 2};
	struct Face f2 = {base + 1, base + 3, base + 2};
	push_back_face(&mesh->faces, &f1);
	push_back_face(&mesh->faces, &f2);
}

int mesh_chunk(
		const struct Chunk *chunk,
		const struct Chunk *northChunk,
		const struct Chunk *eastChunk,
		const struct Chunk *southChunk,
		const struct Chunk *westChunk,
		struct ChunkMesh **out
) {
	assert(chunk != NULL && out != NULL);

	struct ChunkMesh *mesh = malloc(sizeof(struct ChunkMesh));
	if (mesh == NULL) {
		perror("Failed to mesh chunk");
		return -1;
	}

	mesh->vertices = (struct VertexArray){0};
	mesh->faces = (struct FaceArray){0};

	int offsetX = chunk->chunkX * CHUNK_WIDTH;
	int offsetZ = chunk->chunkZ * CHUNK_WIDTH;

	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_WIDTH; z++) {
			for (int y = 0; y < CHUNK_HEIGHT; y++) {

				enum BlockType type = chunk->data[CHUNK_BLOCK_INDEX(x, z, y)];

				if (type == AIR_BLOCK) {
					continue;
				}

				// +X (East)
				if (
					(x + 1 < CHUNK_WIDTH && chunk->data[CHUNK_BLOCK_INDEX(x + 1, z, y)] == AIR_BLOCK) ||
					(x + 1 == CHUNK_WIDTH && eastChunk && eastChunk->data[CHUNK_BLOCK_INDEX(0, z, y)] == AIR_BLOCK)
				) {
					add_face(mesh, x + 1 + offsetX, y, z + offsetZ, 0, 1, 0, 0, 0, 1);
				}

				// -X (West)
				if (
					(x - 1 >= 0 && chunk->data[CHUNK_BLOCK_INDEX(x - 1, z, y)] == AIR_BLOCK)
					|| (x - 1 == -1 && westChunk && westChunk->data[CHUNK_BLOCK_INDEX(CHUNK_WIDTH - 1, z, y)] == AIR_BLOCK)
				) {
					add_face(mesh, x + offsetX, y, z + offsetZ, 0, 1, 0, 0, 0, 1);
				}

				// +Z (North)
				if (
					(z + 1 < CHUNK_WIDTH && chunk->data[CHUNK_BLOCK_INDEX(x, z + 1, y)] == AIR_BLOCK)
					|| (z + 1 == CHUNK_WIDTH && northChunk && northChunk->data[CHUNK_BLOCK_INDEX(x, 0, y)] == AIR_BLOCK)
				) {
					add_face(mesh, x + offsetX, y, z + 1 + offsetZ, 0, 1, 0, 1, 0, 0);
				}

				// -Z (South)
				if (
					(z - 1 >= 0 && chunk->data[CHUNK_BLOCK_INDEX(x, z - 1, y)] == AIR_BLOCK)
					|| (z - 1 == -1 && southChunk && southChunk->data[CHUNK_BLOCK_INDEX(x, CHUNK_WIDTH - 1, y)] == AIR_BLOCK)
				) {
					add_face(mesh, x + offsetX, y, z + offsetZ, 0, 1, 0, 1, 0, 0);
				}

				// +Y (Top)
				if (
					(y + 1 == CHUNK_HEIGHT || chunk->data[CHUNK_BLOCK_INDEX(x, z, y + 1)] == AIR_BLOCK)
				) {
					add_face(mesh, x + offsetX, y + 1, z + offsetZ, 1, 0, 0, 0, 0, 1);
				}

				// -Y (Bottom)
				if (
					(y == 0 || chunk->data[CHUNK_BLOCK_INDEX(x, z, y - 1)] == AIR_BLOCK)
				) {
					add_face(mesh, x + offsetX, y, z + offsetZ, 1, 0, 0, 0, 0, 1);
				}
			}
		}
	}

	*out = mesh;
	return 0;
}

int save_chunk_mesh_to_obj_file(
	const char *filename,
	struct ChunkMesh **meshes,
	size_t mesh_count
) {
	if (!filename || !meshes || mesh_count == 0) {
		return -1;
	}

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		return -1;
	}

	size_t vertex_offset = 0;

	for (size_t m = 0; m < mesh_count; m++) {
		const struct ChunkMesh *mesh = meshes[m];

		// Skip empty meshes
		if (
			(!mesh->vertices.data || mesh->vertices.length == 0)
			&& (!mesh->faces.data || mesh->faces.length == 0)
		) {
			continue;
		}

		fprintf(fp, "o chunk_%zu\n", m); // separate each mesh as an object/group

		// Write vertices
		for (size_t i = 0; i < mesh->vertices.length; i++) {
			const struct Vertex *v = &mesh->vertices.data[i];
			if (fprintf(fp, "v %f %f %f\n", v->x, v->y, v->z) < 0) {
				fclose(fp);
				return -1;
			}
		}

		// Write faces (OBJ uses 1-based indexing, adjusted by vertex_offset)
		for (size_t i = 0; i < mesh->faces.length; i++) {
			const struct Face *f = &mesh->faces.data[i];
			if (
				fprintf(
					fp,
					"f %zu %zu %zu\n",
					f->v1 + 1 + vertex_offset,
					f->v2 + 1 + vertex_offset,
					f->v3 + 1 + vertex_offset
				) < 0
			) {
				fclose(fp);
				return -1;
			}
		}

		// Update vertex offset for next mesh
		vertex_offset += mesh->vertices.length;
	}

	fclose(fp);
	return 0;
}
