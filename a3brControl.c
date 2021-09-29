//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
// Description:
//		Send high level commands to the IRC to control robot execution state.
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

//This gets called by A3brWebService if the HTTP request fails in any way.
void A3brControlErrorCallback( struct A3brControl* inst, httpResponseHeader_t * header, unsigned char * data){
	inst->internal.error = 1;
	inst->internal.done = 0;
	inst->internal.busy = 0;
	inst->internal.errorID = A3BR_ERR_HTTP_ERROR;
	brsstrcpy(inst->internal.errorString, header->status);
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brControlSuccessCallback( struct A3brControl* inst, httpResponseHeader_t * header, unsigned char * data){
	
	inst->internal.error = 0;
	inst->internal.done = 1;
	inst->internal.busy = 0;
}

void A3brControl(struct A3brControl* inst){
	
	A3brWebServiceLink_typ *connection = inst->ident;
	if( !connection ){
		inst->error = 1;
		inst->errorID = A3BR_ERR_INVALID_IDENT;
		brsstrcpy(inst->errorString, "Invalid ident (check connect block)");
		return;
	}

	//This command applies power to the motors.
	if( inst->powerOn && !inst->internal._powerOn ){
		inst->internal._powerOn = inst->powerOn;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_POST; 
		brsstrcpy( request.uri, "rw/panel/ctrlstate" );
		brsstrcat( request.uri, "?action=setctrlstate&json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsstrcpy( request.parameters[0].name, "ctrl-state" );
		brsstrcpy( request.parameters[0].value, "motoron" );
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request );						
		connection->stateRequest = clock_ms();
	}
	else if( !inst->powerOn ){
		inst->internal._powerOn = 0;
	}
	
	//This command removes power from the motors.
	if( inst->powerOff && !inst->internal._powerOff ){
		inst->internal._powerOff = inst->powerOff;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_POST; 
		brsstrcpy( request.uri, "rw/panel/ctrlstate" );
		brsstrcat( request.uri, "?action=setctrlstate&json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsstrcpy( request.parameters[0].name, "ctrl-state" );
		brsstrcpy( request.parameters[0].value, "motoroff" );
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request );						
		connection->stateRequest = clock_ms();
	}
	else if( !inst->powerOff ){
		inst->internal._powerOff = 0;
	}
	
	//This command will start the RAPID program execution on the IRC.
	if( inst->start && !inst->internal._start ){
		inst->internal._start = 1;
		
		//Validate input arguments.
		if(inst->cycle != A3BR_RAPID_CYCLE_ST_ONCE && inst->cycle != A3BR_RAPID_CYCLE_ST_FOREVER && inst->cycle != A3BR_RAPID_CYCLE_ST_ASIS){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_CYCLE;
			brsstrcpy(inst->internal.errorString, "Invalid cycle type (must be once, forever, or as is)");
			return;
		}
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_POST;
		brsstrcpy( request.uri, "/rw/rapid/execution" );
		brsstrcat( request.uri, "?action=start&json=1" );
		
		//Pass in all required parameters for the start command.
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;
		brsstrcpy( request.parameters[0].name, "cycle" );
		if(inst->cycle == A3BR_RAPID_CYCLE_ST_ONCE)
			brsstrcpy( request.parameters[0].value, "once" );
		else if(inst->cycle == A3BR_RAPID_CYCLE_ST_FOREVER)
			brsstrcpy( request.parameters[0].value, "forever" );
		else if(inst->cycle == A3BR_RAPID_CYCLE_ST_ASIS)
			brsstrcpy( request.parameters[0].value, "asis" );
		//All other parameters are hard coded for now.
		brsstrcpy( request.parameters[1].name, "regain" );
		brsstrcpy( request.parameters[1].value, "continue" );
		brsstrcpy( request.parameters[2].name, "execmode" );
		brsstrcpy( request.parameters[2].value, "continue" );				
		brsstrcpy( request.parameters[3].name, "condition" );
		brsstrcpy( request.parameters[3].value, "none" );
		brsstrcpy( request.parameters[4].name, "stopatbp" );
		brsstrcpy( request.parameters[4].value, "disabled" );
		brsstrcpy( request.parameters[5].name, "alltaskbytsp" );
		brsstrcpy( request.parameters[5].value, "false" );

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);
		connection->stateRequest = clock_ms();
	}
	else if( !inst->start ){
		inst->internal._start = 0;
	}
	
	//This command will stop (i.e. interrupt) current RAPID
	//program execution on the IRC.
	if( inst->stop && !inst->internal._stop ){
		inst->internal._stop = 1;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_POST;
		brsstrcpy( request.uri, "/rw/rapid/execution");
		brsstrcat( request.uri, "?action=stop&json=1");	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);					
		connection->stateRequest = clock_ms();		
	}
	else if( !inst->stop ){
		inst->internal._stop = 0;
	}
	
	//This command will reset the program pointer to its default starting location.
	if( inst->reset && !inst->internal._reset ){
		inst->internal._reset= 1;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Create and populate new request.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.method = httpMETHOD_POST; 
		brsstrcpy( request.uri, "/rw/rapid/execution");
		brsstrcat( request.uri, "?action=resetpp&json=1");	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;

		//Pass request to A3brWebService for processing.
		BufferAddToBottom( &connection->requestBuffer, &request);
		connection->stateRequest = clock_ms();
	}
	else if( !inst->reset ){
		inst->internal._reset= 0;
	}
	
	if( inst->requestMastership > inst->internal._requestMastership ){
		inst->internal._requestMastership = inst->requestMastership;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Initialize request structure.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;
		
		//Create a folder to hold the new program files.	
		request.method = httpMETHOD_POST; 						
		brsstrcpy( request.uri, "/rw/mastership" );
		brsstrcat( request.uri, "?action=request" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsmemset(&request.parameters, 0, sizeof(request.parameters));
		BufferAddToBottom( &connection->requestBuffer, &request );			
	}
	else if( !inst->requestMastership ){
		inst->internal._requestMastership = 0;
	}	
	
	if( inst->releaseMastership > inst->internal._releaseMastership ){
		inst->internal._releaseMastership = inst->releaseMastership;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Initialize request structure.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.errorCallback = &A3brControlErrorCallback;
		request.successCallback = &A3brControlSuccessCallback;
		
		//Create a folder to hold the new program files.	
		request.method = httpMETHOD_POST; 						
		brsstrcpy( request.uri, "/rw/mastership" );
		brsstrcat( request.uri, "?action=release&json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsmemset(&request.parameters, 0, sizeof(request.parameters));
		BufferAddToBottom( &connection->requestBuffer, &request );			
	}
	else if( !inst->releaseMastership ){
		inst->internal._releaseMastership = 0;
	}
	
	inst->busy = inst->internal.busy;
	inst->done = inst->internal.done;
	inst->error = inst->internal.error;
	inst->errorID = inst->internal.errorID;
	brsstrcpy(inst->errorString, inst->internal.errorString);
	
	//Reset internal vars and errors if no commands are active.
	if(!inst->powerOn && !inst->powerOff && !inst->reset && !inst->start && !inst->stop && !inst->requestMastership && !inst->releaseMastership){	
		inst->internal.busy = 0;
		inst->internal.done = 0;
		inst->internal.error = 0;
		inst->internal.errorID = 0;
		brsstrcpy(inst->internal.errorString, "");
	}			
}


