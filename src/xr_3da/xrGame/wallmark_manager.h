#pragma once

using SHADER_VECTOR = xr_vector<ref_shader>;
using SHADER_VECTOR_IT = SHADER_VECTOR::iterator;

class CWalmarkManager
{
private:
	SHADER_VECTOR					m_wallmarks;
	Fvector							m_pos;
public:
	CObject*						m_owner;
				CWalmarkManager		()																																					;
				~CWalmarkManager	()																																					;
		void	Load				(LPCSTR section)																																	;
		void	Clear				()																																					;
		void	AddWallmark			(const Fvector& dir, const Fvector& start_pos, float range, float wallmark_size,SHADER_VECTOR& wallmarks_vector,int t)								;
//		void	PlaceWallmark		(const Fvector& dir, const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)			;
//		void	PlaceWallmark		(const Fvector& dir, const Fvector& start_pos, float trace_dist, float wallmark_size,CObject* ignore_obj)											;
//		void	PlaceWallmarks		( const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)								;
		void	PlaceWallmarks		( const Fvector& start_pos);
		
		void	__stdcall StartWorkflow	();
//		void	PlaceWallmarks		(const Fvector& start_pos,CObject* ignore_obj)																										;
};