
// MFCOLEDragAndDrop.h : MFCOLEDragAndDrop アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CMFCOLEDragAndDropApp:
// このクラスの実装については、MFCOLEDragAndDrop.cpp を参照してください。
//

class CMFCOLEDragAndDropApp : public CWinAppEx
{
public:
	CMFCOLEDragAndDropApp();


// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCOLEDragAndDropApp theApp;
