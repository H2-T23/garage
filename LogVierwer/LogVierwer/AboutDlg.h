#pragma once

#include "resource.h"       // メイン シンボル

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
	virtual void DoDataExchange(CDataExchange* pDX){    // DDX/DDV サポート
		CDialog::DoDataExchange(pDX);
	}

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};
