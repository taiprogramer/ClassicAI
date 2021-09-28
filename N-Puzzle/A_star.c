// Implement solution for N-Puzzle with A_star algorithm (class:
// Best First Search)
// A_star means:
//	- f = (g + h) (min is better)
//	h: heuristic value (approximation cost from current state to goal)
//	g: cost from current state to start state
//	if f equal -> compare g
//	if g equal -> first-come first-served
// This code is written in NeoVim by taiprogramer

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 3 // 3 x 3 matrix (8-puzzle)
#define EMPTY_TILE 0  // empty tile notation
struct State {
	int matrix[MATRIX_SIZE][MATRIX_SIZE];
	int row, col; // position of empty tile, count from zero
};

void set_state(struct State *state, int matrix[][MATRIX_SIZE], int row,
               int col);
bool same_state(struct State s1, struct State s2);
void print_state(struct State state);

// available actions
bool empty_left(struct State state, struct State *new_state);  // op_code = 1
bool empty_right(struct State state, struct State *new_state); // op_code = 2
bool empty_up(struct State state, struct State *new_state);    // op_code = 3
bool empty_down(struct State state, struct State *new_state);  // op_code = 4

// Lookup Table (LUT)
bool (*actions[5])(struct State state, struct State *new_state) = {
    NULL, empty_left, empty_right, empty_up, empty_down};

bool execute_action(struct State state, struct State *new_state, int op_code);

struct Node {
	struct Node *parent;
	struct State state;
	int g, h, f;
};

struct Node *A_star(struct State start_state, struct State goal_state);
int h1(struct State s1, struct State s2);

void print_result(struct Node *goal_node);

static void *allocations[10000];
static int num_alloc = 0;

void cleanup();

int main() {
	int start_state_matrix[][MATRIX_SIZE] = {
	    {1, 4, 2},
	    {3, 7, 5},
	    {6, 8, 0},
	};
	int goal_state_matrix[][MATRIX_SIZE] = {
	    {0, 1, 2},
	    {3, 4, 5},
	    {6, 7, 8},
	};

	struct State start_state, goal_state;
	set_state(&start_state, start_state_matrix, 2, 2);
	set_state(&goal_state, goal_state_matrix, 0, 0);

	struct Node *result = A_star(start_state, goal_state);

	if (result == NULL)
		puts("I\'m limited by the technology of my time.");

	print_result(result);

	cleanup();
	return 0;
}

void set_state(struct State *state, int matrix[][MATRIX_SIZE], int row,
               int col) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			state->matrix[i][j] = matrix[i][j];
			if (matrix[i][j] == 0) {
				state->row = row;
				state->col = col;
			}
		}
	}
}

bool same_state(struct State s1, struct State s2) {
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			if (s1.matrix[i][j] != s2.matrix[i][j])
				return false;
		}
	}

	return true;
}

void print_state(struct State state) {
	puts("");
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			printf("%d ", state.matrix[i][j]);
		}
		puts("");
	}
	puts("");
}

bool empty_left(struct State state, struct State *new_state) {
	// deep copy old state
	*new_state = state;

	int row = state.row;
	int col = state.col;
	// empty box already on the left
	if (col == 0)
		return false;
	// perform swap
	new_state->matrix[row][col] = state.matrix[row][col - 1];
	new_state->matrix[row][col - 1] = EMPTY_TILE;
	new_state->col--;
	return true;
}

bool empty_right(struct State state, struct State *new_state) {
	*new_state = state;

	int row = state.row;
	int col = state.col;
	// empty box already on the right
	if (col == 2)
		return false;

	new_state->matrix[row][col] = state.matrix[row][col + 1];
	new_state->matrix[row][col + 1] = EMPTY_TILE;
	new_state->col++;

	return true;
}

bool empty_up(struct State state, struct State *new_state) {
	*new_state = state;

	int row = state.row;
	int col = state.col;
	// empty box already on the top
	if (row == 0)
		return false;

	new_state->matrix[row][col] = state.matrix[row - 1][col];
	new_state->matrix[row - 1][col] = EMPTY_TILE;
	new_state->row--;

	return true;
}

bool empty_down(struct State state, struct State *new_state) {
	*new_state = state;

	int row = state.row;
	int col = state.col;

	// empty box already on the bottom
	if (row == 2)
		return false;

	new_state->matrix[row][col] = state.matrix[row + 1][col];
	new_state->matrix[row + 1][col] = EMPTY_TILE;
	new_state->row++;

	return true;
}

bool execute_action(struct State state, struct State *new_state, int op_code) {
	if (op_code < 1 || op_code > 4)
		return false;
	return actions[op_code](state, new_state);
}

void print_result(struct Node *goal_node) {
	if (goal_node == NULL)
		return;

	print_result(goal_node->parent);
	print_state(goal_node->state);
}

int h1(struct State s1, struct State s2) {
	int count = 0;
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			if (s1.matrix[i][j] != 0) {
				if (s1.matrix[i][j] != s2.matrix[i][j])
					count++;
			}
		}
	}
	return count;
}

bool find_node(struct State state, struct Node *arr[100], int len) {
	while (len > 0) {
		struct Node *node = arr[--len];
		if (node == NULL)
			continue;
		if (same_state(node->state, state))
			return true;
	}

	return false;
}

int cmp(const void *a, const void *b) {
	struct Node *node1 = *((struct Node **)a);
	struct Node *node2 = *((struct Node **)b);
	if (node1->f > node2->f)
		return -1;
	if (node1->f < node2->f)
		return 1;
	return 0;
}

struct Node *A_star(struct State start_state, struct State goal_state) {
	struct Node *open[100];
	int num_open = 0;
	struct Node *closed[100];
	int num_closed = 0;

	struct Node *root = malloc(sizeof(struct Node));

	allocations[num_alloc++] = root;
	root->state = start_state;
	root->parent = NULL;
	root->g = 0;
	root->h = h1(start_state, goal_state);
	root->f = root->g + root->h;
	open[num_open++] = root;

	while (num_open > 0) {
		struct Node *node = open[--num_open];
		if (num_closed == 100)
			return NULL;
		closed[num_closed++] = node;

		if (same_state(node->state, goal_state))
			return node;

		// perform actions
		// left-right-up-down
		for (int i = 1; i <= 4; ++i) {
			struct State new_state;
			if (!execute_action(node->state, &new_state, i))
				continue;
			if (find_node(new_state, closed, num_closed) ||
			    find_node(new_state, open, num_open))
				continue;

			struct Node *new_node = malloc(sizeof(struct Node));
			allocations[num_alloc++] = new_node;
			new_node->state = new_state;
			new_node->parent = node;
			new_node->g = node->g + 1;
			new_node->h = h1(new_node->state, goal_state);
			new_node->f = new_node->g + new_node->h;
			if (num_open == 100)
				return NULL;
			open[num_open++] = new_node;
		}
		// perform quick sort desc order
		qsort(open, num_open, sizeof(struct Node *), cmp);
	}

	return NULL;
}

void cleanup() {
	while (num_alloc > 0) {
		free(allocations[--num_alloc]);
	}
}
