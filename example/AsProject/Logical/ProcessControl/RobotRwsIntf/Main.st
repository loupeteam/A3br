(*
* File: Cyclic.st
* Copyright (c) 2023 Loupe
* https://loupe.team
* 
* This file is part of PiperModuleTemplate, licensed under the MIT License.
*)

PROGRAM _INIT
	rws.connect.enable := TRUE;
	rws.connect.configuration.hostname := '192.168.125.1';  //specific per robot used
	rws.connect.configuration.username := 'admin';// Default User 'admin';  //specific per robot used
	rws.connect.configuration.password := 'robotics';  //specific per robot used
	rws.connect.configuration.apiVersion := A3BR_API_VERSION_2;  //Set to Version RWS 2.0 if Robot has a Omnicore processor
	IF rws.connect.configuration.apiVersion = A3BR_API_VERSION_1 THEN
		rws.connect.configuration.port := 80;
	ELSE
		rws.connect.configuration.port := 443;
	END_IF
	
	rws.setIO.value := 0;
	
END_PROGRAM

PROGRAM _CYCLIC
	// Call RWS connection fub
	rws.connect();
	// Check to verify that a connection has been established
	IF (NOT rws.connect.connected) THEN
		RETURN;
	END_IF
	
	// Setting an output signal
	IF EDGE(executeSetIO) THEN
		rws.setIO.execute := TRUE;
	END_IF
	rws.setIO.ident := rws.connect.ident;
	rws.setIO.pSignal := ADR('ABB_Scalable_IO_0_DO1'); //ADR('RAPID/T_ROB1/General/activeAlarm');
	
	// Reading an input signal
	rws.getIO.ident := rws.connect.ident;
	rws.getIO.pSignal := ADR('I/O System/ABB_Scalable_IO_0_DI1');
	
	// Control Robot
	rws.control.ident := rws.connect.ident;
	
	// Get Symbol
	rws.getSymbol.ident := rws.connect.ident;
	
	// IO Watch 
	rws.ioWatch.ident := rws.connect.ident;
	
	// Program Control
	rws.programControl.ident := rws.connect.ident;
	
	// Set Symbol
	rws.setSymbol.ident := rws.connect.ident;
	
	// Call all FUBs. 
	rws.setIO();
	rws.setIO.execute := FALSE;
	rws.getIO();
	rws.control();
	rws.getSymbol();
	rws.ioWatch();
	rws.programControl();
	rws.setSymbol();

END_PROGRAM

PROGRAM _EXIT
	rws.connect.enable := FALSE;
	 
END_PROGRAM

