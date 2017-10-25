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
void a_all_pids();
void b_tids(int pid);
void c_child_pids(int pid);
void d_process_name(int pid);
void e_state(int pid);
void f_cmdline(int pid);
void g_parent_pid(int pid);
void h_all_ancients_of_pid(int pid);
void i_Vmsize(int pid);
void j_VmRSS(int pid);
void k_exit();

void socket_server();

int main(int argc, char **argv)
{
// write someting here...
	socket_server();
	//a_all_pids();
	//printf("%d\n", SERVER_PORT);
	//printf("%d\n", BUFFER_SIZE);
	return 0;
}

void socket_server()
{

	struct sockaddr_in server_addr;
	int connection_socket;
	int ret;
	char buffer[BUFFER_SIZE] = "Hi client! ";
	char buffer1[BUFFER_SIZE];
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

		/* Send message */
		send(client_socket, buffer, sizeof(buffer), 0);
		int res=recv(client_socket, buffer1, sizeof(buffer1), 0);
		printf("receive from client: %s, %d bytes\n", buffer1,res);
		//write(fd, buffer1, sizeof(buffer1));
		/* close(client) */
		close(client_socket);

	}

	/* close(server) , but never get here because of the loop */
	close(connection_socket);

}

void a_all_pids()
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc");
	int isPID = 0;
	int i = 0;
	char name[256];
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
				printf("%s\n", name);
			}
		}
		closedir(d);
	}
}


