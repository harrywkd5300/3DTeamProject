#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CShader;
class CMyEffect;
class DLL_EXPORT CEffectOBJ : public CGameObject
{
protected:
	explicit CEffectOBJ(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffectOBJ(const CEffectOBJ& rhs); // 풀링을 위한 복사생성자
	virtual ~CEffectOBJ(void) = default;

public:
	void	Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	void	Set_Target_Transform(CTransform* pTargetTrans) { m_pTargetTrans = pTargetTrans; }
	void	Set_Frame_Pointer(const D3DXMATRIX* pMatFrame) { m_pMatFrame = pMatFrame; }
	void	Set_Matrix(const D3DXMATRIX& matFrame) { m_matParent = matFrame; }

public:
	void	Set_Trans_Type(EFFECT_INFO::TRANS_TYPE eType) { m_eTransType = eType; }
	void	Set_EffectTag(_tchar* pEffectTag) { m_pEffectTag = pEffectTag; }

public:
	list<CMyEffect*>*		Get_EffectList(void) { return &m_listEffects; }
	EFFECT_INFO::TRANS_TYPE	Get_Trans_type(void) { return m_eTransType; }
	_tchar*					Get_EffectTag(void) { return m_pEffectTag; }


public:
	HRESULT		Add_Effect(CMyEffect* pNewEffect);
	void		Reset(void);

public:
	virtual HRESULT Ready_GameObject(void);
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(LPD3DXEFFECT	pEffect);

protected:
	//CTransform*		m_pTransformCom = nullptr; // 자기 자신
	CRenderer*		m_pRendererCom = nullptr;
	CShader*		m_pShaderCom = nullptr;

protected:
	EFFECT_INFO::TRANS_TYPE		m_eTransType = EFFECT_INFO::TYPE_END;
	_tchar*		m_pEffectTag = nullptr;

protected:
	// 1) 뼈 따라가는 이펙트 경우
	CGameObject*		m_pTarget = nullptr;
	CTransform*			m_pTargetTrans = nullptr;
	const D3DXMATRIX*	m_pMatFrame = nullptr;

	// 2) 단발성 이펙트 경우  + 위치 이펙트인 경우(트랜스 행렬로)
	D3DXMATRIX		m_matParent;

	bool	m_bIsClone = false;
protected:
	list<CMyEffect*>	m_listEffects;
	//list<CMyEffect*>	m_listRecycleEffects;
	typedef list<CMyEffect*> LISTEFFECTS;

protected:
	virtual HRESULT		Ready_Component(void);

public:
	static CEffectOBJ*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual CEffectOBJ* Clone(void);
#ifndef _FINAL
public:
	void Debug_Render();
	int iStartCount = 0;
#endif
protected:
	virtual _ulong		Free(void) override;
};

END

