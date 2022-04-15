#pragma once

// For.타이머들을 여러개 보관하기위해 만들어진 클래스
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CFrame;
class DLL_EXPORT CFrame_Manager : public CBase
{
	DECLARE_SINGLETON(CFrame_Manager)
private:
	explicit CFrame_Manager(void);
	virtual ~CFrame_Manager(void);


public: //추가하겠다.
	HRESULT Add_Frame(const TCHAR* pFrameTag, const float& fCallCnt);
	bool Permit_Call(const TCHAR* pFrameTag, const float& fTimeDelta);
private:
	map<const TCHAR*, CFrame*>			m_mapFrame;
	typedef map<const TCHAR*, CFrame*>	MAPFRAME;
private:
	CFrame* Find_Frame(const TCHAR* pFrameTag);
private:
	virtual _ulong Free(void) final;
};

END
