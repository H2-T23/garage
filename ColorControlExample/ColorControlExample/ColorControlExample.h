
// ColorControlExample.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CColorControlExampleApp:
// このクラスの実装については、ColorControlExample.cpp を参照してください。
//

class CColorControlExampleApp : public CWinApp
{
public:
	CColorControlExampleApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CColorControlExampleApp theApp;