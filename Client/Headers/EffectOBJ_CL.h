//#pragma once
//
//#include "Defines.h"
//#include "EffectOBJ.h"
//
//namespace Engine
//{
//	class CMyEffect;
//}
//
//class CEffectOBJ_CL final : public Engine::CEffectOBJ
//{
//private:
//	explicit CEffectOBJ_CL(LPDIRECT3DDEVICE9 pGraphicDev);
//	virtual ~CEffectOBJ_CL(void) = default;
//
//public:
//	virtual HRESULT		Add_Effect(CMyEffect* pNewEffect) final;
//	//virtual void		Init_EffectList(Engine::CMyEffect* pOriginEffect, _uint& iListIndex) final;
//
//public:
//	virtual HRESULT	Ready_GameObject(void) final;
//	virtual _int	Update_GameObject(const _float& fTimeDelta);
//	virtual void	Render_GameObject(void);
//
//private:
//	HRESULT		Ready_Component(void);
//	void		Reset_EffectList(_uint& iListIndex);
//public:
//	virtual HRESULT		Ready_EffectComponent(Engine::CMyEffect* pNewEffect) final;
//
//public:
//	static CEffectOBJ_CL*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
//#ifndef _FINAL
//public:
//	void Debug_Render();
//#endif
//private:
//	virtual _ulong		Free(void) final;
//};
//
