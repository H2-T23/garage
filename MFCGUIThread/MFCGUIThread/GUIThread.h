#pragma once




// CGUIThread

class CGUIThread : public CWinThread
{
	DECLARE_DYNCREATE(CGUIThread)

protected:
	CGUIThread();           // 動的生成で使用される protected コンストラクタ
	virtual ~CGUIThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


