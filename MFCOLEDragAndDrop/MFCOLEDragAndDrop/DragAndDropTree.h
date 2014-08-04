#pragma once
#include "FixedCtrlPane.h"
#include "DropTarget.h"

template<class OLEDROPTARGET>
class IDragAndDropTarget {
protected:
	OLEDROPTARGET		m_target;

	virtual DROPEFFECT	OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void		OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT	OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT	OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
	virtual BOOL		OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};


// CDragAndDropTree

class CDragAndDropTree : public CFixedCtrlPane/*, public CDropTarget*/ {
	DECLARE_DYNAMIC(CDragAndDropTree)
public:
	CDragAndDropTree()
	 : m_bDragging(FALSE)
	 , m_pDragImage(NULL) 
	{
	}
	
	virtual ~CDragAndDropTree(){
		if( m_pDragImage ){
			delete m_pDragImage;
		}
	}

protected:
	enum {NUM_BUTTONS = 9, IDI_TREE=1001};
	CTreeCtrl			m_Tree;
	CImageList			m_Image;
	CImageList*			m_pDragImage;
	BOOL				m_bDragging;
	HTREEITEM			m_hDragItem, m_hDropItem;
	int					m_nDropIndex;
	CWnd*				m_pDropWnd;

	CDropTarget			m_DropTarget;

	void		DropItemOnTree (CTreeCtrl* pTree);
	void		DropItemOnList (CListCtrl* pList);

protected:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnBeginDrag(NMHDR* pnmhdr, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};


