
// MFCGUIThread.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "MFCGUIThread.h"
#include "MainFrm.h"
#include "AboutDlg.h"

#include "GUIThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCGUIThreadApp

BEGIN_MESSAGE_MAP(CMFCGUIThreadApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMFCGUIThreadApp::OnAppAbout)
END_MESSAGE_MAP()


// CMFCGUIThreadApp �R���X�g���N�V����

CMFCGUIThreadApp::CMFCGUIThreadApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

// �B��� CMFCGUIThreadApp �I�u�W�F�N�g�ł��B

CMFCGUIThreadApp theApp;


// CMFCGUIThreadApp ������

BOOL CMFCGUIThreadApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE ���C�u���������������܂��B
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	// ���C�� �E�B���h�E���쐬����Ƃ��A���̃R�[�h�͐V�����t���[�� �E�B���h�E �I�u�W�F�N�g���쐬���A
	// ������A�v���P�[�V�����̃��C�� �E�B���h�E�ɃZ�b�g���܂�
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// �t���[�������\�[�X���烍�[�h���č쐬���܂�
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);



	m_pGUIThread	= (CGUIThread*)AfxBeginThread(RUNTIME_CLASS(CGUIThread));


	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// �ڔ��������݂���ꍇ�ɂ̂� DragAcceptFiles ���Ăяo���Ă��������B
	//  SDI �A�v���P�[�V�����ł́AProcessShellCommand �̒���ɂ��̌Ăяo�����������Ȃ���΂Ȃ�܂���B
	return TRUE;
}

int CMFCGUIThreadApp::ExitInstance()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	DWORD	dwRet = ::WaitForSingleObject( m_pGUIThread->m_hThread, 10 );
	if( dwRet == WAIT_TIMEOUT )
	{
		::PostThreadMessage( m_pGUIThread->m_nThreadID, WM_QUIT, 0, 0 );
		dwRet = ::WaitForSingleObject( m_pGUIThread->m_hThread, 1000 );
		if( dwRet != WAIT_OBJECT_0 )
		{
			::TerminateThread( m_pGUIThread->m_hThread, 0 );
		}
	}

	return CWinApp::ExitInstance();
}


// CMFCGUIThreadApp ���b�Z�[�W �n���h��

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void	CMFCGUIThreadApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCGUIThreadApp ���b�Z�[�W �n���h��




