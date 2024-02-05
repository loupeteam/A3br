/* Automation Studio generated header file */
/* Do not edit ! */
/* llhttp 0.01.4 */

#ifndef _LLHTTP_
#define _LLHTTP_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _llhttp_VERSION
#define _llhttp_VERSION 0.01.4
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
#ifdef _SG3
		#include "standard.h"
		#include "tcpcomm.h"
		#include "ringbuflib.h"
		#include "stringext.h"
#endif
#ifdef _SG4
		#include "standard.h"
		#include "tcpcomm.h"
		#include "ringbuflib.h"
		#include "stringext.h"
#endif
#ifdef _SGC
		#include "standard.h"
		#include "tcpcomm.h"
		#include "ringbuflib.h"
		#include "stringext.h"
#endif


/* Constants */
#ifdef _REPLACE_CONST
 #define LLHTTP_HTTPS_PORT 443U
 #define LLHTTP_HTTP_PORT 80U
 #define LLHTTP_MAX_RETRIES 5U
 #define LLHTTP_MAX_NUM_CLIENTS 5U
 #define LLHTTP_MAI_NUM_CLIENTS 4U
 #define LLHTTP_MAX_LEN_CONTENT_TYPE 120U
 #define LLHTTP_MAX_LEN_CONTENT_ACCEPT 255U
 #define LLHTTP_MAX_LEN_URI 500U
 #define LLHTTP_MAX_LEN_HEADER_NAME 120U
 #define LLHTTP_MAX_LEN_HEADER_VALUE 320U
 #define LLHTTP_MAI_NUM_HEADER_LINES 25U
#else
 _GLOBAL_CONST unsigned short LLHTTP_HTTPS_PORT;
 _GLOBAL_CONST unsigned short LLHTTP_HTTP_PORT;
 _GLOBAL_CONST unsigned char LLHTTP_MAX_RETRIES;
 _GLOBAL_CONST unsigned char LLHTTP_MAX_NUM_CLIENTS;
 _GLOBAL_CONST unsigned char LLHTTP_MAI_NUM_CLIENTS;
 _GLOBAL_CONST unsigned char LLHTTP_MAX_LEN_CONTENT_TYPE;
 _GLOBAL_CONST unsigned char LLHTTP_MAX_LEN_CONTENT_ACCEPT;
 _GLOBAL_CONST unsigned short LLHTTP_MAX_LEN_URI;
 _GLOBAL_CONST unsigned char LLHTTP_MAX_LEN_HEADER_NAME;
 _GLOBAL_CONST unsigned short LLHTTP_MAX_LEN_HEADER_VALUE;
 _GLOBAL_CONST unsigned char LLHTTP_MAI_NUM_HEADER_LINES;
#endif




/* Datatypes and datatypes of function blocks */
typedef enum LLHttpMethod_enum
{	LLHTTP_METHOD_GET,
	LLHTTP_METHOD_HEAD,
	LLHTTP_METHOD_POST,
	LLHTTP_METHOD_PUT,
	LLHTTP_METHOD_DELETE,
	LLHTTP_METHOD_CONNECT,
	LLHTTP_METHOD_OPTIONS,
	LLHTTP_METHOD_PATCH,
	LLHTTP_METHOD_TRACE,
	LLHTTP_METHOD_ANY,
	LLHTTP_METHOD_DEFAULT
} LLHttpMethod_enum;

typedef enum LLHttpErr_enum
{	LLHTTP_ERR_VALUE_MISMATCH = -2,
	LLHTTP_ERR_NOT_FOUND = -1,
	LLHTTP_ERR_OK = 0,
	LLHTTP_ERR_PARSE = 50100,
	LLHTTP_ERR_MAX_HEADERS,
	LLHTTP_ERR_INVALID_INPUT,
	LLHTTP_ERR_PACKET_SIZE_TOO_BIG,
	LLHTTP_ERR_
} LLHttpErr_enum;

