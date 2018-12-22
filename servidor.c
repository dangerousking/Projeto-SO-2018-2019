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

void handle_signal(int signal)
{
	const char s_alarme[2] = " ";

	FILE *fd_alarme;
	fd_alarme = fopen("tarefas.txt", "r+");

	FILE *output_alarme;
	output_alarme = fopen("output.txt", "a+");

	char linha_alarme[1000] = "\0";
	fseek(fd_alarme, 0, SEEK_SET);

	int FILHOS = 2;
	int pid[FILHOS][2];

	char *token_alarme;
	int id_token_alarme = 0;
	int id_alarme_executar = 0;
	
	char *argumentos_alarme[50];
	char *argumentos_alarme_executar[50];
	int x_argumentos_alarme = 0;

	time_t tempo_alarme = 0;
	time_t tempo_alarme_executar = 0;
	time_t tempo_sistema_alarme = 0;

	char *data_alarme;
	char *data_hora_alarme;
	int ano_alarme = 0, mes_alarme = 0, dia_alarme = 0, hora_alarme = 0, minuto_alarme = 0, segundo_alarme = 0;
	double segundos_alarme2 = 0;

	int flag = 0;

	while(fgets(linha_alarme, sizeof(linha_alarme), fd_alarme) != NULL)
	{
		token_alarme = strtok(linha_alarme, s_alarme);
		sscanf(token_alarme, "%d-", &id_token_alarme);

		token_alarme = strtok(NULL, s_alarme);
		data_alarme = token_alarme;

		token_alarme = strtok(NULL, s_alarme);
		data_hora_alarme = token_alarme;

		token_alarme = strtok(NULL, s_alarme);

		x_argumentos_alarme = 0;
		while(token_alarme != NULL)
		{
			argumentos_alarme[x_argumentos_alarme] = token_alarme;
			x_argumentos_alarme++;
			token_alarme = strtok(NULL, s_alarme);
		}

		if(strcmp(argumentos_alarme[x_argumentos_alarme-2], "EXECUTADA") == 0 || strcmp(argumentos_alarme[x_argumentos_alarme-2], "CANCELADA") == 0)
		{
			continue;
		}

		id_alarme_executar = id_token_alarme;
				
		sscanf(data_alarme, "%4d-%2d-%2d", &ano_alarme, &mes_alarme, &dia_alarme);
		sscanf(data_hora_alarme, "%2d:%2d:%2d", &hora_alarme, &minuto_alarme, &segundo_alarme);


		struct tm breakdown = {0};

		breakdown.tm_year = ano_alarme - 1900;
		breakdown.tm_mon = mes_alarme - 1;
		breakdown.tm_mday = dia_alarme;
		breakdown.tm_hour = hora_alarme;
		breakdown.tm_min = minuto_alarme;
		breakdown.tm_sec = segundo_alarme;

		tempo_alarme_executar = mktime(&breakdown);

		for(int i_alarme=0; i_alarme < x_argumentos_alarme-1; i_alarme++)
		{
			argumentos_alarme_executar[i_alarme] = argumentos_alarme[i_alarme];
		}

		flag = 1;

		break;
	}

	if(!flag)
	{
		return;
	}

	fseek(fd_alarme, 0, SEEK_SET);

	while(fgets(linha_alarme, sizeof(linha_alarme), fd_alarme) != NULL)
	{
		token_alarme = strtok(linha_alarme, s_alarme);
		sscanf(token_alarme, "%d-", &id_token_alarme);

		token_alarme = strtok(NULL, s_alarme);
		data_alarme = token_alarme;

		token_alarme = strtok(NULL, s_alarme);
		data_hora_alarme = token_alarme;

		token_alarme = strtok(NULL, s_alarme);

		x_argumentos_alarme = 0;
		while(token_alarme != NULL)
		{
			argumentos_alarme[x_argumentos_alarme] = token_alarme;
			x_argumentos_alarme++;
			token_alarme = strtok(NULL, s_alarme);
		}

		if(strcmp(argumentos_alarme[x_argumentos_alarme-2], "EXECUTADA") == 0 || strcmp(argumentos_alarme[x_argumentos_alarme-2], "CANCELADA") == 0)
		{
			continue;
		}
				
		sscanf(data_alarme, "%4d-%2d-%2d", &ano_alarme, &mes_alarme, &dia_alarme);
		sscanf(data_hora_alarme, "%2d:%2d:%2d", &hora_alarme, &minuto_alarme, &segundo_alarme);


		struct tm breakdown = {0};

		breakdown.tm_year = ano_alarme - 1900;
		breakdown.tm_mon = mes_alarme - 1;
		breakdown.tm_mday = dia_alarme;
		breakdown.tm_hour = hora_alarme;
		breakdown.tm_min = minuto_alarme;
		breakdown.tm_sec = segundo_alarme;

		tempo_alarme = mktime(&breakdown);

		if(difftime(tempo_alarme_executar, tempo_alarme) > 0)
		{
			tempo_alarme_executar = tempo_alarme;
			id_alarme_executar = id_token_alarme;

			for(int i_alarme=0; i_alarme < x_argumentos_alarme-1; i_alarme++)
			{
				argumentos_alarme_executar[i_alarme] = argumentos_alarme[i_alarme];
			}
		}else
		if(difftime(tempo_alarme_executar, tempo_alarme) == 0)
		{
			id_alarme_executar = id_token_alarme;

			for(int i_alarme=0; i_alarme < x_argumentos_alarme-1; i_alarme++)
			{
				argumentos_alarme_executar[i_alarme] = argumentos_alarme[i_alarme];
			}
		}

		tempo_sistema_alarme = time(NULL);
		segundos_alarme2 = difftime(tempo_alarme_executar, tempo_sistema_alarme);
	}//while de percorrer ficheiro

	pid[0][1] = id_alarme_executar;
	pid[0][0] = fork();

	if(pid[0][0] == 0)	// Estou no filho
	{
		sleep(segundos_alarme2);

		char linha_executada[1000] = "\0";
		fseek(fd_alarme, 0, SEEK_SET);

		int encontrou = 0;
		int id_executada = 0;
		int position_in_file;


		while(fgets(linha_executada, sizeof(linha_executada), fd_alarme) != NULL && encontrou == 0)
		{
			token_alarme = strtok(linha_executada, s_alarme);
			sscanf(token_alarme, "%d-", &id_executada);

			if(id_executada == pid[0][1])
			{
				encontrou = 1;

				token_alarme = strtok(NULL, "\n");

				position_in_file = ftell(fd_alarme);

				fseek(fd_alarme, position_in_file - 11, SEEK_SET);

				fprintf(fd_alarme, "EXECUTADA \n");
			}
		}

		fprintf(output_alarme, "OUTPUT %d:\n", id_executada);
		fflush(output_alarme);

		fflush(fd_alarme);
		execvp(*argumentos_alarme_executar, argumentos_alarme_executar);
		perror("Falha no exec !");
		_exit(1);
	}

	fclose(fd_alarme);

}//funcao handler_signal()


