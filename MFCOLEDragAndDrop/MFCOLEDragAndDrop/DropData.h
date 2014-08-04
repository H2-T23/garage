
#pragma once


// CDropData コマンド ターゲット

class CDropData : public COleDataSource {
	DECLARE_DYNAMIC(CDropData)
public:
	CDropData();
	virtual ~CDropData();

protected:
	DECLARE_MESSAGE_MAP()
};


