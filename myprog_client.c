#include "myprog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <pthread.h>
#include <unistd.h>

CLIENT *clnt;
client reg_client;
pthread_mutex_t fetch_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fetch_cond = PTHREAD_COND_INITIALIZER;
int new_message = 0;

void *fetch_messages(void *arg)
{
	message *recv_msg;
	char *client_name = (char *)arg;

	while (1)
	{
		sleep(3); // Check for new messages every 3 seconds
		recv_msg = fetch_message_1(&reg_client, clnt);
		if (recv_msg == NULL)
		{

			continue;
		}
		else if (recv_msg->content != NULL)
		{
			printf("Received message from %s: %s\n", recv_msg->sender, recv_msg->content);
		}
		else
		{
			printf("No new messages.\n");
		}
	}
	return NULL;
}

void myapp_prog_1(char *client_name, char *host)
{
	message msg;
	bool_t *reg_result;
	bool_t *send_result;
	pthread_t fetch_thread;

	printf("Connecting to server at %s\n", host);
	clnt = clnt_create(host, MYAPP_PROG, MYAPP_VERSION, "tcp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}

	reg_client.name = client_name;

	printf("Registering client: %s\n", client_name);
	reg_result = register_client_1(&reg_client, clnt);
	if (reg_result == NULL || *reg_result == FALSE)
	{
		fprintf(stderr, "Failed to register client.\n");
		clnt_destroy(clnt);
		exit(1);
	}
	else
	{
		printf("Client registered successfully.\n");
	}

	msg.sender = client_name;

	if (pthread_create(&fetch_thread, NULL, fetch_messages, (void *)client_name) != 0)
	{
		fprintf(stderr, "Failed to create fetch thread.\n");
		clnt_destroy(clnt);
		exit(1);
	}

	char input[256];
	while (1)
	{
		if (fgets(input, sizeof(input), stdin) == NULL)
		{
			break;
		}
		input[strcspn(input, "\n")] = '\0';

		char *recipient = strtok(input, " ");
		char *content = strtok(NULL, "");

		if (recipient == NULL || content == NULL)
		{
			printf("Invalid input format. Please use: recipient message\n");
			continue;
		}

		msg.recipient = recipient;
		msg.content = content;

		printf("Sending message to %s: %s\n", msg.recipient, msg.content);
		send_result = send_message_1(&msg, clnt);
		if (send_result == NULL || *send_result == FALSE)
		{
			fprintf(stderr, "Failed to send message.\n");
		}
		else
		{
			printf("Message sent successfully.\n");
		}
	}

	clnt_destroy(clnt);
	pthread_cancel(fetch_thread);
	pthread_join(fetch_thread, NULL);
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <client_name> <server_host>\n", argv[0]);
		exit(1);
	}
	myapp_prog_1(argv[1], argv[2]);
	return 0;
}
