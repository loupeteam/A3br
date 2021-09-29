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

void A3brCreateModuleSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data);
void A3brCreateFolderSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data);
void A3brProgramControlErrorCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data);
void A3brProgramControlSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data);


//This gets called by A3brWebService if the HTTP request fails in any way.
void A3brProgramControlErrorCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data){
	inst->internal.error = 1;
	inst->internal.done = 0;
	inst->internal.busy = 0;
	inst->internal.errorID = A3BR_ERR_HTTP_ERROR;
	brsstrcpy(inst->internal.errorString, header->status);
}

//This gets called by A3brWebService once the HTTP request has completed successfully. 
void A3brProgramControlSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data){
	
	inst->internal.error = 0;
	inst->internal.done = 1;
	inst->internal.busy = 0;
}

void A3brCreateFolderSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data){

	A3brWebServiceLink_typ *connection = inst->ident;

	A3brWebServiceRequest_typ request;
	brsmemset(&request, 0, sizeof(request));
	request.self = inst;
	request.errorCallback = &A3brProgramControlErrorCallback;
	request.successCallback = &A3brCreateModuleSuccessCallback;	

	//Create the RAPID module file.
	request.method = httpMETHOD_PUT; 						
	brsstrcpy( request.uri, "/fileservice/$home/" );
	brsstrcat( request.uri, inst->pModuleName );
	brsstrcat( request.uri, "/" );
	brsstrcat( request.uri, inst->pModuleName );
	brsstrcat( request.uri, ".mod" );
	brsstrcat( request.uri, "?json=1" );	
	request.dataType = A3BR_REQ_DATA_TYPE_BLOCK;
	request.pBlock = inst->pData;
	request.szBlock = inst->szData;
	BufferAddToBottom( &connection->requestBuffer, &request );				

}

void A3brCreateModuleSuccessCallback( struct A3brProgramControl* inst, httpResponseHeader_t * header, unsigned char * data){

	A3brWebServiceLink_typ *connection = inst->ident;

	A3brWebServiceRequest_typ request;
	brsmemset(&request, 0, sizeof(request));
	request.self = inst;
	request.errorCallback = &A3brProgramControlErrorCallback;
	request.successCallback = &A3brProgramControlSuccessCallback;	

	//Load the newly created module.
	request.method = httpMETHOD_POST; 						
	brsstrcpy( request.uri, "/rw/rapid/tasks/" );
	brsstrcat( request.uri, inst->pTaskName );
	brsstrcat( request.uri, "?action=loadmod&json=1" );	
	request.dataType = A3BR_REQ_DATA_TYPE_PARS;
	brsmemset(&request.parameters, 0, sizeof(request.parameters));
	brsstrcpy( request.parameters[0].name, "modulepath" );
	brsstrcpy( request.parameters[0].value, "$home/" );		
	brsstrcat( request.parameters[0].value, inst->pModuleName );
	brsstrcat( request.parameters[0].value, "/");
	brsstrcat( request.parameters[0].value, inst->pModuleName );
	brsstrcat( request.parameters[0].value, ".mod" );
	brsstrcpy( request.parameters[1].name, "replace" );
	brsstrcpy( request.parameters[1].value, "true" );	
	BufferAddToBottom( &connection->requestBuffer, &request );	
}

