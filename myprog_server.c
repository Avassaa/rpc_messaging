#include "myprog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define MAX_MESSAGES 100

typedef struct
{
	char *name;
	message messages[MAX_MESSAGES];
	int message_count;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

bool_t *register_client_1_svc(client *argp, struct svc_req *rqstp)
{
	static bool_t result;
	result = FALSE;

	(void)rqstp;

	printf("Received registration request for client: %s\n", argp->name);

	for (int i = 0; i < client_count; i++)
	{
		if (strcmp(clients[i].name, argp->name) == 0)
		{
			result = TRUE;
			return &result;
		}
	}

	if (client_count < MAX_CLIENTS)
	{
		clients[client_count].name = strdup(argp->name);
		clients[client_count].message_count = 0;
		pthread_mutex_init(&clients[client_count].mutex, NULL);
		pthread_cond_init(&clients[client_count].cond, NULL);
		client_count++;
		result = TRUE;
	}

	return &result;
}

bool_t *send_message_1_svc(message *argp, struct svc_req *rqstp)
{
	static bool_t result;
	result = FALSE;

	(void)rqstp;

	printf("Received message from %s to %s: %s\n", argp->sender, argp->recipient, argp->content);

	for (int i = 0; i < client_count; i++)
	{
		if (strcmp(clients[i].name, argp->recipient) == 0)
		{
			if (clients[i].message_count < MAX_MESSAGES)
			{
				clients[i].messages[clients[i].message_count].sender = strdup(argp->sender);
				clients[i].messages[clients[i].message_count].recipient = strdup(argp->recipient);
				clients[i].messages[clients[i].message_count].content = strdup(argp->content);
				clients[i].message_count++;
				result = TRUE;

				pthread_mutex_lock(&clients[i].mutex);
				pthread_cond_signal(&clients[i].cond);
				pthread_mutex_unlock(&clients[i].mutex);
			}
			break;
		}
	}

	if (!result)
	{
		printf("Recipient %s not found. Message not sent.\n", argp->recipient);
	}
	return &result;
}

message *fetch_message_1_svc(client *argp, struct svc_req *rqstp)
{
	static message result;
	memset(&result, 0, sizeof(result));

	(void)rqstp;

	printf("Fetching messages for client: %s\n", argp->name);

	for (int i = 0; i < client_count; i++)
	{
		if (strcmp(clients[i].name, argp->name) == 0)
		{
			if (clients[i].message_count > 0)
			{
				result.sender = clients[i].messages[0].sender;
				result.recipient = clients[i].messages[0].recipient;
				result.content = clients[i].messages[0].content;

				for (int j = 1; j < clients[i].message_count; j++)
				{
					clients[i].messages[j - 1] = clients[i].messages[j];
				}
				clients[i].message_count--;
			}
			else
			{
				result.content = NULL;
			}
			break;
		}
	}

	return &result;
}

void myapp_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union
	{
		client register_client_1_arg;
		message send_message_1_arg;
		client fetch_message_1_arg;
	} argument;

	union
	{
		bool_t register_client_1_res;
		bool_t send_message_1_res;
		message fetch_message_1_res;
	} result;

	bool_t (*local)(char *, struct svc_req *);
	xdrproc_t _xdr_argument, _xdr_result;

	switch (rqstp->rq_proc)
	{
	case NULLPROC:
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
		return;

	case register_client:
		_xdr_argument = (xdrproc_t)xdr_client;
		_xdr_result = (xdrproc_t)xdr_bool;
		local = (bool_t(*)(char *, struct svc_req *))register_client_1_svc;
		break;

	case send_message:
		_xdr_argument = (xdrproc_t)xdr_message;
		_xdr_result = (xdrproc_t)xdr_bool;
		local = (bool_t(*)(char *, struct svc_req *))send_message_1_svc;
		break;

	case fetch_message:
		_xdr_argument = (xdrproc_t)xdr_client;
		_xdr_result = (xdrproc_t)xdr_message;
		local = (bool_t(*)(char *, struct svc_req *))fetch_message_1_svc;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}

	memset(&argument, 0, sizeof(argument));
	if (!svc_getargs(transp, _xdr_argument, (caddr_t)&argument))
	{
		svcerr_decode(transp);
		return;
	}

	memset(&result, 0, sizeof(result));
	if (!(*local)((char *)&argument, rqstp))
	{
		svcerr_systemerr(transp);
	}
	else
	{
		if (!svc_sendreply(transp, _xdr_result, (char *)&result))
		{
			svcerr_systemerr(transp);
		}
	}

	if (!svc_freeargs(transp, _xdr_argument, (caddr_t)&argument))
	{
		fprintf(stderr, "unable to free arguments\n");
		exit(1);
	}
	return;
}