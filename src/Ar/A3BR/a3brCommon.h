/*
 * File: a3brCommon.h
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of A3BR, licensed under the MIT License.
 * 
 */

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif

#ifdef __cplusplus
	};
#endif

typedef void (* A3brCallback)( UDINT, LLHttpHeader_typ * header, unsigned char * data, A3BR_API_VERSION_enum apiVersion);
