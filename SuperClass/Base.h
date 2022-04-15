#pragma once
#include <functional>

// ��� ������Ʈ�� �����ϴ� �������� Ŭ������ �θ𰡵ȴ�.
// RefCnt(��������)�� �����ϸ�, �����Ѵ�.
// �������� : ��ü���� �ּҸ� �����ϴ� �������� ������ ����
// (������ü�� �ּҸ� �����ϴ� �����ͺ����� ��� �������� �����Ѵ�.)

// abstract : Class�� �߻�Ŭ����ȭ ��Ų��.

typedef std::function<void(void)> Func;

class _declspec(dllexport) CBase abstract
{
private:
	inline unsigned long MakeKey(void); //�����Ҷ� ����ϴ� �Լ�(������)
protected:
	inline explicit CBase(void);
	inline virtual ~CBase(void);
public:
	inline unsigned long GetRefCnt(void) const; // ���� ���۷��� ī��Ʈ ��ȯ
	inline unsigned long GetKey(void) const; // ��ü ���� Key ��ȯ

	// RefCnt�� ������Ų��.
	// ���ϰ� : ���������� ������Ų ������ ���� ����.
	inline unsigned long AddRef(void);
	// RefCnt�� ���ҽ�Ű��, RefCnt == 0�̶�� ��ü�� �����Ѵ�.
	// ���ϰ� : ���ҽ�Ű�� ������ ���� �����Ѵ�.
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