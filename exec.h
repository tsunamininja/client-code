/*
 * exec.h
 *
 *  Created on: Jan 23, 2016
 *      Author: joe
 */

#ifndef EXEC_H_
#define EXEC_H_

// includes (
#include <errno.h> 		// perror
#include <stdlib.h>		// exit
#include <unistd.h>		// clse dup, execl fork pipe stdoutfileno
#include <sys/wait.h>	// wait pid_t
#include <stdio.h>		// perror and printf
#include <sys/types.h>  // pid_t

// defines
#define STDOUT_BUFFER_SIZE 4096

// function prototypes for linker resolution
int shellCommand(unsigned char *cmd, unsigned char **stdoutBuffer);



#endif /* EXEC_H_ */
