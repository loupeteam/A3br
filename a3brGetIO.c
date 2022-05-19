//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description: 
//		Read the value of an IO signal from the IRC.
//********************************************************************************

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
#include "bur.h"
#include "A3br.h"
#include "A3brCommon.h"

#ifdef __cplusplus
	};
#endif

#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wpointer-sign"
//#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
//#pragma GCC diagnostic ignored "-Wreturn-type"

// Callback function used to store JSON string data in user defined data structure.
signed short A3brGetIOParse(struct A3brGetIO *data, jsmn_callback_data *data2) {
	if(data2->Levels == 4) {
		// Identify the correct structure to use based on the active request.  
		if(!brsstrcmp(&data2->Structure[3], &"_embedded")) {
			if(!brsstrcmp(&data2->Structure[2], &"_state")) {
				if(!brsstrcmp(&data2->Structure[1], &"lvalue")) {
					data->value = brsatoi(data2->Value);
				}
			}
		}
	}
	return 0;
}

//This gets called by A3brWebService if the HTTP request fails in any way.
void A3brGetIOErrorCallback( struct A3brGetIO* inst, LLHttpHeader_typ * header, unsigned char * data){
	inst->internal.error = 1;
	inst->internal.done = 0;
	inst->internal.busy = 0;
	inst->internal.errorID = A3BR_ERR_HTTP_ERROR;
	STRING tempString[20];
	brsitoa(header->status, &tempString);
	brsstrcpy(inst->internal.errorString, tempString);
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brGetIOSuccessCallback( struct A3brGetIO* inst, LLHttpHeader_typ * header, unsigned char * data){
	
	// Declare the data, tokens, and parser
	jsmn_parser parser;
	jsmntok_t tokens[32];

	// Clear all variables
	brsmemset(&parser, 0, sizeof(parser));
	brsmemset(&tokens, 0, sizeof(tokens));

	JsmnInit(&parser);

	// Assign user defined callback function & data
	parser.callback.pFunction= (UDINT)A3brGetIOParse;
	parser.callback.pUserData = (UDINT)inst;
		
	// Parse message
	int returnVar = JsmnParse((UDINT)&parser, (UDINT)data, header->contentLength, (UDINT)&tokens, sizeof(tokens)/sizeof(tokens[0]));

	inst->internal.error = 0;
	inst->internal.done = 1;
	inst->internal.busy = 0;

}

void A3brGetIO(struct A3brGetIO* inst){
	
	A3brWebServiceLink_typ *connection = inst->ident;
	if( !connection ){
		inst->error = 1;
		inst->errorID = A3BR_ERR_INVALID_IDENT;
		return;
	}

	if( inst->execute && !inst->internal._cmd && !inst->internal.busy ){
		inst->internal._cmd = 1;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = LLHTTP_METHOD_GET; 
		brsstrcpy( request.uri, "/rw/iosystem/signals/");
		brsstrcat( request.uri, inst->pSignal);
		brsstrcat( request.uri, "?json=1");	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		request.successCallback = &A3brGetIOSuccessCallback;

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);
	}
	
	inst->busy = inst->internal.busy;
	inst->done = inst->internal.done;
	inst->error = inst->internal.error;
	inst->errorID = inst->internal.errorID;
	brsstrcpy(inst->errorString, inst->internal.errorString);
	
	if( !inst->execute){
		inst->internal._cmd = 0;	
		inst->internal.busy = 0;
		inst->internal.done = 0;
		inst->internal.error = 0;
		inst->internal.errorID = 0;
		brsstrcpy(inst->internal.errorString, "");
	}	
	
}

