#ifndef __JVM_UTILS_H__
#define __JVM_UTILS_H__

#define VARIABLES_OUTPUT_TITLE "Variables dump"
#define BYTE_CODES_OUTPUT_TITLE "Bytecode trace"

#include <stdbool.h>

#include "stack.h"

/**
 * Generic JVM function that executes an action on the given stack using (or not) arguments obtained from the same stack
 * @param   void*  first optional argument to use within
 * @param   void*  second optional argument to use within
 * @param   stack* pointer to an already created stack
 * @return  {@link operation_result} with the result of the operation
 */
typedef operation_result (*jvm_function)(void *, void *, stack *);

/**
 * Byte code supported by the JVM. The byte_codes are sent in hexadecimal values
 */
typedef enum jvm_byte_code {
	ISTORE = 0x36,
	ILOAD = 0x15,
	BIPUSH = 0x10,
	DUP = 0x59,
	IAND = 0x7E,
	IXOR = 0x82,
	IOR = 0x80,
	IREM = 0x70,
	INEG = 0x74,
	IDIV = 0x6C,
	IADD = 0x60,
	IMUL = 0x68,
	ISUB = 0x64
} jvm_byte_code;

/**
 * Generic JVM argument that stores the name of the operation, the bytecode reprensented by it and the pointer to the function
 * that executes the corresponding action
 */
typedef struct jvm_argument {
	jvm_byte_code byte_code;
	const char *byte_code_description;
	bool _requires_vector;
	bool _requires_operand;
	jvm_function func;
} jvm_argument;

/**
 * Private function that creates the corresponding jvm_argument based on byte_code received as parameter
 * @pre     {@param argument} pointer to jvm_argument already allocated
 * @post    {@param argument} pointer to jvm_argument ready to be used
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_argument_detect(jvm_argument *argument, unsigned char byte_code);

/**
 * Function that returns a bool specifying if the argument passed as parameter requires a vector
 * @pre     {@param argument} pointer to jvm_argument ready to be used
 * @return  {@link bool}
 */
bool jvm_argument_requires_vector(jvm_argument *argument);

/**
 * Function that returns a bool specifying if the argument passed as parameter requires a int operand
 * @pre     {@param argument} pointer to jvm_argument ready to be used
 * @return  {@link bool}
 */
bool jvm_argument_requires_operand(jvm_argument *argument);

/************************
 *     JVM_FUNCTIONS
 ************************/

/**
 * JVM function that pushes the int element received as parameter to a stack. Before doing that, the value is extended to signed int
 * @pre     {@param pS} pointer to stack already created and elem not NULL
 * @post    {@param elem} is pushed in the stack
 * @return  {@link operation_result} with the result of the operation
 */
operation_result jvm_function_bipush(void *elem, void *ignored, stack *pS);

/**
 * JVM function that copies the last element from the stack.
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element from {@param pS} is duplicated
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_dup(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the sum.
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element and its lower are added (in the opposite way) and become the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_iadd(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the substraction.
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element and its lower are substracted (in the opposite way) and become the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_isub(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the multiplication
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element and its lower are multiplied (in the opposite way) and become the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_imul(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the division
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element and its lower are divided (in the opposite way) and become the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_idiv(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the remainder
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @post    The top element and its lower are divided (in the opposite way) and the remainder becomes the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_irem(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the result from the AND operation (bit to bit).
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The AND operation is performed between the top element and its lower and the result becomes the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_iand(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the result from the OR operation (bit to bit).
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The OR operation is performed between the top element and its lower and the result becomes the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_ior(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last two elements from the stack and replace them with the result from the XOR operation (bit to bit).
 * The order of the operands is the opposite as the order on the stack (the top is the last)
 * @pre     {@param pS} pointer to stack already created
 * @post    The XOR operation is performed between the top element and its lower and the result becomes the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_ixor(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last element from the stack and replace it with the two's complement from it.
 * @pre     {@param pS} pointer to stack already created
 * @post    The TWO's COMPLEMENT operation is performed in the top element and the result becomes the new top from {@param pS}
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_ineg(void *first_ignored, void *second_ignored, stack *pS);

/**
 * JVM function that takes the last element from the stack and stores it in {@param int_vector} in position {@param position}
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element is extracted and its value is stored in the position from the intvector.
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_istore(void *int_vector, void *position, stack *pS);

/**
 * JVM function that reads the value from {@param int_vector} in position {@param position} and puts it on the top from the stack
 * @pre     {@param pS} pointer to stack already created
 * @post    The top element is now the value extracted from the int_vector in the corresponding position.
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_function_iload(void *int_vector, void *position, stack *pS);

#endif //__JVM_UTILS_H__
