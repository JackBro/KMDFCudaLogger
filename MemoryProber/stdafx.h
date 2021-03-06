// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <SDKDDKVer.h>

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef void *  PVOID;
typedef unsigned char  BOOLEAN;
typedef unsigned long  DWORD;

typedef struct _KEYBOARD_INPUT_DATA {
	union {
		ULONG rawValue;
		struct {
			USHORT UnitId;
			USHORT MakeCode;
			USHORT Flags;
			USHORT Reserved;
		};
	};
	ULONG ExtraInformation;
} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;

typedef struct _LLIST {
	struct _LLIST *previous;
	union {
		PKEYBOARD_INPUT_DATA keyboardBuffer;
		USHORT * keyboardFlag;
	};
} LLIST, *PLLIST;


DWORD WINAPI xmitBuffer(LPVOID voidPointer);