typedef enum LLHttpStatusCode_enum
{	LLHTTP_STAT_Continue = 100,
	LLHTTP_STAT_SwitchingProtocols = 101,
	LLHTTP_STAT_Processing = 102,
	LLHTTP_STAT_EarlyHints = 103,
	LLHTTP_STAT_OK = 200,
	LLHTTP_STAT_Created = 201,
	LLHTTP_STAT_Accepted = 202,
	LLHTTP_STAT_NonAuthoritativeInfo = 203,
	LLHTTP_STAT_NoContent = 204,
	LLHTTP_STAT_ResetContent = 205,
	LLHTTP_STAT_PartialContent = 206,
	LLHTTP_STAT_MultiStatus = 207,
	LLHTTP_STAT_AlreadyReported = 208,
	LLHTTP_STAT_IMUsed = 226,
	LLHTTP_STAT_MultipleChoices = 300,
	LLHTTP_STAT_MovedPermanently = 301,
	LLHTTP_STAT_Found = 302,
	LLHTTP_STAT_SeeOther = 303,
	LLHTTP_STAT_NotModified = 304,
	LLHTTP_STAT_UseProxy = 305,
	LLHTTP_STAT_TemporaryRedirect = 307,
	LLHTTP_STAT_PermanentRedirect = 308,
	LLHTTP_STAT_BadRequest = 400,
	LLHTTP_STAT_Unauthorized = 401,
	LLHTTP_STAT_PaymentRequired = 402,
	LLHTTP_STAT_Forbidden = 403,
	LLHTTP_STAT_NotFound = 404,
	LLHTTP_STAT_MethodNotAllowed = 405,
	LLHTTP_STAT_NotAcceptable = 406,
	LLHTTP_STAT_ProxyAuthRequired = 407,
	LLHTTP_STAT_RequestTimeout = 408,
	LLHTTP_STAT_Conflict = 409,
	LLHTTP_STAT_Gone = 410,
	LLHTTP_STAT_LengthRequired = 411,
	LLHTTP_STAT_PreconditionFailed = 412,
	LLHTTP_STAT_PayloadTooLarge = 413,
	LLHTTP_STAT_URITooLong = 414,
	LLHTTP_STAT_UnsupportedMediaType = 415,
	LLHTTP_STAT_RangeNotSatisfiable = 416,
	LLHTTP_STAT_ExpectationFailed = 417,
	LLHTTP_STAT_ImATeapot = 418,
	LLHTTP_STAT_UnprocessableEntity = 422,
	LLHTTP_STAT_Locked = 423,
	LLHTTP_STAT_FailedDependency = 424,
	LLHTTP_STAT_UpgradeRequired = 426,
	LLHTTP_STAT_PreconditionRequired = 428,
	LLHTTP_STAT_TooManyRequests = 429,
	LLHTTP_STAT_HeaderFieldsTooLarge = 431,
	LLHTTP_STAT_UnavailableLegal = 451,
	LLHTTP_STAT_InternalServerError = 500,
	LLHTTP_STAT_NotImplemented = 501,
	LLHTTP_STAT_BadGateway = 502,
	LLHTTP_STAT_ServiceUnavailable = 503,
	LLHTTP_STAT_GatewayTimeout = 504,
	LLHTTP_STAT_VersionNotSupported = 505,
	LLHTTP_STAT_VariantAlsoNegotiat = 506,
	LLHTTP_STAT_InsufficientStorage = 507,
	LLHTTP_STAT_LoopDetected = 508,
	LLHTTP_STAT_NotExtended = 510,
	LLHTTP_STAT_NetworkAuthRequired = 511,
	LLHTTP_STAT_xxx_max = 1023
} LLHttpStatusCode_enum;

typedef enum LLHttpState_enum
{	LLHTTP_ST_IDLE,
	LLHTTP_ST_HEADER,
	LLHTTP_ST_AUTH,
	LLHTTP_ST_SEND,
	LLHTTP_ST_LISTEN,
	LLHTTP_ST_PARSE,
	LLHTTP_ST_ERROR,
	LLHTTP_ST_CLEAN,
	LLHTTP_ST_
} LLHttpState_enum;

typedef struct LLHttpHeaderField_typ
{	plcstring name[121];
	plcstring value[321];
} LLHttpHeaderField_typ;

typedef struct LLHttpHeader_typ
{	plcstring contentType[121];
	unsigned long contentLength;
	plcstring uri[501];
	plcstring accept[256];
	struct LLHttpHeaderField_typ lines[26];
	signed short version;
	signed short status;
	enum LLHttpMethod_enum method;
} LLHttpHeader_typ;

typedef struct LLHttpHandler_typ
{	enum LLHttpMethod_enum method;
	plcstring uri[501];
	unsigned long self;
	unsigned long newMessageCallback;
	struct LLHttpServerInternalClient_typ* client;
	plcbit busy;
} LLHttpHandler_typ;

typedef struct LLHttpServiceLink_typ
{	struct Buffer_typ requestBuffer;
	struct Buffer_typ responseBuffer;
	struct Buffer_typ handlers;
} LLHttpServiceLink_typ;

