#include<stdint.h>

#define MAX_LEVEL 12

typedef struct {
	int8_t row;
	int8_t col;
} Index2D;

typedef struct {
	uint8_t disk;
	int8_t from;
	int8_t to;
} Movement;

extern uint8_t level;
extern uint8_t disks[3][MAX_LEVEL];

static Index2D find(uint8_t n) {
	Index2D idx = {-1, -1};
	for (int8_t col = 0; col < 3; ++col) {
		for (int8_t layer = (int8_t)(level - 1); layer >= 0; --layer) {
			if (disks[col][layer] == n) {
				idx.col = col;
				idx.row = layer;
				return idx;
			}
		}
	}
	return idx;
}

static uint8_t getTopOf(uint8_t t) {
	for (int layer = 0; layer < level; ++layer)
		if (disks[t][layer]) return disks[t][layer];
	return UINT8_MAX;  // if no disk found, the ground is the largest plate.
}

static void move(Movement m) {
	for (int l = 0; l < level; ++l) {
		if (disks[m.from][l]) {
			disks[m.from][l] = 0;
			break;
		}
	}
	for (int l = level - 1; l >= 0; --l) {
		if (!disks[m.to][l]) {
			disks[m.to][l] = m.disk;
			break;
		}
	}
}

static Movement solve(uint8_t n, uint8_t dest) {
	if (n == 0) return (Movement){.disk=0, .from=-1, .to=-1};
	Index2D idx = find(n);
	if (dest == idx.col) {
		return solve(n - 1, dest);
	} else if (!disks[idx.col][idx.row - 1] && getTopOf(dest) > n) {
		return (Movement){.disk=n, .from=idx.col, .to=(int8_t)dest};
	} else {
		return solve(n - 1, 3 - idx.col - dest);
	}
}

Movement hint() {
	return solve(level, 2);
}

void moveOne() {
	move(solve(level, 2));
}