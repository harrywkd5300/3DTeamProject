#pragma once

#include "MyEffect.h"

BEGIN(Engine)

class CTexture;
class CCamera_Manager;
class CRenderer;
class DLL_EXPORT CMyEffect_Particle final : public CMyEffect
{
private:
	explicit CMyEffect_Particle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMyEffect_Particle(const CMyEffect_Particle& rhs);
	virtual ~CMyEffect_Particle(void) = default;

public: // 툴용
	void			Set_TexComponent(CTexture* pTexture);
	virtual void	Reset_Info(void) final;
	virtual void	Set_ParentMatrix(const D3DXMATRIX* pMatParent) final;

public:
	virtual HRESULT		Ready_GameObject(const EFFECT_INFO& tInfo) final;
	virtual _int		Update_GameObject(const _float& fTimeDelta) final;
	virtual void		Render_GameObject(void) final;

private:
	CTexture*			m_pTextureCom = nullptr;
	CCamera_Manager*	m_pCam_Manager = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

private:
	PTC_INFO		m_tPtcInfo;
	
	_uint			m_iViewportHeight = 0;

	_float			m_fTimeDelta = 0.f;

	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;

	
	JWVECTOR<PTC_ATTRIBUTE*>	m_listParticles;
	JWVECTOR<PTC_ATTRIBUTE*>	m_listRecycles;
	//int					m_listParticlesSize = 0;
	//int					m_listRecyclesSize = 0;

	//list<PTC_ATTRIBUTE*>			m_listParticles;
	//typedef list<PTC_ATTRIBUTE*>	LISTPARTICLES;

	//list<PTC_ATTRIBUTE*>			m_listRecycles;
	//typedef list<PTC_ATTRIBUTE*>	LISTRECYCLES;

private:
	HRESULT			Ready_Component(void);
	virtual void	Add_Particle(void);
	void			Set_Attribute(PTC_ATTRIBUTE* pAtt); // 인자가 각기 다르므로 시스템 개별 함수
	virtual HRESULT	Init_System(void) final;

	//FORCEINLINE void			Push_Back_Particle(PTC_ATTRIBUTE* pAtt);
	//PTC_ATTRIBUTE*				Pop_Back_Particle();
	//PTC_ATTRIBUTE*				Pop_Particle(int iIndex);
	//FORCEINLINE PTC_ATTRIBUTE*	Get_Particle(int iIndex);
	//FORCEINLINE PTC_ATTRIBUTE*	Get_Back_Particle();

	//FORCEINLINE void			Push_Back_Recycle(PTC_ATTRIBUTE* pAtt);
	//PTC_ATTRIBUTE*				Pop_Back_Recycle();



private:
	void	SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
	void	Render_Buffer(void);
	_bool	Is_Inside(const _vec3& vPos);
	//void	Remove_DeadParticles(void);

private:
	void	Curve_Alpha(PTC_ATTRIBUTE* pAtt, float fTimeRatio);
	void	Curve_Size(PTC_ATTRIBUTE* pAtt, float fTimeRatio);
	void	Curve_Speed(PTC_ATTRIBUTE* pAtt, float fTimeRatio);
	void	Curve_Color(PTC_ATTRIBUTE* pAtt, float fTimeRatio);
	void	Curve_Bright(PTC_ATTRIBUTE* pAtt, float fTimeRatio);

public:
	static CMyEffect_Particle* Create(LPDIRECT3DDEVICE9 pGraphicDev, const EFFECT_INFO& tInfo);

public:
	virtual CMyEffect* Clone(void) final;
#ifndef _FINAL
public:
	void Debug_Render();
	int iStartCount = 0;
#endif
protected:
	virtual _ulong Free(void) final;
};

END