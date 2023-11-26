#ifndef H_HANOI_C_SOLVER
#define H_HANOI_C_SOLVER

typedef struct {
	uint8_t disk;
	int8_t from;
	int8_t to;
} Movement;

Movement hint();
void moveOne();

#endif // H_HANOI_C_SOLVER
