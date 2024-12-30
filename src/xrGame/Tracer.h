// Tracer.h: interface for the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
#define AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_
#pragma once

class CBulletManager;

#include "ui_defs.h"

class CTracer
{
	friend CBulletManager;
protected:
	ui_shader			sh_Tracer;
	xr_vector<u32>		m_aColors;
public:
						CTracer		();
						~CTracer	();

						void Render(const Fvector& pos, const Fvector& center, const Fvector& dir, float length, float width, u8 colorID);
};

#endif // !defined(AFX_TRACER_H__AC263474_060A_489E_B84A_E879CD630A1E__INCLUDED_)
