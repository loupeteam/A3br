(*
* File: constats.var
* Copyright (c) 2023 Loupe
* https://loupe.team
* 
* This file is part of A3BR, licensed under the MIT License.
* 
*)

FUNCTION_BLOCK A3brWebService (*Establish the initial connection to the IRC, and handle channeling of all HTTP traffic back and forth.*)
	VAR_INPUT
		enable : BOOL;
		reset : BOOL;
		configuration : A3brWebServiceCfg_typ;
	END_VAR
	VAR_OUTPUT
		ident : UDINT;
		connected : BOOL;
		activeSessions : UINT;
		activeConnections : UINT;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brWebServiceInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brControl (*Send high level commands to the IRC to control robot execution state.*)
	VAR_INPUT
		ident : UDINT;
		powerOn : BOOL;
		powerOff : BOOL;
		start : BOOL;
		stop : BOOL;
		reset : BOOL;
		requestMastership : BOOL;
		releaseMastership : BOOL;
		cycle : A3BR_RAPID_CYCLE_ST_enum;
	END_VAR
	VAR_OUTPUT
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brControlInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brProgramControl (*Send high level commands to the IRC to control robot execution state.*)
	VAR_INPUT
		ident : UDINT;
		loadProgram : BOOL;
		loadModule : BOOL;
		pProgramName : {REDUND_UNREPLICABLE} UDINT;
		pModuleName : UDINT;
		pTaskName : UDINT;
		pData : UDINT;
		szData : UDINT;
	END_VAR
	VAR_OUTPUT
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brProgramControlInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brSetIO (*Write a value to an IO signal on the IRC.*)
	VAR_INPUT
		ident : UDINT;
		execute : BOOL;
		pSignal : REFERENCE TO STRING[A3BR_IO_STR_LEN];
		mode : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		value : {REDUND_UNREPLICABLE} INT; (*not implemented*)
		delay : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		pulses : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		activePulse : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		passivePulse : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
	END_VAR
	VAR_OUTPUT
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brSetIOInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brGetIO (*Read the value of an IO signal from the IRC.*)
	VAR_INPUT
		ident : UDINT;
		execute : BOOL;
		pSignal : REFERENCE TO STRING[A3BR_IO_STR_LEN];
	END_VAR
	VAR_OUTPUT
		value : INT;
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brGetIOInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brSetSymbol (*Write a value to an IO signal on the IRC.*)
	VAR_INPUT
		ident : UDINT;
		execute : BOOL;
		pSignal : REFERENCE TO STRING[A3BR_SYMBOL_STR_LEN];
		pValue : UDINT;
		szValue : UDINT;
		typeValue : A3BR_VAR_TYPE_enum;
		mode : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		delay : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		pulses : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		activePulse : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
		passivePulse : {REDUND_UNREPLICABLE} UDINT; (*not implemented*)
	END_VAR
	VAR_OUTPUT
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brSetSymbolInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brGetSymbol (*Read the value of an IO signal from the IRC.*)
	VAR_INPUT
		ident : UDINT;
		execute : BOOL;
		pSignal : REFERENCE TO STRING[A3BR_SYMBOL_STR_LEN];
		pValue : UDINT;
		szValue : UDINT;
		typeValue : A3BR_VAR_TYPE_enum;
	END_VAR
	VAR_OUTPUT
		busy : BOOL;
		done : BOOL;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brGetSymbolInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brGetState (*Read high level state information from the IRC.*)
	VAR_INPUT
		ident : UDINT;
		enable : BOOL;
		mode : A3BR_GET_STATE_MODE_enum;
		cycleTime : UDINT;
	END_VAR
	VAR_OUTPUT
		rapidExecutionState : A3BR_RAPID_EXEC_ST_enum;
		rapidCycleState : A3BR_RAPID_CYCLE_ST_enum;
		controlState : A3BR_CTRL_ST_enum;
		operationalMode : A3BR_OP_MODE_enum;
		busy : BOOL;
		done : BOOL;
		successCount : UDINT;
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brGetStateInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK A3brIOWatch (*Set up cyclic read/write watch for a list of IO signals on the IRC. *)
	VAR_INPUT
		enable : BOOL;
		ident : UDINT;
		pSignals : REFERENCE TO ARRAY[0..0] OF STRING[A3BR_IO_STR_LEN];
		pValues : REFERENCE TO INT;
		numSignals : USINT;
		maxCycleReads : USINT;
		refresh : TIME;
	END_VAR
	VAR_OUTPUT
		error : BOOL;
		errorID : UINT;
		errorString : STRING[80];
	END_VAR
	VAR
		internal : A3brIOWatchInternal_typ;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION A3brRapidExec2String : UINT
	VAR_INPUT
		rapidExec : UINT;
		pString : UDINT;
	END_VAR
END_FUNCTION

FUNCTION A3brRapidCycle2String : UINT
	VAR_INPUT
		rapidCycle : UINT;
		pString : UDINT;
	END_VAR
END_FUNCTION

FUNCTION A3brCtrlState2String : UINT
	VAR_INPUT
		ctrlState : UINT;
		pString : UDINT;
	END_VAR
END_FUNCTION

FUNCTION A3brOpMode2String : UINT
	VAR_INPUT
		opMode : UINT;
		pString : UDINT;
	END_VAR
END_FUNCTION
