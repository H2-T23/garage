#pragma once

#include "Panel.h"
#include "Log.h"

/**********************************************************************************
 *
 *
 *
 */
class CPropertyPanel : public CPanel {
	DECLARE_DYNAMIC(CPropertyPanel)

public:
	enum {
		IDC_BUTTON1	= 1001, IDC_BUTTON2,
	};

public:
	CPropertyPanel();
	virtual ~CPropertyPanel();

protected:
//	BOOL	PreCreateWindow(CREATESTRUCT cs){
//		if( !CWnd::PreCreateWindow(cs) )
//			return FALSE;
//
//		return TRUE;
//	}
	CButton		m_btn1, m_btn2;

	int		OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( CPanel::OnCreate(lpCreateStruct) == -1 ){
			return -1;
		}

		CSize	szBtn(100, 30);
		DWORD	dwStyle	= (WS_CHILD | WS_VISIBLE);

		if( !m_btn1.Create(_T("Start"), dwStyle, CRect(CPoint(10,10), szBtn), this, IDC_BUTTON1) )
			return -1;

		if( !m_btn2.Create(_T("Suspend"), dwStyle, CRect(CPoint(10, 50), szBtn), this, IDC_BUTTON2) )
			return -1;

		m_btn2.EnableWindow( FALSE );

		return 0;
	}


	afx_msg void	OnBtnClick1( void ){
		CString		str;
		m_btn1.GetWindowText( str );
		if( str.Compare(_T("Start")) == 0 )
		{
			CLogManager::GetInstance().BeginLogReceiver();
			m_btn1.SetWindowText(_T("Stop"));
			m_btn2.EnableWindow( TRUE );
		}
		else
		{
			CLogManager::GetInstance().TerminateLogReceiver();
			m_btn1.SetWindowText(_T("Start"));
			m_btn2.EnableWindow( FALSE );
		}
	}

	afx_msg void	OnBtnClick2( void ){
		CString		str;
		m_btn2.GetWindowText( str );
		if( str.Compare(_T("Suspend")) == 0 )
		{
			CLogManager::GetInstance().SuspendLogReceiver();
			m_btn2.SetWindowText(_T("Resume"));
		}
		else
		{
			CLogManager::GetInstance().ResumeLogReceiver();
			m_btn2.SetWindowText(_T("Suspend"));
		}
	}

protected:
	DECLARE_MESSAGE_MAP()
};


