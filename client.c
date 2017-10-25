#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
// write someting here...
	struct sockaddr_in client_addr;
	int client_socket;
	int ret;
	char buffer[BUFFER_SIZE];
	char resp[10]="clientack";

	/* create socket */
	client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/* initialize value in client_addr */
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
//bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//inet_aton("127.0.0.1", &client_addr.sin_addr);

	/* Connecting to server */
	ret = connect(client_socket, (struct sockaddr*)&client_addr,
	              sizeof(struct sockaddr_in));

	if (ret == -1) {
		fprintf(stderr, "The server is down.\n");
		exit(EXIT_FAILURE);
	}

	/* Receive message from the server and print to screen */
	bzero(buffer, 128);
	recv(client_socket, buffer, sizeof(buffer), 0);
	printf("receive from server: %s\n", buffer);
	send(client_socket,resp,sizeof(resp),0);

	/* Close connection */
	close(client_socket);

	return 0;
}

