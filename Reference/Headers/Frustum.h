#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CFrustum final : public CComponent
{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFrustum(const CFrustum& rhs);
	virtual ~CFrustum() = default;
public:
	HRESULT Ready_Frustum(void);
	// 지형과 절두체의 비교를 통해 그릴지 말지를 선택.
	void Update_Component(const _float& fTimeDelta);
	void LastUpdate_Component(const _float& fTimeDelta); // 다른 곳의 렌더 함수에서 한번 호출
	_bool isIn_Frustum(const _vec3* pPositionInWorldSpace, _float fRadius, bool* pbCheck = nullptr);
	_bool isIn_Frustum_Local(const _matrix& matWorld, const _vec3* pPositionInLocal, _float fRadius);
private:
	_vec3				m_vPoint[8]; // 절두체의 8개 꼭지점(월드스페이스)
	_vec3				m_vOriginalPoint[8]; // 절두체의 8개 꼭지점(Clip Space == z 나누기까지 거친 투영 공간)..
	D3DXPLANE			m_PlaneArr[6];
	bool				m_bUpdate = false;
private:	
	void SetUp_PointsInProjection(void);
	void SetUp_PointsInWorldSpace(void);

public:
	static CFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
public:
	virtual _ulong Free(void);


};

END