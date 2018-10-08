#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>
#include "result.h"

typedef struct stack_node {
	void *_data;
	struct stack_node *_lower;
} stack_node;

typedef struct stack {
	size_t _elem_size;
	size_t _stack_size;
	stack_node *_top;
} stack;

/**
 * Initializes the {@param pS} received as parameter
 * @pre    {@param pS} pointer to stack already allocated
 * @post   {@param pS} pointer to stack ready to be used
 * @return {@link operation_result} with the result of the operation
 */
operation_result stack_create(stack *pS, size_t elem_size);

/**
 * Pushes the element {@param elem} received as parameter in the {@param pS}
 * @pre    {@param pS} pointer to stack already created
 * @post   {@param elem} is added to the top stack
 * @return {@link operation_result} with the result of the operation
 */
operation_result stack_push(stack *pS, int *elem);

/**
 * Pops the element found in the top of {@param pS} and returns the value
 * @pre    {@param pS} pointer to stack already created
 * @post   - The top element from {@param pS} is extracted and returned
 * @return {@link int} with the element
 */
int stack_pop(stack *pS);

/**
 * Frees the memory used by the struct
 * @pre  {@param pS} pointer to stack already created
 * @post the allocated memory is released and {@param pS} pointers to NULL
 */
void stack_destroy(stack *pS);

#endif //__STACK_H__
