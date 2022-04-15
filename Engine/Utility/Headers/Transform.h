// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "Component.h"
#include "VIBuffer.h"
#include "Navigation.h"

// 월드변환행렬의 제작 변환을 지속적으로 수행할 수 있게 해준다.

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
	D3DXQUATERNION		m_QtRot;	// 회전 정보값을 가지고 있는 쿼터니언
	_vec3				m_vInfo[INFO_END]; // 위치,크기,회전 정보
	_matrix				m_matParent;		// 부모 월드 매트릭스
	_matrix				m_matWorld;			// 최종 월드 매트릭스
	_matrix				m_matWorld_FORCED;	// (해당프레임내) 값 변경 이후 계산된 월드 매트릭스
	bool				m_bUpdateWorld = false; // (해당프레임내) 값 변경 이후 m_matWorld 가 계산 되었는지 체크
	//bool				m_bUpdateWorld_FORCED = false; // (해당프레임내) 값 변경 이후 m_matWorld_FORCED 가 계산 되었는지 체크
	
public:

	FORCEINLINE const _vec3* Get_Infomation(INFOMATION eInfo) const { return &m_vInfo[eInfo]; }
	FORCEINLINE const _vec3* Get_WorldRightVec() const { // 월드매트릭스의 오른쪽 방향벡터
		return (_vec3*)&m_matWorld.m[0][0];
	}
	FORCEINLINE const _vec3* Get_WorldUpVec() const { // 월드매트릭스의 위쪽 방향벡터
		return (_vec3*)&m_matWorld.m[1][0];
	}
	FORCEINLINE const _vec3* Get_WorldLookVec() const { // 월드매트릭스의 앞쪽 방향벡터
		return (_vec3*)&m_matWorld.m[2][0];
	}
	_matrix Get_LocalMatrix();
	FORCEINLINE _matrix Get_WorldMatrix() const
	{
		return m_matWorld;
	}
	_matrix Get_WorldMatrix_NotRot() const // 회전행렬 없는 월드매트릭스 얻어오기
	{
		_matrix matWorld = m_matWorld;

		_vec3	vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

		memcpy(&matWorld.m[0][0], &(vRight * D3DXVec3Length((_vec3*)&matWorld.m[0][0])), sizeof(_vec3));
		memcpy(&matWorld.m[1][0], &(vUp * D3DXVec3Length((_vec3*)&matWorld.m[1][0])), sizeof(_vec3));
		memcpy(&matWorld.m[2][0], &(vLook * D3DXVec3Length((_vec3*)&matWorld.m[2][0])), sizeof(_vec3));

		return matWorld;
	}
	_matrix Get_WorldMatrix_NotScale() const // 스케일 적용 안된 월드매트릭스 얻어오기
	{
		_matrix matWorld = m_matWorld;

		D3DXVec3Normalize((_vec3*)matWorld.m[0], (_vec3*)matWorld.m[0]);
		D3DXVec3Normalize((_vec3*)matWorld.m[2], (_vec3*)matWorld.m[1]);
		D3DXVec3Normalize((_vec3*)matWorld.m[3], (_vec3*)matWorld.m[2]);

		return matWorld;
	}
	_matrix Get_RotationMatrix() const	// 회전 행렬 얻어오기
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
	// ------------ (START) 값 변경이 일어나는 함수들 -------------------
	void Update_WorldMatrix(); // 월드 매트릭스 갱신
	void Update_WorldMatrix(_matrix* pMatrix); // 월드 매트릭스 갱신
	
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

	// 이펙트 빌보드 전용, 그외에 사용하는 경우 매우 위험(꼭, 프레임워크에게)
	// 사용시, 기존에 적용했던 Set_Information() 정보, m_matParent가 전부 날라가고
	// 입력된 매트릭스에서 받아온 정보로 대체 된다.
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
	// ------------ (END) 값 변경이 일어나는 함수들 -------------------

	
public: 
	// ------------ (START)값 변경 함수 호출 이후 변경된 값을 즉시 가져오고 싶을때 사용하는 함수들 --------- 
	const _vec3* Get_WorldRightVec_FORCED(); // 즉시 계산된 월드매트릭스의 오른쪽 방향벡터
	const _vec3* Get_WorldUpVec_FORCED(); // 즉시 계산된 월드매트릭스의 위쪽 방향벡터
	const _vec3* Get_WorldLookVec_FORCED(); // 즉시 계산된 월드매트릭스의 앞쪽 방향벡터
	_matrix Get_WorldMatrix_FORCED(); // 즉시 계산된 월드 매트릭스 얻어오기
	_matrix Get_WorldMatrix_NotRot_FORCED(); // 즉시 계산된 회전행렬 없는 월드매트릭스 얻어오기
	_matrix Get_WorldMatrix_NotScale_FORCED();// 즉시 계산된 스케일 없는 월드매트릭스 얻어오기
	_matrix Get_RotationMatrix_FORCED();	// 즉시 계산된 회전 행렬 얻어오기
	// ------------ (END)값 변경 함수 호출 이후 변경된 값을 즉시 가져오고 싶을때 사용하는 함수들 --------- 

public:
	_float Compute_Distance(const _matrix* pWorldMatrix) const; // 길이 계산 함수

	void SetUp_OnGraphicDev(void);
	void SetUp_OnGraphicDev_FORCED(void);
	void SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName); // 월드매트릭스 쉐이더에 인풋
	void SetUp_OnShader_FORCED(LPD3DXEFFECT pEffect, const char* pConstantName); // 즉시 계산된 월드매트릭스 쉐이더에 인풋
	
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