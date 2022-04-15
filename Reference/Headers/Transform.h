// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "Component.h"
#include "VIBuffer.h"
#include "Navigation.h"

// ���庯ȯ����� ���� ��ȯ�� ���������� ������ �� �ְ� ���ش�.

BEGIN(Engine)

class DLL_EXPORT CTransform : public CComponent
{
public:
	enum INFOMATION {INFO_POSITION, INFO_SCALE, INFO_ANGLE, INFO_END};
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
private:
	D3DXQUATERNION		m_QtRot;	// ȸ�� �������� ������ �ִ� ���ʹϾ�
	_vec3				m_vInfo[INFO_END]; // ��ġ,ũ��,ȸ�� ����
	_matrix				m_matParent;		// �θ� ���� ��Ʈ����
	_matrix				m_matWorld;			// ���� ���� ��Ʈ����
	_matrix				m_matWorld_FORCED;	// (�ش������ӳ�) �� ���� ���� ���� ���� ��Ʈ����
	bool				m_bUpdateWorld = false; // (�ش������ӳ�) �� ���� ���� m_matWorld �� ��� �Ǿ����� üũ
	//bool				m_bUpdateWorld_FORCED = false; // (�ش������ӳ�) �� ���� ���� m_matWorld_FORCED �� ��� �Ǿ����� üũ
	
public:

	FORCEINLINE const _vec3* Get_Infomation(INFOMATION eInfo) const { return &m_vInfo[eInfo]; }
	FORCEINLINE const _vec3* Get_WorldRightVec() const { // �����Ʈ������ ������ ���⺤��
		return (_vec3*)&m_matWorld.m[0][0];
	}
	FORCEINLINE const _vec3* Get_WorldUpVec() const { // �����Ʈ������ ���� ���⺤��
		return (_vec3*)&m_matWorld.m[1][0];
	}
	FORCEINLINE const _vec3* Get_WorldLookVec() const { // �����Ʈ������ ���� ���⺤��
		return (_vec3*)&m_matWorld.m[2][0];
	}
	_matrix Get_LocalMatrix();
	FORCEINLINE _matrix Get_WorldMatrix() const
	{
		return m_matWorld;
	}
	_matrix Get_WorldMatrix_NotRot() const // ȸ����� ���� �����Ʈ���� ������
	{
		_matrix matWorld = m_matWorld;

		_vec3	vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

		memcpy(&matWorld.m[0][0], &(vRight * D3DXVec3Length((_vec3*)&matWorld.m[0][0])), sizeof(_vec3));
		memcpy(&matWorld.m[1][0], &(vUp * D3DXVec3Length((_vec3*)&matWorld.m[1][0])), sizeof(_vec3));
		memcpy(&matWorld.m[2][0], &(vLook * D3DXVec3Length((_vec3*)&matWorld.m[2][0])), sizeof(_vec3));

		return matWorld;
	}
	_matrix Get_WorldMatrix_NotScale() const // ������ ���� �ȵ� �����Ʈ���� ������
	{
		_matrix matWorld = m_matWorld;

		D3DXVec3Normalize((_vec3*)matWorld.m[0], (_vec3*)matWorld.m[0]);
		D3DXVec3Normalize((_vec3*)matWorld.m[2], (_vec3*)matWorld.m[1]);
		D3DXVec3Normalize((_vec3*)matWorld.m[3], (_vec3*)matWorld.m[2]);

		return matWorld;
	}
	_matrix Get_RotationMatrix() const	// ȸ�� ��� ������
	{
		_matrix matWorld = m_matWorld;

		_vec3	vPosition(0.f, 0.f, 0.f);

		memcpy(&matWorld.m[3][0], &(vPosition), sizeof(_vec3));

		return matWorld;
	}
	FORCEINLINE _matrix Get_ParentMatrix() const
	{
		return m_matParent;
	}
	_bool Get_IsUpdateWorld( void ) const {
		return m_bUpdateWorld;
	}
	void Clamp_Angle();
private:
	// ------------ (START) �� ������ �Ͼ�� �Լ��� -------------------
	void Update_WorldMatrix(); // ���� ��Ʈ���� ����
	void Update_WorldMatrix(_matrix* pMatrix); // ���� ��Ʈ���� ����
	
public:

	FORCEINLINE void Set_Infomation(INFOMATION eInfo, const _vec3* pVector) {
		memcpy(&m_vInfo[eInfo], pVector, sizeof(_vec3));
		m_bUpdateWorld = false;
		if (eInfo == INFO_ANGLE)
			Clamp_Angle();
	}

	FORCEINLINE void Set_Infomation(INFOMATION eInfo, const _float& fX, const _float& fY, const _float& fZ) {
		m_vInfo[eInfo].x = fX; m_vInfo[eInfo].y = fY; m_vInfo[eInfo].z = fZ;
		m_bUpdateWorld = false;
		if (eInfo == INFO_ANGLE)
			Clamp_Angle();
	}
	FORCEINLINE void Add_Infomation(INFOMATION eInfo, const _vec3* pVector) {
		m_vInfo[eInfo] += *pVector;
		m_bUpdateWorld = false;
		if (eInfo == INFO_ANGLE)
			Clamp_Angle();
	}
	FORCEINLINE void Add_Infomation(INFOMATION eInfo, const _float& fX, const _float& fY, const _float& fZ) {
		m_vInfo[eInfo].x += fX; m_vInfo[eInfo].y += fY; m_vInfo[eInfo].z += fZ;
		m_bUpdateWorld = false;
		if (eInfo == INFO_ANGLE)
			Clamp_Angle();
	}

