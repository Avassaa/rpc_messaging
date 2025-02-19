/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _MYPROG_H_RPCGEN
#define _MYPROG_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C"
{
#endif

	struct client
	{
		char *name;
	};
	typedef struct client client;

	struct message
	{
		char *sender;
		char *recipient;
		char *content;
	};
	typedef struct message message;

#define MYAPP_PROG 0x12471
#define MYAPP_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define register_client 1
	extern bool_t *register_client_1(client *, CLIENT *);
	extern bool_t *register_client_1_svc(client *, struct svc_req *);
#define send_message 2
	extern bool_t *send_message_1(message *, CLIENT *);
	extern bool_t *send_message_1_svc(message *, struct svc_req *);
#define fetch_message 3
	extern message *fetch_message_1(client *, CLIENT *);
	extern message *fetch_message_1_svc(client *, struct svc_req *);
	extern int myapp_prog_1_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define register_client 1
extern bool_t *register_client_1();
extern bool_t *register_client_1_svc();
#define send_message 2
extern bool_t *send_message_1();
extern bool_t *send_message_1_svc();
#define fetch_message 3
extern message *fetch_message_1();
extern message *fetch_message_1_svc();
extern int myapp_prog_1_freeresult();
#endif /* K&R C */

	/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
	extern bool_t xdr_client(XDR *, client *);
	extern bool_t xdr_message(XDR *, message *);

#else /* K&R C */
extern bool_t xdr_client();
extern bool_t xdr_message();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_MYPROG_H_RPCGEN */
