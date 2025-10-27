#include <stdbool.h>
#include "block.h"

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

struct Block StoneBlock = {
	.type = STONE_BLOCK,
	.isSolid = true,
};

struct Block BedrockBlock = {
	.type = STONE_BLOCK,
	.isSolid = true,
};

struct Block Blocks[BLOCK_TYPE_COUNT] = {0};

void init_block_registry(void) {
  Blocks[AIR_BLOCK] = AirBlock;
  Blocks[GRASS_BLOCK] = GrassBlock;
  Blocks[DIRT_BLOCK] = DirtBlock;
  Blocks[STONE_BLOCK] = StoneBlock;
  Blocks[BEDROCK_BLOCK] = BedrockBlock;
}
