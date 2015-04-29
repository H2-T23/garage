#pragma once

#include "ColorControl.h"

class CColorListCtrl : public TColorControl<CListCtrl>
{
	DECLARE_DYNAMIC(CColorListCtrl)
public:
	CColorListCtrl();
	virtual ~CColorListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


