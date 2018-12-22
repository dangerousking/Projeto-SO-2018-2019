#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>


void main(int argc, char* argv[])
{
	if(argc == 1)
	{
		perror("Número de argumentos inválido");
	}

    //printf("%s\n", str);		//imprime a string para testes

	char *str = (char *) malloc(4000);
	for(int x = 1; x<argc; x++)
	{
		strcat(str, argv[x]);
		strcat(str, " ");
	}

    int client_to_server;
    char *myfifo = "/tmp/client_to_server_fifo";

    int server_to_client;
    char *myfifo2 = "/tmp/server_to_client_fifo";

    int id;
    char output[4000];
    char output_tarefa[4000];

    const char s[2] = " ";
	char *token;

    if(strcmp(argv[1],"-a") == 0)
    {
		client_to_server = open(myfifo, O_WRONLY);
		write(client_to_server, str, 4000);
		close(client_to_server);

		server_to_client = open(myfifo2, O_RDONLY);
		read(server_to_client, &id, sizeof(id));
		close(server_to_client);
		printf("%d\n", id);
	}else
	if(strcmp(argv[1],"-l") == 0)
	{
		client_to_server = open(myfifo, O_WRONLY);
		write(client_to_server, str, 4000);
		close(client_to_server);

		server_to_client = open(myfifo2, O_RDONLY);
		while(read(server_to_client, output, sizeof(output)) > 0)
		{
		printf("%s\n", output);
		}
		close(server_to_client);
	}else
	if(strcmp(argv[1], "-c") == 0)
	{
		client_to_server = open(myfifo, O_WRONLY);
		write(client_to_server, str, 4000);
		close(client_to_server);
	}else
	if(strcmp(argv[1], "-r") == 0)
	{
		client_to_server = open(myfifo, O_WRONLY);
		write(client_to_server, str, 4000);
		close(client_to_server);

		server_to_client = open(myfifo2, O_RDONLY);
		while(read(server_to_client, output, sizeof(output)) > 0)
		{}
		usleep(10);
		token = strtok(output, s);//
		sscanf(token, "%d-", &id);//		ID
		printf("Id:\n%d\n\n", id);//

		token = strtok(NULL, s);//
		printf("Data:\n%s ", token);//		DATA
		token = strtok(NULL, s);//
		printf("%s\n\n", token);//


		token = strtok(NULL, s);//			Argumentos
		printf("Linha de comando:\n");
		while(token != NULL && strcmp(token, "EXECUTADA") != 0 && strcmp(token, "CANCELADA") != 0)
		{
			printf("%s ", token);
			token = strtok(NULL, s);
		}
		printf("\n\n");

		while(read(server_to_client, output_tarefa, sizeof(output_tarefa)) > 0)
		{}

		printf("Standard output:\n");
		printf("%s \n\n", output_tarefa);

		close(server_to_client);
	}else
	{
		printf("Comando inválido.\n\n");
	}

	fflush(stdout);
}
