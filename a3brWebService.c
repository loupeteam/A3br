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
		inst->internal.api.apiVersion = inst->configuration.apiVersion;
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
			inst->connection[i].httpRequest.responseSize = sizeof(inst->connection[i].resData);
		}
	}

	inst->activeConnections = 0;
	int status;
	A3brCallback pCallback;
	INT tempStatus;
	for (i = 0; i < MAX_HTTP_CONNECTIONS; i++)
	{
		
		brsstrcpy(inst->connection[i].httpClient.hostname, configuration->hostname);
		if (configuration->apiVersion == A3BR_API_VERSION_2) {
			inst->connection[i].httpClient.https = 1;
		}
		inst->connection[i].httpClient.port = configuration->port;
		
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
				inst->connection[i].currentRequest.method = LLHTTP_METHOD_GET;

				inst->connection[i].reqState = A3BR_REQUEST_ST_GENERATE_STRING;
			}
			else
			{
				break;
			}

		//Generate the request string.
		case A3BR_REQUEST_ST_GENERATE_STRING:
				
			//Clear out the header
			brsmemset(inst->connection[i].reqHeader, 0, sizeof(inst->connection[i].reqHeader));
			addHeaderLine(&inst->connection[i].reqHeader, "connection", "keep-alive");
			addHeaderLine(&inst->connection[i].reqHeader, "keep-alive", "timeout=60");
			if (configuration->apiVersion == A3BR_API_VERSION_2) {
				addHeaderLine(&inst->connection[i].reqHeader, "accept", "application/hal+json;v=2.0");
			}

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
				switch(configuration->apiVersion) {
					case A3BR_API_VERSION_1:
						addHeaderLine(&inst->connection[i].reqHeader, "content-type", "application/x-www-form-urlencoded");
						break;
					case A3BR_API_VERSION_2:
						addHeaderLine(&inst->connection[i].reqHeader, "content-type", "application/x-www-form-urlencoded;v=2.0");
						break;
				}
				inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
			}
			else if (inst->connection[i].currentRequest.dataType == A3BR_REQ_DATA_TYPE_BLOCK)
			{
				inst->connection[i].httpRequest.pContent = inst->connection[i].currentRequest.pBlock;
				inst->connection[i].httpRequest.contentLength = inst->connection[i].currentRequest.szBlock;
				brsstrcpy(inst->connection[i].reqData, "Request data is being handled by an external structure");
				switch(configuration->apiVersion) {
					case A3BR_API_VERSION_1:
						addHeaderLine(&inst->connection[i].reqHeader, "content-type", "text/plain");
						break;
					case A3BR_API_VERSION_2:
						addHeaderLine(&inst->connection[i].reqHeader, "content-type", "text/plain;v=2.0");
						break;
				}
				inst->connection[i].reqState = A3BR_REQUEST_ST_SEND;
			}

		//Send the HTTP request containing the newly generated string.
		//We're re-checking for correct authentication to handle the case
		//where we need to resend a request that faulted out prior due to
		//authentication issues.
		case A3BR_REQUEST_ST_SEND:
			if (inst->authState == A3BR_AUTH_ST_READY && !inst->connection[i].httpRequest.busy)
			{					
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
				tempStatus = inst->connection[i].httpRequest.done ? inst->connection[i].httpRequest.header.status : 0;		
				switch (tempStatus)
				{
					case 0:
						//Wait here for a non-zero response.
						inst->connection[i].responseTimeout.IN = 1;

						if (inst->connection[i].responseTimeout.Q)
						{
							inst->connection[i].responseTimeout.IN = 0;
							if (inst->connection[i].retries > 5)
							{
								//There was an error of some sort fulfilling the request, proceed to call the errorCallback.
								inst->connection[i].reqState = A3BR_REQUEST_ST_IDLE;
								// XTODO: fix feedback. 
							//	brsstrcpy(inst->connection[i].httpRequest.header.status, "Timeout while waiting for server response to request");
								if (inst->connection[i].currentRequest.errorCallback)
								{
									pCallback = (A3brCallback)inst->connection[i].currentRequest.errorCallback;
									pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData, configuration->apiVersion);
								}
							}
							else
							{
								inst->connection[i].retries++;
								inst->connection[i].httpRequest.send = 1;
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
							pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData, configuration->apiVersion);
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
							pCallback(inst->connection[i].currentRequest.self, &inst->connection[i].httpRequest.header, &inst->connection[i].resData, configuration->apiVersion);
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
				// Wait for the client block to be connected.
				if (connection->httpRequest.ident != 0) {				
					brsmemset(&inst->auth, 0, sizeof(inst->auth));
					brsstrcpy(inst->auth.userName, configuration->username);
					brsstrcpy(inst->auth.password, configuration->password);
					brsstrcpy(inst->auth.cnonce, "0a4f113b");
					brsstrcpy(inst->authRequest.uri, "/?json=1");
					
					//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
					brsstrcpy(inst->connection[i].httpRequest.uri, inst->authRequest.uri);
					
					inst->connection[i].httpRequest.method = LLHTTP_METHOD_GET;	
					
					//Clear out the header
					brsmemset(inst->connection[i].reqHeader, 0, sizeof(inst->connection[i].reqHeader));
					inst->connection[i].httpRequest.numUserHeaders = 0;
		
					connection->httpRequest.send = 1;
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
				connection->httpRequest.send = 1;
				
				//Use separate data for the auth so as to not overwrite a valid request.. We should get back to it once auth is ok
				brsstrcpy(connection->httpRequest.uri, inst->authRequest.uri);
				inst->connection[i].httpRequest.method = LLHTTP_METHOD_GET;

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

				if (connection->httpRequest.send && inst->connection[i].retries == 0)
				{
					//If we have cookies, use them
					if (brsstrlen(inst->auth.httpSession) > 0)
					{					
						STRING headerValue[LLHTTP_MAX_LEN_HEADER_VALUE];
						//	brsstrcpy(inst->connection[i].reqHeader[0].name, "Cookie");
						brsstrcpy(headerValue, "-http-session-=");
						brsstrcat(headerValue, inst->auth.httpSession);
						brsstrcat(headerValue, ";");
						//If we have cookies, use them
						if (brsstrlen(inst->auth.ABBCX) > 0)
						{
							brsstrcat(headerValue, "ABBCX=");
							brsstrcat(headerValue, inst->auth.ABBCX);
							brsstrcat(headerValue, ";");
						}
						addHeaderLine(&inst->connection[i].reqHeader, "Cookie", headerValue);
						inst->connection[i].httpRequest.numUserHeaders = getNumHeaders(&inst->connection[i].reqHeader);
					}
					//Do the authorization for V1.0 API (digest auth). 
					else if ((configuration->apiVersion == A3BR_API_VERSION_1) && (brsstrcmp(inst->auth.qop, "auth") == 0))
					{
						STRING nc[8];
						brsitoa(++inst->auth.count, nc);
						brsmemset(inst->auth.nc, 0, sizeof(inst->auth.nc));
						brsmemset(inst->auth.nc, '0', sizeof(inst->auth.nc) - 1);
						brsmemcpy(inst->auth.nc + sizeof(inst->auth.nc) - brsstrlen(nc) - 1, &nc, brsstrlen(nc));

						digestAuth(inst->auth.userName, inst->auth.realm, inst->auth.password, connection->httpRequest.method, connection->httpRequest.uri, inst->auth.nonce, inst->auth.nc, inst->auth.cnonce, inst->auth.qop, inst->auth.digest);
						STRING authHeader[3000];
						generateDigestAuthorization(&inst->auth, connection->httpRequest.uri, authHeader);
						addHeaderLine(&inst->connection[i].reqHeader, "Authorization", authHeader);
						inst->connection[i].httpRequest.numUserHeaders = getNumHeaders(&inst->connection[i].reqHeader);
					}
					//Do the authorization for V2.0 API (basic auth).
					else if (configuration->apiVersion == A3BR_API_VERSION_2)
					{
						STRING authHeader[3000];
						generateBasicAuthorization(&inst->auth, authHeader);
						addHeaderLine(&inst->connection[i].reqHeader, "Authorization", authHeader);
						addHeaderLine(&inst->connection[i].reqHeader, "accept", "application/hal+json;v=2.0");
						inst->connection[i].httpRequest.numUserHeaders = getNumHeaders(&inst->connection[i].reqHeader);
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

		//Go through all the connection and see if Authentication is good
		//Use MAI_HTTP_CONNECTIONS + 1 to account for the dedicated Auth Client
		//Handles transitioning to the correct authentication state based on the response from the server.
		//If 200 is returned, this indicates successful authentication. If 401 is returned, this means that
		//the server did not authorize the request, and authentication must be carried out.
		if (connection->httpRequest.done)
		{								
			// Search for cookies in the response, and record them if found (for future authentication). 
			for (int i = 0; i <= LLHTTP_MAI_NUM_HEADER_LINES; i++) {
				getCookie(&connection->httpRequest.header.lines[i], inst->auth.httpSession, inst->auth.ABBCX);
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
					//Only need to perform digest auth for v1.0; v2.0 uses basic auth instead since it's encrypted. 
					if (configuration->apiVersion == A3BR_API_VERSION_1) {
						int headerIndex = LLHttpgetHeaderIndex(&connection->httpRequest.header.lines, "www-authenticate", 0);
						getDigestParameters(&connection->httpRequest.header.lines[headerIndex].value, inst->auth.realm, inst->auth.qop, inst->auth.nonce, inst->auth.opaque);
					}
					inst->authState = A3BR_AUTH_ST_AUTHENTICATE;
					break;
				case 503:
					//Reset the connection
					//inst->connection[i].httpClient.enable = 0;
					//inst->authState = A3BR_AUTH_ST_INIT;
					brsmemset(inst->auth.httpSession, 0, sizeof(inst->auth.httpSession));
					brsmemset(inst->auth.ABBCX, 0, sizeof(inst->auth.ABBCX));
					inst->connection[i].httpClient.abort = 1;
					inst->authState = A3BR_AUTH_ST_INIT;
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
		
		LLHttpClient(&inst->connection[i].httpClient);
		LLHttpRequest(&inst->connection[i].httpRequest);
		
		inst->connection[i].httpRequest.send = 0;
		inst->connection[i].httpClient.abort = 0;

		inst->connection[i].httpStatus = inst->connection[i].httpRequest.header.status;

		inst->connection[i].connected = inst->connection[i].httpClient.connected;
		if (inst->connection[i].connected)
		{
			inst->activeConnections++;
		}	
	}
	
	inst->authTimeout.PT = 5000;
	TON(&inst->authTimeout);
	inst->authTimeout.IN = 0;

	inst->connected = (inst->authState == A3BR_AUTH_ST_READY);

	inst->_reset = inst->reset;
}

void addHeaderLine(LLHttpHeaderField_typ * headerLine, char * name, char * value) {
	for (int i = 0; i <= LLHTTP_MAI_NUM_HEADER_LINES; i++) {
		if (!brsstrcmp(headerLine[i].name, "")) {
			brsstrcpy(headerLine[i].name, name);
			brsstrcpy(headerLine[i].value, value);
			return;			
		}
	}
}

int getNumHeaders(LLHttpHeaderField_typ * headerLine) {
	for (int i = 0; i <= LLHTTP_MAI_NUM_HEADER_LINES; i++) {
		if (!brsstrcmp(headerLine[i].name, "")) {
			return i;
		}
	}
}