typedef struct LLHttpServiceResponse_typ
{	unsigned long self;
	plcstring uri[501];
	unsigned long status;
	struct LLHttpHeaderField_typ userHeader[26];
	unsigned long pPayload;
	unsigned long payloadLength;
	unsigned long successCallback;
	unsigned long errorCallback;
} LLHttpServiceResponse_typ;

typedef struct LLHttpServiceRequest_typ
{	unsigned long self;
	plcstring uri[501];
	unsigned long method;
	struct LLHttpHeaderField_typ userHeader[26];
	unsigned long pPayload;
	unsigned long payloadLength;
	unsigned long successCallback;
	unsigned long errorCallback;
} LLHttpServiceRequest_typ;

typedef struct LLHttpParse
{
	/* VAR_INPUT (analog) */
	unsigned long data;
	unsigned long dataLength;
	/* VAR_OUTPUT (analog) */
	struct LLHttpHeader_typ header;
	unsigned long content;
	signed long errorId;
	/* VAR_OUTPUT (digital) */
	plcbit partialPacket;
	plcbit partialContent;
	plcbit contentPresent;
	plcbit error;
} LLHttpParse_typ;

typedef struct LLHttpServerInternalClient_typ
{	unsigned char connected;
	struct TCPStream_typ tcpStream;
	unsigned long pReceiveData;
	unsigned long receiveDataSize;
	unsigned long pSendData;
	unsigned long sendDataSize;
	struct Buffer_typ sendBuffer;
	struct Buffer_typ receivedBuffer;
	struct TON_10ms requestTimer;
	unsigned long recvLength;
	struct LLHttpParse parser;
	plcbit requestActive;
	struct LLHttpInternalRequest_typ* pCurrentRequest;
	struct LLHttpInternalRequest_typ* pRecvRequest;
	struct LLHttpServiceResponse_typ* pCurrentResponse;
	struct LLHttpServiceLink_typ api;
	unsigned short bufferStatus;
	plcbit error;
	signed long errorId;
} LLHttpServerInternalClient_typ;

typedef struct LLHttpInternalRequest_typ
{	struct LLHttpHeader_typ header;
	unsigned char contentStart;
} LLHttpInternalRequest_typ;

typedef struct LLHttpServerInternal_typ
{	struct TON responseTimeout;
	unsigned short retries;
	plcstring rawrecvData[20][81];
	plcstring rawSendData[20][81];
	unsigned long bufferSize;
	struct TCPStream_typ tcpStream;
	struct TCPConnectionMgr_typ tcpMgr;
	unsigned long bufferStatus;
	plcbit connected;
	enum LLHttpState_enum state;
	struct LLHttpParse parser;
	struct LLHttpServerInternalClient_typ* pClients;
	struct LLHttpServerInternalClient_typ clients[5];
	unsigned long numClientsConnected;
	unsigned short numClients;
	plcbit initialized;
	struct LLHttpServiceLink_typ api;
} LLHttpServerInternal_typ;

typedef struct LLHttpClientInternal_typ
{	struct TON responseTimeout;
	unsigned short retries;
	plcstring rawrecvData[20][81];
	struct LLHttpHeaderField_typ sendHeader;
	plcstring rawSendData[20][81];
	unsigned long bufferSize;
	struct TCPStream_typ tcpStream;
	struct TCPConnectionMgr_typ tcpMgr;
	struct LLHttpServiceRequest_typ currentRequest;
	struct LLHttpServiceLink_typ api;
	unsigned long bufferStatus;
	plcbit connected;
	enum LLHttpState_enum state;
	struct LLHttpParse parser;
	unsigned long recvLength;
	plcstring tempBuffer[1001];
} LLHttpClientInternal_typ;

typedef struct LLHttpResponseInternal_typ
{	plcbit send;
	plcbit done;
	plcbit busy;
	plcbit error;
	plcbit newRequest;
	struct LLHttpHandler_typ handler;
	struct LLHttpServiceLink_typ* clientApi;
	plcbit initialized;
} LLHttpResponseInternal_typ;

typedef struct LLHttpRequestInternal_typ
{	plcbit send;
	plcbit done;
	plcbit busy;
	plcbit error;
} LLHttpRequestInternal_typ;

typedef struct LLHttpServer
{
	/* VAR_INPUT (analog) */
	unsigned long bufferSize;
	unsigned short numClients;
	plcstring ipAddress[256];
	unsigned long port;
	unsigned short sslIndex;
	/* VAR_OUTPUT (analog) */
	unsigned long ident;
	unsigned short numConnectedClients;
	signed long errorId;
	/* VAR (analog) */
	struct LLHttpServerInternal_typ internal;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit https;
	/* VAR_OUTPUT (digital) */
	plcbit error;
} LLHttpServer_typ;

