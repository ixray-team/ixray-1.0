#pragma once

class CCustomRocket;
class CGameObject;

class CRocketLauncher
{
public:
	CRocketLauncher		();
	~CRocketLauncher	();

	virtual void Load	(LPCSTR section);

			void AttachRocket	(u16 rocket_id, CGameObject* parent_rocket_launcher);
			void DetachRocket	(u16 rocket_id, bool bLaunch);

			void SpawnRocket	(LPCSTR rocket_section, CGameObject* parent_rocket_launcher);
			void LaunchRocket	(const Fmatrix& xform,  const Fvector& vel, const Fvector& angular_vel);

protected:			   
	using ROCKET_VECTOR = xr_vector<CCustomRocket*>;
	using ROCKETIT = ROCKET_VECTOR::iterator;

	ROCKET_VECTOR		m_rockets;
	ROCKET_VECTOR		m_launched_rockets;
	
	CCustomRocket*	getCurrentRocket();
	void			dropCurrentRocket();
	u32				getRocketCount();
	//��������� ��������, ����������� ������ �� �����
	//������
	float			m_fLaunchSpeed;

};