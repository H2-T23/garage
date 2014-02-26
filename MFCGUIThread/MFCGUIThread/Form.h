#pragma once


// CForm

class CForm : public CWnd
{
	DECLARE_DYNAMIC(CForm)

public:
	CForm();
	virtual ~CForm();

	void	PostNcDestroy()
	{
		// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
		TRACE(_T("CForm::PostNcDestroy().\n"));
		delete this;
		CWnd::PostNcDestroy();
	}

protected:
	DECLARE_MESSAGE_MAP()
};


