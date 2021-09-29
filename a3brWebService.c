//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description:
//		Establish the initial connection to the IRC,
//		and handle channeling of all HTTP traffic back and forth.
//********************************************************************************

#include <bur/plctypes.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "A3br.h"
#include "digest.h"
#include "A3brCommon.h"
#ifdef __cplusplus
};
#endif

#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wpointer-sign"
//#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
//#pragma GCC diagnostic ignored "-Wreturn-type"

void a3brSession(A3brWebServiceSession_typ *inst, A3brWebServiceCfg_typ *configuration, Buffer_typ *requests);

void A3brWebService(struct A3brWebService *inst)
{

	int MAX_HTTP_CONNECTIONS = inst->configuration.maxSessions ? inst->configuration.maxSessions : 1;
	MAX_HTTP_CONNECTIONS = MAX_HTTP_CONNECTIONS > 3 ? 3 : MAX_HTTP_CONNECTIONS;
	int i = 0;

	inst->ident = &inst->internal.api;
	inst->connected = 0;
	inst->activeSessions = 0;
	inst->activeConnections = 0;

	if (inst->enable)
	{
		if (!inst->internal.api.requestBuffer.Data)
		{
			BufferInit(&inst->internal.api.requestBuffer, 200, sizeof(A3brWebServiceRequest_typ));
		}
		for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
		{
			a3brSession(&inst->internal.session[i], &inst->configuration, &inst->internal.api.requestBuffer);
			if (inst->internal.session[i].connected)
			{
				inst->connected = 1;
				inst->activeSessions++;
			}
			inst->activeConnections += inst->internal.session[i].activeConnections;
		}
	}
	else
	{   // not enable
		//TODO: clear things out?
	}
}

