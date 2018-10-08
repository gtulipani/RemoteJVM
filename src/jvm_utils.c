#include "jvm_utils.h"
#include "int_vector.h"

#define ISTORE_DESCRIPTION "istore"
#define ILOAD_DESCRIPTION "iload"
#define BIPUSH_DESCRIPTION "bipush"
#define DUP_DESCRIPTION "dup"
#define IAND_DESCRIPTION "iand"
#define IXOR_DESCRIPTION "ixor"
#define IOR_DESCRIPTION "ior"
#define IREM_DESCRIPTION "irem"
#define INEG_DESCRIPTION "ineg"
#define IDIV_DESCRIPTION "idiv"
#define IADD_DESCRIPTION "iadd"
#define IMUL_DESCRIPTION "imul"
#define ISUB_DESCRIPTION "isub"

/**
 * Generic function that executes an action between two ints
 */
typedef int (*integer_function)(int, int);

/**
 * Static function that applies a generic integer_func to the top and lower elements from the stack
 * @return  {@link operation_result} with the result of the operation
 */
static operation_result _jvm_function_apply_two_integer_function(stack *pS,
														  integer_function integer_func);

/**
 * Static function that applies a generic integer_func to the top element from the stack
 * @return  {@link operation_result} with the result of the operation
 */
static operation_result
_jvm_function_apply_integer_function(stack *pS, integer_function integer_func);

/* List of integer-related static functions */
static int _integer_sum(int a, int b) {
	return a + b;
}

static int _integer_sub(int a, int b) {
	return a - b;
}

static int _integer_mul(int a, int b) {
	return a * b;
}

static int _integer_div(int a, int b) {
	return a / b;
}

static int _integer_rem(int a, int b) {
	return a % b;
}

static int _logical_and(int a, int b) {
	return a & b;
}

static int _logical_or(int a, int b) {
	return a | b;
}

static int _logical_xor(int a, int b) {
	return a ^ b;
}

static int _logical_neg(int a, int ignored) {
	return (~a + 1);
}

/**
 * Static function that creates the {@param argument} with the params received as parameters
 * @pre     {@param argument} pointer to jvm_argument already allocated
 * @post    {@param argument} pointer to jvm_argument ready to be used
 * @return  {@link operation_result} with the result of the operation
 */
static operation_result
_jvm_argument_create(const char *byte_code_description, jvm_byte_code byte_code,
					 bool _requires_operand,
					 bool _requires_vector, jvm_function func,
					 jvm_argument *argument) {
	if (!argument)
		return OPERATION_FAILURE_NULL_POINTER;
	argument->byte_code = byte_code;
	argument->byte_code_description = byte_code_description;
	argument->_requires_operand = _requires_operand;
	argument->_requires_vector = _requires_vector;
	argument->func = func;
	return OPERATION_SUCCESS;
}

operation_result
jvm_argument_detect(jvm_argument *argument, unsigned char byte_code) {
	operation_result result;
	bool requires_vector = false;
	bool requires_operand = false;
	switch (byte_code) {
		case ISTORE: {
			requires_vector = true;
			result = _jvm_argument_create(ISTORE_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_istore, argument);
			break;
		}
		case ILOAD: {
			requires_vector = true;
			result = _jvm_argument_create(ILOAD_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_iload, argument);
			break;
		}
		case BIPUSH: {
			requires_operand = true;
			result = _jvm_argument_create(BIPUSH_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_bipush, argument);
			break;
		}
		case DUP: {
			result = _jvm_argument_create(DUP_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_dup, argument);
			break;
		}
		case IAND: {
			result = _jvm_argument_create(IAND_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_iand, argument);
			break;
		}
		case IXOR: {
			result = _jvm_argument_create(IXOR_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_ixor, argument);
			break;
		}
		case IOR: {
			result = _jvm_argument_create(IOR_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_ior, argument);
			break;
		}
		case IREM: {
			result = _jvm_argument_create(IREM_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_irem, argument);
			break;
		}
		case INEG: {
			result = _jvm_argument_create(INEG_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_ineg, argument);
			break;
		}
		case IDIV: {
			result = _jvm_argument_create(IDIV_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_idiv, argument);
			break;
		}
		case IADD: {
			result = _jvm_argument_create(IADD_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_iadd, argument);
			break;
		}
		case IMUL: {
			result = _jvm_argument_create(IMUL_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_imul, argument);
			break;
		}
		case ISUB: {
			result = _jvm_argument_create(ISUB_DESCRIPTION, byte_code,
										  requires_operand, requires_vector,
										  jvm_function_isub, argument);
			break;
		}
		default: {
			return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
		}
	}
	return result;
}

