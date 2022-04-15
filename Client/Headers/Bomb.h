#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;		
	class CMesh_Static;
	class CShader;
}

class CBomb : public CGameObject
{
private:
	explicit CBomb(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBomb() = default;

public:
	void Set_bThrowing(void) { m_bThrowing = false; }

public:
	bool Get_bThrowing(void) { return m_bThrowing; }
	
public:
	virtual HRESULT Ready_GameObject(_matrix* pBone_mat, CTransform* pMonster_Transform) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;


private:
	CTransform*						m_pMonster_Transform = nullptr;
	_matrix*						m_pMonster_Bone_mat = nullptr;

	_matrix							m_Result_mat;

	_vec3							m_vMonster_Look;

	_bool							m_bThrowing = true;
	_bool							m_bInit_Angle = true;

	_float							m_fSpeed = 0.f;
	
	_float							m_fSpeed_Y = 0.f;
	_float							m_fTempSpeed_Y = 0.f;
	_float							m_fSpeed_Minus_Y = 0.f;

	
private:
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CBomb* Create(LPDIRECT3DDEVICE9 pGraphicDev, _matrix* pBone_mat, CTransform* pMonster_Transform);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

