//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
//********************************************************************************

void digestFromHeader( const char *buf, 
	const unsigned char * username, 
	const unsigned char * password, 
	unsigned char * uri, 
	unsigned char * method,
	unsigned char * nc, 
	unsigned char * cnonce,  
	unsigned char * digest );
void digestAuth(unsigned char *username,
	unsigned char * realm,	
	unsigned char * password,
	unsigned int	method,
	unsigned char * uri,
	unsigned char * nonce,
	unsigned char * nc, 
	unsigned char * cnonce, 
	unsigned char * qop, 
	unsigned char * digest);

void getDigestParameters( const unsigned char *buf, unsigned char *realm, unsigned char *qop, unsigned char *nonce, unsigned char *opaque);
unsigned long getCookie( const unsigned char *buf, unsigned char *httpSession, unsigned char *abbcx);
void MD5Test(unsigned char *digest);
unsigned long generateDigestAuthorization( A3brDigestAuthentication_typ *auth, const char *uri , unsigned char * authHeader);

