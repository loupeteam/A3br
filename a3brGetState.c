//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description:
//		Read high level state information from the IRC.
//********************************************************************************

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
#include "A3br.h"
#include "A3brCommon.h"

#ifdef __cplusplus
	};
#endif

#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wpointer-sign"
//#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
//#pragma GCC diagnostic ignored "-Wreturn-type"

// ctrlexecstate: Rapid execution state {running | stopped}
// cycle: Current run mode {forever | asis | once | oncedone}
// ctrlstate: The controller state {init | motoron | motoroff | guardstop | emergencystop | emergencystopreset | sysfail}
// opmode: The operation mode {INIT | AUTO_CH | MANF_CH | MANR | MANF | AUTO | UNDEF}

// Callback function used to stored JSON string data in user defined data structure.
signed short A3brGetRapidParse(struct A3brGetState *data, jsmn_callback_data *data2) {
	if(data2->Levels == 4) {
		// Identify the correct structure to use based on the active request.  
		if(!brsstrcmp(&data2->Structure[3], &"_embedded")) {
			if(!brsstrcmp(&data2->Structure[2], &"_state")) {
				if(!brsstrcmp(&data2->Structure[1], &"opmode")) {
					if(!brsstrcmp(&data2->Structure[0], &"INIT")) {
						data->operationalMode = A3BR_OP_MODE_INIT;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"AUTO_CH")) {
						data->operationalMode = A3BR_OP_MODE_AUTO_CHANGE_REQ;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"MANF_CH")) {
						data->operationalMode = A3BR_OP_MODE_MANUAL_CHANGE_REQ;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"MANR")) {
						data->operationalMode = A3BR_OP_MODE_MANUAL_LOW_SPEED;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"MANF")) {
						data->operationalMode = A3BR_OP_MODE_MANUAL_FULL_SPEED;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"AUTO")) {
						data->operationalMode = A3BR_OP_MODE_AUTO;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"UNDEF")) {
						data->operationalMode = A3BR_OP_MODE_UNDEFINED;
					}				
				}
				else if(!brsstrcmp(&data2->Structure[1], &"ctrlstate")) {
					if(!brsstrcmp(&data2->Structure[0], &"init")) {
						data->controlState = A3BR_CTRL_ST_INIT;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"motoron")) {
						data->controlState = A3BR_CTRL_ST_ON;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"motoroff")) {
						data->controlState = A3BR_CTRL_ST_OFF;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"guardstop")) {
						data->controlState = A3BR_CTRL_ST_GUARDSTOP;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"emergencystop")) {
						data->controlState = A3BR_CTRL_ST_ESTOP;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"emergencystopreset")) {
						data->controlState = A3BR_CTRL_ST_ESTOP_RESET;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"sysfail")) {
						data->controlState = A3BR_CTRL_ST_SYS_FAIL;
					}
					else {
						data->controlState = A3BR_CTRL_ST_UNDEFINED;
					}
				}				
				else if(!brsstrcmp(&data2->Structure[1], &"ctrlexecstate")) {
					if(!brsstrcmp(&data2->Structure[0], &"running")) {
						data->rapidExecutionState = A3BR_RAPID_EXEC_ST_RUNNING;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"stopped")) {
						data->rapidExecutionState = A3BR_RAPID_EXEC_ST_STOPPED;
					}
					else {
						data->rapidExecutionState = A3BR_RAPID_EXEC_ST_UNDEFINED;
					}
				}				
				else if(!brsstrcmp(&data2->Structure[1], &"cycle")) {
					if(!brsstrcmp(&data2->Structure[0], &"forever")) {
						data->rapidCycleState = A3BR_RAPID_CYCLE_ST_FOREVER;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"asis")) {
						data->rapidCycleState = A3BR_RAPID_CYCLE_ST_ASIS;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"once")) {
						data->rapidCycleState = A3BR_RAPID_CYCLE_ST_ONCE;
					}
					else if(!brsstrcmp(&data2->Structure[0], &"oncedone")) {
						data->rapidCycleState = A3BR_RAPID_CYCLE_ST_ONCEDONE;
					}
					else {
						data->rapidCycleState = A3BR_RAPID_CYCLE_ST_UNDEFINED;
					}
				}								
			}
		}
	}
	return 0;
}

