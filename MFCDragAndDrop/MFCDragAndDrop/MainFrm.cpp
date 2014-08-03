
// MainFrm.cpp : CMainFrame �N���X�̎���
//

#include "stdafx.h"
#include "MFCDragAndDrop.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // �X�e�[�^�X ���C�� �C���W�P�[�^�[
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame �R���X�g���N�V����/�f�X�g���N�V����

CMainFrame::CMainFrame()
{
	// TODO: �����o�[�������R�[�h�������ɒǉ����Ă��������B
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Visual Studio 2005 �X�^�C���̃h�b�L���O �E�B���h�E�����L���ɂ��܂�
	CDockingManager::SetDockingMode( DT_SMART );
	// Visual Studio 2005 �X�^�C���̃h�b�L���O �E�B���h�E�̎�����\�������L���ɂ��܂�
	EnableAutoHidePanes( CBRS_ALIGN_ANY );

	EnableDocking( CBRS_ALIGN_ANY );

	// �t���[���̃N���C�A���g�̈�S�̂��߂�r���[���쐬���܂��B
	if( !m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL) )
	{
		TRACE0("�r���[ �E�B���h�E���쐬�ł��܂���ł����B\n");
		return -1;
	}


	BOOL	bResult = FALSE;
	
	DWORD	dwStyle		= (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME);
	DWORD	dwExStyle	= (0);

//	bResult	= m_wndFixedCtrl.Create( NULL, _T("FixedCtrlPane"), dwStyle, CRect(10, 10, 300, 300), this, 1001);
//	if( !bResult ){
//		return -1;
//	}

//	bResult	= m_wndFixedCtrl2.Create( NULL, _T("FixedCtrlPaneSub"), dwStyle, CRect(500,10, 300,300), this, 1002 );
//	if( !bResult ){
//		return -1;
//	}

#if 0
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("�c�[�� �o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;      // �쐬�ł��܂���ł����B
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("�X�e�[�^�X �o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: �c�[�� �o�[���h�b�L���O�\�ɂ��Ȃ��ꍇ�́A������ 3 �s���폜���Ă��������B
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#endif

	bResult	= m_wndListCtrlDock.Create( _T("ListCtrlDock"), this, CSize(300,300), TRUE, 2001 );
	if( !bResult ){
		return -1;
	}

	m_wndListCtrlDock.EnableDocking( CBRS_ALIGN_ANY );
	DockPane( &m_wndListCtrlDock );



	bResult	= m_wndTreeCtrlDock.Create( _T("TreeCtrlDock"), this, CSize(300,300), TRUE, 2002 );
	if( !bResult ){
		return -1;
	}

	m_wndTreeCtrlDock.EnableDocking( CBRS_ALIGN_ANY );
	DockPane( &m_wndTreeCtrlDock );


	return 0;
}

BOOL	CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame �f�f

#ifdef _DEBUG
void	CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void	CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ���b�Z�[�W �n���h���[

void	CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// �r���[ �E�B���h�E�Ƀt�H�[�J�X��^���܂��B
	m_wndView.SetFocus();
}

BOOL	CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// �r���[�ɍŏ��ɃR�}���h����������@���^���܂��B
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ȊO�̏ꍇ�́A����̏������s���܂��B
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

