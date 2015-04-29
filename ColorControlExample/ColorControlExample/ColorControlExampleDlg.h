
// ColorControlExampleDlg.h : ヘッダー ファイル
//

#pragma once
#include "ColorHeaderCtrl.h"
#include "ColorListCtrl.h"


// CColorControlExampleDlg ダイアログ
class CColorControlExampleDlg : public CDialogEx
{
// コンストラクション
public:
	CColorControlExampleDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_COLORCONTROLEXAMPLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

protected:
	CColorHeaderCtrl	m_wndHdr;
	CColorListCtrl		m_wndList;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
