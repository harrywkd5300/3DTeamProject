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
	// ������ ����ü�� �񱳸� ���� �׸��� ������ ����.
	void Update_Component(const _float& fTimeDelta);
	void LastUpdate_Component(const _float& fTimeDelta); // �ٸ� ���� ���� �Լ����� �ѹ� ȣ��
	_bool isIn_Frustum(const _vec3* pPositionInWorldSpace, _float fRadius, bool* pbCheck = nullptr);
	_bool isIn_Frustum_Local(const _matrix& matWorld, const _vec3* pPositionInLocal, _float fRadius);
private:
	_vec3				m_vPoint[8]; // ����ü�� 8�� ������(���彺���̽�)
	_vec3				m_vOriginalPoint[8]; // ����ü�� 8�� ������(Clip Space == z ��������� ��ģ ���� ����)..
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