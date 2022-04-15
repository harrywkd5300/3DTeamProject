
// ShaderTool_KMHView.h : CShaderTool_KMHView 클래스의 인터페이스
//

#pragma once


class CShaderTool_KMHView : public CView
{
protected: // serialization에서만 만들어집니다.
	CShaderTool_KMHView();
	DECLARE_DYNCREATE(CShaderTool_KMHView)

// 특성입니다.
public:
	CShaderTool_KMHDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CShaderTool_KMHView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ShaderTool_KMHView.cpp의 디버그 버전
inline CShaderTool_KMHDoc* CShaderTool_KMHView::GetDocument() const
   { return reinterpret_cast<CShaderTool_KMHDoc*>(m_pDocument); }
#endif

