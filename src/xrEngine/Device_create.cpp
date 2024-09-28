#include "stdafx.h"

#include "render.h"
#include "..\include\xrRender\RenderFactory.h"

void SetupGPU(IRenderDeviceRender* pRender)
{
	// Command line
	char* lpCmdLine = Core.Params;

	BOOL bForceGPU_SW;
	BOOL bForceGPU_NonPure;
	BOOL bForceGPU_REF;

	if (strstr(lpCmdLine, "-gpu_sw") != NULL)
		bForceGPU_SW = TRUE;
	else
		bForceGPU_SW = FALSE;

	if (strstr(lpCmdLine, "-gpu_nopure") != NULL)
		bForceGPU_NonPure = TRUE;
	else
		bForceGPU_NonPure = FALSE;

	if (strstr(lpCmdLine, "-gpu_ref") != NULL)
		bForceGPU_REF = TRUE;
	else
		bForceGPU_REF = FALSE;

	pRender->SetupGPU(bForceGPU_SW, bForceGPU_NonPure, bForceGPU_REF);
}

void CRenderDevice::_SetupStates	()
{
	// General Render States
	mView.identity			();
	mProject.identity		();
	mFullTransform.identity	();
	vCameraPosition.set		(0,0,0);
	vCameraDirection.set	(0,0,1);
	vCameraTop.set			(0,1,0);
	vCameraRight.set		(1,0,0);

	m_pRender->SetupStates();
}

void CRenderDevice::_Create	(LPCSTR shName)
{
	Memory.mem_compact			();

	// after creation
	b_is_Ready					= TRUE;
	_SetupStates				();

	m_pRender->OnDeviceCreate(shName);

	dwFrame						= 0;
}

void CRenderDevice::Create	() 
{
	if (b_is_Ready)		return;		// prevent double call
	Statistic			= xr_new<CStats>();
	if (!m_pRender)
		m_pRender = RenderFactory->CreateRenderDeviceRender();

	SetupGPU(m_pRender);
	Log					("Starting RENDER device...");

#ifdef _EDITOR
	psCurrentVidMode[0]	= dwWidth;
	psCurrentVidMode[1] = dwHeight;
#endif
	fFOV				= 90.f;
	fASPECT				= 1.f;

	m_pRender->Create(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2, true);

	string_path			fname; 
	FS.update_path		(fname,"$game_data$","shaders.xr");

	//////////////////////////////////////////////////////////////////////////
	_Create				(fname);

	PreCache			(0);
}
