
// TabApplication.h : TabApplication アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CTabApplicationApp:
// このクラスの実装については、TabApplication.cpp を参照してください。
//

class CTabApplicationApp : public CWinApp
{
public:
	CTabApplicationApp();


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTabApplicationApp theApp;
