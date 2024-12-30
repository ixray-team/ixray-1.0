#include "stdafx.h"
#pragma once

#include "Rain.h"
#include "igame_persistent.h"
#include "environment.h"

#include "render.h"
#include "igame_level.h"
#include "xr_area.h"
#include "xr_object.h"


//	Warning: duplicated in dxRainRender
static const int	max_desired_items	= 2500;
static const float	source_radius		= 12.5f;
static const float	source_offset		= 40.f;
static const float	max_distance		= source_offset*1.25f;
static const float	sink_offset			= -(max_distance-source_offset);
static const float	drop_length			= 5.f;
static const float	drop_width			= 0.30f;
static const float	drop_angle			= 3.0f;
static const float	drop_max_angle		= deg2rad(10.f);
static const float	drop_max_wind_vel	= 20.0f;
static const float	drop_speed_min		= 40.f;
static const float	drop_speed_max		= 80.f;

const int	max_particles		= 1000;
const int	particles_cache		= 400;
const float particles_time		= .3f;
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffect_Rain::CEffect_Rain()
{
	state							= stIdle;
	
	snd_Ambient.create				("ambient\\rain",st_Effect,sg_Undefined);

	// Moved to p_Render constructor
	p_create						();
}

CEffect_Rain::~CEffect_Rain()
{
	snd_Ambient.destroy				();

	// Cleanup
	p_destroy						();
	// Moved to p_Render destructor
}

// Born
void	CEffect_Rain::Born		(Item& dest, float radius)
{
	Fvector		axis;	
    axis.set			(0,-1,0);
	float gust			= g_pGamePersistent->Environment().wind_strength_factor/10.f;
	float k				= g_pGamePersistent->Environment().CurrentEnv.wind_velocity*gust/drop_max_wind_vel;
	clamp				(k,0.f,1.f);
	float	pitch		= drop_max_angle*k-PI_DIV_2;
    axis.setHP			(g_pGamePersistent->Environment().CurrentEnv.wind_direction,pitch);
    
	Fvector&	view	= Device.vCameraPosition;
	float		angle	= ::Random.randF	(0,PI_MUL_2);
	float		dist	= ::Random.randF	(); dist = _sqrt(dist)*radius; 
	float		x		= dist*_cos		(angle);
	float		z		= dist*_sin		(angle);
	dest.D.random_dir	(axis,deg2rad(drop_angle));
	dest.P.set			(x+view.x-dest.D.x*source_offset,source_offset+view.y,z+view.z-dest.D.z*source_offset);
//	dest.P.set			(x+view.x,height+view.y,z+view.z);
	dest.fSpeed			= ::Random.randF	(drop_speed_min,drop_speed_max);

	float height		= max_distance;
	RenewItem			(dest,height,RayPick(dest.P,dest.D,height,collide::rqtBoth));
}

BOOL CEffect_Rain::RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
	BOOL bRes 			= TRUE;

	collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	bRes 				= g_pGameLevel->ObjectSpace.RayPick( s,d,range,tgt,RQ,E);	
    if (bRes) range 	= RQ.range;
    return bRes;
}

void CEffect_Rain::RenewItem(Item& dest, float height, BOOL bHit)
{
	dest.uv_set			= Random.randI(2);
    if (bHit){
		dest.dwTime_Life= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit	= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.Phit.mad	(dest.P,dest.D,height);
	}else{
		dest.dwTime_Life= Device.dwTimeGlobal + iFloor(1000.f*height/dest.fSpeed) - Device.dwTimeDelta;
		dest.dwTime_Hit	= Device.dwTimeGlobal + iFloor(2*1000.f*height/dest.fSpeed)-Device.dwTimeDelta;
		dest.Phit.set	(dest.P);
	}
}

