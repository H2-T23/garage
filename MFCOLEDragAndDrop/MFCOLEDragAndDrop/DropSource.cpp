// DropSource.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCOLEDragAndDrop.h"
#include "DropSource.h"


// CDropSource

IMPLEMENT_DYNAMIC(CDropSource, COleDropSource)


CDropSource::CDropSource()
{
}

CDropSource::~CDropSource()
{
}


BEGIN_MESSAGE_MAP(CDropSource, COleDropSource)
END_MESSAGE_MAP()



// CDropSource メッセージ ハンドラー
