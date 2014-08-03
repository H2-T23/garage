
// MFCDragAndDrop.h : MFCDragAndDrop アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CMFCDragAndDropApp:
// このクラスの実装については、MFCDragAndDrop.cpp を参照してください。
//

class CMFCDragAndDropApp : public CWinApp
{
public:
	CMFCDragAndDropApp();


// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCDragAndDropApp theApp;
