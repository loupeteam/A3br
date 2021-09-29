(*//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
//*********************************************************************************)

TYPE
	A3brDigestAuthentication_typ : 	STRUCT 
		count : USINT;
		password : STRING[80];
		userName : STRING[80];
		realm : STRING[1000];
		qop : STRING[30];
		nonce : STRING[100];
		opaque : STRING[100];
		cnonce : STRING[80];
		ABBCX : STRING[20];
		httpSession : STRING[100];
		nc : STRING[8];
		digest : STRING[32];
		requestId : STRING[200];
	END_STRUCT;
	A3brWebServiceCfg_typ : 	STRUCT 
		hostname : STRING[80];
		port : UINT;
		username : STRING[80];
		password : STRING[80];
		maxSessions : USINT;
	END_STRUCT;
	A3brWebServiceRequest_typ : 	STRUCT 
		self : UDINT;
		method : UDINT;
		successCallback : UDINT;
		errorCallback : UDINT;
		dataType : A3BR_REQ_DATA_TYPE_enum;
		uri : ARRAY[0..A3BR_STR_ROWS4]OF STRING[80];
		pBlock : UDINT;
		szBlock : UDINT;
		parameters : ARRAY[0..A3BR_MAI_REQUEST_PARS]OF A3brWebServiceRequestPar_typ;
	END_STRUCT;
	A3brWebServiceRequestPar_typ : 	STRUCT 
		name : STRING[80];
		value : STRING[80];
	END_STRUCT;
	A3brWebServiceInternal_typ : 	STRUCT 
		api : A3brWebServiceLink_typ;
		_reset : BOOL;
		session : ARRAY[0..2]OF A3brWebServiceSession_typ;
	END_STRUCT;
	A3brWebServiceSession_typ : 	STRUCT 
		authTimeout : TON;
		authState : A3BR_AUTH_ST_enum;
		auth : A3brDigestAuthentication_typ;
		authRequest : A3brWebServiceAuthRequest_typ;
		connection : ARRAY[0..0]OF A3brWebServiceConnection_typ;
		reset : {REDUND_UNREPLICABLE} BOOL;
		connected : {REDUND_UNREPLICABLE} BOOL;
		activeConnections : {REDUND_UNREPLICABLE} UINT;
		inited : {REDUND_UNREPLICABLE} BOOL;
		error : USINT;
		errorID : USINT;
		errorString : STRING[80];
		_reset : {REDUND_UNREPLICABLE} BOOL;
	END_STRUCT;
	A3brWebServiceConnection_typ : 	STRUCT 
		reqState : A3BR_REQUEST_ST_enum;
		httpClient : httpClient;
		rawReqHeader : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		rawResHeader : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		reqData : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		resData : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		reqHeader : httpRequestHeader_t;
		resHeader : httpResponseHeader_t;
		retries : UINT;
		responseTimeout : TON;
		currentRequest : A3brWebServiceRequest_typ;
		connected : BOOL;
		ping : TON;
		sent : BOOL;
		httpStats : httpStatistics_t;
		httpStatus : UINT;
		prevClientStatus : UINT;
	END_STRUCT;
	A3brWebServiceAuthRequest_typ : 	STRUCT 
		uri : STRING[80];
		rawReqHeader : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		reqData : ARRAY[0..A3BR_STR_ROWS20]OF STRING[80];
		reqHeader : httpRequestHeader_t;
	END_STRUCT;
	A3brWebServiceLink_typ : 	STRUCT 
		requestBuffer : Buffer_typ;
		stateRequest : UDINT;
	END_STRUCT;
	A3brProgramControlInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_loadProgram : BOOL;
		_loadModule : BOOL;
	END_STRUCT;
	A3brControlInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_powerOn : BOOL;
		_powerOff : BOOL;
		_start : BOOL;
		_stop : BOOL;
		_continue : BOOL;
		_reset : BOOL;
		_requestMastership : BOOL;
		_releaseMastership : BOOL;
	END_STRUCT;
	A3brGetStateInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_cmd : BOOL;
		stateRequest : UDINT;
	END_STRUCT;
	A3brSetSymbolInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_cmd : BOOL;
	END_STRUCT;
	A3brSetIOInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_cmd : BOOL;
	END_STRUCT;
	A3brIOWatchIOInternal_typ : 	STRUCT 
		oValue : INT;
		value : INT;
		pValue : REFERENCE TO INT;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_STRUCT;
	A3brIOWatchInternal_typ : 	STRUCT 
		readIndex : USINT;
		refresh : TON;
		pIO : UDINT;
	END_STRUCT;
	A3brGetSymbolInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_cmd : BOOL;
		data : ARRAY[0..79]OF USINT;
	END_STRUCT;
	A3brGetIOInternal_typ : 	STRUCT 
		done : BOOL;
		busy : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
		_cmd : BOOL;
	END_STRUCT;
	A3BR_AUTH_ST_enum : 
		(
		A3BR_AUTH_ST_INIT := 0,
		A3BR_AUTH_ST_WAIT_FOR_SERVER := 1,
		A3BR_AUTH_ST_AUTHENTICATE := 2,
		A3BR_AUTH_ST_AUTHENTICATE_SENT := 3,
		A3BR_AUTH_ST_READY := 4,
		A3BR_AUTH_ST_ERROR := 500
		);
	A3BR_REQUEST_ST_enum : 
		(
		A3BR_REQUEST_ST_IDLE := 0,
		A3BR_REQUEST_ST_GENERATE_STRING := 1,
		A3BR_REQUEST_ST_SEND := 2,
		A3BR_REQUEST_ST_PROCESS_RESPONSE := 3
		);
	A3BR_VAR_TYPE_enum : 
		(
		A3BR_VAR_TYPE_BOOL,
		A3BR_VAR_TYPE_INT,
		A3BR_VAR_TYPE_REAL,
		A3BR_VAR_TYPE_STRING
		);
END_TYPE

(*States*)

TYPE
	A3BR_RAPID_EXEC_ST_enum : 
		(
		A3BR_RAPID_EXEC_ST_UNDEFINED,
		A3BR_RAPID_EXEC_ST_RUNNING, (*ABB constant "running"*)
		A3BR_RAPID_EXEC_ST_STOPPED (*ABB constant "stopped"*)
		);
	A3BR_RAPID_CYCLE_ST_enum : 
		(
		A3BR_RAPID_CYCLE_ST_UNDEFINED,
		A3BR_RAPID_CYCLE_ST_FOREVER, (*ABB constant "forever": this repeats the program forever*)
		A3BR_RAPID_CYCLE_ST_ASIS, (*ABB constant "asis": this keeps the current cycle state on the IRC5*)
		A3BR_RAPID_CYCLE_ST_ONCE, (*ABB constant "once": this runs the program a single time*)
		A3BR_RAPID_CYCLE_ST_ONCEDONE (*ABB constant "oncedone"*)
		);
	A3BR_CTRL_ST_enum : 
		(
		A3BR_CTRL_ST_UNDEFINED,
		A3BR_CTRL_ST_INIT, (*ABB constant "init": The robot is starting up. It will shift to state motors off when it has started.*)
		A3BR_CTRL_ST_ON, (*ABB constant "motoron": The robot is ready to move, either by jogging or by running programs.*)
		A3BR_CTRL_ST_OFF, (*ABB constant "motoroff": The robot is in a standby state where there is no power to the robot's motors. The state has to be shifted to motors on before the robot can move.*)
		A3BR_CTRL_ST_GUARDSTOP, (*ABB constant "guardstop": The robot is stopped because the safety runchain is opened. For instance, a door to the robot's cell might be open.*)
		A3BR_CTRL_ST_ESTOP, (*ABB constant "emergencystop": The robot is stopped because emergency stop was activated.*)
		A3BR_CTRL_ST_ESTOP_RESET, (*ABB constant "emergencystopreset": The robot is ready to leave emergency stop state. The emergency stop is no longer activated, but the state transition isn't yet confirmed.*)
		A3BR_CTRL_ST_SYS_FAIL (*ABB constant "sysfail": The robot is in a system failure state. Restart required.*)
		);
	A3BR_OP_MODE_enum : 
		(
		A3BR_OP_MODE_UNDEFINED, (*ABB constant "UNDEF": Undefined*)
		A3BR_OP_MODE_INIT, (*ABB constant "INIT": State init*)
		A3BR_OP_MODE_AUTO_CHANGE_REQ, (*ABB constant "AUTO_CH": State change request for automatic mode*)
		A3BR_OP_MODE_MANUAL_CHANGE_REQ, (*ABB constant "MANF_CH": State change request for manual mode & full speed*)
		A3BR_OP_MODE_MANUAL_LOW_SPEED, (*ABB constant "MANR": State manual mode & reduced speed*)
		A3BR_OP_MODE_MANUAL_FULL_SPEED, (*ABB constant "MANF": State manual mode & full speed*)
		A3BR_OP_MODE_AUTO (*ABB constant "AUTO": State automatic mode*)
		);
	A3BR_ERR_enum : 
		(
		A3BR_ERR_INVALID_IDENT := 56000,
		A3BR_ERR_AUTH_TIMEOUT := 56001,
		A3BR_ERR_RESPONSE_TIMEOUT := 56002,
		A3BR_ERR_HTTP_ERROR := 56003,
		A3BR_ERR_INSUFFICIENT_SIGNALS := 56004,
		A3BR_ERR_INVALID_CYCLE := 56005,
		A3BR_ERR_INVALID_NAME := 56006,
		A3BR_ERR_INVALID_DATA_ADDRESS := 56007,
		A3BR_ERR_INVALID_DATA_SIZE := 56008
		);
	A3BR_REQ_DATA_TYPE_enum : 
		(
		A3BR_REQ_DATA_TYPE_PARS := 0,
		A3BR_REQ_DATA_TYPE_BLOCK := 1
		);
END_TYPE
