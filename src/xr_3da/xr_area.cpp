#include "stdafx.h"

#include "xr_area.h"
#include "xr_object.h"
#include "xrLevel.h"
#include "xr_collide_form.h"

using namespace	collide;

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace	( )
#ifdef PROFILE_CRITICAL_SECTIONS
	:Lock(MUTEX_PROFILE_ID(CObjectSpace::Lock))
#endif // PROFILE_CRITICAL_SECTIONS
{
#ifdef DEBUG
	sh_debug.create				("debug\\wireframe","$null");
#endif
	m_BoundingVolume.invalidate	();
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace	( )
{
#ifdef DEBUG
	sh_debug.destroy			();
#endif
}
//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest	( xr_vector<CObject*>&	q_nearest, const Fvector &point, float range, CObject* ignore_object )
{
	// Query objects
	q_nearest.resize(0);
	Fsphere				Q;	Q.set	(point,range);
	Fvector				B;	B.set	(range,range,range);
	g_SpatialSpace->q_box(r_spatial,0,STYPE_COLLIDEABLE,point,B);

	// Iterate
	xr_vector<ISpatial*>::iterator	it	= r_spatial.begin	();
	xr_vector<ISpatial*>::iterator	end	= r_spatial.end		();
	for (; it!=end; it++)		{
		CObject* O				= (*it)->dcast_CObject		();
		if (0==O)				continue;
		if (O==ignore_object)	continue;
		Fsphere mS				= { O->spatial.sphere.P, O->spatial.sphere.R	};
		if (Q.intersect(mS))	q_nearest.push_back(O);
	}

	return q_nearest.size();
}
//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest( xr_vector<CObject*>&	q_nearest, ICollisionForm* obj, float range)
{
	CObject*	O		= obj->Owner	();
	return				GetNearest( q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O );
}

void CObjectSpace::Load(CDB::build_callback build_callback) {
	Load("$level$", "level.cform", build_callback);
}

void	CObjectSpace::Load(LPCSTR path, LPCSTR fname, CDB::build_callback build_callback) {
#ifdef USE_ARENA_ALLOCATOR
	Msg("CObjectSpace::Load, g_collision_allocator.get_allocated_size() - %d", int(g_collision_allocator.get_allocated_size() / 1024.0 / 1024));
#endif // #ifdef USE_ARENA_ALLOCATOR
	IReader* F = FS.r_open(path, fname);
	R_ASSERT(F);
	Load(F, build_callback);
}

void CObjectSpace::Load(IReader* F, CDB::build_callback build_callback) {
	hdrCFORM H;
	F->r(&H, sizeof(hdrCFORM));
	Fvector* verts = (Fvector*)F->pointer();
	CDB::TRI* tris = (CDB::TRI*)(verts + H.vertcount);
	Create(verts, tris, H, build_callback);
	FS.r_close(F);
}


void CObjectSpace::Create(Fvector* verts, CDB::TRI* tris, const hdrCFORM& H, CDB::build_callback build_callback) {
	R_ASSERT(CFORM_CURRENT_VERSION == H.version);
	Static.build(verts, H.vertcount, tris, H.facecount, build_callback);
	m_BoundingVolume.set(H.aabb);
	g_SpatialSpace->initialize(m_BoundingVolume);
	g_SpatialSpacePhysic->initialize(m_BoundingVolume);
}


//void CObjectSpace::Load	()
//{
//	IReader *F					= FS.r_open	("$level$", "level.cform");
//	R_ASSERT					(F);
//
//	hdrCFORM					H;
//	F->r						(&H,sizeof(hdrCFORM));
//	Fvector*	verts			= (Fvector*)F->pointer();
//	CDB::TRI*	tris			= (CDB::TRI*)(verts+H.vertcount);
//	R_ASSERT					(CFORM_CURRENT_VERSION==H.version);
//	Static.build				( verts, H.vertcount, tris, H.facecount, build_callback );
//
//	m_BoundingVolume.set				(H.aabb);
//	g_SpatialSpace->initialize			(m_BoundingVolume);
//	g_SpatialSpacePhysic->initialize	(m_BoundingVolume);
//	FS.r_close					(F);
//}
//----------------------------------------------------------------------
#ifdef DEBUG
void CObjectSpace::dbgRender()
{
	R_ASSERT(bDebug);

	RCache.set_Shader(sh_debug);
	for (u32 i=0; i<q_debug.boxes.size(); i++)
	{
		Fobb&		obb		= q_debug.boxes[i];
		Fmatrix		X,S,R;
		obb.xform_get(X);
		RCache.dbg_DrawOBB(X,obb.m_halfsize,D3DCOLOR_XRGB(255,0,0));
		S.scale		(obb.m_halfsize);
		R.mul		(X,S);
		RCache.dbg_DrawEllipse(R,D3DCOLOR_XRGB(0,0,255));
	}
	q_debug.boxes.clear();

	for (u32 i=0; i<dbg_S.size(); i++)
	{
		std::pair<Fsphere,u32>& P = dbg_S[i];
		Fsphere&	S = P.first;
		Fmatrix		M;
		M.scale		(S.R,S.R,S.R);
		M.translate_over(S.P);
		RCache.dbg_DrawEllipse(M,P.second);
	}
	dbg_S.clear();
}
#endif
