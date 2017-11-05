/*************************************************
		Class  : OS
		Project: Hw1_machine_monitor
		Author : Min-Sheng Wu
*************************************************/

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

void a_all_pids(int *new_fd);
void b_tids(int *new_fd, int pid);
void c_child_pids(int *new_fd, int pid);
void d_process_name(int *new_fd, int pid);
void e_state(int *new_fd, int pid);
void f_cmdline(int *new_fd, int pid);
void g_parent_pid(int *new_fd, int pid);
void h_all_ancients_of_pid(int *new_fd, int pid);
void i_Vmsize(int *new_fd, int pid);
void j_VmRSS(int *new_fd, int pid);
void k_exit(int *new_fd);
void parse_status(int pid, char *substr, char *out);

void socket_server();

void *thread_function(void *arg);

int main(int argc, char **argv)
{
	socket_server();
	return 0;
}

void socket_server()
{
	struct addrinfo hints, *res;
	int socketfd, new_fd;
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	int ret;

	int result;
	fd_set readfds, testfds;
	pthread_t thread_a;
	pthread_attr_t attr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;// Use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;// fill in my IP for me

	getaddrinfo(NULL, SERVER_PORT, &hints, &res);

	/* Create socket */
	socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (socketfd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* No wait time to reuse the socket*/
	int reeuseaddr = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reeuseaddr,
	           sizeof(reeuseaddr));

	/* Bind a port number to socket */
	ret = bind(socketfd, res->ai_addr, res->ai_addrlen);

	if (ret == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* Make it listen to socket with max 'BACKLOG' connections */
	ret = listen(socketfd, BACKLOG);
	if (ret == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	FD_ZERO(&readfds);
	FD_SET(socketfd, &readfds);

	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

	printf("server waiting\n");
	while(1) {
		int fd;
		testfds = readfds;
		result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0,
		                (struct timeval *) 0);

		if(result < 1) {
			perror("server error");
			exit(EXIT_FAILURE);
		}

		for(fd = 0; fd < FD_SETSIZE; fd++) {
			if(FD_ISSET(fd, &testfds)) {
				/* New a connection */
				if(fd == socketfd) {
					/* Wait and accept the connection */
					addr_size = sizeof(client_addr);
					new_fd = accept(socketfd, (struct sockaddr *)&client_addr,
					                (socklen_t *)&addr_size);
					if(new_fd == -1) {
						perror("accept");
						exit(EXIT_FAILURE);
					}

					FD_SET(new_fd, &readfds);
					printf("add client fd %d \n", new_fd);
					pthread_create(&thread_a, &attr, thread_function, (void *)(intptr_t)new_fd);
				}
			}
		}
	}
	/* Close(server) , but never get here because of the loop */
	close(socketfd);
	freeaddrinfo(res);
}

/* Thread to handle the client fd */
void *thread_function(void *arg)
{
	char quest[1];
	int pid[1];
	int new_fd = (intptr_t)arg;
	while(1) {

		/* Receive quest from the client*/
		memset(&quest, 0, 1);
		memset(&pid, 0, 1);
		int recv_num = recv(new_fd, quest, sizeof(quest), 0);
		if( recv_num < 0) {
			perror("recv ");
			break;
		} else if( recv_num == 0) {
			printf("client fd %d disconnect\n", new_fd);
			break;
		} else {
			if(quest[0] == 'a') {
				a_all_pids(&new_fd);
			} else if(quest[0] == 'b') {
				recv(new_fd, pid, sizeof(pid), 0);
				b_tids(&new_fd, pid[0]);
			} else if(quest[0] == 'c') {
				recv(new_fd, pid, sizeof(pid), 0);
				c_child_pids(&new_fd, pid[0]);
			} else if(quest[0] == 'd') {
				recv(new_fd, pid, sizeof(pid), 0);
				d_process_name(&new_fd, pid[0]);
			} else if(quest[0] == 'e') {
				recv(new_fd, pid, sizeof(pid), 0);
				e_state(&new_fd, pid[0]);
			} else if(quest[0] == 'f') {
				recv(new_fd, pid, sizeof(pid), 0);
				f_cmdline(&new_fd, pid[0]);
			} else if(quest[0] == 'g') {
				recv(new_fd, pid, sizeof(pid), 0);
				g_parent_pid(&new_fd, pid[0]);
			} else if(quest[0] == 'h') {
				recv(new_fd, pid, sizeof(pid), 0);
				h_all_ancients_of_pid(&new_fd, pid[0]);
			} else if(quest[0] == 'i') {
				recv(new_fd, pid, sizeof(pid), 0);
				i_Vmsize(&new_fd, pid[0]);
			} else if(quest[0] == 'j') {
				recv(new_fd, pid, sizeof(pid), 0);
				j_VmRSS(&new_fd, pid[0]);
			} else if(quest[0] == 'k') {
				printf("client fd %d disconnect\n", new_fd);
				break;
			}
		}
	}
	pthread_exit("");
}

void a_all_pids(int *new_fd)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc/");
	int isPID = 0;
	int i = 0;
	char name[BUFFER_SIZE]="";
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
				send(*new_fd, name, BUFFER_SIZE, 0);
			}
		}
		closedir(d);
	}
	send(*new_fd, "\0", 1, 0);
}

