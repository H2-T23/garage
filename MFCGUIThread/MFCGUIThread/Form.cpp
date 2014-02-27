// Form.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCGUIThread.h"
#include "Form.h"


// CForm

IMPLEMENT_DYNAMIC(CForm, CWnd)

CForm::CForm()
{
	TRACE(_T("CForm::CForm().\n"));
}

CForm::~CForm()
{
	TRACE(_T("CForm::~CForm().\n"));
}


BEGIN_MESSAGE_MAP(CForm, CWnd)
END_MESSAGE_MAP()



// CForm メッセージ ハンドラ



