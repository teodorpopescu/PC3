#ifndef STACK_H
#define STACK_H

#define STACK_DEFAULT_CAPACITY 100

/*   Structures declarations   */
#pragma pack(1)
typedef struct {
	short x;
	short y;
} point_t;
#pragma pack()
typedef struct {
	int size;
	int capacity;
	point_t *data;
} stack_t;

/*   Functions declarations   */
/**
 *    Initialize @p_stack with a capacity equal with STACK_DEFAULT_CAPACITY.
 *    @return 0 if successful or an error code otherwise;
 */
int initialize_stack(stack_t *p_stack);

/**
 *    Add an element to @p_stack. If the new element will exceed the current
 * capacity, the stack will be reallocated.
 *    @return 0 if successful or an error code otherwise;
 */
int stack_push(stack_t *p_stack, int x, int y);

/**
 *    Remove an element from @p_stack.
 *    @return 0 if successful or an error code otherwise;
 */
int stack_pop(stack_t *p_stack);

/**
 *    Query the x member of the newest element of @p_stack.
 *    @return the desired value or 0 if there is no element in the stack;
 */
short stack_query_x(stack_t *p_stack);

/**
 *    Query the y member of the newest element of @p_stack.
 *    @return the desired value or 0 if there is no element in the stack;
 */
short stack_query_y(stack_t *p_stack);

/**
 *    Check if the stack is empty.
 *    @return 1 if the stack is empty or 0 otherwise;
 */
short stack_is_empty(stack_t *p_stack);

/**
 *    Deallocate the data of the stack and set both capacity and size to 0.
 *    @return 0 if successful or an error code otherwise;
 */
int clear_stack(stack_t *p_stack);

#endif
