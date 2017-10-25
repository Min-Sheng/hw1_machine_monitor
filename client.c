#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_ask();

int main(int argc, char **argv)
{
	struct sockaddr_in client_addr;
	int client_socket;
	int ret;
	char quest[1];
	int pid[1];
	char ans[BUFFER_SIZE];

	/* Create socket */
	client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* Initialize value in client_addr */
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Linking to server...");

	while(1) {

		printf("\n====================================================\n");

		/* Connecting to server */
		ret = connect(client_socket, (struct sockaddr*)&client_addr,
		              sizeof(struct sockaddr_in));

		if (ret == -1) {
			fprintf(stderr, "The server is down.\n");
			exit(EXIT_FAILURE);
		}

		else {
			print_ask();
		}

		/*Send quest*/
		memset(&quest, 0, 1);
		memset(&pid, 0, 1);
		if(scanf("%c",&quest[0]) == 1) {
			if (quest[0] == 'a') {
				send(client_socket,quest,sizeof(quest),0);
				printf("\n");
			} else if (quest[0] == 'b') {
				send(client_socket, quest, sizeof(quest), 0);
				printf("pid? ");
				if(scanf("%d",&pid[0]) == 1) {
					send(client_socket, pid, sizeof(pid), 0);
					printf("\n");
					printf("[tid] ");
				} else {
					printf("Failed.\n");
				}
			}
		} else {
			printf("Failed.\n");
		}
		/* Receive ans from the server and print to screen */
		memset(&ans, 0, BUFFER_SIZE);

		while(1) {
			int res = recv(client_socket, ans, sizeof(ans), 0);
			if(res == 0) {
				break;
			}
			printf("%s\n", ans);
		}
	}
	/* Close connection */
	close(client_socket);

	return 0;
}
void print_ask()
{
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
}
