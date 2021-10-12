/************************************************************
                    Frog Jumping Problem
                      - taiprogramer -
 Description:

 We have 6 frogs, 3 yellow frogs (number 1) and 3 red frogs (number 2). We have
 7 squares in which each frog stays. Each frog can go 1 step if have an empty
 square in front of the face and can go 2 steps if have a frog with a different
 color in front of the face and after that is an empty square.

 Initial state: [1] [1] [1] [] [2] [2] [2]
 Goal state   : [2] [2] [2] [] [1] [1] [1]

 Implement depth-first search algorithm to find goal state.

 From the author: Start reading from `main` func.
*************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define YELLOW_FROG 1
#define RED_FROG 2
#define EMPTY_ROCK 0

typedef struct {
	int rocks[7]; // 1110222
	int empty_rock;
} State;

void set_state(int rocks[7], int empty_rock, State *s);
void print_state(State s);
bool same_state(State s1, State s2);

typedef struct Node {
	State state;
	struct Node *parent;
	int opt_code;
} Node;

bool create_node(State state, Node *parent, int opt_code, Node **result);

// actions
bool yellow_frog_goes_one_step(State s, State *result);
bool yellow_frog_goes_two_steps(State s, State *result);
bool red_frog_goes_one_step(State s, State *result);
bool red_frog_goes_two_steps(State s, State *result);

bool (*actions[5])(State, State *) = {
    NULL, yellow_frog_goes_one_step, yellow_frog_goes_two_steps,
    red_frog_goes_one_step, red_frog_goes_two_steps};

char *msg[1024] = {
    "Initial state",
    "Yellow frog goes one step",
    "Yellow frog goes two steps",
    "Red frog goes one step",
    "Red frog goes two steps",
};

bool execute_action(State s, State *result, int opt_code);

// codeberg.org/taiprogramer/learn-c-lang
// Stack prototype
#define MAX_LENGTH_OF_STACK 1000
typedef Node *Stack_ElementType;
typedef struct {
	Stack_ElementType value[MAX_LENGTH_OF_STACK];
	int top_index;
} Stack;

void makenull_stack(Stack *stack);
bool is_empty_stack(Stack stack);
bool is_full_stack(Stack stack);
void push_stack(Stack_ElementType value, Stack *stack);
void pop_stack(Stack *stack);
Stack_ElementType top_stack(Stack stack);

bool find_state(State s, Stack stack);

bool dfs(State start, State goal, Node **result);

#define NUM_MEM 100
static void *allocations[NUM_MEM];
static int num_alloc = 0;
void cleanup(void);
void print_result(Node *);

int main(void) {
	State start, goal;
	int rocks[7] = {1, 1, 1, 0, 2, 2, 2};
	set_state(rocks, 3, &start);

	int new_rocks[7] = {2, 2, 2, 0, 1, 1, 1};
	set_state(new_rocks, 3, &goal);

	Node *result;
	bool solvable = dfs(start, goal, &result);
	if (solvable)
		print_result(result);
	else
		printf("I\'m limited by the technology at my time.");

	cleanup();
	return 0;
}

void print_result(Node *goal_node) {
	if (goal_node == NULL)
		return;

	print_result(goal_node->parent);
	printf("Action: %s\n", msg[goal_node->opt_code]);
	print_state(goal_node->state);
}

void set_state(int rocks[7], int empty_rock, State *s) {
	for (int i = 0; i < 7; ++i) {
		s->rocks[i] = rocks[i];
	}
	s->empty_rock = empty_rock;
}

void print_state(State s) {
	for (int i = 0; i < 7; ++i) {
		printf("%d ", s.rocks[i]);
	}
	puts("");
}

bool same_state(State s1, State s2) {
	for (int i = 0; i < 7; ++i) {
		if (s1.rocks[i] != s2.rocks[i])
			return false;
	}
	return true;
}

bool create_node(State state, Node *parent, int opt_code, Node **result) {
	if (num_alloc == (NUM_MEM))
		return false;
	Node *node = malloc(sizeof(Node));
	if (node == NULL)
		return false;

	allocations[num_alloc++] = node;
	node->state = state;
	node->parent = parent;
	node->opt_code = opt_code;
	*result = node;
	return true;
}

bool execute_action(State s, State *result, int opt_code) {
	if (opt_code < 1 || opt_code > 4)
		return false;

	return actions[opt_code](s, result);
}

bool yellow_frog_goes_one_step(State s, State *result) {
	*result = s;
	// left-most position 0111222
	if (s.empty_rock == 0)
		return false;
	if (s.rocks[s.empty_rock - 1] != YELLOW_FROG)
		return false;
	// YELLOW_FROG
	// perform swap
	result->empty_rock = s.empty_rock - 1;
	result->rocks[s.empty_rock] = s.rocks[result->empty_rock];
	result->rocks[result->empty_rock] = EMPTY_ROCK;
	return true;
}

bool yellow_frog_goes_two_steps(State s, State *result) {
	*result = s;
	// can't jump 2 steps if empty_rock < 2
	// 1011222
	if (s.empty_rock < 2)
		return false;
	if (s.rocks[s.empty_rock - 1] != RED_FROG)
		return false;
	if (s.rocks[s.empty_rock - 2] != YELLOW_FROG)
		return false;

	// perform swap
	result->empty_rock = s.empty_rock - 2;
	result->rocks[s.empty_rock] = s.rocks[result->empty_rock];
	result->rocks[result->empty_rock] = EMPTY_ROCK;
	return true;
}

bool red_frog_goes_one_step(State s, State *result) {
	*result = s;
	// right-most position 1112220
	if (s.empty_rock == 6)
		return false;
	if (s.rocks[s.empty_rock + 1] != RED_FROG)
		return false;
	// RED_FROG
	// perform swap
	result->empty_rock = s.empty_rock + 1;
	result->rocks[s.empty_rock] = s.rocks[result->empty_rock];
	result->rocks[result->empty_rock] = EMPTY_ROCK;
	return true;
}

bool red_frog_goes_two_steps(State s, State *result) {
	*result = s;

	// can't jump 2 steps if empty_rock > 4
	// 1011222
	if (s.empty_rock > 4)
		return false;
	if (s.rocks[s.empty_rock + 1] != YELLOW_FROG)
		return false;
	if (s.rocks[s.empty_rock + 2] != RED_FROG)
		return false;

	// perform swap
	result->empty_rock = s.empty_rock + 2;
	result->rocks[s.empty_rock] = s.rocks[result->empty_rock];
	result->rocks[result->empty_rock] = EMPTY_ROCK;
	return true;
}

void makenull_stack(Stack *stack) { stack->top_index = MAX_LENGTH_OF_STACK; }

bool is_empty_stack(Stack stack) {
	return stack.top_index == MAX_LENGTH_OF_STACK;
}

bool is_full_stack(Stack stack) { return stack.top_index == 0; }

void push_stack(Stack_ElementType value, Stack *stack) {
	if (is_full_stack(*stack)) {
		printf("push_stack: stack is full.\n");
		return;
	}

	stack->top_index--;
	stack->value[stack->top_index] = value;
}

void pop_stack(Stack *stack) {
	if (is_empty_stack(*stack)) {
		printf("pop_stack: stack is empty.\n");
		return;
	}

	stack->top_index++;
}

Stack_ElementType top_stack(Stack stack) {
	return stack.value[stack.top_index];
}

bool dfs(State start_state, State goal_state, Node **result) {
	Stack open, closed;
	makenull_stack(&open);
	makenull_stack(&closed);

	Node *root;
	if (!create_node(start_state, NULL, 0, &root))
		return false;

	push_stack(root, &open);

	while (!is_empty_stack(open)) {
		Node *node = top_stack(open);
		pop_stack(&open);
		push_stack(node, &closed);

		if (same_state(node->state, goal_state)) {
			*result = node;
			return true;
		}

		// perform actions
		for (int i = 1; i <= 4; ++i) {
			State new_state;
			if (!execute_action(node->state, &new_state, i))
				continue;
			if (find_state(new_state, closed) ||
			    find_state(new_state, open))
				continue;

			Node *new_node;
			if (!create_node(new_state, node, i, &new_node))
				return false;

			push_stack(new_node, &open);
		}
	}

	return false;
}

void cleanup(void) {
	while (num_alloc > 0) {
		free(allocations[--num_alloc]);
	}
}

bool find_state(State s, Stack stack) {
	while (!is_empty_stack(stack)) {
		Node *node = top_stack(stack);
		if (same_state(s, node->state))
			return true;
		pop_stack(&stack);
	}
	return false;
}
