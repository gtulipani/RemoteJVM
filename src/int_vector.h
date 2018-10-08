#ifndef __INT_VECTOR_H__
#define __INT_VECTOR_H__

#include <stdio.h>
#include "result.h"

typedef struct int_vector {
	int *_data;
	int _size;
} int_vector;

/**
 * Initializes the {@param v} with the {@param size} received as parameter
 * @pre    {@param v} pointer to int_vector already allocated
 * @post   {@param v} pointer to int_vector ready to be used
 * @return {@link operation_result} with the result of the operation
 */
operation_result int_vector_create(int_vector *v, int size);

/**
 * Gets an element from the vector {@param v} located in position {@param pos}
 * @param  pos starts from 0 until (_size - 1)
 * @pre    {@param v} pointer to int_vector ready to be used
 * @post   - The element from {@param v} located in position {@param pos} is retrieved and returned
 * @return {@link int} with the value
 */
int int_vector_get(const int_vector *v, int pos);

/**
 * Sets an element in the vector {@param v} located in position {@param pos}
 * @param  pos starts from 0 until (_size - 1)
 * @pre    - {@param v} pointer to int_vector ready to be used
 *         - {@param pos} is a valid one
 * @post   - The element from {@param v} located in position {@param pos} is replaced with {@param src}
 * @return {@link operation_result} with the result of the operation
 */
operation_result int_vector_set(int_vector *v, int pos, int src);

int int_vector_size(int_vector *v);

/**
 * Destroys the allocated memory in {@param v}
 * @post   the allocated memory is released in {@param v}
 */
void int_vector_destroy(int_vector *v);

/**
 * Function that prints the variables from the int_vector received as parameter
 * @pre  {@param v} pointer to int_vector ready to be used
 * @return {@link operation_result} with the result of the operation
 */
void int_vector_print_elements(int_vector *vec, FILE* out);

#endif //__INT_VECTOR_H__
