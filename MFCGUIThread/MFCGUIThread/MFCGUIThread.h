
// MFCGUIThread.h : MFCGUIThread アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル

class CGUIThread;

// CMFCGUIThreadApp:
// このクラスの実装については、MFCGUIThread.cpp を参照してください。
//

class CMFCGUIThreadApp : public CWinApp
{
public:
	CMFCGUIThreadApp();

// オーバーライド
public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

// 実装
	CGUIThread*		m_pGUIThread;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCGUIThreadApp theApp;