void a3brSession(A3brWebServiceSession_typ *inst, A3brWebServiceCfg_typ *configuration, Buffer_typ *requests)
{

	int i = 0;

	int MAX_HTTP_CONNECTIONS = sizeof(inst->connection) / sizeof(inst->connection[0]);
	if (!inst->inited)
	{
		inst->inited = 1;
		for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
		{
			//Assign initial parameters to HTTP blocks.
			inst->connection[i].reqHeader.rawHeader.pData = &inst->connection[i].rawReqHeader;
			inst->connection[i].reqHeader.rawHeader.dataSize = sizeof(inst->connection[i].rawReqHeader);

			inst->connection[i].resHeader.rawHeader.pData = &inst->connection[i].rawResHeader;
			inst->connection[i].resHeader.rawHeader.dataSize = sizeof(inst->connection[i].rawResHeader);

			brsstrcpy(inst->connection[i].reqHeader.connection, "keep-alive");
			brsstrcpy(inst->connection[i].reqHeader.keepAlive, "timeout=60");

			inst->connection[i].httpClient.pResponseHeader = &inst->connection[i].resHeader;
			inst->connection[i].httpClient.pResponseData = &inst->connection[i].resData;
			inst->connection[i].httpClient.responseDataSize = sizeof(inst->connection[i].resData);
			inst->connection[i].httpClient.pStatistics = &inst->connection[i].httpStats;
			
		}

		inst->authRequest.reqHeader.rawHeader.pData = &inst->authRequest.rawReqHeader;
		inst->authRequest.reqHeader.rawHeader.dataLen = 0;
		inst->authRequest.reqHeader.rawHeader.dataSize = sizeof(inst->authRequest.rawReqHeader);
	}

	inst->activeConnections = 0;
	int status;
	A3brCallback pCallback;
	for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
	{

		inst->connection[i].httpClient.pHost = &configuration->hostname;
		inst->connection[i].httpClient.hostPort = configuration->port;

		//Handles the generation of HTTP requests to be sent to the server.
		switch (inst->connection[i].reqState)
		{

		//Wait for a new request to be placed in the buffer (they get placed here by other blocks in this library).
		case A3BR_REQUEST_ST_IDLE:
			inst->connection[i].httpClient.enable = 1;
			inst->connection[i].ping.IN = 1;

			if (requests->NumberValues > 0)
			{
				//Only proceed with request if authentication parameters are available ("READY").
				if (inst->authState == A3BR_AUTH_ST_READY && inst->connection[i].httpClient.status == ERR_OK)
				{
					BufferCopyItems(requests, 0, 1, &inst->connection[i].currentRequest, 0);
					BufferRemoveTop(requests);
					brsmemset(inst->connection[i].reqData, 0, 1);
					inst->connection[i].reqState = A3BR_REQUEST_ST_GENERATE_STRING;
				}
				else
				{
					break;
				}
			}
			else if (inst->connection[i].ping.Q && inst->connected)
			{
				brsmemset(&inst->connection[i].currentRequest, 0, sizeof(inst->connection[i].currentRequest));
				brsstrcpy(inst->connection[i].currentRequest.uri, "/?json=1");
				inst->connection[i].currentRequest.method = httpMETHOD_GET;

				inst->connection[i].reqState = A3BR_REQUEST_ST_GENERATE_STRING;
			}
			else
			{
				break;
			}

		//Generate the request string.
		case A3BR_REQUEST_ST_GENERATE_STRING:

			if (inst->connection[i].currentRequest.dataType == A3BR_REQ_DATA_TYPE_PARS)
			{
				inst->connection[i].httpClient.pRequestData = &inst->connection[i].reqData;
				UDINT pParameters = &inst->connection[i].reqData;
				brsstrcpy(pParameters, "");
				int parIndex = 0;
				for (; parIndex <= A3BR_MAI_REQUEST_PARS; parIndex++)
				{
					if (brsstrcmp(inst->connection[i].currentRequest.parameters[parIndex].name, "") != 0)
					{
						if (brsstrcmp(pParameters, "") != 0)
						{
							brsstrcat(pParameters, "&");
						}
						brsstrcat(pParameters, inst->connection[i].currentRequest.parameters[parIndex].name);
						brsstrcat(pParameters, "=");
						brsstrcat(pParameters, inst->connection[i].currentRequest.parameters[parIndex].value);
						inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
					}
					else
					{
						break;
					}
				}
				brsstrcpy(inst->connection[i].reqHeader.contentType, "application/x-www-form-urlencoded");
				inst->connection[i].httpClient.requestDataLen = brsstrlen(&inst->connection[i].reqData);
			}
			else if (inst->connection[i].currentRequest.dataType == A3BR_REQ_DATA_TYPE_BLOCK)
			{
				inst->connection[i].httpClient.pRequestData = inst->connection[i].currentRequest.pBlock;
				inst->connection[i].httpClient.requestDataLen = inst->connection[i].currentRequest.szBlock;
				brsstrcpy(inst->connection[i].reqData, "Request data is being handled by an external structure");
				brsstrcpy(inst->connection[i].reqHeader.contentType, "text/plain");
				inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
			}

			//					brsstrcpy(inst->connection[i].reqHeader.protocol, "HTTP/1.1");
			brsstrcpy(inst->connection[i].reqHeader.host, configuration->hostname);

		//Send the HTTP request containing the newly generated string.
		//We're re-checking for correct authentication to handle the case
		//where we need to resend a request that faulted out prior due to
		//authentication issues.
		case A3BR_REQUEST_ST_SEND:
			if (inst->authState == A3BR_AUTH_ST_READY && inst->connection[i].httpClient.status == ERR_OK)
			{
				brsmemset( &inst->connection[i].resHeader, 0, sizeof(inst->connection[i].resHeader) - sizeof( inst->connection[i].resHeader.rawHeader ));
				brsmemset( &inst->connection[i].rawResHeader, 0, sizeof(inst->connection[i].rawResHeader) );
					
				inst->connection[i].httpClient.pRequestHeader = &inst->connection[i].reqHeader;
				inst->connection[i].reqHeader.rawHeader.pData = &inst->connection[i].rawReqHeader;
				inst->connection[i].reqHeader.rawHeader.dataSize = sizeof(inst->connection[i].rawReqHeader);

				brsstrcpy(inst->connection[i].reqHeader.connection, "keep-alive");
				brsstrcpy(inst->connection[i].reqHeader.keepAlive, "timeout=60");

				inst->connection[i].httpClient.pUri = &inst->connection[i].currentRequest.uri;
				inst->connection[i].httpClient.method = inst->connection[i].currentRequest.method;
				inst->connection[i].httpClient.option = httpOPTION_HTTP_11;

				inst->connection[i].httpClient.send = 1;
				inst->connection[i].reqState = A3BR_REQUEST_ST_PROCESS_RESPONSE;
				inst->connection[i].retries = 0;
				
									
			}
			else
			{
				break;
			}
		//Wait for the server to send back the response and process it.
		case A3BR_REQUEST_ST_PROCESS_RESPONSE:

			switch (inst->connection[i].httpClient.httpStatus)
			{
			case 0:
				//Wait here for a non-zero response.
				inst->connection[i].responseTimeout.IN = 1;

				if (inst->connection[i].responseTimeout.Q)
				{
					if (inst->connection[i].retries > 5)
					{
						inst->connection[i].httpClient.send = 0;
						inst->connection[i].httpClient.abort = 1;
						//There was an error of some sort fulfilling the request, proceed to call the errorCallback.
						inst->connection[i].reqState = A3BR_REQUEST_ST_IDLE;
						brsstrcpy(inst->connection[i].resHeader.status, "Timeout while waiting for server response to request");
						if (inst->connection[i].currentRequest.errorCallback)
						{
							pCallback = (A3brCallback)inst->connection[i].currentRequest.errorCallback;
							pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].resHeader, &inst->connection[i].resData);
						}
					}
					else
					{
						if( inst->connection[i].httpClient.status == 0 ){
							inst->connection[i].responseTimeout.IN = 0;
							inst->connection[i].retries++;
							inst->connection[i].httpClient.send = 1;												
						}
						else{
							inst->connection[i].httpClient.abort = 1;						
						}
					}
				}
				break;
			case 200: // HTTP code for "OK"
			case 201: // HTTP code for "Created"
			case 204: // HTTP code for "OK, but no return content"
				//The request was successfully fulfilled, proceed to call successCallback.
				inst->connection[i].reqState = A3BR_REQUEST_ST_IDLE;
				if (inst->connection[i].currentRequest.successCallback)
				{
					pCallback = (A3brCallback)inst->connection[i].currentRequest.successCallback;
					pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].resHeader, &inst->connection[i].resData);
				}
				break;
			case 503:
			case 401: // HTTP code for "Unauthorized"
				//Getting here means that we were successfully authenticated at one point in time, and that the IRC5
				//somehow lost that authentication (maybe is was rebooted). We need to allow the authentication to take
				//place and then let this request run again.
				inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
				break;
			default:
				//There was an error of some sort fulfilling the request, proceed to call the errorCallback.
				inst->connection[i].reqState = A3BR_REQUEST_ST_IDLE;
				if (inst->connection[i].currentRequest.errorCallback)
				{
					pCallback = (A3brCallback)inst->connection[i].currentRequest.errorCallback;
					pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].resHeader, &inst->connection[i].resData);
				}
				break;
			}
			break;
		}

		inst->connection[i].responseTimeout.PT = inst->connection[i].responseTimeout.PT ? inst->connection[i].responseTimeout.PT : 500;
		TON(&inst->connection[i].responseTimeout);
		inst->connection[i].responseTimeout.IN = 0;

		inst->connection[i].ping.PT = inst->connection[i].ping.PT ? inst->connection[i].ping.PT : 30000;
		TON(&inst->connection[i].ping);
		inst->connection[i].ping.IN = 0;

		A3brWebServiceConnection_typ *connection = &inst->connection[i];

		//Handles the authentication of the client with the server.
		switch (inst->authState)
		{

			//Send a connection packet to check the authentication state.
			case A3BR_AUTH_ST_INIT:
				connection->httpClient.option = httpOPTION_HTTP_11;
				connection->httpClient.pHost = &configuration->hostname;
				connection->httpClient.hostPort = configuration->port;
				connection->httpClient.method = httpMETHOD_GET;

				//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
				connection->httpClient.pUri = &inst->authRequest.uri;

				connection->httpClient.pRequestHeader = &inst->authRequest.reqHeader;
				connection->httpClient.pRequestData = &inst->authRequest.reqData;
				connection->httpClient.requestDataLen = 0;

				brsmemset(&inst->auth, 0, sizeof(inst->auth));
				brsstrcpy(inst->auth.userName, configuration->username);
				brsstrcpy(inst->auth.password, configuration->password);
				brsstrcpy(inst->auth.cnonce, "0a4f113b");
				brsstrcpy(inst->authRequest.uri, "/?json=1");

				if( connection->httpClient.phase != httpPHASE_NOT_CONNECTED ){
					connection->httpClient.enable = 0;
				}
				else if(connection->httpClient.status == ERR_FUB_ENABLE_FALSE ){
					connection->httpClient.enable = 1;				
				}
				else{	
					connection->httpClient.send = 1;
					inst->authState = A3BR_AUTH_ST_WAIT_FOR_SERVER;
				}
				break;

			//Wait for the server to respond to the connection packet.
			//The logic is handled outside this state machine because it
			//needs to be checked every time a response is received from
			//the server.
			case A3BR_AUTH_ST_WAIT_FOR_SERVER:
				inst->authTimeout.IN = 1;
				if (inst->authTimeout.Q)
				{
					inst->authTimeout.IN = 0;
					inst->error = 1;
					inst->errorID = A3BR_ERR_AUTH_TIMEOUT;
					brsstrcpy(inst->errorString, "Timeout waiting for server to reply with authentication information");
					inst->authState = A3BR_AUTH_ST_ERROR;
				}
				break;

			//Send a packet with authorization parameters.
			case A3BR_AUTH_ST_AUTHENTICATE:
				connection->httpClient.send = 1;
				
				//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
				connection->httpClient.pUri = &inst->authRequest.uri;

				connection->httpClient.method = httpMETHOD_GET;
			
				connection->httpClient.pRequestHeader = &inst->authRequest.reqHeader;
				connection->httpClient.pRequestData = &inst->authRequest.reqData;
				connection->httpClient.requestDataLen = 0;

				brsmemset( &inst->connection[i].resHeader, 0, sizeof(inst->connection[i].resHeader) - sizeof( inst->connection[i].resHeader.rawHeader ));
				brsmemset( &inst->connection[i].rawResHeader, 0, sizeof(inst->connection[i].rawResHeader) );

				inst->authState = A3BR_AUTH_ST_AUTHENTICATE_SENT;

			case A3BR_AUTH_ST_AUTHENTICATE_SENT:
				//Wait.. here and don't send more packets
				inst->authTimeout.IN = 1;
				if (inst->authTimeout.Q)
				{
					inst->authTimeout.IN = 0;
					inst->error = 1;
					inst->errorID = A3BR_ERR_AUTH_TIMEOUT;
					brsstrcpy(inst->errorString, "Timeout waiting for server to reply with authentication information");
					inst->authState = A3BR_AUTH_ST_ERROR;
				}
			//Authentication parameters are available, append them to the next HTTP request.
			case A3BR_AUTH_ST_READY:

				if (connection->httpClient.send)
				{

					httpRequestHeader_t *pReqHeader = (httpRequestHeader_t *)connection->httpClient.pRequestHeader;

					char *rawReqHeader = pReqHeader->rawHeader.pData;
					if( rawReqHeader ){
						//Clear out the header
						brsmemset(rawReqHeader, 0, sizeof(connection->rawReqHeader));
						//If we have cookies, use them
						if (brsstrlen(inst->auth.httpSession) > 0)
						{
							brsstrcat(rawReqHeader, "Cookie:");
							brsstrcat(rawReqHeader, "-http-session-=");
							brsstrcat(rawReqHeader, inst->auth.httpSession);
							brsstrcat(rawReqHeader, ";");
							//If we have cookies, use them
							if (brsstrlen(inst->auth.ABBCX) > 0)
							{
								brsstrcat(rawReqHeader, "ABBCX=");
								brsstrcat(rawReqHeader, inst->auth.ABBCX);
								brsstrcat(rawReqHeader, ";");
								//brsstrcat(rawReqHeader, ";\r\n");
							}
						}
						//Do the authorization
						else if (brsstrcmp(inst->auth.qop, "auth") == 0)
						{
							STRING nc[8];
							brsitoa(++inst->auth.count, nc);
							brsmemset(inst->auth.nc, 0, sizeof(inst->auth.nc));
							brsmemset(inst->auth.nc, '0', sizeof(inst->auth.nc) - 1);
							brsmemcpy(inst->auth.nc + sizeof(inst->auth.nc) - brsstrlen(nc) - 1, &nc, brsstrlen(nc));

							digestAuth(inst->auth.userName, inst->auth.realm, inst->auth.password, connection->httpClient.method, connection->httpClient.pUri, inst->auth.nonce, inst->auth.nc, inst->auth.cnonce, inst->auth.qop, inst->auth.digest);
							STRING authHeader[3000];
							generateDigestAuthorization(&inst->auth, connection->httpClient.pUri, authHeader);
							brsstrcat(rawReqHeader, authHeader);
						}
						//Log the data
						pReqHeader->rawHeader.dataLen = brsstrlen(rawReqHeader);
						if( pReqHeader->rawHeader.dataLen ==0 ){
							pReqHeader->rawHeader.pData = 0;
							pReqHeader->rawHeader.dataSize = 0;
						}	
					}
				}
				break;

			//If an error occurred during authentication, hang out here until a reset is received.
			case A3BR_AUTH_ST_ERROR:
				if ((inst->reset) && (!inst->_reset))
				{
					inst->error = 0;
					inst->errorID = 0;
					brsstrcpy(inst->errorString, "");
					inst->authState = A3BR_AUTH_ST_INIT;
				}
				break;
		}

		inst->connection[i].sent = inst->connection[i].httpClient.send;
		inst->connection[i].httpClient.httpStatus = 0;
		httpClient(&inst->connection[i].httpClient);

		//Handle the bug for content length == 0
		status = brsatoi(inst->connection[i].resHeader.status);
		if (status != 0 
			&& inst->connection[i].httpClient.status == 65535 
			&& inst->connection[i].httpClient.phase == 2
			&& ((httpResponseHeader_t*)inst->connection[i].httpClient.pResponseHeader)->contentLength == 0
			)
		{
			inst->connection[i].httpClient.abort = 1;
			httpClient(&inst->connection[i].httpClient);			
			inst->connection[i].httpClient.httpStatus = status;
		}
		
		if(inst->connection[i].httpClient.status == 0 && (inst->connection[i].prevClientStatus == 65535 || inst->connection[i].httpClient.send) && status) {
			inst->connection[i].httpClient.httpStatus = status;
		}
		inst->connection[i].prevClientStatus = inst->connection[i].httpClient.status;
		
		inst->connection[i].httpStatus = inst->connection[i].httpClient.httpStatus ? inst->connection[i].httpClient.httpStatus : inst->connection[i].httpStatus;
		inst->connection[i].httpClient.send = 0;
		inst->connection[i].httpClient.abort = 0;
		if (inst->connection[i].httpClient.phase > 1)
		{
			inst->connection[i].connected = 1;
		}
		else if (inst->connection[i].httpClient.phase == 0)
		{
			inst->connection[i].connected = 0;
		}
		if (inst->connection[i].connected)
		{
			inst->activeConnections++;
		}
	}

	inst->authTimeout.PT = 5000;
	TON(&inst->authTimeout);
	inst->authTimeout.IN = 0;

	//Go through all the connection and see if Authentication is good
	// Use MAI_HTTP_CONNECTIONS + 1 to account for the dedicated Auth Client
	for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
	{
		//Handles transitioning to the correct authentication state based on the response from the server.
		//If 200 is returned, this indicates successful authentication. If 401 is returned, this means that
		//the server did not authorize the request, and authentication must be carried out.
		A3brWebServiceConnection_typ *connection = &inst->connection[i];

		if (connection->httpClient.httpStatus != 0)
		{

			UDINT pCookie = ((httpResponseHeader_t *)connection->httpClient.pResponseHeader)->rawHeader.pData;
			while (pCookie)
			{
				pCookie = getCookie(pCookie, inst->auth.httpSession, inst->auth.ABBCX);
			}

			switch (connection->httpClient.httpStatus)
			{
			case 200: // HTTP code for "OK"
			case 201: // HTTP code for "Created"
			case 204: // HTTP code for "No content"
				inst->authState = A3BR_AUTH_ST_READY;
				break;
			case 401: // HTTP code for "Unauthorized"
				//Get the authentication parameters.
				brsmemset(inst->auth.httpSession, 0, sizeof(inst->auth.httpSession));
				brsmemset(inst->auth.ABBCX, 0, sizeof(inst->auth.ABBCX));
				getDigestParameters(((httpResponseHeader_t *)connection->httpClient.pResponseHeader)->rawHeader.pData, inst->auth.realm, inst->auth.qop, inst->auth.nonce, inst->auth.opaque);
				inst->authState = A3BR_AUTH_ST_AUTHENTICATE;
				break;
			case 503:
				//Reset the connection
				//inst->connection[i].httpClient.enable = 0;
				//inst->authState = A3BR_AUTH_ST_INIT;
					inst->authState = A3BR_AUTH_ST_READY;
				break;
			default:
				//Catch errors related to authentication only.
				if (inst->authState < A3BR_AUTH_ST_READY)
				{
					inst->error = 1;
					inst->errorID = A3BR_ERR_HTTP_ERROR;
					inst->authState = A3BR_AUTH_ST_ERROR;
				}
			}
		}
	}

	inst->connected = (inst->authState == A3BR_AUTH_ST_READY);

	inst->_reset = inst->reset;
}
