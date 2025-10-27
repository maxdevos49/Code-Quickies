#ifndef BLOCKS_H
#define BLOCKS_H 1

#include <stdbool.h>

enum BlockType {
	AIR_BLOCK = 0,
	DIRT_BLOCK,
	GRASS_BLOCK,
	STONE_BLOCK,
	BEDROCK_BLOCK,
	BLOCK_TYPE_COUNT
};

struct Block {
  enum BlockType type;
  bool isSolid;
};

extern struct Block Blocks[BLOCK_TYPE_COUNT];

void init_block_registry(void);

#endif
