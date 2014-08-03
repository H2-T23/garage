#pragma once
#include "FixedCtrlPane.h"


// CDragAndDropTree

class CDragAndDropTree : public CFixedCtrlPane {
	DECLARE_DYNAMIC(CDragAndDropTree)
public:
	CDragAndDropTree() : m_bDragging(FALSE), m_pDragImage(NULL) {
	}
	virtual ~CDragAndDropTree(){
		if( m_pDragImage ){
			delete m_pDragImage;
		}
	}

protected:
	enum		{NUM_BUTTONS = 9, IDI_TREE=1001};
	CTreeCtrl	m_Tree;
	CImageList	m_Image;
	CImageList* m_pDragImage;
	BOOL		m_bDragging;
	HTREEITEM	m_hDragItem, m_hDropItem;
	int			m_nDropIndex;
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


