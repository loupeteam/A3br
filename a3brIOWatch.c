//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description:
//		Set up cyclic read/write watch for a list of IO signals on the IRC. 
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

// Callback function used to stored JSON string data in user defined data structure.
signed short A3brParseIOParse(struct A3brIOWatchIOInternal_typ *data, jsmn_callback_data *data2) {
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
void A3brIOWatchErrorCallback( struct A3brIOWatchIOInternal_typ* inst, LLHttpHeader_typ * header, unsigned char * data){
	inst->error= 1;
	inst->busy= 0;
	inst->errorID = A3BR_ERR_HTTP_ERROR;
	STRING tempString[20];
	brsitoa(header->status, &tempString);
	brsstrcpy(inst->errorString, tempString);	
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brIOWatchSuccessCallback( struct A3brIOWatchIOInternal_typ* inst, LLHttpHeader_typ * header, unsigned char * data){
	
	inst->error= 0;
	inst->busy= 0;
	
	// Declare the data, tokens, and parser
	jsmn_parser parser;
	jsmntok_t tokens[32];

	// Clear all variables
	brsmemset(&parser, 0, sizeof(parser));
	brsmemset(&tokens, 0, sizeof(tokens));

	JsmnInit(&parser);

	// Assign user defined callback function & data
	parser.callback.pFunction= (UDINT)A3brParseIOParse;
	parser.callback.pUserData = (UDINT)inst;
	
	inst->oValue = inst->value;
		
	// Parse message
	int returnVar = JsmnParse((UDINT)&parser, (UDINT)data, header->contentLength, (UDINT)&tokens, sizeof(tokens)/sizeof(tokens[0]));

	if(inst->oValue != inst->value){
		*inst->pValue = inst->value;		
		inst->oValue = inst->value;
	}	
	
}

void A3brIOWatch(struct A3brIOWatch* inst){
	
	if(inst->enable){
	
		A3brWebServiceLink_typ *connection = inst->ident;
		if( !connection ){
			inst->error = 1;
			inst->errorID = A3BR_ERR_INVALID_IDENT;
			return;
		}
	
		//Require that at least one signal is specified.
		if( inst->numSignals == 0){
			inst->error = 1;
			inst->errorID = A3BR_ERR_INSUFFICIENT_SIGNALS;
			return;
		}
	
		//Allocate memory for the signals that are to be watched.
		if( !inst->internal.pIO ){ 
			TMP_alloc( sizeof(A3brIOWatchIOInternal_typ) * inst->numSignals , (void**)&inst->internal.pIO );	
		}
	
		A3brIOWatchIOInternal_typ *pIOPoints = inst->internal.pIO;
		int i, maxreads;
	
		inst->internal.refresh.PT = inst->refresh;		
		TON(&inst->internal.refresh);
		inst->internal.refresh.IN = 1;
		
		//Handle reading io points every time the internal timer elapses. 
		//This creates a series of requests for all specified points. 
		if( inst->internal.refresh.Q ){
			inst->internal.refresh.IN = 0;	
			
			//Initialize request structure.
			A3brWebServiceRequest_typ request;
			brsmemset(&request, 0, sizeof(request));
			request.self = inst;
			request.method = LLHTTP_METHOD_GET; 
			request.errorCallback = &A3brIOWatchErrorCallback;
			request.successCallback = &A3brIOWatchSuccessCallback;
	
			maxreads = inst->maxCycleReads > 0 ? inst->maxCycleReads : inst->numSignals;
	
			for( i=0; (i < maxreads); i++ ){
				if( brsstrcmp( &inst->pSignals[inst->internal.readIndex], "") && !pIOPoints[inst->internal.readIndex].busy ){
					//Populate request elements that are specific to the targeted IO signal. 
					request.self = &(pIOPoints[inst->internal.readIndex]);
					pIOPoints[inst->internal.readIndex].pValue = &(inst->pValues[inst->internal.readIndex]);
					pIOPoints[inst->internal.readIndex].busy = 1;
					brsstrcpy( request.uri, "/rw/iosystem/signals/" );
					brsstrcat( request.uri, inst->pSignals[inst->internal.readIndex] );
					brsstrcat( request.uri, "?json=1" );
					request.dataType = A3BR_REQ_DATA_TYPE_PARS;
					BufferAddToBottom( &connection->requestBuffer, &request);
				}
	
				inst->internal.readIndex++;
				if( inst->internal.readIndex == inst->numSignals ){
					inst->internal.readIndex=0;
				}
				
			}
		}
		
		//Handle writing to the io points.
		for( i=0; (i < inst->numSignals); i++ ){
			if( brsstrcmp(&inst->pSignals[i], "") ){
				if( pIOPoints[i].oValue != inst->pValues[i] ){
					pIOPoints[i].oValue = inst->pValues[i];			
					pIOPoints[i].value = inst->pValues[i];			
					
					//Create and populate new request.
					A3brWebServiceRequest_typ request;
					brsmemset(&request, 0, sizeof(request));	
					request.self = &(pIOPoints[i]);
					request.method = LLHTTP_METHOD_POST; 
					request.errorCallback = &A3brIOWatchErrorCallback;
//					request.successCallback = &A3brIOWatchSuccessCallback;
					brsstrcpy( request.uri, "/rw/iosystem/signals/");
					brsstrcat( request.uri, inst->pSignals[i]);
					brsstrcat( request.uri, "?action=set&json=1");
				
					//Create the value that will be written to the IO point.
					request.dataType = A3BR_REQ_DATA_TYPE_PARS;
					brsstrcpy( request.parameters[0].name, "lvalue");
					STRING lvalue[10];
					brsitoa( inst->pValues[i], lvalue);		
					brsstrcpy( request.parameters[0].value, lvalue);
					
					//Pass request to A3brWebService for processing.
					BufferAddToTop( &connection->requestBuffer, &request);					
				}
			}		
		}
		
		//Extract overall FUB status from individual signal statuses.
		inst->error = 0;
		inst->errorID = 0;
		brsstrcpy(inst->errorString, "");
		for( i=0; (i < inst->numSignals); i++ ){
			if(pIOPoints[i].error){				
				inst->error = 1;
				inst->errorID = pIOPoints[i].errorID;
				brsstrcpy(inst->errorString, pIOPoints[i].errorString);
			}
		}		
	}
	else{
		inst->error = 0;
		inst->errorID = 0;
		brsstrcpy(inst->errorString, "");
		//Deallocate memory.
		if( inst->internal.pIO != 0 ){ 
			TMP_free( sizeof(A3brIOWatchIOInternal_typ) * inst->numSignals , inst->internal.pIO );
			inst->internal.pIO = 0;	
		}
	}
}

