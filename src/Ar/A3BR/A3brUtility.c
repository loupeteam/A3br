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

// TODO: It would be nice to change these strings to variales that can be modified outside the library (by the application)
unsigned short A3brRapidExec2String(unsigned short rapidExec, unsigned long pString) {
	if(!pString) return 0;
	
	switch(rapidExec){
		case A3BR_RAPID_EXEC_ST_UNDEFINED:
			brsstrcpy(pString, "Undefined"); 
			break;
		
		case A3BR_RAPID_EXEC_ST_RUNNING:
			brsstrcpy(pString, "Running"); 
			break;
		
		case A3BR_RAPID_EXEC_ST_STOPPED:
			brsstrcpy(pString, "Stopped"); 
			break;
		
		default:
			brsstrcpy(pString, "Invalid"); 
			break;
		
	}

	return 0;
}


unsigned short A3brRapidCycle2String(unsigned short rapidCycle, unsigned long pString) {
	if(!pString) return 0;
	
	switch(rapidCycle){
		case A3BR_RAPID_CYCLE_ST_UNDEFINED:
			brsstrcpy(pString, "Undefined"); 
			break;
		
		case A3BR_RAPID_CYCLE_ST_FOREVER:
			brsstrcpy(pString, "Forever"); 
			break;
		
		case A3BR_RAPID_CYCLE_ST_ASIS:
			brsstrcpy(pString, "As is"); 
			break;
		
		case A3BR_RAPID_CYCLE_ST_ONCE:
			brsstrcpy(pString, "Once"); 
			break;
		
		case A3BR_RAPID_CYCLE_ST_ONCEDONE:
			brsstrcpy(pString, "Once done"); 
			break;
		
		default:
			brsstrcpy(pString, "Invalid"); 
			break;
	
	}

	return 0;
}


unsigned short A3brCtrlState2String(unsigned short ctrlState, unsigned long pString) {
	if(!pString) return 0;
	
	switch(ctrlState){
		case A3BR_CTRL_ST_UNDEFINED:
			brsstrcpy(pString, "Undefined"); 
			break;
		
		case A3BR_CTRL_ST_ON:
			brsstrcpy(pString, "On"); 
			break;
		
		case A3BR_CTRL_ST_OFF:
			brsstrcpy(pString, "Off"); 
			break;
		
		case A3BR_CTRL_ST_GUARDSTOP:
			brsstrcpy(pString, "Guard stop"); 
			break;
		
		case A3BR_CTRL_ST_ESTOP:
			brsstrcpy(pString, "Estop"); 
			break;
		
		case A3BR_CTRL_ST_ESTOP_RESET:
			brsstrcpy(pString, "Estop reset"); 
			break;
		
		case A3BR_CTRL_ST_SYS_FAIL:
			brsstrcpy(pString, "System fail"); 
			break;
		
		default:
			brsstrcpy(pString, "Invalid"); 
			break;
	
	}

	return 0;
}


unsigned short A3brOpMode2String(unsigned short opMode, unsigned long pString) {
	if(!pString) return 0;
	
	switch(opMode){
		case A3BR_OP_MODE_UNDEFINED:
			brsstrcpy(pString, "Undefined"); 
			break;
		
		case A3BR_OP_MODE_INIT:
			brsstrcpy(pString, "Init"); 
			break;
		
		case A3BR_OP_MODE_AUTO_CHANGE_REQ:
			brsstrcpy(pString, "Auto change"); 
			break;
		
		case A3BR_OP_MODE_MANUAL_CHANGE_REQ:
			brsstrcpy(pString, "Manual change"); 
			break;
		
		case A3BR_OP_MODE_MANUAL_LOW_SPEED:
			brsstrcpy(pString, "Manual low"); 
			break;
		
		case A3BR_OP_MODE_MANUAL_FULL_SPEED:
			brsstrcpy(pString, "Manual full"); 
			break;
		
		case A3BR_OP_MODE_AUTO:
			brsstrcpy(pString, "Auto"); 
			break;
		
		default:
			brsstrcpy(pString, "Invalid"); 
			break;
	
	}

	return 0;
}
