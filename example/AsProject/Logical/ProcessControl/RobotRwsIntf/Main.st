(*
* File: Cyclic.st
* Copyright (c) 2023 Loupe
* https://loupe.team
* 
* This file is part of PiperModuleTemplate, licensed under the MIT License.
*)

PROGRAM _INIT
	a3brConnect_0.enable := TRUE;
	a3brConnect_0.configuration.hostname := '192.168.125.1';  //specific per robot used
	//a3brConnect_0.configuration.port := 80;
	a3brConnect_0.configuration.username := 'admin';// Default User 'admin';  //specific per robot used
	a3brConnect_0.configuration.password := 'robotics';  //specific per robot used
	a3brConnect_0.configuration.apiVersion := A3BR_API_VERSION_2;  //Set to Version RWS 2.0 if Robot has a Omnicore processor
	IF a3brConnect_0.configuration.apiVersion = A3BR_API_VERSION_1 THEN
		a3brConnect_0.configuration.port := 80;
	ELSE
		a3brConnect_0.configuration.port := 443;
	END_IF
	
	a3brSetIO_0.value := 0;
	 
	//transferdifference
END_PROGRAM

PROGRAM _CYCLIC
	
	//Add application code inside the connected IF statement
	IF (a3brConnect_0.connected) THEN
		IF EDGE(executeSetIO) THEN
			a3brSetIO_0.execute := TRUE;
		END_IF
		
		//Setting an output signal
		a3brSetIO_0.ident := a3brConnect_0.ident;
		a3brSetIO_0.pSignal := ADR('ABB_Scalable_IO_0_DO1');
		
		//Reading an input signal
		a3brGetIO_0.ident := a3brConnect_0.ident;
		a3brGetIO_0.pSignal := ADR('ABB_Scalable_IO_0_DI1');
		
	END_IF
	
	// Call all FUBs. 
	a3brConnect_0();
	a3brSetIO_0();
	a3brGetIO_0();
	a3brSetIO_0.execute := FALSE;
	 
END_PROGRAM

PROGRAM _EXIT
	a3brConnect_0.enable := FALSE;
	 
END_PROGRAM