//This gets called by A3brWebService if the HTTP request fails in any way.
void A3brGetStateErrorCallback( struct A3brGetState* inst, httpResponseHeader_t * header, unsigned char * data){
	inst->internal.error = 1;
	inst->internal.done = 0;
	inst->internal.busy = 0;
	inst->internal.errorID = A3BR_ERR_HTTP_ERROR;
	brsstrcpy(inst->internal.errorString, header->status);
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brGetStateSuccessCallback( struct A3brGetState* inst, httpResponseHeader_t * header, unsigned char * data){
	
	// Declare the data, tokens, and parser
	jsmn_parser parser;
	jsmntok_t tokens[32];

	// Clear all variables
	brsmemset(&parser, 0, sizeof(parser));
	brsmemset(&tokens, 0, sizeof(tokens));

	JsmnInit(&parser);

	// Assign user defined callback function & data
	parser.callback.pFunction= (UDINT)A3brGetRapidParse;
	parser.callback.pUserData = (UDINT)inst;
	
	// Parse message
	int returnVar = JsmnParse((UDINT)&parser, (UDINT)data, header->contentLength, (UDINT)&tokens, sizeof(tokens)/sizeof(tokens[0]));
	
	// Important: don't update the busy, done, and error outputs here, because there are more requests to be processed. 
}

//This gets called by A3brWebService once the last HTTP request has completed successfully. 
void A3brGetStateFinalSuccessCallback( struct A3brGetState* inst, httpResponseHeader_t * header, unsigned char * data){
	
	// Declare the data, tokens, and parser
	jsmn_parser parser;
	jsmntok_t tokens[32];

	// Clear all variables
	brsmemset(&parser, 0, sizeof(parser));
	brsmemset(&tokens, 0, sizeof(tokens));

	JsmnInit(&parser);

	// Assign user defined callback function & data
	parser.callback.pFunction= (UDINT)A3brGetRapidParse;
	parser.callback.pUserData = (UDINT)inst;
	
	// Parse message
	int returnVar = JsmnParse((UDINT)&parser, (UDINT)data, header->contentLength, (UDINT)&tokens, sizeof(tokens)/sizeof(tokens[0]));
	
	// Since this is the last request, these values can now be correctly updated. 
	inst->internal.error = 0;
	inst->internal.done = 1;
	inst->internal.busy = 0;

}

void A3brGetState(struct A3brGetState* inst){

	A3brWebServiceLink_typ *connection = inst->ident;
	if( !connection ){
		inst->error = 1;
		inst->errorID = A3BR_ERR_INVALID_IDENT;
		return;
	}
	
	if( ((inst->execute && !inst->internal._cmd) ||	(connection->stateRequest > inst->internal.stateRequest)) && !inst->internal.busy ){
		inst->internal._cmd = 1;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;

		//Create and populate first new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_GET; 
		request.errorCallback = &A3brGetStateErrorCallback;
		request.successCallback = &A3brGetStateSuccessCallback;
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsstrcpy( request.uri, "/rw/rapid/execution");
		brsstrcat( request.uri, "?json=1");
					
		//Pass first request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);

		//Create and populate second new request.
		brsstrcpy( request.uri, "/rw/panel/ctrlstate");
		brsstrcat( request.uri, "?json=1");
				
		//Pass second request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);

		//Create and populate third new request.
		request.successCallback = &A3brGetStateFinalSuccessCallback;
		brsstrcpy( request.uri, "/rw/panel/opmode");
		brsstrcat( request.uri, "?json=1");
		
		//Pass third request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);
		
		inst->internal.stateRequest = clock_ms();
	}
	
	inst->busy = inst->internal.busy;
	inst->done = inst->internal.done;
	inst->error = inst->internal.error;
	inst->errorID = inst->internal.errorID;
	brsstrcpy(inst->errorString, inst->internal.errorString);
	
	if( !inst->execute ){
		inst->internal._cmd = 0;
		inst->rapidExecutionState = A3BR_RAPID_EXEC_ST_UNDEFINED;
		inst->rapidCycleState = A3BR_RAPID_CYCLE_ST_UNDEFINED;
		inst->operationalMode = A3BR_OP_MODE_UNDEFINED;
		inst->controlState = A3BR_CTRL_ST_UNDEFINED;
		inst->internal.busy = 0;
		inst->internal.done = 0;
		inst->internal.error = 0;
		inst->internal.errorID = 0;
		brsstrcpy(inst->internal.errorString, "");
	}	
	
}

