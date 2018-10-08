#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdint.h>

#include "result.h"

typedef struct jvm_client {
	const char *host;
	const char *port;
	int32_t var_size;
	FILE *src;
} jvm_client;

/**
 * Initializes the {@param self} with the {@param host}, {@param port}, {@param var_size} and {@param src} received as parameters
 * @pre     {@param self} pointer to jvm_client already allocated
 * @post    {@param self} pointer to jvm_client ready to be used
 * @return  {@link operation_result} with the result of the operation
 */
operation_result
jvm_client_config(const char *host, const char *port, int32_t var_size,
				  FILE *src,
				  jvm_client *self);

/**
 * Starts the {@param self} in the host and port already configured:
 *          - The client will try to connect with the host and port configured.
 *          - The client will send first a message through the socket with
 *            4 big endian bytes representing a signed int containing the
 *            quantity of variables to store in memory
 *          - The client will then read a size-fixed quantity of byte_codes from the
 *            FILE and send them through the socket. Will continue with this step
 *            until all the byte_codes are sent.
 *          - The client will close the socket for writing
 *          - The client will receive the variables stored by the server, each one
 *            of them as 4 big endian bytes representing a signed int
 *          - The client will print the variables in stdout
 * @pre     {@param self} pointer to jvm_client already configured
 * @return
 */
operation_result jvm_client_start(jvm_client *self);

/**
 * Destroys the {@param self} by freeing the memory. Besides, the src FILE is closed.
 * @pre     {@param self} pointer to jvm_client already allocated
 * @post    The memory allocated is released
 */
void jvm_client_destroy(jvm_client *self);

#endif //__CLIENT_H__
