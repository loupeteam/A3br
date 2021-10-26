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
			inst->connection[i].httpRequest.pUserHeader = &inst->connection[i].reqHeader;
			inst->connection[i].httpRequest.pResponse = &inst->connection[i].resData;
//
//			inst->connection[i].resHeader.rawHeader.pData = &inst->connection[i].rawResHeader;
//			inst->connection[i].resHeader.rawHeader.dataSize = sizeof(inst->connection[i].rawResHeader);
//
//			inst->connection[i].httpClient.pResponseHeader = &inst->connection[i].resHeader;
//			inst->connection[i].httpClient.pResponseData = &inst->connection[i].resData;
//			inst->connection[i].httpClient.responseDataSize = sizeof(inst->connection[i].resData);
//			inst->connection[i].httpClient.pStatistics = &inst->connection[i].httpStats;
			
		}
	}

	inst->activeConnections = 0;
	int status;
	A3brCallback pCallback;
	for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
	{
		
		brsstrcpy(inst->connection[i].httpClient.configuration.hostname, configuration->hostname);
		inst->connection[i].httpClient.configuration.port = configuration->port;
//		inst->connection[i].httpClient.configuration.localIPAddress = ...;
//		inst->connection[i].httpClient.configuration.localPort = ;
		
		if (inst->connection[i].httpClient.connected) {
			inst->connection[i].httpRequest.ident = inst->connection[i].httpClient.ident;
		}

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
				if (inst->authState == A3BR_AUTH_ST_READY && inst->connection[i].httpClient.connected && !inst->connection[i].httpRequest.busy)
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
				inst->connection[i].httpRequest.pContent = &inst->connection[i].reqData;
				inst->connection[i].httpRequest.contentLength = brsstrlen(&inst->connection[i].reqData);					
				brsstrcpy(inst->connection[i].httpRequest.contentType, "application/x-www-form-urlencoded");
			}
			else if (inst->connection[i].currentRequest.dataType == A3BR_REQ_DATA_TYPE_BLOCK)
			{
				inst->connection[i].httpRequest.pContent = inst->connection[i].currentRequest.pBlock;
				inst->connection[i].httpRequest.contentLength = inst->connection[i].currentRequest.szBlock;
				brsstrcpy(inst->connection[i].reqData, "Request data is being handled by an external structure");
				brsstrcpy(inst->connection[i].httpRequest.contentType, "text/plain");
				inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
			}

		//Send the HTTP request containing the newly generated string.
		//We're re-checking for correct authentication to handle the case
		//where we need to resend a request that faulted out prior due to
		//authentication issues.
		case A3BR_REQUEST_ST_SEND:
			if (inst->authState == A3BR_AUTH_ST_READY && !inst->connection[i].httpRequest.busy)
			{					
//				brsstrcpy(inst->connection[i].reqHeader[0].name, "Connection");
//				brsstrcpy(inst->connection[i].reqHeader[0].value, "keep-alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].name, "Keep-Alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].value, "timeout=60");	
//				brsstrcpy(inst->connection[i].reqHeader[2].name, "Via");
//				brsstrcpy(inst->connection[i].reqHeader[2].value, "HTTP/1.1");
				inst->connection[i].httpRequest.numUserHeaders = 0;

				brsstrcpy(inst->connection[i].httpRequest.uri, inst->connection[i].currentRequest.uri);
				inst->connection[i].httpRequest.method = inst->connection[i].currentRequest.method;

				inst->connection[i].httpRequest.send = 1;
				inst->connection[i].reqState = A3BR_REQUEST_ST_PROCESS_RESPONSE;
				inst->connection[i].retries = 0;												
			}
			else
			{
				break;
			}
			
		//Wait for the server to send back the response and process it.
		case A3BR_REQUEST_ST_PROCESS_RESPONSE:

			switch (inst->connection[i].httpRequest.header.status)
			{
			case 0:
				//Wait here for a non-zero response.
				inst->connection[i].responseTimeout.IN = 1;

				if (inst->connection[i].responseTimeout.Q)
				{
					if (inst->connection[i].retries > 5)
					{
						inst->connection[i].httpRequest.send = 0;
						//There was an error of some sort fulfilling the request, proceed to call the errorCallback.
						inst->connection[i].reqState = A3BR_REQUEST_ST_IDLE;
						// XTODO
						brsstrcpy(inst->connection[i].httpRequest.header.status, "Timeout while waiting for server response to request");
						if (inst->connection[i].currentRequest.errorCallback)
						{
							pCallback = (A3brCallback)inst->connection[i].currentRequest.errorCallback;
							pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData);
						}
					}
					else
					{
						// XTODO: does it need to reset send for a scan first? 
						if( inst->connection[i].httpRequest.done ){
							inst->connection[i].responseTimeout.IN = 0;
							inst->connection[i].retries++;
							inst->connection[i].httpRequest.send = 1;												
						}
						else{
							inst->connection[i].httpRequest.send = 0;						
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
					pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData);
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
					pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData);
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
				
				brsmemset(&inst->auth, 0, sizeof(inst->auth));
				brsstrcpy(inst->auth.userName, configuration->username);
				brsstrcpy(inst->auth.password, configuration->password);
				brsstrcpy(inst->auth.cnonce, "0a4f113b");
				brsstrcpy(inst->authRequest.uri, "/?json=1");
				
				//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
				brsstrcpy(inst->connection[i].httpRequest.uri, inst->authRequest.uri);
				
				inst->connection[i].httpRequest.method = HTTP_METHOD_GET;
//				brsstrcpy(inst->connection[i].reqHeader[0].name, "Connection");
//				brsstrcpy(inst->connection[i].reqHeader[0].value, "keep-alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].name, "Keep-Alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].value, "timeout=60");	
//				brsstrcpy(inst->connection[i].reqHeader[2].name, "Via");
//				brsstrcpy(inst->connection[i].reqHeader[2].value, "HTTP/1.1");
				
				inst->connection[i].httpRequest.numUserHeaders = 0;
	
				connection->httpRequest.send = 1;
				inst->authState = A3BR_AUTH_ST_WAIT_FOR_SERVER;
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
				connection->httpRequest.send = 1;
				
				//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
				brsstrcpy(connection->httpRequest.uri, inst->authRequest.uri);

				connection->httpRequest.method = HTTP_METHOD_GET;
			
//				brsstrcpy(inst->connection[i].reqHeader[0].name, "Connection");
//				brsstrcpy(inst->connection[i].reqHeader[0].value, "keep-alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].name, "Keep-Alive");
//				brsstrcpy(inst->connection[i].reqHeader[1].value, "timeout=60");	
//				brsstrcpy(inst->connection[i].reqHeader[2].name, "Via");
//				brsstrcpy(inst->connection[i].reqHeader[2].value, "HTTP/1.1");
				inst->connection[i].httpRequest.numUserHeaders = 0;

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

				if (connection->httpRequest.send)
				{
					//Clear out the header
					brsmemset(inst->connection[i].reqHeader, 0, sizeof(inst->connection[i].reqHeader));
					//If we have cookies, use them
					if (brsstrlen(inst->auth.httpSession) > 0)
					{
						brsstrcpy(inst->connection[i].reqHeader[0].name, "Cookie");
						brsstrcpy(inst->connection[i].reqHeader[0].value, "-http-session-=");
						brsstrcat(inst->connection[i].reqHeader[0].value, inst->auth.httpSession);
						brsstrcat(inst->connection[i].reqHeader[0].value, ";");
						//If we have cookies, use them
						if (brsstrlen(inst->auth.ABBCX) > 0)
						{
							brsstrcat(inst->connection[i].reqHeader[0].value, "ABBCX=");
							brsstrcat(inst->connection[i].reqHeader[0].value, inst->auth.ABBCX);
							brsstrcat(inst->connection[i].reqHeader[0].value, ";");
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

						digestAuth(inst->auth.userName, inst->auth.realm, inst->auth.password, connection->httpRequest.method, connection->httpRequest.uri, inst->auth.nonce, inst->auth.nc, inst->auth.cnonce, inst->auth.qop, inst->auth.digest);
						STRING authHeader[3000];
						generateDigestAuthorization(&inst->auth, connection->httpRequest.uri, authHeader);
						brsstrcpy(inst->connection[i].reqHeader[0].name, "Authorization");
						brsstrcpy(inst->connection[i].reqHeader[0].value, authHeader);
						inst->connection[i].httpRequest.numUserHeaders = 1;
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

		inst->connection[i].sent = inst->connection[i].httpRequest.send;
		HttpClient(&inst->connection[i].httpClient);
		HttpRequest(&inst->connection[i].httpRequest);


		
		inst->connection[i].httpStatus = inst->connection[i].httpRequest.header.status;
		inst->connection[i].httpRequest.send = 0;

		inst->connection[i].connected = inst->connection[i].httpClient.connected;
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

		if (connection->httpRequest.done)
		{					
			for (int counter = 0; counter < HTTP_MAI_NUM_HEADER_LINES; counter++) 
			{
				getCookie(&connection->httpRequest.header.lines[counter], inst->auth.httpSession, inst->auth.ABBCX);
			}

			switch (connection->httpRequest.header.status)
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
					int headerIndex = HttpgetHeaderIndex(&connection->httpRequest.header.lines, "www-authenticate", 0);
					//	for (int counter = 0; counter < HTTP_MAI_NUM_HEADER_LINES; counter++) {
					//		if (!brsstrcmp(connection->httpRequest.header.lines[counter].name, "WWW-Authenticate")) {
					getDigestParameters(&connection->httpRequest.header.lines[headerIndex].value, inst->auth.realm, inst->auth.qop, inst->auth.nonce, inst->auth.opaque);
					//		}
					//	}
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
