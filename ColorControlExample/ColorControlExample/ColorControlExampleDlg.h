
// ColorControlExampleDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "ColorHeaderCtrl.h"
#include "ColorListCtrl.h"


// CColorControlExampleDlg �_�C�A���O
class CColorControlExampleDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CColorControlExampleDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_COLORCONTROLEXAMPLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

protected:
	CColorHeaderCtrl	m_wndHdr;
	CColorListCtrl		m_wndList;

// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
