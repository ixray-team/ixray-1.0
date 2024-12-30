// xrAPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../Include/xrApi/xrAPI.h"

XRAPI_API IRender_interface*	Render = nullptr;
XRAPI_API IRenderFactory*	RenderFactory = nullptr;
XRAPI_API CDUInterface*	DU = nullptr;
XRAPI_API xr_token*	vid_mode_token = nullptr;
XRAPI_API IUIRender* UIRender = nullptr;
//XRAPI_API CSound_manager_interface*	Sound = nullptr;
XRAPI_API CGameMtlLibrary*	PGMLib = nullptr;

#ifdef DEBUG
	XRAPI_API IDebugRender*	DRender = nullptr;
#endif // DEBUG
