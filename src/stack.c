#include <memory.h>
#include "stack.h"

operation_result stack_create(stack *pS, size_t elem_size) {
	pS->_elem_size = elem_size;
	pS->_stack_size = 0;
	pS->_top = NULL;
	return OPERATION_SUCCESS;
}

operation_result stack_push(stack *pS, int *elem) {
	if (!elem || !pS)
		return OPERATION_FAILURE_NULL_POINTER;

	stack_node *node = malloc(sizeof(stack_node));
	if (!node)
		return OPERATION_FAILURE_NO_MEMORY;

	node->_data = malloc(sizeof(pS->_elem_size));
	if (!node->_data) {
		return OPERATION_FAILURE_NO_MEMORY;
	}

	memcpy(node->_data, elem, pS->_elem_size);

	node->_lower = pS->_top;
	pS->_top = node;
	pS->_stack_size += 1;
	return OPERATION_SUCCESS;
}

int stack_pop(stack *pS) {
	if (!pS->_top) {
		return OPERATION_FAILURE_NULL_POINTER;
	}

	stack_node *node = pS->_top;
	int result = *(int*)node->_data;

	pS->_top = node->_lower;
	free(node->_data);
	free(node);

	pS->_stack_size -= 1;
	return result;
}

void stack_destroy(stack *pS) {
	while (pS->_top != NULL) {
		stack_pop(pS);
	}
	pS->_stack_size = 0;
}