bool jvm_argument_requires_vector(jvm_argument *argument) {
	return argument->_requires_vector;
}

bool jvm_argument_requires_operand(jvm_argument *argument) {
	return argument->_requires_operand;
}

operation_result jvm_function_bipush(void *elem, void *ignored, stack *pS) {
	if (!elem || !pS)
		return OPERATION_FAILURE_NULL_POINTER;
	signed int i = *(char *) elem;
	//Elem is casted to signed int * before being pushed
	return (stack_push(pS, &i));
}

operation_result
jvm_function_dup(void *first_ignored, void *second_ignored, stack *pS) {
	operation_result result;
	if (!pS) {
		result = OPERATION_FAILURE_NULL_POINTER;
	} else {
		int top = stack_pop(pS);
		if (stack_push(pS, &top) != OPERATION_SUCCESS) {
			result = OPERATION_FAILURE_NO_MEMORY;
		} else {
			result = stack_push(pS, &top);
		}
	}
	return result;
}

operation_result jvm_function_istore(void *vector, void *position, stack *pS) {
	operation_result result;
	if (!pS) {
		result = OPERATION_FAILURE_NULL_POINTER;
	} else {
		int_vector *v = (int_vector *) vector;
		//Cast the void * to int_vector *
		int pos = *(unsigned char *) position;
		//Cast the void * first to unsigned char and then to int
		if (!v) {
			result = OPERATION_FAILURE_NULL_POINTER;
		} else {
			int top = stack_pop(pS);
			result = int_vector_set(v, pos, top);
		}
	}
	return result;
}

operation_result jvm_function_iload(void *vector, void *position, stack *pS) {
	operation_result result;
	if (!pS) {
		result = OPERATION_FAILURE_NULL_POINTER;
	} else {
		int_vector *v = (int_vector *) vector;
		//Cast the void * to int_vector *
		int pos = *(unsigned char *) position;
		//Cast the void * first to unsigned char and then to int
		if (!v) {
			result = OPERATION_FAILURE_NULL_POINTER;
		} else {
			int top = int_vector_get(v, pos);
			result = stack_push(pS, &top);
		}
	}
	return result;
}

operation_result
jvm_function_iadd(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _integer_sum);
}

operation_result
jvm_function_isub(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _integer_sub);
}

operation_result
jvm_function_imul(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _integer_mul);
}

operation_result
jvm_function_idiv(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _integer_div);
}

operation_result
jvm_function_irem(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _integer_rem);
}

operation_result
jvm_function_iand(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _logical_and);
}

operation_result
jvm_function_ior(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _logical_or);
}

operation_result
jvm_function_ixor(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_two_integer_function(pS, _logical_xor);
}

operation_result
jvm_function_ineg(void *first_ignored, void *second_ignored, stack *pS) {
	return _jvm_function_apply_integer_function(pS, _logical_neg);
}



operation_result _jvm_function_apply_two_integer_function(stack *pS,
														  integer_function integer_func) {
	operation_result result;
	if (!pS) {
		result = OPERATION_FAILURE_NULL_POINTER;
	} else {
		int top = stack_pop(pS);
		int lower = stack_pop(pS);
		int func_result = integer_func(lower, top);
		//Apply the integer_func with lower and top as arguments
		result = stack_push(pS, &func_result);
	}
	return result;
}

operation_result
_jvm_function_apply_integer_function(stack *pS, integer_function integer_func) {
	operation_result result;
	if (!pS) {
		result = OPERATION_FAILURE_NULL_POINTER;
	} else {
		int top = stack_pop(pS);
		int func_result = integer_func(top, 0);
		//Apply the integer_func with only top argument
		result = stack_push(pS, &func_result);
	}
	return result;
}