void b_tids(int *new_fd, int pid)
{

	DIR           *d;
	struct dirent *dir;
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE]="";
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/task/");
	d = opendir(path);
	int isTID = 0;
	int i = 0;
	char name[BUFFER_SIZE]="";
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
				send(*new_fd, name, BUFFER_SIZE, 0);
			}
		}
		closedir(d);
	} else {
		strcpy(name,"No such pid.");
		send(*new_fd, name, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void c_child_pids(int *new_fd, int pid)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir("/proc/");
	char ppid[BUFFER_SIZE];
	sprintf(ppid, "%d", pid);
	int isPID = 0;
	int i = 0;
	char cpid[BUFFER_SIZE]="";
	char name[BUFFER_SIZE]="";
	int cpid_exist=0;
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			strcpy(cpid, dir->d_name);
			while (i < strlen(cpid)) {
				if(cpid[i] > 57 || cpid[i] < 48) {
					isPID = 0;
					break;
				} else
					isPID = 1;
				i++;
			}
			if (isPID == 1) {
				parse_status(atoi(cpid), "PPid:", name);
				if (!strcmp(name, ppid)) {
					cpid_exist=1;
					send(*new_fd, cpid, BUFFER_SIZE, 0);
				}
			}
		}
		if(cpid_exist==0) {
			strcpy(name,"No child pid.");
			send(*new_fd, name, BUFFER_SIZE, 0);
		}
		closedir(d);
	} else {
		strcpy(name,"No such pid.");
		send(*new_fd, name, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void d_process_name(int *new_fd, int pid)
{
	char pname[BUFFER_SIZE]="";
	parse_status(pid, "Name:",pname);
	if( *pname=='\0' ) {
		strcpy(pname, "No such pid.");
		send(*new_fd, pname, BUFFER_SIZE, 0);
	} else {
		send(*new_fd, pname, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void e_state(int *new_fd, int pid)
{
	char state[BUFFER_SIZE]="";
	parse_status(pid, "State:",state);
	if( *state=='\0' ) {
		strcpy(state, "No such pid.");
		send(*new_fd, state, BUFFER_SIZE, 0);
	} else {
		send(*new_fd, state, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void f_cmdline(int *new_fd, int pid)
{
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE];
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/cmdline");
	FILE *fp;
	char cmdline[BUFFER_SIZE]="";
	char buffer[BUFFER_SIZE]="";
	int no_cmdline = 1;
	fp = fopen(path, "r");
	if (fp == NULL) {
		no_cmdline = 0;
		strcpy(cmdline, "No such pid.");
		send(*new_fd, cmdline, BUFFER_SIZE, 0);
	} else {
		while(1) {
			if(fgets(buffer,BUFFER_SIZE,fp) == NULL) {
				break;
			} else {
				no_cmdline = 0;
				strcpy(cmdline, buffer);
				send(*new_fd, cmdline, BUFFER_SIZE, 0);
			}
		}
		fclose(fp);
	}
	if(no_cmdline==1) {
		strcpy(cmdline, "No cmdline.");
		send(*new_fd, cmdline, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void g_parent_pid(int *new_fd, int pid)
{
	char ppid[BUFFER_SIZE]="";
	parse_status(pid, "PPid:",ppid);
	if( *ppid=='\0' ) {
		strcpy(ppid, "No such pid.");
		send(*new_fd, ppid, BUFFER_SIZE, 0);
	} else {
		send(*new_fd, ppid, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void h_all_ancients_of_pid(int *new_fd, int pid)
{
	char apid[BUFFER_SIZE]="";
	char out[BUFFER_SIZE]="";
	while (1) {
		parse_status(pid, "PPid:", apid);
		if( *apid=='\0' ) {
			strcpy(apid, "No such pid.");
			send(*new_fd, apid, BUFFER_SIZE, 0);
			break;
		} else {
			strcpy(out, "->");
			strcat(out, apid);
			send(*new_fd, out, BUFFER_SIZE, 0);
		}
		if (atoi(apid)==0)
			break;
		pid = atoi(apid);
	}
	send(*new_fd, "\0", 1, 0);
}

void i_Vmsize(int *new_fd, int pid)
{
	char vmsize[BUFFER_SIZE]="";
	parse_status(pid, "VmSize:",vmsize);
	if( *vmsize=='\0' ) {
		strcpy(vmsize, "No such pid.");
		send(*new_fd, vmsize, BUFFER_SIZE, 0);
	} else if(*vmsize==' ') {
		strcat(vmsize, "No VmSize.");
		send(*new_fd, vmsize, BUFFER_SIZE, 0);
	} else {
		strcat(vmsize, " kB");
		send(*new_fd, vmsize, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void j_VmRSS(int *new_fd, int pid)
{
	char vmrss[BUFFER_SIZE]="";
	parse_status(pid, "VmRSS:",vmrss);
	if( *vmrss=='\0' ) {
		strcpy(vmrss, "No such pid.");
		send(*new_fd, vmrss, BUFFER_SIZE, 0);
	} else if(*vmrss==' ') {
		strcat(vmrss, "No VmRSS.");
		send(*new_fd, vmrss, BUFFER_SIZE, 0);
	} else {
		strcat(vmrss, " kB");
		send(*new_fd, vmrss, BUFFER_SIZE, 0);
	}
	send(*new_fd, "\0", 1, 0);
}

void parse_status(int pid, char *substr, char *out)
{
	char path[BUFFER_SIZE] = "/proc/\0";
	char pid_path[BUFFER_SIZE];
	sprintf(pid_path, "%d", pid);
	strcat(path, pid_path);
	strcat(path, "/status");
	FILE *fp;
	char buffer[BUFFER_SIZE]="";
	fp = fopen(path, "r");
	int str_exist=0;
	if(fp==NULL) {
		strcpy(out, "\0");
	} else {
		while(fgets(buffer,BUFFER_SIZE,fp) != NULL) {
			if(strstr(buffer, substr)!=NULL) {
				if(buffer[0]!='\0') {
					str_exist=1;
					char *temp;
					strtok(buffer, "\t");
					temp = strtok(NULL, " ");
					strtok(temp, "\n");
					strcpy(out, temp);
				}
			}
		}
		if(str_exist==0) {
			strcpy(out, " ");
		}
		fclose(fp);
	}
}
