#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#ifndef NARGS
#define NARGS 4
#endif


int
main(int argc, char *argv[])
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	int n = 1;
	char *args[NARGS + 2];
	int status;
	args[0] = argv[1];
	args[NARGS + 1] = NULL;
	while ((nread = getline(&line, &len, stdin)) != -1) {
		line[strcspn(line, "\n")] = '\0';
		args[n] = strdup(line);
		n++;

		if (n > NARGS) {
			pid_t pid = fork();
			if (pid == 0) {
				execvp(args[0], args);
				perror("execvp");
				exit(EXIT_FAILURE);
			} else if (pid < 0) {
				perror("fork");
				exit(EXIT_FAILURE);
			} else {
				wait(&status);
				for (int i = 1; i < n; i++) {
					free(args[i]);
				}
				n = 1;
			}
		}
	}

	if (n > 1) {
		args[n] = NULL;
		pid_t pid = fork();
		if (pid == 0) {
			execvp(args[0], args);
			perror("execvp");
			exit(EXIT_FAILURE);
		} else if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		} else {
			wait(&status);
		}
	}

	for (int i = 1; i < n; i++) {
		free(args[i]);
	}

	free(line);

	exit(EXIT_SUCCESS);
}