#pragma once
#include <functional>

// 모든 프로젝트에 존재하는 내가만든 클래스의 부모가된다.
// RefCnt(참조갯수)를 보관하며, 관리한다.
// 참조갯수 : 객체들의 주소를 보관하는 포인터형 변수의 갯수
// (원본객체의 주소를 보관하는 포인터변수의 경우 갯수에서 제외한다.)

// abstract : Class를 추상클래스화 시킨다.

typedef std::function<void(void)> Func;

class _declspec(dllexport) CBase abstract
{
private:
	inline unsigned long MakeKey(void); //생성할때 사용하는 함수(사용금지)
protected:
	inline explicit CBase(void);
	inline virtual ~CBase(void);
public:
	inline unsigned long GetRefCnt(void) const; // 현재 레퍼런스 카운트 반환
	inline unsigned long GetKey(void) const; // 객체 고유 Key 반환

	// RefCnt를 증가시킨다.
	// 리턴값 : 참조갯수를 증가시킨 이후의 값을 리턴.
	inline unsigned long AddRef(void);
	// RefCnt를 감소시키되, RefCnt == 0이라면 객체를 삭제한다.
	// 리턴값 : 감소시키기 이전의 값을 리턴한다.
	inline unsigned long Release(void);
private:
	unsigned long	m_dwRefCnt = 0;
	unsigned long	m_dwKey = 0;

public:
	virtual unsigned long Free(void) = 0;

#ifndef _FINAL

public:
	void Debug_Render() {};

	typedef struct tag_DebugNode
	{
		const wchar_t* str = nullptr;
		Func debug_func;
		//void(*func)(void) = nullptr;
		tag_DebugNode* pNextNode = nullptr;
		tag_DebugNode* pPrevNode = nullptr;

		void Delete()
		{
			if (pNextNode != nullptr)
			{
				pNextNode->Delete();
			}
			delete this;
		}

		static void Tag(tag_DebugNode*& pDebugNode, const wchar_t* pTagStr, Func func_Debug_Render)
		{
			while (pDebugNode->pNextNode != nullptr)
			{
				pDebugNode = pDebugNode->pNextNode;
			}
			tag_DebugNode* newNode = new tag_DebugNode;
			pDebugNode->pNextNode = newNode;
			newNode->pPrevNode = pDebugNode;
			newNode->pNextNode = nullptr;
			newNode->str = pTagStr;
			newNode->debug_func = func_Debug_Render;
			pDebugNode = newNode;
		}

		static Func Set_NextFunc(tag_DebugNode*& pDebugNode)
		{
			if (pDebugNode->pNextNode != nullptr)
			{
				pDebugNode = pDebugNode->pNextNode;
			}
			return pDebugNode->debug_func;
		}
		static Func Set_PrevFunc(tag_DebugNode*& pDebugNode)
		{
			if (pDebugNode->pPrevNode != nullptr)
			{
				pDebugNode = pDebugNode->pPrevNode;
			}
			return pDebugNode->debug_func;
		}
		static Func Set_LeafFunc(tag_DebugNode*& pDebugNode)
		{
			while (pDebugNode->pNextNode != nullptr)
			{
				pDebugNode = pDebugNode->pNextNode;
			}
			return pDebugNode->debug_func;
		}
		static Func Get_LeafFunc(tag_DebugNode*& pDebugNode)
		{
			tag_DebugNode* tmpNode = pDebugNode;
			while (tmpNode->pNextNode != nullptr)
			{
				tmpNode = tmpNode->pNextNode;
			}
			return tmpNode->debug_func;
		}
		static void Set_LeafNode(tag_DebugNode*& pDebugNode)
		{
			while (pDebugNode->pNextNode != nullptr)
			{
				pDebugNode = pDebugNode->pNextNode;
			}
		}
		static void Set_TopNode(tag_DebugNode*& pDebugNode)
		{
			while (pDebugNode->pPrevNode != nullptr)
			{
				pDebugNode = pDebugNode->pPrevNode;
			}
		}
		static const wchar_t* Get_CurTagName(tag_DebugNode*& pDebugNode)
		{
			return pDebugNode->str;
		}
		static const wchar_t* Get_LeafTagName(tag_DebugNode*& pDebugNode)
		{
			tag_DebugNode* tmpNode = pDebugNode;
			while (tmpNode->pNextNode != nullptr)
			{
				tmpNode = tmpNode->pNextNode;
			}
			return tmpNode->str;
		}

	}DEBUGNODE;



private:
	tag_DebugNode* m_pDebugNode = nullptr;

	void DestroyDebugNode()
	{
		m_pDebugNode->Delete();
	};
protected:
	void Tag(const wchar_t* pTagStr, Func func_Debug_Render)
	{
		DEBUGNODE::Tag(m_pDebugNode, pTagStr, func_Debug_Render);
	}

public:
	Func Set_NextFunc()
	{
		return DEBUGNODE::Set_NextFunc(m_pDebugNode);
	}
	Func Set_PrevFunc()
	{
		return DEBUGNODE::Set_PrevFunc(m_pDebugNode);
	}
	Func Set_LeafFunc()
	{
		return DEBUGNODE::Set_LeafFunc(m_pDebugNode);
	}
	Func Get_LeafFunc()
	{
		return DEBUGNODE::Get_LeafFunc(m_pDebugNode);
	}
	void Set_LeafNode()
	{
		DEBUGNODE::Set_LeafNode(m_pDebugNode);
	}
	void Set_TopNode()
	{
		DEBUGNODE::Set_TopNode(m_pDebugNode);
	}
	const wchar_t* Get_CurTagName()
	{
		return DEBUGNODE::Get_CurTagName(m_pDebugNode);
	}
	const wchar_t* Get_LeafTagName()
	{
		return DEBUGNODE::Get_LeafTagName(m_pDebugNode);
	}
#endif
};

#include "Base.inl"