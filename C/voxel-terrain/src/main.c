#include "block.h"
#include "performance.h"
#include "chunk.h"
#include <stdio.h>

/**
 * TODO: Dynamic Chunk Streaming System
 *
 * 1. Chunk Memory Management
 *    - Implement an LRU (Least Recently Used) cache to manage loaded chunks.
 *      • Use a hashmap for O(1) chunk lookup by (chunkX, chunkZ) coordinate.
 *      • When capacity is reached, evict least recently used chunks (not accessed/rendered recently).
 *      • Each cache entry should store:
 *          - Chunk pointer
 *          - Last access timestamp or frame counter
 *          - State: {Unloaded, Loading, Ready, Dirty}
 *
 * 2. Chunk Loading Strategy
 *    - Generate chunk coordinates in a spiral pattern around the player’s current chunk position.
 *      • The spiral radius defines view distance (in chunks).
 *      • As the player moves, reuse overlapping chunks and load new ones on the edges.
 *      • Spawn chunk generation jobs in worker threads to avoid blocking the main thread.
 *
 * 3. Render Ordering
 *    - Maintain a list of *visible* chunks each frame.
 *      • Start with the spiral-ordered chunk list.
 *      • Apply an insertion sort by distance to camera center for fine-tuned ordering.
 *      • Optionally use camera forward vector to prioritize chunks within the field of view (dot product test).
 *    - Opaque pass:
 *        → Render front-to-back (early Z culling).
 *    - Transparent pass:
 *        → Render back-to-front using the same list reversed.
 *      • Consider simplifying distant transparent surfaces (e.g., render far water as opaque).
 *
 * 4. Visibility Optimization (optional, later)
 *    - Determine chunks within the player’s frustum using simple AABB–frustum intersection.
 *    - If the player is underground or looking straight down, dynamically shrink render radius.
 *      • Example: looking down → render 3×3 or 5×5 chunks only.
 */


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



//   struct ChunkMesh *northMesh = NULL;
//   if (mesh_chunk(northChunk, NULL, NULL, chunk, NULL, &northMesh) < 0) {
//     fprintf(stderr, "Failed to mesh north chunk!\n");
//     return EXIT_FAILURE;
//   }

//   struct ChunkMesh *eastMesh = NULL;
//   if (mesh_chunk(eastChunk, NULL, NULL, NULL, chunk, &eastMesh) < 0) {
//     fprintf(stderr, "Failed to mesh east chunk!\n");
//     return EXIT_FAILURE;
//   }

//   struct ChunkMesh *southMesh = NULL;
//   if (mesh_chunk(southChunk, chunk, NULL, NULL, NULL, &southMesh) < 0) {
//     fprintf(stderr, "Failed to mesh south chunk!\n");
//     return EXIT_FAILURE;
//   }

//   struct ChunkMesh *westMesh = NULL;
//   if (mesh_chunk(westChunk, NULL, chunk, NULL, NULL, &westMesh) < 0) {
//     fprintf(stderr, "Failed to mesh west chunk!\n");
//     return EXIT_FAILURE;
//   }

    double meshChunkStart = get_time_ms();

	struct ChunkMesh *mesh = NULL;
	if (mesh_chunk(chunk, northChunk, eastChunk, southChunk, westChunk, &mesh) < 0) {
		fprintf(stderr, "Failed to mesh chunk!\n");
		return EXIT_FAILURE;
	}

    double meshChunkEnd = get_time_ms();

    printf("[Benchmark] mesh_chunk took %.3f ms\n",
  		 meshChunkEnd - meshChunkStart);

    printf("[Benchmark] Vertices: %zu, Faces: %zu\n", mesh->vertices.length,
  		 mesh->faces.length);

    double vertexKB = (mesh->vertices.length * sizeof(struct Vertex)) / 1024.0;
	double indicesKB = (mesh->faces.length * sizeof(struct Face)) / 1024.0;
	double totalKB = vertexKB + indicesKB;

    printf("[Benchmark] GPU memory: %.2f KB (vertices), %.2f KB (indices), %.2f KB total\n", vertexKB, indicesKB, totalKB);

//   struct ChunkMesh *meshes[5];
//   meshes[0] = mesh;
//   meshes[1] = northMesh;
//   meshes[2] = eastMesh;
//   meshes[3] = southMesh;
//   meshes[4] = westMesh;

//   // Output obj for testing purposes(Open in blender)
//   if (save_chunk_mesh_to_obj_file("test.obj", meshes, 5) < 0) {
//     fprintf(stderr, "Failed to output chunk mesh!\n");
//     return EXIT_FAILURE;
//   }

  free(chunk);
  free(northChunk);
  free(eastChunk);
  free(southChunk);
  free(westChunk);
  chunk = NULL;

  return EXIT_SUCCESS;
}