int main(int argc, char* argv[])
{
	int client_to_server;
	char *myfifo = "/tmp/client_to_server_fifo";

	int server_to_client;
	char *myfifo2 = "/tmp/server_to_client_fifo";

	mkfifo(myfifo, 0666);
	mkfifo(myfifo2, 0666);	

	int id = 0, id_executar = 0, inicio = 1;
	char buffer[4000];
	
	FILE *fd;
	fd = fopen("tarefas.txt", "a+");

	FILE *fd_rescrita;
	fd_rescrita = fopen("tarefas.txt", "r+");
	
	FILE *output;
	output = fopen("output.txt", "a+"); 
	
	dup2(fileno(output), fileno(stdout));

	int FILHOS = 2;
	int pid[FILHOS][2];
	
	/*
	struct sigaction signalAction;
	signalAction.sa_handler = handle_signal;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = 0;
	sigaction(SIGCHLD, &signalAction, NULL);
	*/
	//signal(SIGALRM, handle_signal);
	signal(SIGCHLD, handle_signal);
	//signal(SIGUSR1, handle_signal);

	while(1)
	{
		const char s[2] = " ";
		char *token;
		char *token2;
		char *token3;
		char *token4;

		char *data;
		char *data2;

		char *data_hora;
		char *data_hora2;

		time_t tempo = 0;
		time_t tempo2 = 0;
		time_t tempo_sistema = 0;

		int ano = 0, mes = 0, dia = 0, hora = 0, minuto = 0, segundo = 0;
		int ano2 = 0, mes2 = 0, dia2 = 0, hora2 = 0, minuto2 = 0, segundo2 = 0;

		client_to_server = open(myfifo, O_RDONLY);		
		read(client_to_server, buffer, sizeof(buffer));
		close(client_to_server);

		char *argumentos[50];
		char *argumentos_executar[50];

		token = strtok(buffer, s);

		if(strcmp(token, "-a") == 0)
		{
			id++;

			server_to_client = open(myfifo2, O_WRONLY);
			write(server_to_client, &id, sizeof(id));
			close(server_to_client);


			fprintf(fd,"%d- ", id);		//Gravar id
			token = strtok(NULL, s);	//Comecar no 1º para escrever no ficheiro
			fprintf(fd, "%s ", token);
			data = token;

			token = strtok(NULL, s);
			data_hora = token;

			sscanf(data, "%4d-%2d-%2d", &ano, &mes, &dia);
			sscanf(data_hora, "%2d:%2d:%2d", &hora, &minuto, &segundo);

			struct tm breakdown = {0};

			breakdown.tm_year = ano - 1900;
			breakdown.tm_mon = mes - 1;
			breakdown.tm_mday = dia;
			breakdown.tm_hour = hora;
			breakdown.tm_min = minuto;
			breakdown.tm_sec = segundo;

			tempo = mktime(&breakdown);
			tempo_sistema = time(NULL);
			double segundos_alarme = difftime(tempo, tempo_sistema);
			
			int x_argumentos = 0;

			while(token != NULL)
			{
				fprintf(fd, "%s ", token);
				token = strtok(NULL, s);
				argumentos[x_argumentos] = token;
				x_argumentos++;
			}
			fprintf(fd, "           \n");
			fflush(fd);


			if(inicio == 1)
			{
				inicio = 0;

				id_executar = id;

				pid[0][1] = id_executar;
				pid[0][0] = fork();
		
				if(pid[0][0] == 0)	// Estou no filho
				{
					sleep(segundos_alarme);

					char linha_executada[1000] = "\0";
					fseek(fd_rescrita, 0, SEEK_SET);

					int encontrou = 0;
					int id_executada = 0;
					int position_in_file;


					while(fgets(linha_executada, sizeof(linha_executada), fd_rescrita) != NULL && encontrou == 0)
					{
						token3 = strtok(linha_executada, s);
						sscanf(token3, "%d-", &id_executada);

						if(id_executada == pid[0][1])
						{
							encontrou = 1;

							token3 = strtok(NULL, "\n");

							position_in_file = ftell(fd_rescrita);

							fseek(fd_rescrita, position_in_file - 11, SEEK_SET);

							fprintf(fd_rescrita, "EXECUTADA \n");
						}
					}

					fprintf(output, "OUTPUT %d:\n", id_executada);
					fflush(output);

					fflush(fd_rescrita);
					execvp(*argumentos, argumentos);
					perror("Falha no exec !");
					_exit(1);
				}
			}else
			{
				char linha[1000] = "\0";
				fseek(fd, 0, SEEK_SET);

				int id_token = 0;
				int encontrou = 0;

				int x_argumentos = 0;
				int flag_executado = 0;

				while(fgets(linha, sizeof(linha), fd) != NULL && encontrou == 0)
				{
					token2 = strtok(linha, s);
					sscanf(token2, "%d-", &id_token);

					if(id_token == id_executar)
					{
						encontrou = 1;
						
						token2 = strtok(NULL, s);
						data2 = token2;

						token2 = strtok(NULL, s);
						data_hora2 = token2;

						token2 = strtok(NULL, s);

						x_argumentos = 0;
						while(token2 != NULL)
						{
							argumentos[x_argumentos] = token2;
							x_argumentos++;
							token2 = strtok(NULL, s);
						}

						if(strcmp(argumentos[x_argumentos-2], "EXECUTADA") == 0 || strcmp(argumentos[x_argumentos-2], "CANCELADA") == 0)
						{
							flag_executado = 1;
							break;
						}
					

						sscanf(data2, "%4d-%2d-%2d", &ano2, &mes2, &dia2);
						sscanf(data_hora2, "%2d:%2d:%2d", &hora2, &minuto2, &segundo2);


						struct tm breakdown = {0};

						breakdown.tm_year = ano2 - 1900;
						breakdown.tm_mon = mes2 - 1;
						breakdown.tm_mday = dia2;
						breakdown.tm_hour = hora2;
						breakdown.tm_min = minuto2;
						breakdown.tm_sec = segundo2;

						tempo2 = mktime(&breakdown);		
					}

				}//while de percorrer o ficheiro

				if(flag_executado) //Ver o proximo a ser executado
				{
					
					char linha[1000] = "\0";
					fseek(fd, 0, SEEK_SET);

					while(fgets(linha, sizeof(linha), fd) != NULL)
					{
						token2 = strtok(linha, s);
						sscanf(token2, "%d-", &id_token);

						token2 = strtok(NULL, s);
						data2 = token2;

						token2 = strtok(NULL, s);
						data_hora2 = token2;

						token2 = strtok(NULL, s);

						x_argumentos = 0;
						while(token2 != NULL)
						{
							argumentos[x_argumentos] = token2;
							x_argumentos++;
							token2 = strtok(NULL, s);
						}

						if(strcmp(argumentos[x_argumentos-2], "EXECUTADA") == 0 || strcmp(argumentos[x_argumentos-2], "CANCELADA") == 0)
						{
							continue;
						}
				
						sscanf(data2, "%4d-%2d-%2d", &ano2, &mes2, &dia2);
						sscanf(data_hora2, "%2d:%2d:%2d", &hora2, &minuto2, &segundo2);

						struct tm breakdown = {0};

						breakdown.tm_year = ano2 - 1900;
						breakdown.tm_mon = mes2 - 1;
						breakdown.tm_mday = dia2;
						breakdown.tm_hour = hora2;
						breakdown.tm_min = minuto2;
						breakdown.tm_sec = segundo2;

						tempo2 = mktime(&breakdown);

						if(difftime(tempo, tempo2) > 0)
						{
							tempo = tempo2;
							id_executar = id_token;

							for(int i=0; i < x_argumentos-1; i++)
							{
								argumentos_executar[i] = argumentos[i];
							}
						}else
						if(difftime(tempo, tempo2) == 0)
						{
							id_executar = id_token;

							for(int i=0; i < x_argumentos-1; i++)
							{
								argumentos_executar[i] = argumentos[i];
							}
						}
					}//while de percorrer ficheiro

					tempo_sistema = time(NULL);
					segundos_alarme = difftime(tempo2, tempo_sistema);

					pid[0][1] = id_executar;
					pid[0][0] = fork();

					if(pid[0][0] == 0) // Estou no filho
					{
						sleep(segundos_alarme);

						char linha_executada[1000] = "\0";
						fseek(fd_rescrita, 0, SEEK_SET);

						int encontrou = 0;
						int id_executada = 0;
						int position_in_file;

						while(fgets(linha_executada, sizeof(linha_executada), fd_rescrita) != NULL && encontrou == 0)
						{
							token4 = strtok(linha_executada, s);
							sscanf(token4, "%d-", &id_executada);

							if(id_executada == pid[0][1])
							{
								encontrou = 1;

								token4 = strtok(NULL, "\n");

								position_in_file = ftell(fd_rescrita);

								fseek(fd_rescrita, position_in_file - 11, SEEK_SET);

								fprintf(fd_rescrita, "EXECUTADA \n");
							}	
						}	

						fprintf(output, "OUTPUT %d:\n", id_executada);
						fflush(output);

						fflush(fd_rescrita);
						execvp(*argumentos_executar, argumentos_executar);
						perror("Falha no exec !");
						_exit(1);
					}
				}//if flag
				else
				if(difftime(tempo, tempo2) < 0)
				{
					id_executar = id;

					tempo_sistema = time(NULL);
					double segundos_alarme = difftime(tempo, tempo_sistema);

					kill(pid[0][0], SIGKILL);

					pid[0][1]=id;
					pid[0][0]=fork();

					if(pid[0][0] == 0) // Estou no filho
					{
						sleep(segundos_alarme);

						char linha_executada[1000] = "\0";
						fseek(fd_rescrita, 0, SEEK_SET);

						int encontrou = 0;
						int id_executada = 0;
						int position_in_file;

						while(fgets(linha_executada, sizeof(linha_executada), fd_rescrita) != NULL && encontrou == 0)
						{
							token4 = strtok(linha_executada, s);
							sscanf(token4, "%d-", &id_executada);

							if(id_executada == pid[0][1])
							{
								encontrou = 1;

								token4 = strtok(NULL, "\n");

								position_in_file = ftell(fd_rescrita);

								fseek(fd_rescrita, position_in_file - 11, SEEK_SET);

								fprintf(fd_rescrita, "EXECUTADA \n");
							}	
						}	
			
						fprintf(output, "OUTPUT %d:\n", id_executada);
						fflush(output);
						
						fflush(fd_rescrita);
						execvp(*argumentos, argumentos);
						perror("Falha no exec !");
						_exit(1);
					}

				}//else if difftime

			}//else do if(inicio = 1)

		}else//else do if("-a")
		if(strcmp(token, "-l") == 0)
		{
			char linha_ficheiro[1000] = "\0";
			char linha_final[4000] = "\0";
			
			server_to_client = open(myfifo2, O_WRONLY);
			fseek(fd, 0, SEEK_SET);

			while(fgets(linha_ficheiro, sizeof(linha_ficheiro), fd) != NULL)
			{
				strcat(linha_final, linha_ficheiro);
			}

			write(server_to_client, linha_final, strlen(linha_final)+1);
			close(server_to_client);
		}else
		if(strcmp(token, "-c") == 0)
		{

			int id_cancelar = 0;

			token = strtok(NULL, s);

			sscanf(token, "%d", &id_cancelar);

			if(id_cancelar == pid[0][1])
			{
				char linha_executada[1000] = "\0";
				fseek(fd_rescrita, 0, SEEK_SET);

				int encontrou = 0;
				int id_executada = 0;
				int position_in_file;

				while(fgets(linha_executada, sizeof(linha_executada), fd_rescrita) != NULL && encontrou == 0)
				{
					token4 = strtok(linha_executada, s);
					sscanf(token4, "%d-", &id_executada);

					if(id_executada == pid[0][1])
					{
						encontrou = 1;

						token4 = strtok(NULL, "\n");

						position_in_file = ftell(fd_rescrita);
						fseek(fd_rescrita, position_in_file - 11, SEEK_SET);
						fprintf(fd_rescrita, "CANCELADA \n");
					}	
				}

				kill(pid[0][0], SIGKILL);
			}else //se nao for a ultima a ser executada
			{
				char linha_executada[1000] = "\0";
				fseek(fd_rescrita, 0, SEEK_SET);

				int encontrou = 0;
				int id_executada = 0;
				int position_in_file;

				while(fgets(linha_executada, sizeof(linha_executada), fd_rescrita) != NULL && encontrou == 0)
				{
					token4 = strtok(linha_executada, s);
					sscanf(token4, "%d-", &id_executada);

					if(id_executada == id_cancelar)
					{
						encontrou = 1;

						token4 = strtok(NULL, "\n");

						position_in_file = ftell(fd_rescrita);
						fseek(fd_rescrita, position_in_file - 11, SEEK_SET);
						fprintf(fd_rescrita, "CANCELADA \n");
					}
				}
			}
		}else
		if(strcmp(token, "-r") == 0)
		{
			int id_procurar = 0;

			token = strtok(NULL, s);

			sscanf(token, "%d", &id_procurar);

			char linha_executada[1000] = "\0";
			char linha_final[4000] = "\0";
			fseek(fd, 0, SEEK_SET);

			char output_executado[1000] = "\0";
			char output_enviar[4000] = "\0";
			fseek(output, 0, SEEK_SET);

			int encontrou = 0;
			int id_executada = 0;

			server_to_client = open(myfifo2, O_WRONLY);

			while(fgets(linha_executada, sizeof(linha_executada), fd) != NULL && encontrou == 0)
			{
				strcpy(linha_final, linha_executada);

				token4 = strtok(linha_executada, s);

				sscanf(token4, "%d-", &id_executada);

				if(id_executada == id_procurar)
				{
					encontrou = 1;

					write(server_to_client, linha_final, strlen(linha_final));
					close(server_to_client);
				}
			}
			usleep(10);
			server_to_client = open(myfifo2, O_WRONLY);
			encontrou = 0;
			id_executada = 0;

			while(fgets(output_executado, sizeof(output_executado), output) != NULL && encontrou == 0)
			{	
				token4 = strtok(output_executado, s);
				token4 = strtok(NULL, s);

				sscanf(token4, "%d:", &id_executada);

				if(id_executada == id_procurar)
				{
					encontrou = 1;
				}
					
			}

			int fim = 0;
			char output_teste[100] = "\0";
			while(fgets(output_executado, sizeof(output_executado), output) != NULL)
			{
				strcpy(output_teste, output_executado);
				token4 = strtok(output_teste, s);

				if(strcmp(token4, "OUTPUT") == 0)
				{
					break;
				}

				strcat(output_enviar, output_executado);

			}

			write(server_to_client, output_enviar, strlen(output_enviar));
			close(server_to_client);

		}//if == (-r)

	}//while(1)


	fclose(fd);
	fclose(fd_rescrita);
	fclose(output);

	return 0;
}
