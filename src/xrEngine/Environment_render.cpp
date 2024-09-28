#include "stdafx.h"
#pragma hdrstop

#include "Environment.h"
#include "render.h"

#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"

#include "igame_level.h"

//-----------------------------------------------------------------------------
// Environment render
//-----------------------------------------------------------------------------
extern float psHUD_FOV;

void CEnvironment::RenderSky		()
{
	if (!g_pGameLevel)		
		return;

	m_pRender->RenderSky(*this);
}

void CEnvironment::RenderClouds			()
{
	if (!g_pGameLevel)		
		return;

	m_pRender->RenderClouds(*this);
}

void CEnvironment::RenderFlares		()
{
	if (!g_pGameLevel)			
		return;

	// 1
	eff_LensFlare->Render			(FALSE,TRUE,TRUE);
}

void CEnvironment::RenderLast		()
{
	if (!g_pGameLevel)			
		return;

	// 2
	eff_Rain->Render				();
	eff_Thunderbolt->Render			();
}

void CEnvironment::OnDeviceCreate()
{
//.	bNeed_re_create_env			= TRUE;
	m_pRender->OnDeviceCreate();

	// weathers
	{
		EnvsMapIt _I,_E;
		_I		= WeatherCycles.begin();
		_E		= WeatherCycles.end();
		for (; _I!=_E; _I++)
			for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
				(*it)->on_device_create();
	}
	// effects
	{
		EnvsMapIt _I,_E;
		_I		= WeatherFXs.begin();
		_E		= WeatherFXs.end();
		for (; _I!=_E; _I++)
			for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
				(*it)->on_device_create();
	}


	Invalidate	();
	OnFrame		();
}

void CEnvironment::OnDeviceDestroy()
{
	m_pRender->OnDeviceDestroy();

	// weathers
	{
		EnvsMapIt _I,_E;
		_I		= WeatherCycles.begin();
		_E		= WeatherCycles.end();
		for (; _I!=_E; _I++)
			for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
				(*it)->on_device_destroy();
	}
	// effects
	{
		EnvsMapIt _I,_E;
		_I		= WeatherFXs.begin();
		_E		= WeatherFXs.end();
		for (; _I!=_E; _I++)
			for (EnvIt it=_I->second.begin(); it!=_I->second.end(); it++)
				(*it)->on_device_destroy();
	}
	CurrentEnv.destroy();

}

#ifdef _EDITOR
void CEnvironment::ED_Reload()
{
	OnDeviceDestroy			();
	OnDeviceCreate			();
}
#endif

