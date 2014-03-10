
// LogVierwer.h : LogVierwer アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


/**********************************************************************************
 *
 *
 *
 */
class CLogVierwerApp : public CWinApp
{
public:
	CLogVierwerApp();

public:
	virtual BOOL InitInstance();

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};
/**********************************************************************************
 *
 *
 *
 */
extern CLogVierwerApp theApp;
