#pragma once

struct CFontManager;
class CUICursor;

#include "../Include/xrRender/UIRender.h"
#include "../Include/xrRender/FactoryPtr.h"

#include "ui_defs.h"

class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					()	{Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);};
	virtual				~CDeviceResetNotifier					()	{Device.seqDeviceReset.Remove(this);};
	virtual void		OnDeviceReset							()	{};

};


class ui_core: public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	Fvector2		m_pp_scale_;
	Fvector2		m_scale_;
	Fvector2*		m_current_scale;

	IC float		ClientToScreenScaledX			(float left)				{return left * m_current_scale->x;};
	IC float		ClientToScreenScaledY			(float top)					{return top * m_current_scale->y;};
public:
	xr_stack<Frect> m_Scissors;
	
					ui_core							();
					~ui_core						();
	CFontManager*	Font							()							{return m_pFontManager;}
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	void			ClientToScreenScaled			(Fvector2& dest, float left, float top);
	void			ClientToScreenScaled			(Fvector2& src_and_dest);
	void			ClientToScreenScaledWidth		(float& src_and_dest);
	void			ClientToScreenScaledHeight		(float& src_and_dest);

	Frect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return (m_bPostprocess)?m_2DFrustumPP:m_2DFrustum;}
	void			PushScissor						(const Frect& r, bool overlapped=false);
	void			PopScissor						();

	void			pp_start						();
	void			pp_stop							();
	void			RenderFont						();

	virtual void	OnDeviceReset					();
	static	bool	is_16_9_mode					();
	static	float	get_current_kx					();
	shared_str		get_xml_name					(LPCSTR fn);
};

extern CUICursor*	GetUICursor						();
extern ui_core*		UI								();
