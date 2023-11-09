# Change log

- 0.4.0:
	- Add initial support for RWS2.0 (i.e. the version that ships with Omnicore robots)
	- NOTE that the only supported blocks with 2.0 currently are GetIO, SetIO, GetSymbol, SetSymbol, and GetState.

- 0.3.5:
    - Fix bug that would append auth or cookies many times on connection loss

- 0.3.4:
    - Fix 503 bug by closing the connection if detected

- 0.3.3-032921:
    - Add status enum 2 string functions
  
- 0.3.2-032321:
    - Reduce maximum of http connections to 1
    - Workaround BUG with http status == 503

- 0.3.1:
    - Fix Start not returning done
    - Fix some commands not report errors correctly  

- 0.3.0:
	- Add support for maximum of 3 sessions
	- Add support for maximum of 2 http connections per session
	- Workaround BUG with http status == 200 and content-length == 0

- 0.2.1:
	- Split power command into powerOn and powerOff

- 0.2.0:
	- Add in support for BOOL and REAL datatypes for symbol operations

- 0.1.1:
	- Revert HTTP protocol version to 0.1.1 to fix lost response error
	- Fix JsmnParse argument bug (was passing in type instead of length)
	- Fix behavior of done and busy bits for GetState block

- 0.1.0:
	- Add new SetSymbol and GetSymbol blocks
	- Add ProgramControl block (this is experimental at this point)

- 0.0.5:
    - Update dependencies versions

- 0.0.4:
    - Update dependencies versions 

- 0.0.3:
	- Update FUB API to conform to style guide
	- Expand error handling on all blocks
	- Expand inline comments and add enums for clarity
 
- 0.0.2:
    - Modify types to conform to style guide
    - Fix some warnings
		   
- 0.0.1:
    - First version



