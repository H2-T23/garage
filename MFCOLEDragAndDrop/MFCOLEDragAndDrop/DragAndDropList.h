#pragma once
#include "FixedCtrlPane.h"


// CDragAndDropList

class CDragAndDropList : public CFixedCtrlPane {
	DECLARE_DYNAMIC(CDragAndDropList)
public:
	CDragAndDropList()
		: m_bDragging(FALSE)
		, m_pDragImage(NULL)
	{
	}
	
	virtual ~CDragAndDropList(){
		if( m_pDragImage ){
			delete m_pDragImage;
		}
	}

protected:
	enum		{NUM_BUTTONS = 9, IDI_LIST=1001};
	CListCtrl	m_List;
	CImageList	m_Image;
	CImageList* m_pDragImage;
	BOOL		m_bDragging;
	int			m_nDragIndex, m_nDropIndex;
	HTREEITEM	m_hDropItem;
	CWnd*		m_pDropWnd;

	void DropItemOnTree (CTreeCtrl* pTree);
	void DropItemOnList (CListCtrl* pList);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBeginDrag(NMHDR* pnmhdr, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};