void	CEffect_Rain::OnFrame	()
{
#ifndef _EDITOR
	if (!g_pGameLevel)			return;
#endif
	// Parse states
	float	factor				= g_pGamePersistent->Environment().CurrentEnv.rain_density;
	float	hemi_factor			= 1.f;
#ifndef _EDITOR
	CObject* E 					= g_pGameLevel->CurrentViewEntity();
	if (E&&E->renderable_ROS())
		hemi_factor				= 1.f-2.0f*(0.3f-_min(_min(1.f,E->renderable_ROS()->get_luminocity_hemi()),0.3f));
#endif

	switch (state)
	{
	case stIdle:		
		if (factor<EPS_L)		return;
		state					= stWorking;
		snd_Ambient.play		(0,sm_Looped);
		snd_Ambient.set_range	(source_offset,source_offset*2.f);
	break;
	case stWorking:
		if (factor<EPS_L){
			state				= stIdle;
			snd_Ambient.stop	();
			return;
		}
		break;
	}

	// ambient sound
	if (snd_Ambient._feedback()){
		Fvector					sndP;
		sndP.mad				(Device.vCameraPosition,Fvector().set(0,1,0),source_offset);
		snd_Ambient.set_position(sndP);
		snd_Ambient.set_volume	(1.1f*factor*hemi_factor);
	}
}

//#include "xr_input.h"
void	CEffect_Rain::Render()
{
	if (!g_pGameLevel)
		return;

	m_pRender->Render(*this);
}

// startup _new_ particle system
void	CEffect_Rain::Hit		(Fvector& pos)
{
	if (0!=::Random.randI(2))	return;
	Particle*	P	= p_allocate();
	if (!P)
		return;

	const Fsphere& bv_sphere = m_pRender->GetDropBounds();

	P->time						= particles_time;
	P->mXForm.rotateY			(::Random.randF(PI_MUL_2));
	P->mXForm.translate_over	(pos);
	P->mXForm.transform_tiny	(P->bounds.P,bv_sphere.P);
	P->bounds.R					= bv_sphere.R;
}

// initialize particles pool
void CEffect_Rain::p_create		()
{
	// pool
	particle_pool.resize	(max_particles);
	for (u32 it=0; it<particle_pool.size(); it++)
	{
		Particle&	P	= particle_pool[it];
		P.prev			= it?(&particle_pool[it-1]):0;
		P.next			= (it<(particle_pool.size()-1))?(&particle_pool[it+1]):0;
	}
	
	// active and idle lists
	particle_active	= 0;
	particle_idle	= &particle_pool.front();
}

// destroy particles pool
void CEffect_Rain::p_destroy	()
{
	// active and idle lists
	particle_active	= 0;
	particle_idle	= 0;
	
	// pool
	particle_pool.clear	();
}

// _delete_ node from _list_
void CEffect_Rain::p_remove	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	Particle*	prev		= P->prev;	P->prev = NULL;
	Particle*	next		= P->next;	P->next	= NULL;
	if (prev) prev->next	= next;
	if (next) next->prev	= prev;
	if (LST==P)	LST			= next;
}

// insert node at the top of the head
void CEffect_Rain::p_insert	(Particle* P, Particle* &LST)
{
	VERIFY		(P);
	P->prev					= 0;
	P->next					= LST;
	if (LST)	LST->prev	= P;
	LST						= P;
}

// determine size of _list_
int CEffect_Rain::p_size	(Particle* P)
{
	if (0==P)	return 0;
	int cnt = 0;
	while (P)	{
		P	=	P->next;
		cnt +=	1;
	}
	return cnt;
}

// alloc node
CEffect_Rain::Particle*	CEffect_Rain::p_allocate	()
{
	Particle*	P			= particle_idle;
	if (0==P)				return NULL;
	p_remove	(P,particle_idle);
	p_insert	(P,particle_active);
	return		P;
}

// xr_free node
void	CEffect_Rain::p_free(Particle* P)
{
	p_remove	(P,particle_active);
	p_insert	(P,particle_idle);
}
