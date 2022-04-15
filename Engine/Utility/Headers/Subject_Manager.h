#pragma once


#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CObserver;

class DLL_EXPORT CSubject_Manager : public CBase
{

public:	
	enum SUBJECTTYPE { TYPE_STATIC, TYPE_DYNAMIC, TYPE_END }; // Scene별로 관리 
	// TYPE_DYNAMIC 의 경우에는 씬이 끝나면 매번 지워주어야한다. // Stage 같은 개별 씬들의 데이터를 지우기위함.
	// Scene의 Free에서 Delete_DynamicData 함수 실행
	
	DECLARE_SINGLETON(CSubject_Manager)
private:
	explicit CSubject_Manager();
	virtual ~CSubject_Manager() = default;
	
public:
	list<void*>*	GetDataList(int iMessage, SUBJECTTYPE eType); // 저장된 리스트 Get함수

public: // Observer
	void AddObserver(CObserver* pObserver, SUBJECTTYPE eType); // 오브젝트 저장 , Static 및 Dynamic 설정 
	void RemoveObserver(CObserver* pObserver, SUBJECTTYPE eType); // 오브젝트를 리스트에서 삭제 -> 오브젝트의 경우에는 각자생성한 객체에서 소멸
	void Notify(int iMessage, SUBJECTTYPE eType, void* pData = NULL); // 데이터의 정보가 변했다면 호출할 함수.
	
public: // Data
	void AddData(int iMessage, SUBJECTTYPE eType, void* pData); // 데이터 삽입
	void RemoveData(int iMessage, SUBJECTTYPE eType, void* pData); // 데이터 지우기

	void ReleaseData(SUBJECTTYPE eType); // Type에 맞는 저장된 데이터 전부 지우기

	void Delete_DynamicData(); // Dynamic Data 전부 지우기
private:
	map<_int, list<void*>> m_mapData[TYPE_END]; // Data를 보관할 리스트변수 // -> List로 담겨있는데 좀더 고민해봐야 할 것같다.
	typedef map<_int, list<void*>> MAPSUBJECT; 
	
	list<CObserver*>	m_ListObserver[TYPE_END]; // 옵저버를 보관할 리스트변수
	typedef list<CObserver*> OBSERVERLIST;

public:
	virtual _ulong Free(); // 프리에서는 데이터를 동적할당된 메모리를 해제하지않음. // 각 객체에서 소멸 해주기 위함.
	

};

END