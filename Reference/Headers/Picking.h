#pragma once

#include "Component.h"

BEGIN(Engine)
class CVIBuffer;
class DLL_EXPORT CPicking : public CComponent
{
private:
	explicit CPicking(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPicking(const CPicking& rhs);
	virtual ~CPicking() = default;
public:
	HRESULT Ready_Picking(void);

	
	//_vec3* Picking_ToBuffer(HWND hWnd, _vec3* vOutPos, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);
	//_vec3* Picking_ToBuffer(HWND hWnd, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);

	//��ŷ�� ��� ��ŷ�� ���彺���̽� ������ ���Ͱ� vOutPos�� ä������ bool�� ��ȯ
	bool Picking_ToBuffer(HWND hWnd, _vec3* vOutPos, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);

	//��ŷ�� ��� ��ŷ�� ���彺���̽� ������ ���Ͱ� vOutPos��, �Ÿ����� pfDist�� ä������ bool ��ȯ
	bool Picking_ToMesh(HWND hWnd, _vec3* vOutPos, _float* pfDist, LPD3DXMESH* ppMesh, const _matrix* pWorldMatrix);

	//��ŷ�� ��� bool ��ȯ
	bool Chekc_Pick_ToMesh(HWND hWnd, LPD3DXMESH* ppMesh, const _matrix * pWorldMatrix);
public:
	virtual CComponent* Clone(void);
public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);
};

END