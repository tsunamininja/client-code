/*
 * exec.c
 *
 *  Created on: Jan 23, 2016
 *      Author: joe
 */

// this module is responsible for implementing shell like functionality
// while allowing the buffering of stdout/stderror streams to enable
// custom multiplexing like capabilities over protocols of choice.

// header file
#include "exec.h"
#include "bufferHelper.h"

int debug1 = 1;

/*
 * function name - shellCommand
 *
 * arguments - _command => pointer to command string to pass to exec
 * 			   _stdoutBuffer => buffer to stdout for caller return
 *
 * return value - stdout buffer size... !, count
 *
 * comments - maybe just don't do ** as args unless we need to return int
 *
 */
int shellCommand(unsigned char *_command, unsigned char **_stdoutBuffer)
{
	if(debug1)
	{
		printf("===== shellCommand() ===== \n");
		printf("command -> %s 			\n", _command);
		printf("stdoutBuffer (not printing) \n" );
		printf("----------------------------------\n");
	}

	/* pipe -> fork -> exec */

	int flag = 0;
	*_stdoutBuffer = malloc(sizeof(unsigned char) * STDOUT_BUFFER_SIZE);
	unsigned int readCount = STDOUT_BUFFER_SIZE;

	// malloc return value;

	int filedes[2];

	if (pipe(filedes) == -1)
	{
		perror("pipe");
		exit(1);
	}

	// fork
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0) // inside child
	{
		// attach STDOUT_FILENO to the the write end of the pipe
		while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}

		// attach STDOUT_ERRNO to the write end of the pipe
		while ((dup2(filedes[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}
		close(filedes[1]);
		close(filedes[0]); // child does not need and has not used STDIN

		// somehow filedes[1] is associated with the write end of the pipe
		// which stdout will "write to" as opposed to "writing" to the tty
		execl("/bin/sh", "sh", "-c", _command, (char*)0);
		perror("execl");
		_exit(1);
	    // child now exits and we return to parent
	}

	// back in parent.. let's read from the "read end" filedes[0] of the pipe
	ssize_t count = read(filedes[0], *_stdoutBuffer, readCount);

	printf("count -> %u \n", count);

	if (count == -1)
	{
		perror("read");
		exit(1);
	}
    else if (count == 0)
    {
    	flag = 0;
    }

	// done reading from stdin buffer
	close(filedes[0]);
	wait(0);

	if(debug1)
		printf("=====/end shellCommand() ===== \n");

	// now return buffer to caller
	return count;
}
