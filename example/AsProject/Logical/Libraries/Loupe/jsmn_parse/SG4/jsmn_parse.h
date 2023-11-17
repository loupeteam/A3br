/* Automation Studio generated header file */
/* Do not edit ! */
/* jsmn_parse 1.05.0 */

#ifndef _JSMN_PARSE_
#define _JSMN_PARSE_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _jsmn_parse_VERSION
#define _jsmn_parse_VERSION 1.05.0
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
#ifdef _SG3
		#include "AsBrStr.h"
#endif
#ifdef _SG4
		#include "AsBrStr.h"
#endif
#ifdef _SGC
		#include "AsBrStr.h"
#endif


/* Constants */
#ifdef _REPLACE_CONST
 #define JSMN_MAX_TOKENS 128U
 #define JSMN_MAI_STRUCT_LEVEL 15U
 #define JSMN_STRLEN_CALLBACK_DATA 320U
 #define JSMN_STRLEN_CACHE 1000U
#else
 _GLOBAL_CONST unsigned char JSMN_MAX_TOKENS;
 _GLOBAL_CONST unsigned char JSMN_MAI_STRUCT_LEVEL;
 _GLOBAL_CONST unsigned short JSMN_STRLEN_CALLBACK_DATA;
 _GLOBAL_CONST unsigned short JSMN_STRLEN_CACHE;
#endif




/* Datatypes and datatypes of function blocks */
typedef enum jsmnerr
{	JSMN_ERROR_NOMEM = -1,
	JSMN_ERROR_INVAL = -2,
	JSMN_ERROR_PART = -3
} jsmnerr;

typedef enum json_token_type_enum
{	JSON_UNDEFINED,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_PRIMITIVE
} json_token_type_enum;

typedef struct json_token
{	enum json_token_type_enum Type;
	signed short Start;
	signed short End;
	signed short Size;
	signed short parent;
	plcbit cached;
} json_token;

typedef struct jsmntok_t
{	enum json_token_type_enum type;
	signed short start;
	signed short end;
	signed short size;
	signed short parent;
	plcbit cached;
} jsmntok_t;

typedef struct jsmn_callback
{	unsigned long* pFunction;
	unsigned long* pUserData;
} jsmn_callback;

typedef struct jsmn_parser
{	unsigned long pos;
	unsigned long toknext;
	signed short toksuper;
	struct jsmn_callback callback;
	plcbit isValue;
	unsigned long* pcache;
	unsigned long endpos;
	plcstring cache[1001];
} jsmn_parser;

typedef struct jsmn_callback_advanced_data
{	unsigned long* pValue;
	unsigned long ValueLen;
} jsmn_callback_advanced_data;

typedef struct jsmn_callback_data
{	plcstring Name[321];
	enum json_token_type_enum Type;
	plcstring Value[321];
	unsigned char Levels;
	plcstring Structure[16][321];
	unsigned long Size;
	struct jsmn_callback_advanced_data Advanced;
} jsmn_callback_data;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC signed short JsmnInit(unsigned long parser);
_BUR_PUBLIC signed short JsmnParse(unsigned long parser, unsigned long js, unsigned short len, unsigned long tokens, unsigned short num_tokens);
_BUR_PUBLIC signed short JsonParse(unsigned long pJsonString, unsigned long pTokenArray);
_BUR_PUBLIC signed short JsonTokenEqual(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName);
_BUR_PUBLIC signed short JsonGetValue(unsigned long pJsonString, unsigned long pTokenArray, unsigned long pName, unsigned long pStrValue, unsigned long StrLength);


#ifdef __cplusplus
};
#endif
#endif /* _JSMN_PARSE_ */

