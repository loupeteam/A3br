//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description:
//		Write a value to a data symbol signal on the IRC.
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

//lvalue : logical signal value (Digital/Analog value) (The lvalue is NOT used for mode: invert and toggle)
//mode : Write mode, identifying type of value to write {value | invert | pulse | toggle | delay}
//value : Write supplied value.
//invert: Inverts the signal.
//pulse : Pulse the output according to parameter arguments.
//toggle: Pulse by toggling current signal value.
//	delay : Write supplied value using "queued delayed" mode.
//Delay : Delay time before activation, in ms
//	Pulses : Number of pulses
//	ActivePulse : Active pulse length, in ms
//	PassivePulse : Passive pulse length, in ms
//userlog : Log changes on controller {true | false), set if the setting shall be logged as Event log. Default value is 'false'

//This gets called by A3brWebService if the HTTP request fails in any way.
void A3brSetSymbolErrorCallback( struct A3brSetSymbol* inst, LLHttpHeader_typ * header, unsigned char * data, A3BR_API_VERSION_enum apiVersion){
	inst->internal.error = 1;
	inst->internal.done = 0;
	inst->internal.busy = 0;
	inst->internal.errorID = A3BR_ERR_HTTP_ERROR;
	STRING tempString[20];
	brsitoa(header->status, &tempString);
	brsstrcpy(inst->internal.errorString, tempString);
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brSetSymbolSuccessCallback( struct A3brSetSymbol* inst, LLHttpHeader_typ * header, unsigned char * data, A3BR_API_VERSION_enum apiVersion){
	inst->internal.error = 0;
	inst->internal.done = 1;
	inst->internal.busy = 0;
}

void A3brSetSymbol(struct A3brSetSymbol* inst){
	
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
		request.method = LLHTTP_METHOD_POST; 
		brsstrcpy( request.uri, "/rw/rapid/symbol/data/");
		brsstrcat( request.uri, inst->pSignal );
		brsstrcat( request.uri, "?action=set&json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsstrcpy( request.parameters[0].name, "value" );
		if(inst->typeValue == A3BR_VAR_TYPE_BOOL) {	
			BOOL *pValue = (BOOL*)inst->pValue;	
			if (*pValue == 1) {
				brsstrcpy( request.parameters[0].value, "TRUE" );
			} else {
				brsstrcpy( request.parameters[0].value, "FALSE" );
			}
		} else if(inst->typeValue == A3BR_VAR_TYPE_INT) {	
			INT *pValue = (INT*)inst->pValue;
			STRING value[10];
			brsitoa( *pValue, value );		
			brsstrcpy( request.parameters[0].value, value );			
		} else if(inst->typeValue == A3BR_VAR_TYPE_REAL) {	
			REAL *pValue = (REAL*)inst->pValue;
			STRING value[80];
			brsftoa( *pValue, value );		
			brsstrcpy( request.parameters[0].value, value );				
		} else if(inst->typeValue == A3BR_VAR_TYPE_STRING) {
			brsstrcpy(&request.parameters[0].value, &"\"");
			brsstrcat(&request.parameters[0].value, inst->pValue);
			brsstrcat(&request.parameters[0].value, &"\"");
		}
		request.errorCallback = &A3brSetSymbolErrorCallback;
		request.successCallback = &A3brSetSymbolSuccessCallback;
		
		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);					
	}
	
	inst->busy = inst->internal.busy;
	inst->done = inst->internal.done;
	inst->error = inst->internal.error;
	inst->errorID = inst->internal.errorID;
	brsstrcpy(inst->errorString, inst->internal.errorString);
	
	if( !inst->execute ){
		inst->internal._cmd = 0;	
		inst->internal.busy = 0;
		inst->internal.done = 0;
		inst->internal.error = 0;
		inst->internal.errorID = 0;
		brsstrcpy(inst->internal.errorString, "");
	}	
	
}


