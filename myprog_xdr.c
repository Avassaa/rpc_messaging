#include "myprog.h"

bool_t xdr_client(XDR *xdrs, client *objp)
{
	if (!xdr_string(xdrs, &objp->name, ~0))
	{
		return FALSE;
	}
	return TRUE;
}

bool_t xdr_message(XDR *xdrs, message *objp)
{
	if (!xdr_string(xdrs, &objp->sender, ~0))
	{
		return FALSE;
	}
	if (!xdr_string(xdrs, &objp->recipient, ~0))
	{
		return FALSE;
	}
	if (!xdr_string(xdrs, &objp->content, ~0))
	{
		return FALSE;
	}
	return TRUE;
}