
#pragma once


// CDropSource コマンド ターゲット

class CDropSource : public COleDropSource {
	DECLARE_DYNAMIC(CDropSource)
public:
	CDropSource();
	virtual ~CDropSource();

protected:
	DECLARE_MESSAGE_MAP()
};


