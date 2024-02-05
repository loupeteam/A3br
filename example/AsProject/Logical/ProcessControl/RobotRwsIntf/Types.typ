(*
* File: Cyclic.st
* Copyright (c) 2023 Loupe
* https://loupe.team
* 
* This file is part of PiperModuleTemplate, licensed under the MIT License.
*)

TYPE
	rwsA3br_typ : STRUCT
		connect : A3brWebService;
		control : A3brControl;
		getIO : A3brGetIO;
		getSymbol : A3brGetSymbol;
		ioWatch : A3brIOWatch;
		programControl : A3brProgramControl;
		setIO : A3brSetIO;
		setSymbol : A3brSetSymbol;
	
	END_STRUCT;
END_TYPE
