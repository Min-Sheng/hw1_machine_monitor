#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
//void *communicate_to_client();
void a_all_pids(int *client_socket);
void b_tids(int *client_socket, int pid);
void c_child_pids(int *client_socket, int pid);
void d_process_name(int *client_socket, int pid);
void e_state(int *client_socket, int pid);
void f_cmdline(int *client_socket, int pid);
void g_parent_pid(int *client_socket, int pid);
void h_all_ancients_of_pid(int *client_socket, int pid);
void i_Vmsize(int *client_socket, int pid);
void j_VmRSS(int *client_socket, int pid);
void k_exit(int *client_socket);

void socket_server();

int main(int argc, char **argv)
{
// write someting here...
	socket_server();
	//a_all_pids((&ans)[BUFFER_SIZE]);
	//printf("%d\n", SERVER_PORT);
	//printf("%d\n", BUFFER_SIZE);
	return 0;
}

void socket_server()
{

	struct sockaddr_in server_addr;
	int connection_socket;
	int ret;
	char quest[2];
	int reuseaddr = 1;
	socklen_t reuseaddr_len;

	/* create sockett */
	connection_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (connection_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* initialize structure server_addr */
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	//bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	/* this line is different from client */
	server_addr.sin_addr.s_addr = INADDR_ANY;

	reuseaddr_len = sizeof(reuseaddr);
	setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
	           reuseaddr_len);

	/* Assign a port number to socket */
	ret = bind(connection_socket, (struct sockaddr*)&server_addr,
	           sizeof(struct sockaddr_in));

	if (ret == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* make it listen to socket with max 20 connections */
	ret = listen(connection_socket, 20);
	if (ret == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* infinity loop -- accepting connection from client forever */
	while(1) {
		int client_socket;
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(client_addr);

		/* Wait and Accept connection */
		client_socket = accept(connection_socket, (struct sockaddr*)&client_addr,
		                       (socklen_t *)&addrlen);
		if (client_socket == -1) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		/* Receive quest from the client*/
		memset(&quest, 0, 1);
		recv(client_socket, quest, sizeof(quest), 0);

		if(quest[0] == 'a') {
			a_all_pids(&client_socket);
		}
		if(quest[0] == 'b') {
			b_tids(&client_socket, 1);
		}
		/* close(client) */
		close(client_socket);

	}

	/* close(server) , but never get here because of the loop */
	close(connection_socket);

}

void a_all_pids(int *client_socket)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc/");
	int isPID = 0;
	int i = 0, j = 0;
	char name[BUFFER_SIZE];
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			strcpy(name, dir->d_name);
			while (i<strlen(name)) {
				if(name[i] > 57 || name[i] < 48) {
					isPID = 0;
					break;
				} else
					isPID = 1;
				i++;
			}
			if (isPID == 1) {
				send(*client_socket, name, BUFFER_SIZE, 0);
				j++;
			}
		}
		closedir(d);
	}
}


void b_tids(int *client_socket, int pid)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc/1/task/");
	int isTID = 0;
	int i = 0, j = 0;
	char name[BUFFER_SIZE];
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			strcpy(name, dir->d_name);
			while (i<strlen(name)) {
				if(name[i] > 57 || name[i] < 48) {
					isTID = 0;
					break;
				} else
					isTID = 1;
				i++;
			}
			if (isTID == 1) {
				send(*client_socket, name, BUFFER_SIZE, 0);
				j++;
			}
		}
		closedir(d);
	}
}
