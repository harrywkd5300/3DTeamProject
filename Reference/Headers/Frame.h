#pragma once

#include "Engine_Defines.h"
#include "Base.h"

// ���ʿ� ������ ������ŭ�� Ʈ�縦 �����Ѵ�.

BEGIN(Engine)

class CFrame : public CBase
{
private:
	explicit CFrame(void);
	virtual ~CFrame(void);
public:
	HRESULT Initialize(const float& fCallCnt);
	bool Permit_Call(const float& fTimeDelta);
private:
	float		m_fTimeAcc;
	float		m_fTimePerSec;

public: // ������ ���õ� �Լ�.
	static CFrame* Create(const float& fCallCnt);
private:
	virtual _ulong Free(void);

};

END
