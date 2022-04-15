#pragma once

#include "Engine_Defines.h"
#include "Base.h"

// 일초에 지정한 갯수만큼만 트루를 리턴한다.

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

public: // 생성과 관련된 함수.
	static CFrame* Create(const float& fCallCnt);
private:
	virtual _ulong Free(void);

};

END
