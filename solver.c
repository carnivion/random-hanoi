#include<stdint.h>

#define MAX_LEVEL 12

typedef struct {
	uint8_t row;
	uint8_t col;
} Index2D;

extern uint8_t level;
extern uint8_t disks[3][MAX_LEVEL];

static Index2D find(uint8_t n) {
	Index2D idx = {-1, -1};
	for (int col = 0; col < 3; ++col) {
		for (int layer = level - 1; layer >= 0; --layer) {
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
	return -1;
}

void solve(uint8_t n, uint8_t dest) {
	if (n == 0) return;
	Index2D idx = find(n);
	while (1) {  // for sequential solving
		if (dest == idx.col) {
			solve(n - 1, dest);
			return;
		} else if (!disks[idx.col][idx.row - 1] && getTopOf(dest) > n) {
			disks[idx.col][idx.row] = 0;
			for (int l = level - 1; l >= 0; --l) {
				if (!disks[dest][l]) {
					disks[dest][l] = n;
					break;
				}
			}
			// solve(n - 1, dest);  // uncomment for sequential solving
			return;
		} else {
			solve(n - 1, 3 - idx.col - dest);
			return;  // comment sequential solving
		}
	}
}

void hint() {
	solve(level, 2);
}