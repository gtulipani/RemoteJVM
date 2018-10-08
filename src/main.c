#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "jvm_server.h"
#include "jvm_client.h"

#define PROGRAM_SUCCESS 0
#define PROGRAM_FAILURE 1

#define CLIENT_ARGUMENT "client"
#define SERVER_ARGUMENT "server"

/**
 * Static function that parses the client arguments and calls server_config. The program should be executed like this:
 *              ./program server <port>
 * @param argc
 * @param argv
 */
static operation_result
parse_client_args(jvm_client *c, int argc, char *argv[]) {
	if (argc < 5 || argc > 6) {
		return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
	} else {
		const char *port = argv[3];
		//Convert the var_size to int
		int32_t var_size = (int32_t) strtol(argv[4], (char **) NULL, 10);
		if (errno == ERANGE) {
			return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
		} else {
			//Use filename or stdin
			FILE *src = (argc == 5) ? stdin : fopen(argv[5], "rb");
			if (!src) {
				return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
			}
			return jvm_client_config(argv[2], port, var_size, src, c);
		}
	}
}

/**
 * Static function that parses the server arguments and calls client_config. The program should be executed like this:
 *              ./program client <host> <port> <N> [<filename>]
 * If no filename is specified, stdin will be used
 * @param argc
 * @param argv
 */
static operation_result
parse_server_args(jvm_server *s, int argc, char *argv[]) {
	if (argc != 3) {
		return OPERATION_FAILURE_ILLEGAL_ARGUMENT;
	} else {
		const char *port = argv[2];
		return jvm_server_config(port, s);
	}
}

int main(int argc, char *argv[]) {
	int programResult;
	if (argc == 1) { // No arguments were specified
		programResult = PROGRAM_FAILURE;
	} else {
		const char *modeArgument = argv[1];
		if (strcmp(modeArgument, CLIENT_ARGUMENT) == 0) {
			jvm_client c;
			if (parse_client_args(&c, argc, argv) != OPERATION_SUCCESS) {
				programResult = PROGRAM_FAILURE;
			} else {
				programResult = (jvm_client_start(&c) != OPERATION_SUCCESS)
								? PROGRAM_FAILURE : PROGRAM_SUCCESS;
				jvm_client_destroy(&c);
			}
		} else if (strcmp(modeArgument, SERVER_ARGUMENT) == 0) {
			jvm_server s;
			if (parse_server_args(&s, argc, argv) != OPERATION_SUCCESS) {
				programResult = PROGRAM_FAILURE;
			} else {
				programResult = (jvm_server_start(&s) != OPERATION_SUCCESS)
								? PROGRAM_FAILURE : PROGRAM_SUCCESS;
			}
		} else {
			programResult = PROGRAM_FAILURE;
		}
	}
	return programResult;
}
