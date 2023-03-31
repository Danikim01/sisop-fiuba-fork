#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

int
main()
{
	int fd1[2];
	int fd2[2];
	int id;
	int value, rand_num;

	if (pipe(fd1) == -1) {
		fprintf(stderr, "An error has occurred.\n");
		return -1;
	}

	if (pipe(fd2) == -1) {
		fprintf(stderr, "An error has occurred.\n");
		return -1;
	}

	printf("Hola, soy PID %d\n", getpid());
	printf("\t- primer pipe me devuelve [%d , %d]\n", fd1[0], fd1[1]);
	printf("\t- segundo pipe me devuelve [%d, %d]\n", fd2[0], fd2[1]);

	id = fork();

	if (id == -1) {
		fprintf(stderr, "An error has occurred.\n");
		return -1;
	}

	if (id == 0) {
		close(fd1[1]);
		close(fd2[0]);

		read(fd1[0], &value, sizeof(int));

		printf("Donde fork me devuelve %d: \n", id);
		printf("- getpid me devuelve: %d\n", getpid());
		printf("- getppid me devuelve: %d\n", getppid());

		printf("- recibo valor %d via fd=%d\n", value, fd1[0]);
		printf("- reenvio valor en fd=%d y termino\n", fd2[1]);
		write(fd2[1], &value, sizeof(int));

		close(fd2[1]);
		close(fd1[0]);

	} else {
		close(fd1[0]);
		close(fd2[1]);

		srand(time(NULL));
		rand_num = rand() % 10;
		printf("Donde fork me devuelve %d: \n", id);
		printf("- getpid me devuelve: %d\n", getpid());
		printf("- getppid me devuelve: %d\n", getppid());

		printf("- random me devuelve: %d \n", rand_num);
		printf("- envio valor: %d a traves de fd=%d\n", rand_num, fd1[1]);
		write(fd1[1], &rand_num, sizeof(int));

		read(fd2[0], &value, sizeof(int));
		printf("Hola, de nuevo PID %d\n", getpid());
		printf("- recibi valor %d vía fd=%d\n", value, fd2[0]);
		wait(NULL);
		close(fd1[1]);
		close(fd2[0]);
	}
}