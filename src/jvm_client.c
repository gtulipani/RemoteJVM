#include <sys/socket.h>

#include "jvm_client.h"
#include "socket.h"
#include "jvm_utils.h"

#define CHUNK_SIZE 100

/**
 * Static function that reads the bytes from the FILE and sends them in chunks through
 * the socket
 */
static operation_result
send_byte_codes(socket_t *skt, jvm_client *self, size_t chunk_size) {
	fseek(self->src, 0, SEEK_END);
	long len = ftell(self->src);
	rewind(self->src);

	//Create chunks and send them through the socket
	char *buffer = (char *) malloc((size_t) chunk_size);
	if (!buffer) {
		return OPERATION_FAILURE_NO_MEMORY;
	}

	for (int i = 0; i < len; i += chunk_size) {
		size_t bytes_read = fread(buffer, 1, chunk_size, self->src);
		if (bytes_read == 0) {
			free(buffer);
			return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
		}

		if (socket_send(skt, buffer, bytes_read) == SOCKET_CONNECTION_ERROR) {
			free(buffer);
			return OPERATION_FAILURE_CONNECTION_FAILED;
		}
	}

	free(buffer);
	return OPERATION_SUCCESS;
}

/**
 * Static function that receives the variables stored by the server and print
 * them in stdout as hexadecimal numbers in uppercase of 8 bytes
 */
static operation_result
print_received_variables(socket_t *skt, jvm_client *self) {
	int expected_variables_quantity = (self->var_size);
	// Allocate sufficient memory for the variables to receive

	printf("%s\n", VARIABLES_OUTPUT_TITLE);

	// Receive the integers from the socket
	for (int interation = 0;
		 interation < expected_variables_quantity; interation++) {
		int received_int;
		if (socket_recv_int(skt, &received_int) ==
			SOCKET_CONNECTION_ERROR) {
			return OPERATION_FAILURE_CONNECTION_FAILED;
		}
		// Print the integer in stdout
		printf("%08x\n", received_int);
	}

	return OPERATION_SUCCESS;
}

operation_result
jvm_client_config(const char *host, const char *port, int32_t var_size,
				  FILE *src,
				  jvm_client *self) {
	if (!host || !src || !self)
		return OPERATION_FAILURE_NULL_POINTER;

	self->host = host;
	self->port = port;
	self->var_size = var_size;
	self->src = src;
	return OPERATION_SUCCESS;
}

void jvm_client_destroy(jvm_client *self) {
	fclose(self->src);
}

operation_result jvm_client_start(jvm_client *self) {
	socket_t socket;

	// Connect through the socket
	if (socket_connect(&socket, self->host, self->port) ==
		SOCKET_CONNECTION_ERROR) {
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Send the quantity of variables through the socket
	if (socket_send_int(&socket, self->var_size) ==
		SOCKET_CONNECTION_ERROR) {
		socket_close(&socket);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Send the byte_codes in chunks
	if (send_byte_codes(&socket, self, CHUNK_SIZE) != OPERATION_SUCCESS) {
		socket_close(&socket);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Shutdowns socket for writing
	if (socket_shutdown(&socket, SHUT_WR) == SOCKET_CONNECTION_ERROR) {
		socket_close(&socket);
		return OPERATION_FAILURE_CONNECTION_FAILED;
	}

	// Receive the stored variables through the socket
	operation_result result = print_received_variables(&socket, self);

	// Closes the socket entirely
	socket_close(&socket);

	return result;
}
