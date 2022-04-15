#pragma once

// For.Ÿ�̸ӵ��� ������ �����ϱ����� ������� Ŭ����
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


public: //�߰��ϰڴ�.
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
