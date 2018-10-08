#include <stdlib.h>

#include "int_vector.h"

operation_result int_vector_create(int_vector *v, int size) {
	v->_data = malloc(size * sizeof(int));
	v->_size = size;
	if (v->_data) {
		for (int pos = 0; pos < size; pos++) {
			int_vector_set(v, pos, 0);
		}
		return OPERATION_SUCCESS;
	} else {
		return OPERATION_FAILURE_NO_MEMORY;
	}
}

int int_vector_get(const int_vector *v, int pos) {
	return v->_data[pos];
}

operation_result int_vector_set(int_vector *v, int pos, int src) {
	if (pos >= v->_size || pos < 0) {
		return OPERATION_FAILURE_OUT_OF_BOUNDS;
	} else {
		v->_data[pos] = src;
		return OPERATION_SUCCESS;
	}
}

int int_vector_size(int_vector *v) {
	if (!v) {
		return -1;
	} else {
		return v->_size;
	}
}

void int_vector_destroy(int_vector *v) {
	free(v->_data);
}

void int_vector_print_elements(int_vector *vec, FILE* out) {
	for (int i = 0; i < vec->_size; i++) {
		fprintf(out, "%08x\n", int_vector_get(vec, i));
	}
}