	void Add_Angle(D3DXQUATERNION pQuaterRot);
	void Set_Angle(D3DXQUATERNION pQuaterRot);
	void Set_Angle(_matrix matRotationWorld);

	// ����Ʈ ������ ����, �׿ܿ� ����ϴ� ��� �ſ� ����(��, �����ӿ�ũ����)
	// ����, ������ �����ߴ� Set_Information() ����, m_matParent�� ���� ���󰡰�
	// �Էµ� ��Ʈ�������� �޾ƿ� ������ ��ü �ȴ�.
	void Set_WorldMatrix_ForEffect(_matrix matWorld) { 	
		m_matWorld = matWorld;
		m_matWorld_FORCED = m_matWorld;
		D3DXQuaternionRotationMatrix(&m_QtRot, &matWorld);
		
		m_vInfo[INFO_SCALE].x = D3DXVec3Length((_vec3*)m_matWorld.m[0]);
		m_vInfo[INFO_SCALE].y = D3DXVec3Length((_vec3*)m_matWorld.m[1]);
		m_vInfo[INFO_SCALE].z = D3DXVec3Length((_vec3*)m_matWorld.m[2]);

		Set_Angle(m_QtRot);
	
		m_vInfo[INFO_POSITION] = *(_vec3*)m_matWorld.m[3];
		D3DXMatrixIdentity(&m_matParent);

		m_bUpdateWorld = true;
	}


	FORCEINLINE void Set_ParentMatrix(const _matrix* pParentMatrix)
	{
		if (nullptr == pParentMatrix)
			return;
		m_matParent = *pParentMatrix;
		m_bUpdateWorld = false;
	}

	HRESULT Ready_Transform(void);

	void Go_Straight(const _float& fSpeed, const _float& fTimeDelta) {
		_vec3	vDirection;
		m_vInfo[INFO_POSITION] += *D3DXVec3Normalize(&vDirection, (_vec3*)&m_matWorld.m[2][0]) * fSpeed * fTimeDelta;
		m_bUpdateWorld = false;
	}

	void Go_Right(const _float& fSpeed, const _float& fTimeDelta) {
		_vec3	vDirection;
		m_vInfo[INFO_POSITION] += *D3DXVec3Normalize(&vDirection, (_vec3*)&m_matWorld.m[0][0]) * fSpeed * fTimeDelta;
		m_bUpdateWorld = false;
	}

	void Go_Direction(_vec3 vDirection, const _float& fSpeed, const _float& fTimeDelta) {
		D3DXVec3Normalize(&vDirection, &vDirection);
		m_vInfo[INFO_POSITION] += vDirection * fSpeed * fTimeDelta;
		m_bUpdateWorld = false;
	}

	void Go_Direction_NotNormalize(_vec3 vDirection, const _float& fSpeed, const _float& fTimeDelta)
	{
		m_vInfo[INFO_POSITION] += vDirection * fSpeed * fTimeDelta;
		m_bUpdateWorld = false;
	}

	
	FORCEINLINE void Rotation_X(const _float& fAngle, const _float& fTimeDelta = 1.f) {
		m_vInfo[INFO_ANGLE].x = fAngle;
		m_bUpdateWorld = false;
		Clamp_Angle();
	}
	FORCEINLINE void Rotation_Y(const _float& fAngle, const _float& fTimeDelta = 1.f) {
		m_vInfo[INFO_ANGLE].y = fAngle;
		m_bUpdateWorld = false;
		Clamp_Angle();
	}
	FORCEINLINE void Rotation_Z(const _float& fAngle, const _float& fTimeDelta = 1.f) {
		m_vInfo[INFO_ANGLE].z = fAngle;
		m_bUpdateWorld = false;
		Clamp_Angle();
	}
	void Move_OnBuffer(Engine::CVIBuffer* pTargetBuffer);
	void Move_OnNavigation(Engine::CNavigation* pNavigation);
	// ------------ (END) �� ������ �Ͼ�� �Լ��� -------------------

	
public: 
	// ------------ (START)�� ���� �Լ� ȣ�� ���� ����� ���� ��� �������� ������ ����ϴ� �Լ��� --------- 
	const _vec3* Get_WorldRightVec_FORCED(); // ��� ���� �����Ʈ������ ������ ���⺤��
	const _vec3* Get_WorldUpVec_FORCED(); // ��� ���� �����Ʈ������ ���� ���⺤��
	const _vec3* Get_WorldLookVec_FORCED(); // ��� ���� �����Ʈ������ ���� ���⺤��
	_matrix Get_WorldMatrix_FORCED(); // ��� ���� ���� ��Ʈ���� ������
	_matrix Get_WorldMatrix_NotRot_FORCED(); // ��� ���� ȸ����� ���� �����Ʈ���� ������
	_matrix Get_WorldMatrix_NotScale_FORCED();// ��� ���� ������ ���� �����Ʈ���� ������
	_matrix Get_RotationMatrix_FORCED();	// ��� ���� ȸ�� ��� ������
	// ------------ (END)�� ���� �Լ� ȣ�� ���� ����� ���� ��� �������� ������ ����ϴ� �Լ��� --------- 

public:
	_float Compute_Distance(const _matrix* pWorldMatrix) const; // ���� ��� �Լ�

	void SetUp_OnGraphicDev(void);
	void SetUp_OnGraphicDev_FORCED(void);
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName); // �����Ʈ���� ���̴��� ��ǲ
	void SetUp_OnShader_FORCED(LPD3DXEFFECT pEffect, const char* pConstantName); // ��� ���� �����Ʈ���� ���̴��� ��ǲ
	
public:
	virtual void Update_Component(const _float& fTimeDelta);

private:
	virtual CComponent* Clone(void);
public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);
};

END