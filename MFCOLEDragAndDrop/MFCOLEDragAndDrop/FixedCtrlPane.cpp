// FixedCtrlPane.cpp : ŽÀ‘•ƒtƒ@ƒCƒ‹
//


#include "FixedCtrlPane.h"

// CFixedCtrlPane
IMPLEMENT_DYNAMIC(CFixedCtrlPane, CWnd)

BEGIN_MESSAGE_MAP(CFixedCtrlPane, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


CFixedCtrlPane::CFixedCtrlPane()
{
	try
	{
		m_strClassName = AfxRegisterWndClass(
								CS_VREDRAW | CS_HREDRAW,
								::LoadCursor(NULL, IDC_ARROW),
								(HBRUSH) ::GetStockObject(WHITE_BRUSH),
								::LoadIcon(NULL, IDI_APPLICATION));
	}
	catch(CResourceException* pEx)
	{
		AfxMessageBox(_T("Couldn't register class! (Already registered?)"));
		pEx->Delete();
	}
}

CFixedCtrlPane::~CFixedCtrlPane()
{
}


