#pragma once

#include "resource.h"       // ���C�� �V���{��

/**********************************************************************************
 *
 *
 *
 */
class CAboutDlg : public CDialog
{
public:
	CAboutDlg() : CDialog(CAboutDlg::IDD){
	}

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX){    // DDX/DDV �T�|�[�g
		CDialog::DoDataExchange(pDX);
	}

// ����
protected:
	DECLARE_MESSAGE_MAP()
};
