
// Mdb2TxtApp.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
  #error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

/**************************************************************************
 * CMdb2TxtApp:
 * このクラスの実装については、Mdb2Txt.cpp を参照してください。
 *
 */
class CMdb2TxtApp : public CWinAppEx
{
public:
	CMdb2TxtApp();

// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装
	DECLARE_MESSAGE_MAP()
};

/**************************************************************************
 *
 */
extern CMdb2TxtApp theApp;
