#ifndef __SERVER_H__
#define __SERVER_H__

#include "result.h"

typedef struct jvm_server {
	const char* port;
} jvm_server;

/**
 * Initializes the {@param server} with the {@param port} received as parameters.
 * @pre     {@param server} pointer to jvm_server already allocated
 * @post    {@param server} pointer to jvm_server ready to be used
 * @return  {@link operation_result} with the result of the operation
 */
operation_result jvm_server_config(const char* port, jvm_server *server);

/**
 * Starts the {@param server} in the port already configured:
 *          - The server will wait for a client to connect in the port.
 *          - The server will receive a message through the socket with the following information:
 *                  - {@link jvm_utils.h#VARS_INTEGER_BITS} big endian bytes representing a signed int containing the quantity of variables to store in memory
 *                  - Further bytes representing byte_codes to be executed by the server
 *          - The server will perform the following actions for each one of the byte_codes:
 *                  - Execute it
 *                  - Print its symbolic name in stdout
 *          - The server will print the variables stored in memory in stdout in hex with 8 digits
 *          - The server will send a message through the socket with the variables, each one of them as {@link jvm_utils.h#VARS_INTEGER_BITS} big endian bytes representing signed int
 *          - The server will close the socket for both reading and writing
 * @pre     {@param server} pointer to server already configured
 * @return
 */
operation_result jvm_server_start(jvm_server *server);

#endif //__SERVER_H__
