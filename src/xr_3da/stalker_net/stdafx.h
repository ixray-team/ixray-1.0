// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <sdkddkver.h>

#ifndef VC_EXTRALEAN
#	define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif // VC_EXTRALEAN

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components

#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#	include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define NO_XRNEW
#undef	min
#undef	max
#include "../../xrCore/xrCore.h"