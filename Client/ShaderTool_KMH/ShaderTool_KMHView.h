
// ShaderTool_KMHView.h : CShaderTool_KMHView Ŭ������ �������̽�
//

#pragma once


class CShaderTool_KMHView : public CView
{
protected: // serialization������ ��������ϴ�.
	CShaderTool_KMHView();
	DECLARE_DYNCREATE(CShaderTool_KMHView)

// Ư���Դϴ�.
public:
	CShaderTool_KMHDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CShaderTool_KMHView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ShaderTool_KMHView.cpp�� ����� ����
inline CShaderTool_KMHDoc* CShaderTool_KMHView::GetDocument() const
   { return reinterpret_cast<CShaderTool_KMHDoc*>(m_pDocument); }
#endif

