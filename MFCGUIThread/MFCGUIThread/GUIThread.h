#pragma once




// CGUIThread

class CGUIThread : public CWinThread
{
	DECLARE_DYNCREATE(CGUIThread)

protected:
	CGUIThread();           // ���I�����Ŏg�p����� protected �R���X�g���N�^
	virtual ~CGUIThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