typedef struct LLHttpClient
{
	/* VAR_INPUT (analog) */
	unsigned long bufferSize;
	plcstring hostname[256];
	unsigned short port;
	plcstring localIPAddress[256];
	unsigned short localPort;
	unsigned short sslIndex;
	/* VAR_OUTPUT (analog) */
	unsigned long ident;
	signed long errorId;
	/* VAR (analog) */
	struct LLHttpClientInternal_typ internal;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit abort;
	plcbit https;
	/* VAR_OUTPUT (digital) */
	plcbit connected;
	plcbit error;
} LLHttpClient_typ;

typedef struct LLHttpRequest
{
	/* VAR_INPUT (analog) */
	unsigned long ident;
	enum LLHttpMethod_enum method;
	plcstring uri[501];
	unsigned long pUserHeader;
	unsigned long numUserHeaders;
	unsigned long pContent;
	plcstring contentType[121];
	unsigned long contentLength;
	unsigned long pResponse;
	unsigned long responseSize;
	/* VAR_OUTPUT (analog) */
	struct LLHttpHeader_typ header;
	unsigned long responseLength;
	/* VAR (analog) */
	struct LLHttpRequestInternal_typ internal;
	/* VAR_INPUT (digital) */
	plcbit send;
	/* VAR_OUTPUT (digital) */
	plcbit busy;
	plcbit done;
	plcbit error;
} LLHttpRequest_typ;

typedef struct LLHttpResponse
{
	/* VAR_INPUT (analog) */
	unsigned long ident;
	enum LLHttpMethod_enum method;
	plcstring uri[81];
	unsigned long pUserHeader;
	unsigned long numUserHeaders;
	unsigned long status;
	unsigned long pContent;
	plcstring contentType[121];
	unsigned long contentLength;
	unsigned long pRequest;
	unsigned long requestSize;
	/* VAR_OUTPUT (analog) */
	struct LLHttpHeader_typ requestHeader;
	unsigned long requestLength;
	/* VAR (analog) */
	struct LLHttpResponseInternal_typ internal;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit send;
	/* VAR_OUTPUT (digital) */
	plcbit enabled;
	plcbit newRequest;
	plcbit busy;
	plcbit done;
	plcbit error;
} LLHttpResponse_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void LLHttpServer(struct LLHttpServer* inst);
_BUR_PUBLIC void LLHttpClient(struct LLHttpClient* inst);
_BUR_PUBLIC void LLHttpRequest(struct LLHttpRequest* inst);
_BUR_PUBLIC void LLHttpResponse(struct LLHttpResponse* inst);
_BUR_PUBLIC void LLHttpParse(struct LLHttpParse* inst);
_BUR_PUBLIC plcbit LLHttpHeaderContains(unsigned long headerlines, unsigned long name, unsigned long value);
_BUR_PUBLIC signed short LLHttpgetHeaderIndex(unsigned long headerlines, unsigned long name, unsigned long value);
_BUR_PUBLIC signed long LLHttpAddHeaderField(unsigned long headerlines, unsigned long numLines, unsigned long name, unsigned long value);
_BUR_PUBLIC plcbit LLHttpUriMatch(unsigned long a, unsigned long b);
_BUR_PUBLIC plcbit LLHttpMethodMatch(enum LLHttpMethod_enum a, enum LLHttpMethod_enum b);
_BUR_PUBLIC signed long LLHttpBuildResponse(unsigned long data, unsigned long response, unsigned long dataSize, unsigned long* pLen);
_BUR_PUBLIC plcbit LLHttpAddHandler(unsigned long ident, unsigned long pHandler);
_BUR_PUBLIC plcbit LLHttpStatus_isError(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_isServerError(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_isClientError(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_isRedirection(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_isSuccessful(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_isInformational(signed short code);
_BUR_PUBLIC plcbit LLHttpStatus_getDescription(signed short code, unsigned long dest);


__asm__(".section \".plc\"");

/* Additional IEC dependencies */
__asm__(".ascii \"iecdep \\\"Logical/Libraries/Loupe/tcpcomm/TCPComm.var\\\" scope \\\"global\\\"\\n\"");

__asm__(".previous");

#ifdef __cplusplus
};
#endif
#endif /* _LLHTTP_ */

