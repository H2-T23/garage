// MainFrame.h 

#include "UseMFC.h"

#include "OpenGLView.h"


class CMainFrame : public CFrameWnd {
public:
	CMainFrame( void );
	virtual ~CMainFrame( void );
	
protected:
	DECLARE_DYNCREATE(CMainFrame)

	COpenGLView		m_wndView;
	
public:
	virtual BOOL	PreCreateWindow( CREATESTRUCT& cs );
	
protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg int		OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void	OnSetFocus( CWnd* pOldWnd );
};

