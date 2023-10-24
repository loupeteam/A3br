//********************************************************************************
// Author:    Josh Polansky
// Created:   January 29, 2020
//********************************************************************************

#include "string.h"

#include <bur/plctypes.h>
#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#endif


/* Add prototype support.  */
#ifndef PROTO
#if defined (USE_PROTOTYPES) ? USE_PROTOTYPES : defined (__STDC__)
#define PROTO(ARGS) ARGS
#else
#define PROTO(ARGS) ()
#endif
#endif

#include "md5.h"
#include "A3br.h"
#include "base64.h"

#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wpointer-sign"
//#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
//#pragma GCC diagnostic ignored "-Wreturn-type"


int getParameter(const char *buf, char *value){

	char* start = strchr(buf,'"');
	if(!start) return 1;

	char* end = strchr(++start,'"');
	if(!end) return 1;

	memcpy(value, start, end - start);
	memset(value + (end - start), 0, 1);
	return 0;
}

int getCookieParameter(const char *buf, char *value){
	if(!buf) return 1;
	char* start = strchr(buf,'=');
	if(!start) return 1;

	char* end = strchr(++start,';');
	if(!end)
		end = strchr(++start,'\n');
	if(!end) return 1;
	memcpy(value, start, end - start);
	memset(value + (end - start), 0, 1);
	return 0;
}

void getDigestParameters( const unsigned char *auth, unsigned char *realm, unsigned char *qop, unsigned char *nonce, unsigned char *opaque){
	
	//	HTTP/1.1 401 Unauthorized
	//	Vary: Accept-Encoding
	//	X-Frame-Options: SAMEORIGIN
	//	Content-Type: text/plain
	//	X-Content-Type-Options: nosniff
	//	Date: Fri, 17 Jan 2020 19:40:36 GMT
	//	Cache-Control: no-cache
	//	Content-Length: 29
	//	X-XSS-Protection: 1; mode=block
	//	Connection: Keep-Alive
	//	WWW-Authenticate: Digest realm="validusers@robapi.abb", domain="/", qop="auth", nonce="OGJkNjI0ODc0MDY5OGMwNzp2YWxpZHVzZXJzQHJvYmFwaS5hYmI6MTZmYjUwNThiYjM6Yw==", opaque="799d5", algorithm="MD5", stale="FALSE"
	//	Accept-Ranges: bytes
	
//	char *auth = strstr(buf, "WWW-Authenticate");
	if(!auth) return;
	getParameter( strstr(auth, "realm"), realm);	
	getParameter( strstr(auth, "qop"), qop);
	getParameter( strstr(auth, "nonce"), nonce);
	getParameter( strstr(auth, "opaque"), opaque);
	return;		
}

unsigned long getCookie( LLHttpHeaderField_typ *buf, unsigned char *httpSession, unsigned char *abbcx){
	
	//	HTTP/1.1 200 OK
	//	Set-Cookie: -http-session-=7::http.session::2c07adcb8b7fb7e586f7d85e77a64519; path=/; domain=127.0.0.1; httponly
	//	Set-Cookie: ABBCX=17; path=/; domain=127.0.0.1; httponly
	//	Vary: Accept-Encoding
	//	X-Frame-Options: SAMEORIGIN
	//	Content-Type: application/json
	//	X-Content-Type-Options: nosniff
	//	Date: Tue, 21 Jan 2020 18:04:13 GMT
	//	Cache-Control: no-cache="set-cookie", max-age=0, no-cache, no-store
	//	Content-Length: 1262
	//	X-XSS-Protection: 1; mode=block
	//	Connection: Keep-Alive
	//	Pragma: no-cache
	//	Expires: -1
	//	Accept-Ranges: bytes

	if(!buf) return 0;
	if (!brsstrcmp(buf->name, "set-cookie")) {	
		getCookieParameter(strstr(buf->value, "-http-session-"),httpSession);
		getCookieParameter(strstr(buf->value, "ABBCX"),abbcx);
	}
	return;	
}

