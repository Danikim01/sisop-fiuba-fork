#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void
procesar_primos(int fd1[2])
{
	int valor_leido;
	if (read(fd1[0], &valor_leido, sizeof(int)) > 0) {
		int id_fork, fd2[2];

		printf("primo %d\n", valor_leido);

		if (pipe(fd2) < 0) {
			perror("Error en pipe");
			return;
		}

		id_fork = fork();

		if (id_fork > 0) {
			close(fd2[0]);
			int siguiente;
			while (read(fd1[0], &siguiente, 4) > 0) {
				if (siguiente % valor_leido != 0) {
					write(fd2[1], &siguiente, 4);
				}
			}
			close(fd2[1]);
			close(fd1[0]);
			wait(NULL);
		} else {
			close(fd1[0]);
			close(fd2[1]);
			procesar_primos(fd2);
			close(fd2[0]);
		}
	}
	close(fd1[0]);
}

int
main(int argc, char *argv[])
{
	int numero = atoi(argv[1]);
	int id_fork;
	int fd1[2];

	if (pipe(fd1) < 0) {
		perror("Error en pipe");
		return -1;
	}

	id_fork = fork();

	if (id_fork > 0) {
		close(fd1[0]);
		int index = 2;
		while (index <= numero) {
			write(fd1[1], &index, sizeof(int));
			index++;
		}

		close(fd1[1]);
		wait(NULL);

	} else {
		close(fd1[1]);
		procesar_primos(fd1);
		close(fd1[0]);
	}


	return 0;
}