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
		// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
		TRACE(_T("CForm::PostNcDestroy().\n"));
		delete this;
		CWnd::PostNcDestroy();
	}

protected:
	DECLARE_MESSAGE_MAP()
};