void MD5(unsigned char const *buf, unsigned char *digest){

	struct MD5Context context;
	unsigned char checksum[32];

	MD5Init (&context);
	MD5Update (&context, buf, strlen(buf));
	MD5Final (checksum, &context);
	memcpy( digest, &checksum, sizeof(checksum) );
	
	return;
}
void MD5Test(unsigned char *digest){

//Example from wikipedia:	
//	HA1 = MD5( "Mufasa:testrealm@host.com:Circle Of Life" )
//		= 939e7578ed9e3c518a452acee763bce9
//
//		HA2 = MD5( "GET:/dir/index.html" )
//		= 39aff3a2bab6126f332b942af96d3366
//
//		Response = MD5( "939e7578ed9e3c518a452acee763bce9:dcd98b7102dd2f0e8b11d0f600bfb0c093:00000001:0a4f113b:auth:39aff3a2bab6126f332b942af96d3366" )
//		= 6629fae49393a05397450978507c4ef1
	
	unsigned char buf[1000];
	unsigned char HA1[32];
	unsigned char HA2[32];
	unsigned char digestHEX[32];
	unsigned char HA1S[33];
	unsigned char HA2S[33];

	memset(HA1,0,sizeof(HA1));	
	memset(HA2,0,sizeof(HA2));	
	
	MD5( "Mufasa:testrealm@host.com:Circle Of Life", HA1);
	MD5( "GET:/dir/index.html", HA2);
	ByteToHexString(HA1, 16, HA1S);
	ByteToHexString(HA2, 16, HA2S);

	memset( buf,0,sizeof(buf));
	strcat( buf, HA1S);
	strcat( buf, ":dcd98b7102dd2f0e8b11d0f600bfb0c093:00000001:0a4f113b:auth:");
	strcat( buf, HA2S);
		
	MD5( buf, digestHEX);
	ByteToHexString(digestHEX, 16, digest);
	return;
}
void digestAuth(unsigned char *username,unsigned char *realm,unsigned char * password,unsigned int method,unsigned char * uri,unsigned char * nonce, unsigned char *nc, unsigned char *cnonce, unsigned char *qop, unsigned char *digest){

	
	//	If the algorithm directive's value is "MD5" or unspecified, then HA1 is
	//	HA1 = MD5(username:realm:password)

	//	If the algorithm directive's value is "MD5-sess", then HA1 is
	//	HA1 = MD5(MD5(username:realm:password):nonce:cnonce)

	//	If the qop directive's value is "auth" or is unspecified, then HA2 is
	//	HA2 = MD5(method:digestURI)

	//	If the qop directive's value is "auth-int", then HA2 is
	//	HA2 = MD5(method:digestURI:MD5(entityBody))

		
	unsigned char buf[1000];
	unsigned char HA1[32];
	unsigned char HA2[32];
	unsigned char digestHEX[32];
	unsigned char HA1S[33];
	unsigned char HA2S[33];

	memset(HA1,0,sizeof(HA1));	
	memset(HA2,0,sizeof(HA2));	
	
		
	memset(buf,0,sizeof(buf));	
	strcat( buf, username);
	strcat( buf, ":");
	strcat( buf, realm);
	strcat( buf, ":");
	strcat( buf, password);
	MD5( buf, HA1);

	memset( buf,0,sizeof(buf));
	switch(method){
		case LLHTTP_METHOD_GET:
			strcat( buf, "GET");
			break;
		case LLHTTP_METHOD_POST:
			strcat( buf, "POST");
			break;
		case LLHTTP_METHOD_PUT:
			strcat( buf, "PUT");
			break;
		case LLHTTP_METHOD_DELETE:
			strcat( buf, "DELETE");
			break;
	}
	strcat( buf, ":");
	strcat( buf, uri);
	MD5( buf, HA2);

	ByteToHexString(HA1, 16, HA1S);
	ByteToHexString(HA2, 16, HA2S);
	
//	If the qop directive's value is "auth" or "auth-int", then compute the response as follows:
//	response = MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)

//	If the qop directive is unspecified, then compute the response as follows:
//	response = MD5(HA1:nonce:HA2)
	memset( buf,0,sizeof(buf));
	strcat( buf, HA1S);
	strcat( buf, ":");
	strcat( buf, nonce);
	strcat( buf, ":");
	strcat( buf, nc);
	strcat( buf, ":");
	strcat( buf, cnonce);
	strcat( buf, ":");
	strcat( buf, qop);
	strcat( buf, ":");
	strcat( buf, HA2S);
		
	MD5( buf, digestHEX);
	ByteToHexString(digestHEX, 16, digest);
	return;
}

void digestFromHeader( const char *buf, const char *username, const char * password, char *uri, char * method, char *nc, char *cnonce, char *digest){
	
	unsigned char realm[1000], qop[20], nonce[1000], opaque[80];
		
	getDigestParameters(buf, realm, qop, nonce, opaque);	
	digestAuth( username, realm, password, method, uri, nonce, nc, cnonce, qop, digest);
	return;
}

unsigned long generateDigestAuthorization( A3brDigestAuthentication_typ *auth, const char *uri , unsigned char * authHeader){
	
	brsstrcpy(authHeader, "Digest username=\"");
	brsstrcat(authHeader,auth->userName);
	brsstrcat(authHeader,"\", realm=\"");
	brsstrcat(authHeader,auth->realm);
	brsstrcat(authHeader,"\", nonce=\"");
	brsstrcat(authHeader,auth->nonce);
	brsstrcat(authHeader,"\", uri=\"");
	brsstrcat(authHeader,uri);
	brsstrcat(authHeader,"\", algorithm=\"");
	brsstrcat(authHeader,"MD5");
	brsstrcat(authHeader,"\", response=\"");
	brsstrcat(authHeader,auth->digest);
	brsstrcat(authHeader,"\", opaque=\"");
	brsstrcat(authHeader,auth->opaque);
	brsstrcat(authHeader,"\", qop=\"");
	brsstrcat(authHeader,auth->qop);
	brsstrcat(authHeader,"\", nc=");
	brsstrcat(authHeader,auth->nc);
	brsstrcat(authHeader,", cnonce=\"");
	brsstrcat(authHeader,auth->cnonce);
	brsstrcat(authHeader,"\"");
	
	return 0;
	
}

unsigned long generateBasicAuthorization( A3brDigestAuthentication_typ *auth, unsigned char * authHeader){
	
	unsigned char credentials[200];
	brsstrcpy(credentials, auth->userName);
	brsstrcat(credentials, ":");
	brsstrcat(credentials, auth->password);
	
	// Encode the credentials using base64. 
	unsigned char base64Credentials[200];
	memset(base64Credentials, 0, sizeof(base64Credentials));
	Base64encode(base64Credentials, credentials, strlen(credentials));
	
	// Create authorization header
	brsstrcpy(authHeader, "Basic ");
	brsstrcat(authHeader, base64Credentials);
	
	return 0;
	
}