void A3brProgramControl(struct A3brProgramControl* inst){
	
	A3brWebServiceLink_typ *connection = inst->ident;
	if( !connection ){
		inst->error = 1;
		inst->errorID = A3BR_ERR_INVALID_IDENT;
		brsstrcpy(inst->errorString, "Invalid ident (check connect block)");
		return;
	}
	
	if( inst->loadProgram > inst->internal._loadProgram ){
		inst->internal._loadProgram = inst->loadProgram;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Check input arguments for validity. 
		if(!brsstrcmp(inst->pProgramName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid program name");
			return;
		}
		else if(!brsstrcmp(inst->pModuleName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid module name");
			return;
		}
		else if(!brsstrcmp(inst->pTaskName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid task name");
			return;
		}
		else if(inst->pData == 0){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_DATA_ADDRESS;
			brsstrcpy(inst->internal.errorString, "Invalid data address (pData)");
			return;
		}
		else if(inst->szData == 0){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_DATA_SIZE;
			brsstrcpy(inst->internal.errorString, "Invalid data size (szData)");
			return;
		}
		
		//Initialize request structure.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.errorCallback = &A3brProgramControlErrorCallback;
		request.successCallback = &A3brProgramControlSuccessCallback;
		
		//Create a folder to hold the new program files.	
		request.method = httpMETHOD_POST; 						
		brsstrcpy( request.uri, "/fileservice/$home/" );
		brsstrcat( request.uri, "?json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsmemset(&request.parameters, 0, sizeof(request.parameters));
		brsstrcpy(request.parameters[0].name, "fs-newname");
		brsstrcpy(request.parameters[0].value, inst->pProgramName);
		brsstrcpy(request.parameters[1].name, "fs-action");
		brsstrcpy(request.parameters[1].value, "create");
		BufferAddToBottom( &connection->requestBuffer, &request );	
		
		//Create the RAPID module file.
		request.method = httpMETHOD_PUT; 						
		brsstrcpy( request.uri, "/fileservice/$home/" );
		brsstrcat( request.uri, inst->pProgramName );
		brsstrcat( request.uri, "/" );
		brsstrcat( request.uri, inst->pModuleName );
		brsstrcat( request.uri, ".mod" );
		brsstrcat( request.uri, "?json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_BLOCK;
		request.pBlock = inst->pData;
		request.szBlock = inst->szData;
		BufferAddToBottom( &connection->requestBuffer, &request );
		
		//Create the .pgf file.
		request.method = httpMETHOD_PUT; 						
		brsstrcpy( request.uri, "/fileservice/$home/" );
		brsstrcat( request.uri, inst->pProgramName );
		brsstrcat( request.uri, "/" );
		brsstrcat( request.uri, inst->pProgramName );
		brsstrcat( request.uri, ".pgf" );
		brsstrcat( request.uri, "?json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_BLOCK;
		STRING pgfData[200];
		brsstrcpy(&pgfData, &"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n<Program>\r\n    <Module>");
		brsstrcat(&pgfData, inst->pModuleName);
		brsstrcat(&pgfData, &".mod</Module>\r\n</Program>");
		request.pBlock = pgfData;
		request.szBlock = brsstrlen(pgfData);
		BufferAddToBottom( &connection->requestBuffer, &request );				
		
		//Load the newly created program.
		request.method = httpMETHOD_POST; 						
		brsstrcpy( request.uri, "/rw/rapid/tasks/" );
		brsstrcat( request.uri, inst->pTaskName );
		brsstrcat( request.uri, "/program" );
		brsstrcat( request.uri, "?action=loadprog&json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsmemset(&request.parameters, 0, sizeof(request.parameters));
		brsstrcpy( request.parameters[0].name, "progpath" );
		brsstrcpy( request.parameters[0].value, "$home/" );		
		brsstrcat( request.parameters[0].value, inst->pProgramName );
		brsstrcat( request.parameters[0].value, "/" );
		brsstrcat( request.parameters[0].value, inst->pProgramName );
		brsstrcat( request.parameters[0].value, ".pgf" );
		BufferAddToBottom( &connection->requestBuffer, &request );	
		
	}
	else if( !inst->loadProgram ){
		inst->internal._loadProgram = 0;
	}	
	
	if( inst->loadModule > inst->internal._loadModule ){
		inst->internal._loadModule = inst->loadModule;
		
		//Report busy status during operation.
		inst->internal.error = 0;
		inst->internal.done = 0;
		inst->internal.busy = 1;
		
		//Check input arguments for validity. 
		if(!brsstrcmp(inst->pProgramName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid program name");
			return;
		}
		else if(!brsstrcmp(inst->pModuleName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid module name");
			return;
		}
		else if(!brsstrcmp(inst->pTaskName, "")){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_NAME;
			brsstrcpy(inst->internal.errorString, "Invalid task name");
			return;
		}
		else if(inst->pData == 0){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_DATA_ADDRESS;
			brsstrcpy(inst->internal.errorString, "Invalid data address (pData)");
			return;
		}
		else if(inst->szData == 0){
			inst->internal.error = 1;
			inst->internal.errorID = A3BR_ERR_INVALID_DATA_SIZE;
			brsstrcpy(inst->internal.errorString, "Invalid data size (szData)");
			return;
		}
		
		//Initialize request structure.
		A3brWebServiceRequest_typ request;
		brsmemset(&request, 0, sizeof(request));
		request.self = inst;
		request.errorCallback = &A3brCreateFolderSuccessCallback;//Can fail if the folder exists.. so lets just try to keep going
		request.successCallback = &A3brCreateFolderSuccessCallback;	
		
		//Create a folder to hold the new module files.	
		request.method = httpMETHOD_POST; 						
		brsstrcpy( request.uri, "/fileservice/$home/" );
		brsstrcat( request.uri, "?json=1" );	
		request.dataType = A3BR_REQ_DATA_TYPE_PARS;
		brsmemset(&request.parameters, 0, sizeof(request.parameters));
		brsstrcpy(request.parameters[0].name, "fs-newname");
		brsstrcpy(request.parameters[0].value, inst->pModuleName);
		brsstrcpy(request.parameters[1].name, "fs-action");
		brsstrcpy(request.parameters[1].value, "create");
		BufferAddToBottom( &connection->requestBuffer, &request );
				
	}
	else if( !inst->loadModule ){
		inst->internal._loadModule = 0;
	}		
	
	inst->busy = inst->internal.busy;
	inst->done = inst->internal.done;
	inst->error = inst->internal.error;
	inst->errorID = inst->internal.errorID;
	brsstrcpy(inst->errorString, inst->internal.errorString);
	
	//Reset internal vars and errors if no commands are active.
	if( !inst->loadProgram && !inst->loadModule ){	
		inst->internal.busy = 0;
		inst->internal.done = 0;
		inst->internal.error = 0;
		inst->internal.errorID = 0;
		brsstrcpy(inst->internal.errorString, "");
	}			
}


