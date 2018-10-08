#include "jvm_server.h"
#include "int_vector.h"
#include "jvm_utils.h"
#include "socket.h"

#define CHUNK_SIZE 100

static operation_result send_variables(socket_t *socket, int_vector *vec) {
	int variables_quantity = int_vector_size(vec);
	for (int i = 0; i < variables_quantity; i++) {
		if (socket_send_int(socket, int_vector_get(vec, i)) ==
			SOCKET_CONNECTION_ERROR) {
			return OPERATION_FAILURE_CONNECTION_FAILED;
		}
	}
	return OPERATION_SUCCESS;
}

/**
 * Static function that process each byte_code by:
 *          - Creating a {@link jvm_argument} with the byte_code
 *          - Executing the jvm_argument
 *          - Printing the jvm_argument
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
process_byte_codes_chunk(const char *byte_codes, long bytes, int_vector *vec,
						 stack *s) {
	bool error = false;
	int i = 0;
	while (!error && i < bytes) {
		unsigned char byte_code = (unsigned char) byte_codes[i++];
		jvm_argument arg;
		if (jvm_argument_detect(&arg, byte_code) == OPERATION_SUCCESS) {
			if (jvm_argument_requires_vector(&arg)) {
				// First argument int_vector, second position
				if (i == bytes) {
					error = true;
				} else {
					unsigned char position = (unsigned char) byte_codes[i++];
					arg.func(vec, &position, s);
				}
			} else {
				if (jvm_argument_requires_operand(&arg)) {
					// First argument is the next byte_code
					if (i == bytes) {
						error = true;
					} else {
						char extra_argument = byte_codes[i++];
						arg.func(&extra_argument, NULL, s);
					}
				} else {
					// No extra arguments are needed
					arg.func(NULL, 0, s);
				}
			}
			printf("%s\n", arg.byte_code_description);
		} // Ignore unknown byte_codes
	}
	return error ? OPERATION_FAILURE_ILLEGAL_ARGUMENT : OPERATION_SUCCESS;
}

/**
 * Static function that receives all the byte_codes to be executed in chunks
 * through the socket
 */
static operation_result
receive_and_process_byte_codes(socket_t *skt, int_vector *vec, stack *s,
							   int chunk_size) {
	printf("%s\n", BYTE_CODES_OUTPUT_TITLE);

	// Allocate necessary memory for the chunk
	char *buffer = (char *) malloc((size_t) chunk_size);
	if (!buffer) {
		return OPERATION_FAILURE_NO_MEMORY;
	}

	long bytes_received = 0;
	bool finished = false;

	// Receive and process all the byte codes
	do {
		bytes_received = socket_recv(skt, buffer, chunk_size);
		if (bytes_received <= 0) {
			finished = true;
		} else if (bytes_received > 0) {
			// We send the corresponding chunk as buffer + bytes_already_processed
			// We send the quantity of bytes as bytes_received - bytes_already_processed
			if (process_byte_codes_chunk(buffer, bytes_received, vec, s) !=
				OPERATION_SUCCESS) {
				finished = true;
			}
		}
	} while (!finished);

	// Print extra line dividing byte_codes trace from the variables dump
	printf("\n");

	free(buffer);
	return OPERATION_SUCCESS;
}

/**
 * Static function that receives the quantity of variables to store in memory
 * through the socket and creates the corresponding int_vector
 */
static operation_result
receive_variables_quantity(socket_t *remote_skt, int_vector *vec) {
	int variables_quantity;

	// Receive the integer with the quantity of variables through the socket
	if (socket_recv_int(remote_skt, &variables_quantity) ==
		SOCKET_CONNECTION_ERROR) {
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Create the int_vector with the received quantity
	int_vector_create(vec, variables_quantity);

	return OPERATION_SUCCESS;
}

operation_result jvm_server_config(const char *port, jvm_server *server) {
	if (!server)
		return OPERATION_FAILURE_NULL_POINTER;
	server->port = port;
	return OPERATION_SUCCESS;
}

operation_result jvm_server_start(jvm_server *server) {
	socket_t my_socket;
	socket_t remote_connection_socket;

	// Configure the socket to be a listener in the given port
	if (socket_bind_and_address(&my_socket, server->port) ==
		SOCKET_CONNECTION_ERROR) {
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Wait and accept for an incoming connection
	if (socket_accept(&my_socket, &remote_connection_socket) ==
		SOCKET_CONNECTION_ERROR) {
		socket_close(&my_socket);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Receive the quantity of variables through the socket
	int_vector vec;
	if (receive_variables_quantity(&remote_connection_socket, &vec) !=
		OPERATION_SUCCESS) {
		socket_close(&my_socket);
		socket_close(&remote_connection_socket);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Creates the stack
	stack s;
	stack_create(&s, sizeof(int));

	//Receive the byte_codes in chunks and process them
	if (receive_and_process_byte_codes(&remote_connection_socket, &vec, &s,
									   CHUNK_SIZE) != OPERATION_SUCCESS) {
		socket_close(&my_socket);
		socket_close(&remote_connection_socket);
		int_vector_destroy(&vec);
		stack_destroy(&s);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Destroys the stack
	stack_destroy(&s);

	// Print the stored variables in stdout
	printf("%s\n", VARIABLES_OUTPUT_TITLE);
	int_vector_print_elements(&vec, stdout);

	// Send the variables through the socket
	operation_result result = send_variables(&remote_connection_socket, &vec);

	// Destroys the int_vector
	int_vector_destroy(&vec);

	// Closes the original and the peer socket entirely
	socket_close(&my_socket);
	socket_close(&remote_connection_socket);

	return result;
}
