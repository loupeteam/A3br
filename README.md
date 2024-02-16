# Info
Library is provided by Loupe  
https://loupe.team  
info@loupe.team  
1-800-240-7042  

# Description
ABB provides a REST API for interfacing with its robots, called Robot Web Services. This library and program package implements an HTTP client to handle communication between a B&R PLC and an ABB IRC’s Robot Web Services server. It thereby provides read and write access to a wide range of settings and parameters on the IRC, as ABB has exposed most of its inner workings via this API. 

### Program Package
This program package will install the ProcessControl B&R package that will contain the RobotRwsIntf Program for a Automation Studio project and all its dependencies.  
The RobotRwsIntf program implements the function blocks found in the A3br library.  The configuration found in the program will be specific and unique for each robot the B&R PLC will be able to communicate and control. 

### Library Package
Many different types of process-level communication between the two systems are possible using this library:
-Autoconnect the B&R PLC to the IRC via unique configuration attributes.
-Retrieve the current state of the IRC (RAPID state, program execution state, etc).
-Read/write the value of an IO signal on the IRC.
-Read/write the value of a data symbol (i.e. RAPID var) on the IRC.
-Take mastery control of the IRC.
-Turn Motors On and Off. 
-Control program execution (start, stop, reset PP)  

# Installation
To install using the Loupe Package Manager (LPM), in an initialized Automation Studio project directory run `lpm install a3brprog`. Note that this will also pull in the library package as a dependency.  
If you only want to install the library package, run `lpm install a3br`.   
For more information about LPM, see https://loupeteam.github.io/LoupeDocs/tools/lpm.html

# Documentation
For more documentation and examples, see https://loupeteam.github.io/LoupeDocs/libraries/a3br.html (or you can run `lpm docs a3br`)

# Licensing

This project is primarily licensed under the [MIT License](LICENSE). However, it includes components under the [Apple Public Source License (APSL)](LICENSE-APSL) and the [Apache Group License](LICENSE-Apache). Please refer to the respective files for full license texts.

-base64.c  
-base64.h