#include "stdafx.h"
#pragma once

#include "render.h"
#include "Thunderbolt.h"
#include "igame_persistent.h"
#include "LightAnimLibrary.h"

#include "igame_level.h"
#include "xr_area.h"
#include "xr_object.h"

static const float MAX_DIST_FACTOR = 0.95f;

SThunderboltDesc::SThunderboltDesc(CInifile* pIni, LPCSTR sect)
{
	name						= sect;
	color_anim					= LALib.FindItem (pIni->r_string ( sect,"color_anim" )); VERIFY(color_anim);
	color_anim->fFPS			= (float)color_anim->iFrameCount;
    m_GradientTop.shader 		= pIni->r_string ( sect,"gradient_top_shader" );
    m_GradientTop.texture		= pIni->r_string ( sect,"gradient_top_texture" );
    m_GradientTop.fRadius		= pIni->r_fvector2(sect,"gradient_top_radius"  );
    m_GradientTop.fOpacity 		= pIni->r_float	 ( sect,"gradient_top_opacity" );
    m_GradientTop.m_pFlare->CreateShader(*m_GradientTop.shader, *m_GradientTop.texture);
    m_GradientCenter.shader 	= pIni->r_string ( sect,"gradient_center_shader" );
    m_GradientCenter.texture	= pIni->r_string ( sect,"gradient_center_texture" );
    m_GradientCenter.fRadius	= pIni->r_fvector2(sect,"gradient_center_radius"  );
    m_GradientCenter.fOpacity 	= pIni->r_float	 ( sect,"gradient_center_opacity" );
    m_GradientCenter.m_pFlare->CreateShader(*m_GradientCenter.shader, *m_GradientCenter.texture);

    // models
    LPCSTR m_name;
	m_name				= pSettings->r_string(sect,"lightning_model");
    m_pRender->CreateModel(m_name);

    // sound
	m_name				= pSettings->r_string(sect,"sound");
    if (m_name&&m_name[0]) snd.create(m_name,st_Effect,sg_Undefined);
}

SThunderboltDesc::~SThunderboltDesc()
{
    m_pRender->DestroyModel();
    m_GradientTop.m_pFlare->DestroyShader();
    m_GradientCenter.m_pFlare->DestroyShader();
    snd.destroy						();
}

//----------------------------------------------------------------------------------------------
// collection
//----------------------------------------------------------------------------------------------
SThunderboltCollection::SThunderboltCollection(CInifile* pIni, LPCSTR sect)
{
	section			= sect;
	int tb_count	= pIni->line_count(sect);
	for (int tb_idx=0; tb_idx<tb_count; tb_idx++){
		LPCSTR		N, V;
		if (pIni->r_line(sect,tb_idx,&N,&V))
			palette.push_back(xr_new<SThunderboltDesc>(pIni,N));
	}
}
SThunderboltCollection::~SThunderboltCollection()
{
	for (DescIt d_it=palette.begin(); d_it!=palette.end(); d_it++)
		xr_delete(*d_it);
	palette.clear	();
}

//----------------------------------------------------------------------------------------------
// thunderbolt effect
//----------------------------------------------------------------------------------------------
CEffect_Thunderbolt::CEffect_Thunderbolt()
{
	current		= 0;
	life_time	= 0.f;
    state		= stIdle;
    next_lightning_time = 0.f;
    bEnabled	= FALSE;

    // params
    p_var_alt		= pSettings->r_fvector2							( "thunderbolt_common","altitude" );  
	p_var_alt.x		= deg2rad(p_var_alt.x); p_var_alt.y	= deg2rad(p_var_alt.y);
    p_var_long		= deg2rad	(				 pSettings->r_float	( "thunderbolt_common","delta_longitude" ));
    p_min_dist		= _min		(MAX_DIST_FACTOR,pSettings->r_float	( "thunderbolt_common","min_dist_factor" ));
    p_tilt			= deg2rad	(pSettings->r_float					( "thunderbolt_common","tilt" ));
    p_second_prop	= pSettings->r_float							( "thunderbolt_common","second_propability" );
    clamp			(p_second_prop,0.f,1.f);
    p_sky_color		= pSettings->r_float							( "thunderbolt_common","sky_color" );
    p_sun_color		= pSettings->r_float							( "thunderbolt_common","sun_color" );
	p_fog_color		= pSettings->r_float							( "thunderbolt_common","fog_color" );
}

CEffect_Thunderbolt::~CEffect_Thunderbolt()
{
	for (CollectionVecIt d_it=collection.begin(); d_it!=collection.end(); d_it++)
    	xr_delete				(*d_it);
	collection.clear			();
}

int CEffect_Thunderbolt::AppendDef(CInifile* pIni, LPCSTR sect)
{
	if (!sect||(0==sect[0])) return -1;
	for (CollectionVecIt it=collection.begin(); it!=collection.end(); it++)
		if ((*it)->section==sect)	return int(it-collection.begin());
	collection.push_back		(xr_new<SThunderboltCollection>(pIni,sect));
	return collection.size()-1;
}

