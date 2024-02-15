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
	rws.connect.configuration.username := 'Default User';//specific per robot used
	rws.connect.configuration.password := 'robotics';  //specific per robot used
	rws.connect.configuration.apiVersion := A3BR_API_VERSION_2;  //Set to Version RWS 2.0 if Robot has a Omnicore processor
	IF rws.connect.configuration.apiVersion = A3BR_API_VERSION_1 THEN
		// RWS API v1.0 uses HTTP and is able use port 80
		rws.connect.configuration.port := 80;
	ELSE
		// RWS API v2.0 uses HTTPS and thus requires port 443
		rws.connect.configuration.port := 443;
	END_IF
	
	rws.setIO.value := 0;

	rws.getState.enable := TRUE;
	rws.getState.mode := A3BR_GET_STATE_MODE_CONTINUOUS;
	rws.getState.cycleTime := 1000; // units are milliseconds
	
	rws.control.cycle := A3BR_RAPID_CYCLE_ST_ONCE;
	
	ioSignalPath := 'Local/ABB_Scalable_IO_0_DI1';
	sizeStringPathCheck := SIZEOF(ioSignalPath);
	
END_PROGRAM

PROGRAM _CYCLIC
	// Call RWS connection fub
	rws.connect();
	// Check to verify that a connection has been established
	IF (NOT rws.connect.connected) THEN
		RETURN;
	END_IF
	
	// Setting an output signal
	rws.setIO.ident := rws.connect.ident;
	rws.setIO.pSignal := ADR(ioSignalPath);
	
	// Reading an input signal
	rws.getIO.ident := rws.connect.ident;
	rws.getIO.pSignal := ADR(ioSignalPath);
	
	// Control Robot
	rws.control.ident := rws.connect.ident;
	
	// Get Symbol
	rws.getSymbol.ident := rws.connect.ident;
	rws.getSymbol.pSignal := ADR('RAPID/T_ROB1/Wizard/cycle');
	rws.getSymbol.pValue := ADR(cycles);
	rws.getSymbol.szValue := SIZEOF(cycles);
	rws.getSymbol.typeValue := A3BR_VAR_TYPE_INT;
	
	// Get State
	rws.getState.ident := rws.connect.ident;
	
	// IO Watch 
	rws.ioWatch.ident := rws.connect.ident;
	
	// Program Control
	rws.programControl.ident := rws.connect.ident;
	
	// Set Symbol
	rws.setSymbol.ident := rws.connect.ident;
	rws.setSymbol.pSignal := ADR('RAPID/T_ROB1/Wizard/cycle');
	rws.setSymbol.pValue := ADR(cycles);
	rws.setSymbol.szValue := SIZEOF(cycles);
	rws.setSymbol.typeValue := A3BR_VAR_TYPE_INT;
	
	// Call all FUBs. 
	rws.setIO();
	rws.getIO();
	rws.control();
	rws.getSymbol();
	rws.getState();
	rws.ioWatch();
	rws.programControl();
	rws.setSymbol();

END_PROGRAM

PROGRAM _EXIT
	rws.connect.enable := FALSE;
	 
END_PROGRAM

