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
	a3brConnect_0.configuration.port := 80;
	a3brConnect_0.configuration.username := 'admin';  //specific per robot used
	a3brConnect_0.configuration.password := 'robotics';  //specific per robot used
	a3brConnect_0.configuration.apiVersion := A3BR_API_VERSION_2;  //Set to Version RWS 2.0 if Robot has a Omnicore processor
	a3brSetIO_0.value := 0;
	 
END_PROGRAM

PROGRAM _CYCLIC
	
	//Add application code inside the connected IF statement
	IF (a3brConnect_0.connected) THEN
		;
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

