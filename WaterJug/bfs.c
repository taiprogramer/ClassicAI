// Description: BFS Algorithm for WaterJug problem
// Author: taiprogramer
// License: MIT
// Website: https://taiprogramer.xyz

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define X_CAPACITY 9 // x jug have capacity 9 liters.
#define Y_CAPACITY 4 // y jug have capacity 4 liters.
#define GOAL 6       // x jug or y jug have exactly 6 liters.
typedef struct {
	int x, y;
} State;

void init_state(State *state, int x, int y);
bool is_goal_state(State state);

typedef struct Node {
	State state;
	struct Node *parent;
	// operator code
	// which operator parent node did to produce current node
	int op_code;
} Node;

Node *bfs(State initial_state);

// define operator
// In Water Jug, we have 6 actions
bool pour_water_full_x(State state, State *result);  // op_code = 1
bool pour_water_full_y(State state, State *result);  // op_code = 2
bool pour_water_empty_x(State state, State *result); // op_code = 3
bool pour_water_empty_y(State state, State *result); // op_code = 4
bool pour_water_xy(State state, State *result);      // op_code = 5
bool pour_water_yx(State state, State *result);      // op_code = 6

// Lookup Table (LUT)
bool (*actions[7])(State state, State *result) = {NULL,
                                                  pour_water_full_x,
                                                  pour_water_full_y,
                                                  pour_water_empty_x,
                                                  pour_water_empty_y,
                                                  pour_water_xy,
                                                  pour_water_yx};

const char *action_texts[7] = {
    "First state",
    "Pour water full to x",
    "Pour water full to y",
    "Empty x",
    "Empty y",
    "Pour water from x to y",
    "Pour water from y to x",
};

bool call_operator(State state, State *result, int op_code) {
	if (op_code < 1 || op_code > 6)
		return false;

	return actions[op_code](state, result);
}

#define MAX_LENGTH_OF_QUEUE 100
typedef struct {
	Node *value[MAX_LENGTH_OF_QUEUE];
	int front, rear;
} Queue;

void makenull_queue(Queue *);
bool is_empty_queue(Queue);
bool is_full_queue(Queue);
Node *front_queue(Queue);
void enqueue(Node *, Queue *);
void dequeue(Queue *);

bool find_state(State state, Queue queue);
void print_step_by_step_to_goal(Node *goal_node);

void *allocations[1000]; // memory management
int num_allocations = 0; // number of memory allocations
void clean_up();

int main() {
	State initial_state = {0, 0};
	Node *node = bfs(initial_state);
	print_step_by_step_to_goal(node);
	clean_up();
	return 0;
}

void makenull_queue(Queue *queue) {
	queue->front = -1;
	queue->rear = -1;
}

bool is_empty_queue(Queue queue) { return queue.front == -1; }

bool is_full_queue(Queue queue) {
	return ((queue.rear - queue.front + 1) % MAX_LENGTH_OF_QUEUE) == 0;
}

Node *front_queue(Queue queue) {
	if (!is_empty_queue(queue))
		return queue.value[queue.front];

	printf("front_queue: queue is empty.\n");
	return NULL;
}

void enqueue(Node *element, Queue *queue) {
	if (is_full_queue(*queue)) {
		printf("enqueue: queue is full.\n");
		return;
	}

	queue->rear = (queue->rear + 1) % MAX_LENGTH_OF_QUEUE;
	queue->value[queue->rear] = element;

	if (is_empty_queue(*queue))
		queue->front++;
}

void dequeue(Queue *queue) {
	if (is_empty_queue(*queue)) {
		printf("dequeue: queue is empty.\n");
		return;
	}

	if (queue->front == queue->rear) {
		makenull_queue(queue);
		return;
	}

	queue->front = (queue->front + 1) % MAX_LENGTH_OF_QUEUE;
}

bool pour_water_full_x(State state, State *result) {
	if (state.x < X_CAPACITY) {
		result->x = X_CAPACITY;
		result->y = state.y;
		return true;
	}
	return false;
}

bool pour_water_full_y(State state, State *result) {
	if (state.y < Y_CAPACITY) {
		result->y = Y_CAPACITY;
		result->x = state.x;
		return true;
	}
	return false;
}

bool pour_water_empty_x(State state, State *result) {
	if (state.x > 0) {
		result->x = 0;
		result->y = state.y;
		return true;
	}
	return false;
}

bool pour_water_empty_y(State state, State *result) {
	if (state.y > 0) {
		result->y = 0;
		result->x = state.x;
		return true;
	}
	return false;
}

bool pour_water_xy(State state, State *result) {
	if (state.x > 0 && state.y < Y_CAPACITY) {
		int y_max_need = Y_CAPACITY - state.y;
		if (state.x > y_max_need) {
			result->y = Y_CAPACITY;
			result->x = state.x - y_max_need;
			return true;
		}
		if (state.x <= y_max_need) {
			result->y = state.y + state.x;
			result->x = 0;
			return true;
		}
	}
	return false;
}

bool pour_water_yx(State state, State *result) {
	if (state.y > 0 && state.x < X_CAPACITY) {
		int x_max_need = X_CAPACITY - state.x;
		if (state.y > x_max_need) {
			result->x = X_CAPACITY;
			result->y = state.y - x_max_need;
			return true;
		}
		if (state.y <= x_max_need) {
			result->x = state.x + state.y;
			result->y = 0;
		}
	}
	return false;
}

void init_state(State *state, int x, int y) {
	state->x = x;
	state->y = y;
}

bool is_goal_state(State state) { return state.x == GOAL || state.y == GOAL; }

bool find_state(State state, Queue queue) {
	while (!is_empty_queue(queue)) {
		Node *node = front_queue(queue);
		dequeue(&queue);
		if (node->state.x == state.x && node->state.y == state.y)
			return true;
	}

	return false;
}

Node *bfs(State initial_state) {
	Queue open, closed;
	makenull_queue(&open);
	makenull_queue(&closed);

	Node *root = (Node *)malloc(sizeof(Node));
	allocations[num_allocations++] = root;
	root->state = initial_state;
	root->op_code = 0;
	root->parent = NULL;
	enqueue(root, &open);

	while (!is_empty_queue(open)) {
		Node *node = front_queue(open);
		dequeue(&open);
		enqueue(node, &closed);

		if (is_goal_state(node->state))
			return node;

		int op_code = 1;
		for (; op_code <= 6; ++op_code) {
			State new_state;
			init_state(&new_state, 0, 0);
			if (call_operator(node->state, &new_state, op_code)) {
				if (find_state(new_state, closed) ||
				    find_state(new_state, open))
					continue;

				Node *new_node = (Node *)malloc(sizeof(Node));
				allocations[num_allocations++] = new_node;
				new_node->state = new_state;
				new_node->op_code = op_code;
				new_node->parent = node;
				enqueue(new_node, &open);
			}
		}
	}

	return NULL;
}

void clean_up() {
	while (num_allocations > 0) {
		free(allocations[--num_allocations]);
	}
}

void print_step_by_step_to_goal(Node *goal_node) {
	if (goal_node == NULL) {
		puts("Unreachable goal.");
		return;
	}

	Node *node = goal_node; // just to avoid confusing
	Node *steps[100];
	int i = 0;

	while (node->parent != NULL) {
		steps[i++] = node;
		node = node->parent;
	}

	steps[i++] = node; // root node (have no parent)

	int j = 0; // action counter
	while (i > 0) {
		--i;
		Node *node = steps[i];
		printf("Action %d: %s\n", j++, action_texts[node->op_code]);
		printf("\tX: %d | Y: %d\n", node->state.x, node->state.y);
	}
}
