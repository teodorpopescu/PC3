#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

int initialize_stack(stack_t *p_stack)
{
	p_stack->size = 0;
	p_stack->capacity = STACK_DEFAULT_CAPACITY;
	p_stack->data = malloc(STACK_DEFAULT_CAPACITY * sizeof(point_t));
	if (p_stack->data == NULL) {
		fprintf(stderr, "Stack initialization failed\n");
		return 1;
	}
	return 0;
}

int stack_push(stack_t *p_stack, int x, int y)
{
	if (p_stack->size >= p_stack->capacity) {
		p_stack->capacity *= 2;
		point_t *tmp = realloc(p_stack->data,
			p_stack->capacity * sizeof(point_t));
		if (tmp == NULL) {
			fprintf(stderr, "Stack reallocation failed\n");
			return 1;
		}
		p_stack->data = tmp;
	}
	p_stack->data[p_stack->size].x = x;
	p_stack->data[p_stack->size].y = y;
	++p_stack->size;
	return 0;
}

int stack_pop(stack_t *p_stack)
{
	if (p_stack->size == 0) {
		fprintf(stderr, "Trying to pop an empty stack\n");
		return 1;
	}
	--p_stack->size;
	return 0;
}

short stack_query_x(stack_t *p_stack)
{
	if (p_stack->size == 0) {
		fprintf(stderr, "Trying to query an empty stack\n");
		return 0;
	} else {
		return p_stack->data[p_stack->size - 1].x;
	}
}

short stack_query_y(stack_t *p_stack)
{
	if (p_stack->size == 0) {
		fprintf(stderr, "Trying to query an empty stack\n");
		return 0;
	} else {
		return p_stack->data[p_stack->size - 1].y;
	}
}

short stack_is_empty(stack_t *p_stack)
{
	if (p_stack->size == 0) return 1;
	else return 0;
}

int clear_stack(stack_t *p_stack)
{
	if(p_stack == NULL) return 0;
	if(p_stack->data == NULL) return 0;
	free(p_stack->data);
	p_stack->size = 0;
	p_stack->capacity = 0;
	return 0;
}
