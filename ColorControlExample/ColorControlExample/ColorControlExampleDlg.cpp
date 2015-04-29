
// ColorControlExampleDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "AboutDlg.h"
#include "ColorControlExample.h"
#include "ColorControlExampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CColorControlExampleDlg �_�C�A���O



CColorControlExampleDlg::CColorControlExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CColorControlExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CColorControlExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColorControlExampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CColorControlExampleDlg ���b�Z�[�W �n���h���[

BOOL CColorControlExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	DWORD	dwStyle	= (WS_CHILD | WS_VISIBLE | WS_BORDER);
	CRect	rect(0, 0, 400, 30);

	if( m_wndHdr.Create( dwStyle, rect, this, 1001 ) ){
		m_wndHdr.SetFont( GetFont() );

		TCHAR buf[256] = {0};

		HDITEM hItem = {0};
		hItem.mask = (HDI_FORMAT | HDI_TEXT | HDI_WIDTH);
		hItem.fmt			= HDF_CENTER | HDF_STRING | HDF_OWNERDRAW;
		hItem.cchTextMax	= 256;
		hItem.pszText		= buf;
		hItem.cxy			= 100;

		_tcscpy_s( buf, _T("Col1") );
		m_wndHdr.InsertItem( 0, &hItem );

		_tcscpy_s( buf, _T("Col2") );
		m_wndHdr.InsertItem( 1, &hItem );

		_tcscpy_s( buf, _T("Col3") );
		m_wndHdr.InsertItem( 2, &hItem );

		m_wndHdr.SetBkColor( 0, RGB(0xFF,0,0) );
		m_wndHdr.SetBkColor( 1, RGB(0,0xFF,0) );
		m_wndHdr.SetBkColor( 2, RGB(0,0,0xFF) );
	}

	rect.MoveToY( 100 );
	rect.bottom += 100;

	if( m_wndList.Create(dwStyle | LVS_REPORT, rect, this, 1002) ){
		m_wndList.SetFont( GetFont() );

		m_wndList.InsertColumn( 0, _T("Col1"), 0, 100 );
		m_wndList.InsertColumn( 1, _T("Col2"), 0, 100 );
		m_wndList.InsertColumn( 2, _T("Col3"), 0, 100 );

		m_wndList.InsertItem( 0, _T("Row1") );
		m_wndList.InsertItem( 1, _T("Row2") );
		m_wndList.InsertItem( 2, _T("Row3") );

		m_wndList.SetBkColor( 0, RGB(0xFF,0,0) );
		m_wndList.SetBkColor( 1, RGB(0,0xFF,0) );
		m_wndList.SetBkColor( 2, RGB(0,0,0xFF) );

		m_wndList.SetBkColor( 1, 2, RGB(0xCC,0xCC,0xCC) );
	}

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CColorControlExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CColorControlExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CColorControlExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

