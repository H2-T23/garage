// DropData.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCOLEDragAndDrop.h"
#include "DropData.h"


// CDropData

IMPLEMENT_DYNAMIC(CDropData, COleDataSource)


CDropData::CDropData()
{
}

CDropData::~CDropData()
{
}


BEGIN_MESSAGE_MAP(CDropData, COleDataSource)
END_MESSAGE_MAP()



// CDropData メッセージ ハンドラー
