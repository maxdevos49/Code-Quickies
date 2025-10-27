#include "lru-cache.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define CIRCLE_RADIUS 50.0f

#define VISUAL_RADIUS 10

#define CHUNK_WIDTH		   16
#define CHUNK_HEIGHT	   128
#define CHUNK_CACHE_MARGIN 2

struct Game {
	struct {
		float x;
		float y;
		float z;
	} player;

	int renderDistance;
	int simulationDistance;

	LRUCACHE *chunkCache;
};

#define GOTO(row, col) printf("\033[%d;%dH", (row), (col))
#define CLEAR()		   printf("\033[2J")
#define HIDE_CURSOR()  printf("\033[?25l")
#define SHOW_CURSOR()  printf("\033[?25h")
#define RESET_COLOR()  printf("\033[0m")

typedef enum {
	CHUNK_COLOR_PLAYER,
	CHUNK_COLOR_ACTIVE,
	CHUNK_COLOR_LOADED,
	CHUNK_COLOR_UNLOADED,
	CHUNK_COLOR_PENDING,
	CHUNK_COLOR_ERROR
} ChunkColor;

/**
 * Returns ANSI color codes for the chosen chunk color.
 * Uses bright foregrounds with dark or high-contrast backgrounds.
 */
static inline const char *get_chunk_color(ChunkColor color) {
	switch (color) {
	case CHUNK_COLOR_PLAYER:
		return "\033[1;30;46m"; // bright black on cyan
	case CHUNK_COLOR_ACTIVE:
		return "\033[1;37;42m"; // white on green
	case CHUNK_COLOR_LOADED:
		return "\033[1;30;43m"; // black on yellow
	case CHUNK_COLOR_UNLOADED:
		return "\033[1;37;40m"; // white on black
	case CHUNK_COLOR_PENDING:
		return "\033[1;30;44m"; // black on blue
	case CHUNK_COLOR_ERROR:
		return "\033[1;37;41m"; // white on red
	default:
		return "\033[0m"; // reset
	}
}

/**
 * Draws a grid centered around the player (radius defines how far out).
 * Example: radius=2 produces a 5x5 grid of blank cells.
 */
void draw_grid(void) {
	CLEAR();

	for (int z = -VISUAL_RADIUS; z <= VISUAL_RADIUS; z++) {
		for (int x = -VISUAL_RADIUS; x <= VISUAL_RADIUS; x++) {
			GOTO(z + VISUAL_RADIUS + 2, (x + VISUAL_RADIUS) * 2 + 2);

			// Mark the world origin (0,0)
			if (x == 0 && z == 0) {
				printf("\033[1;37;41m●\033[0m"); // red dot
			} else {
				printf("\033[47m  \033[0m"); // normal empty cell
			}
		}
	}
	fflush(stdout);
}

void draw_chunk_state(int chunkX, int chunkZ, ChunkColor color, char label, int playerChunkX, int playerChunkZ) {
	// Relative to player
	int relX = chunkX - playerChunkX;
	int relZ = chunkZ - playerChunkZ;

	if (relX < -VISUAL_RADIUS || relX > VISUAL_RADIUS || relZ < -VISUAL_RADIUS || relZ > VISUAL_RADIUS) {
		return; // out of visual field
	}

	GOTO(relZ + VISUAL_RADIUS + 2, (relX + VISUAL_RADIUS) * 2 + 2);
	printf("%s%c \033[0m", get_chunk_color(color), label);
	fflush(stdout);
}

int main(void) {
	HIDE_CURSOR();
	srand(time(NULL));

	struct Game game = {
		.player = {
			.x = 18.0F,
			.y = ((float)rand() / (float)RAND_MAX) * 128.0F,
			.z = 0,
		},
		.renderDistance = 1};

	for (int t = 0; t < 1000; t++) {	// START GAME LOOP
		float angle = (float)t * 0.5F; // speed of rotation
		game.player.x = CIRCLE_RADIUS * cosf(angle);
		game.player.z = CIRCLE_RADIUS * sinf(angle);

		if (game.chunkCache == NULL) {
			size_t radius = game.renderDistance + CHUNK_CACHE_MARGIN;
			size_t diameter = (radius * 2 + 1);
			size_t cache_capacity = diameter * diameter;
			game.chunkCache = lru_cache_create(cache_capacity);

			assert(game.chunkCache != NULL && "Chunk cache failed to allocate.");
		}

		int playerBlockX = (int)game.player.x;
		int playerBlockY = (int)game.player.y;
		int playerBlockZ = (int)game.player.z;
		int playerChunkX = playerBlockX / CHUNK_WIDTH;
		int playerChunkZ = playerBlockZ / CHUNK_WIDTH;

		printf(
			"XYZ: %0.4f / %0.04f / %0.4f\n"
			"Block: %d %d %d\n"
			"Chunk: %d 0 %d\n",
			game.player.x, game.player.y, game.player.z,
			playerBlockX, playerBlockY, playerBlockZ,
			playerChunkX, playerChunkZ);

		//
		// Spiral around player for processing relevant chunks
		//

		draw_grid();

		int x = 0;
		int z = 0;
		int dx = 0;
		int dz = -1;
		int maxSide = (game.renderDistance * 2 + 1);
		int maxSteps = maxSide * maxSide;

		for (int i = 0; i < maxSteps; i++) {
			int chunkX = playerChunkX + x;
			int chunkZ = playerChunkZ + z;

			void *chunk = lru_cache_get(game.chunkCache, chunkX, chunkZ);
			if (chunk != NULL) {
				// TODO calculate player distance from chunk center and assign distance to each chunk

				// TODO if: chunk is within a specified distance of the player tick the chunk

				// TODO if: chunk mesh is out of date or does not exist yet mark it for meshing(Done on a different thread so its async)
				// TODO else: push chunk onto a list for rendering
				draw_chunk_state(chunkX, chunkZ, CHUNK_COLOR_LOADED, 'L', playerChunkX, playerChunkZ);
			} else {
				chunk = "Placeholder Chunk!";
				lru_cache_put(game.chunkCache, chunkX, chunkZ, chunk);
				draw_chunk_state(chunkX, chunkZ, CHUNK_COLOR_PENDING, '?', playerChunkX, playerChunkZ);
			}

			if (x == z || (x < 0 && x == -z) || (x > 0 && x == 1 - z)) {
				int temp = dx;
				dx = -dz;
				dz = temp;
			}

			x += dx;
			z += dz;

			draw_chunk_state(playerChunkX, playerChunkZ, CHUNK_COLOR_PLAYER, 'P', playerChunkX, playerChunkZ);

			draw_chunk_state(0, 0, CHUNK_COLOR_ERROR, '*', playerChunkX, playerChunkZ);

			// usleep(50000);
		};

		//
		// Render stage
		//

		//TODO sort chunk list for rendering for true distances.

		//TODO render chunks

	} // END GAME LOOP

	fflush(stdout);

	sleep(100);

	SHOW_CURSOR();
	printf("\033[%d;0H", game.renderDistance * 2 + 3); // move cursor below grid
	RESET_COLOR();
	printf("\n");
	return EXIT_SUCCESS;
}
