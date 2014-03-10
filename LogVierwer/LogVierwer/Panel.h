#pragma once
/**********************************************************************************
 *
 *
 *
 */
class CPanel : public CWnd {
	DECLARE_DYNAMIC(CPanel)
protected:

public:
	CPanel();
	virtual ~CPanel();

protected:
	BOOL	PreCreateWindow( CREATESTRUCT& cs ){
		if( !CWnd::PreCreateWindow(cs) ){
			return FALSE;
		}

	//	cs.style	|= (WS_CLIPCHILDREN);

		return TRUE;
	}

protected:
	DECLARE_MESSAGE_MAP()
};


