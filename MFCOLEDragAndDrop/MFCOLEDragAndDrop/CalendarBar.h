
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CCalendarBar �E�B���h�E

class CCalendarBar : public CWnd
{
// �R���X�g���N�V����
public:
	CCalendarBar();

// ����
protected:
	CMonthCalCtrl m_wndCalendar;
	int m_nMyCalendarsY;
	CImageList m_Images;

// �I�[�o�[���C�h
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID = (UINT)-1);
	virtual BOOL PreTranslateMessage(MSG *pMsg);

// ����
public:
	virtual ~CCalendarBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd *pOldWnd);

	DECLARE_MESSAGE_MAP()
};
