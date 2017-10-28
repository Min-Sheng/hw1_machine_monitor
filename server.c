#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>

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
void parse_status(int pid, char *substr, char *out);

void socket_server();

int main(int argc, char **argv)
{
	socket_server();
	return 0;
}

void socket_server()
{

	struct sockaddr_in server_addr;
	int connection_socket;
	int ret;
	char quest[1];
	int pid[1];
	int reuseaddr = 1;
	socklen_t reuseaddr_len;

	/* Create socket */
	connection_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (connection_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* Initialize structure server_addr */
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
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
	while(1) {
		/* Make it listen to socket with max 20 connections */
		ret = listen(connection_socket, 20);
		if (ret == -1) {
			perror("listen");
			exit(EXIT_FAILURE);
		}

		int client_socket;
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(client_addr);
		client_socket = accept(connection_socket, (struct sockaddr *)&client_addr,
		                       (socklen_t *)&addrlen);
		/* Wait and Accept connection */
		if (client_socket == -1) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		/* Infinity loop -- accepting connection from client forever */
		while(1) {

			/* Receive quest from the client*/
			memset(&quest, 0, 1);
			memset(&pid, 0, 1);
			recv(client_socket, quest, sizeof(quest), 0);

			if(quest[0] == 'a') {
				a_all_pids(&client_socket);
			} else if(quest[0] == 'b') {
				recv(client_socket, pid, sizeof(pid), 0);
				b_tids(&client_socket, pid[0]);
			} else if(quest[0] == 'd') {
				recv(client_socket, pid, sizeof(pid), 0);
				d_process_name(&client_socket, pid[0]);
			} else if(quest[0] == 'e') {
				recv(client_socket, pid, sizeof(pid), 0);
				e_state(&client_socket, pid[0]);
			} else if(quest[0] == 'f') {
				recv(client_socket, pid, sizeof(pid), 0);
				f_cmdline(&client_socket, pid[0]);
			} else if(quest[0] == 'g') {
				recv(client_socket, pid, sizeof(pid), 0);
				g_parent_pid(&client_socket, pid[0]);
			} else if(quest[0] == 'i') {
				recv(client_socket, pid, sizeof(pid), 0);
				i_Vmsize(&client_socket, pid[0]);
			} else if(quest[0] == 'j') {
				recv(client_socket, pid, sizeof(pid), 0);
				j_VmRSS(&client_socket, pid[0]);
			} else if(quest[0] == 'k') {
				/* Close(client) */
				close(client_socket);
				break;
			}
		}
	}
	/* Close(server) , but never get here because of the loop */
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
	char end[1] = "\0";
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
		send(*client_socket, end, BUFFER_SIZE, 0);
		closedir(d);
	}
}

void b_tids(int *client_socket, int pid)
{

	DIR           *d;
	struct dirent *dir;
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE];
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/task/");
	d = opendir(path);
	int isTID = 0;
	int i = 0, j = 0;
	char name[BUFFER_SIZE];
	char end[1] = "\0";
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
		send(*client_socket, end, BUFFER_SIZE, 0);
		closedir(d);
	}
}
void d_process_name(int *client_socket, int pid)
{
	char pname[BUFFER_SIZE];
	char end[1] = "\0";
	parse_status(pid, "Name:",pname);
	send(*client_socket, pname, BUFFER_SIZE, 0);
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void e_state(int *client_socket, int pid)
{
	char state[BUFFER_SIZE];
	char end[1] = "\0";
	parse_status(pid, "State:",state);
	send(*client_socket, state, BUFFER_SIZE, 0);
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void f_cmdline(int *client_socket, int pid)
{
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE];
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/cmdline");
	FILE *fp;
	char cmdline[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	char end[1] = "\0";
	int no_cmdline = 1;
	fp = fopen(path, "r");
	if (fp == NULL) {
		no_cmdline = 0;
		strcpy(cmdline, "No such pid or fail to open file!");
		send(*client_socket, cmdline, BUFFER_SIZE, 0);
	} else {
		while(1) {
			if(fgets(buffer,BUFFER_SIZE,fp) == NULL) {
				break;
			} else {
				no_cmdline = 0;
				strcpy(cmdline, buffer);
				send(*client_socket, cmdline, BUFFER_SIZE, 0);
			}
		}
		fclose(fp);
	}
	if(no_cmdline==1) {
		strcpy(cmdline, "No cmdline.");
		send(*client_socket, cmdline, BUFFER_SIZE, 0);
	}
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void g_parent_pid(int *client_socket, int pid)
{
	char ppid[BUFFER_SIZE];
	char end[1] = "\0";
	parse_status(pid, "PPid:",ppid);
	send(*client_socket, ppid, BUFFER_SIZE, 0);
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void i_Vmsize(int *client_socket, int pid)
{
	char vmsize[BUFFER_SIZE];
	char end[1] = "\0";
	parse_status(pid, "VmSize:",vmsize);
	strcat(vmsize, " kB");
	send(*client_socket, vmsize, BUFFER_SIZE, 0);
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void j_VmRSS(int *client_socket, int pid)
{
	char vmrss[BUFFER_SIZE];
	char end[1] = "\0";
	parse_status(pid, "VmRSS:",vmrss);
	strcat(vmrss, " kB");
	send(*client_socket, vmrss, BUFFER_SIZE, 0);
	send(*client_socket, end, BUFFER_SIZE, 0);
}
void parse_status(int pid, char *substr, char *out)
{
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE];
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/status");
	FILE *fp;
	char buffer[BUFFER_SIZE];
	fp = fopen(path, "r");
	if(fp==NULL) {
		strcpy(out, "Open file failed!");
	} else {
		while(fgets(buffer,BUFFER_SIZE,fp) != NULL) {
			if(strstr(buffer, substr)!=NULL) {
				char *temp;
				strtok(buffer, "\t");
				temp = strtok(NULL, " ");
				strcpy(out, temp);
			}
		}
		fclose(fp);
	}
}
