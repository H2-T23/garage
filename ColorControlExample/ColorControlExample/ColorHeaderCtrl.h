#pragma once

#include "ColorControl.h"

//class CHeaderCtrlEx : public CHeaderCtrl
//{
//	DECLARE_DYNAMIC(CHeaderCtrlEx)
//public:
//	CHeaderCtrlEx();
//	virtual ~CHeaderCtrlEx();
//
//protected:
//	DECLARE_MESSAGE_MAP()
//};

class CColorHeaderCtrl : public TColorControl<CHeaderCtrl>
{
	DECLARE_DYNAMIC(CColorHeaderCtrl)
public:
	CColorHeaderCtrl();
	virtual ~CColorHeaderCtrl();


public:
	BOOL SubclassWindow(HWND hWnd);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


