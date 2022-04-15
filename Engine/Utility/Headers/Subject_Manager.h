#pragma once


#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CObserver;

class DLL_EXPORT CSubject_Manager : public CBase
{

public:	
	enum SUBJECTTYPE { TYPE_STATIC, TYPE_DYNAMIC, TYPE_END }; // Scene���� ���� 
	// TYPE_DYNAMIC �� ��쿡�� ���� ������ �Ź� �����־���Ѵ�. // Stage ���� ���� ������ �����͸� ���������.
	// Scene�� Free���� Delete_DynamicData �Լ� ����
	
	DECLARE_SINGLETON(CSubject_Manager)
private:
	explicit CSubject_Manager();
	virtual ~CSubject_Manager() = default;
	
public:
	list<void*>*	GetDataList(int iMessage, SUBJECTTYPE eType); // ����� ����Ʈ Get�Լ�

public: // Observer
	void AddObserver(CObserver* pObserver, SUBJECTTYPE eType); // ������Ʈ ���� , Static �� Dynamic ���� 
	void RemoveObserver(CObserver* pObserver, SUBJECTTYPE eType); // ������Ʈ�� ����Ʈ���� ���� -> ������Ʈ�� ��쿡�� ���ڻ����� ��ü���� �Ҹ�
	void Notify(int iMessage, SUBJECTTYPE eType, void* pData = NULL); // �������� ������ ���ߴٸ� ȣ���� �Լ�.
	
public: // Data
	void AddData(int iMessage, SUBJECTTYPE eType, void* pData); // ������ ����
	void RemoveData(int iMessage, SUBJECTTYPE eType, void* pData); // ������ �����

	void ReleaseData(SUBJECTTYPE eType); // Type�� �´� ����� ������ ���� �����

	void Delete_DynamicData(); // Dynamic Data ���� �����
private:
	map<_int, list<void*>> m_mapData[TYPE_END]; // Data�� ������ ����Ʈ���� // -> List�� ����ִµ� ���� ����غ��� �� �Ͱ���.
	typedef map<_int, list<void*>> MAPSUBJECT; 
	
	list<CObserver*>	m_ListObserver[TYPE_END]; // �������� ������ ����Ʈ����
	typedef list<CObserver*> OBSERVERLIST;

public:
	virtual _ulong Free(); // ���������� �����͸� �����Ҵ�� �޸𸮸� ������������. // �� ��ü���� �Ҹ� ���ֱ� ����.
	

};

END