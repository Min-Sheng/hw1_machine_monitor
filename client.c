/*************************************************
		Class  : OS
		Project: Hw1_machine_monitor
		Author : Min-Sheng Wu
*************************************************/

#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv)
{
	struct addrinfo hints, *res;
	int socketfd;
	int ret;
	char quest[1];
	int pid[1];
	char ans[BUFFER_SIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;// Use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;// Auto fill in my IP for me

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

	/* Connecting to server */
	printf("Connecting to server...\n");
	ret = connect(socketfd, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		fprintf(stderr, "The server is down.\n");
		exit(EXIT_FAILURE);
	}

	while(1) {

		printf("====================================================\n");

		printf("(a) list all process ids\n");
		printf("(b) thread's IDs\n");
		printf("(c) clild's PIDs\n");
		printf("(d) process name\n");
		printf("(e) state of process (D,R,S,T,t,W,X,Z)\n");
		printf("(f) command line of excuting process (cmdline)\n");
		printf("(g) parent's PID\n");
		printf("(h) all ancients of PIDs\n");
		printf("(i) virtual memory size (Vmsize)\n");
		printf("(j) physical memory size (VmRSS)\n");
		printf("(k) exit\n");
		printf("which? ");

		setbuf(stdin,NULL);
		/*Send quest*/
		memset(&quest, 0, 1);
		memset(&pid, 0, 1);
		if(scanf("%s",quest) == 1) {
			if (quest[0] == 'a') {
				send(socketfd,quest,sizeof(quest),0);
				printf("[pid] ");
				printf("\n");
			} else if (quest[0] == 'b') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[tid] ");
				} else {
					printf("Failed.\n");
				}
			} else if(quest[0] == 'c') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[Children's pid] ");
					printf("\n");
				}
			} else if(quest[0] == 'd') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[Name] ");
				}
			} else if(quest[0] == 'e') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[State] ");
				}
			} else if(quest[0] == 'f') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[cmdline] ");
				}
			} else if(quest[0] == 'g') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[ppid] ");
				}
			} else if(quest[0] == 'h') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[Ancient's pids] ");
					printf("\n");
				}
			} else if(quest[0] == 'i') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[Vmsize] ");
				}
			} else if(quest[0] == 'j') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(socketfd, pid, sizeof(pid), 0);
					printf("\n");
					printf("[VmRSS] ");
				}
			} else if(quest[0] == 'k') {
				send(socketfd, quest, sizeof(quest), 0);
				printf("The client exited.\n");
				break;
			} else {
				printf("Please enter the correct character.\n");
				break;
			}
		} else {
			printf("Failed.\n");
			break;
		}
		/* Receive ans from the server and print to screen */
		memset(&ans, 0, BUFFER_SIZE);

		while(1) {
			recv(socketfd, ans, sizeof(ans), 0);
			if (ans[0]=='\0') {
				break;
			}
			printf("%s\n", ans);
		}
	}
	/* Close connection */
	close(socketfd);

	return 0;
}