BOOL CEffect_Thunderbolt::RayPick(const Fvector& s, const Fvector& d, float& dist)
{
	BOOL bRes 	= TRUE;
#ifdef _EDITOR
    bRes 				= Tools->RayPick	(s,d,dist,0,0);
#else
	collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	bRes 				= g_pGameLevel->ObjectSpace.RayPick(s,d,dist,collide::rqtBoth,RQ,E);	
    if (bRes) dist	 	= RQ.range;
    else{
        Fvector N	={0.f,-1.f,0.f};
        Fvector P	={0.f,0.f,0.f};
        Fplane PL; PL.build(P,N);
        float dst	=dist;
        if (PL.intersectRayDist(s,d,dst)&&(dst<=dist)){dist=dst; return true;}else return false;
    }
#endif
    return bRes;
}
#define FAR_DIST g_pGamePersistent->Environment().CurrentEnv.far_plane

void CEffect_Thunderbolt::Bolt(int id, float period, float lt)
{
	VERIFY					(id>=0 && id<(int)collection.size());
	state 		            = stWorking;
	life_time	            = lt+Random.randF(-lt*0.5f,lt*0.5f);
    current_time            = 0.f;
    current		            = collection[id]->GetRandomDesc(); VERIFY(current);

    Fmatrix XF,S;
    Fvector pos,dev;
    float sun_h, sun_p; 
    g_pGamePersistent->Environment().CurrentEnv.sun_dir.getHP			(sun_h,sun_p);
    float alt	            = Random.randF(p_var_alt.x,p_var_alt.y);
    float lng	            = Random.randF(sun_h-p_var_long+PI,sun_h+p_var_long+PI); 
    float dist	            = Random.randF(FAR_DIST*p_min_dist,FAR_DIST*MAX_DIST_FACTOR);
    current_direction.setHP	(lng,alt);
    pos.mad		            (Device.vCameraPosition,current_direction,dist);
    dev.x		            = Random.randF(-p_tilt,p_tilt);
    dev.y		            = Random.randF(0,PI_MUL_2);
    dev.z		            = Random.randF(-p_tilt,p_tilt);
    XF.setXYZi	            (dev);               

    Fvector light_dir 		= {0.f,-1.f,0.f};
    XF.transform_dir		(light_dir);
    lightning_size			= FAR_DIST*2.f;
    RayPick					(pos,light_dir,lightning_size);

    lightning_center.mad	(pos,light_dir,lightning_size*0.5f);

    S.scale					(lightning_size,lightning_size,lightning_size);
    XF.translate_over		(pos);
    current_xform.mul_43	(XF,S);

    float next_v			= Random.randF();

    if (next_v<p_second_prop){
	    next_lightning_time = Device.fTimeGlobal+lt+EPS_L;
    }else{
	    next_lightning_time = Device.fTimeGlobal+period+Random.randF(-period*0.3f,period*0.3f);
		current->snd.play_no_feedback		(0,0,dist/300.f,&pos,0,0,&Fvector2().set(dist/2,dist*2.f));
    }


	current_direction.invert			();	// for env-sun
}

void CEffect_Thunderbolt::OnFrame(int id, float period, float duration)
{
	BOOL enabled			= (id>=0);
	if (bEnabled!=enabled){
    	bEnabled			= enabled;
	    next_lightning_time = Device.fTimeGlobal+period+Random.randF(-period*0.5f,period*0.5f);
    }else if (bEnabled&&(Device.fTimeGlobal>next_lightning_time)){ 
    	if (state==stIdle && (id>=0)) Bolt(id,period,duration);
    }
	if (state==stWorking){
    	if (current_time>life_time) state = stIdle;
    	current_time	+= Device.fTimeDelta;
		Fvector fClr;		
		int frame;
		u32 uClr		= current->color_anim->CalculateRGB(current_time/life_time,frame);
		fClr.set		(float(color_get_R(uClr))/255.f,float(color_get_G(uClr)/255.f),float(color_get_B(uClr)/255.f));

        lightning_phase	= 1.5f*(current_time/life_time);
        clamp			(lightning_phase,0.f,1.f);

        g_pGamePersistent->Environment().CurrentEnv.sky_color.mad(fClr,p_sky_color);
        g_pGamePersistent->Environment().CurrentEnv.sun_color.mad(fClr,p_sun_color);
		g_pGamePersistent->Environment().CurrentEnv.fog_color.mad(fClr,p_fog_color);

		if (::Render->get_generation()==IRender_interface::GENERATION_R2)	{
			g_pGamePersistent->Environment().CurrentEnv.sun_dir = current_direction;
			VERIFY2(g_pGamePersistent->Environment().CurrentEnv.sun_dir.y<0,"Invalid sun direction settings while CEffect_Thunderbolt");

		} 
    }
}

void CEffect_Thunderbolt::Render()
{
    if (state == stWorking)
        m_pRender->Render(*this);
}