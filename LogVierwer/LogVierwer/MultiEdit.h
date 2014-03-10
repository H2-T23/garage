#pragma once


// CMultiEdit
/**********************************************************************************
 *
 *
 *
 */
class CMultiEdit : public CEdit {
	DECLARE_DYNAMIC(CMultiEdit)

public:
	CMultiEdit();
	virtual ~CMultiEdit();

protected:
	BOOL	PreCreateWindow( CREATESTRUCT& cs ){
		if( !CEdit::PreCreateWindow(cs) ){
			return FALSE;
		}

		cs.style		|= ES_MULTILINE;

		return TRUE;
	}

protected:
	DECLARE_MESSAGE_MAP()
};


