// OpenGLView.h 
#pragma once

/*******************************************************************************
 *
 *
 */
class COpenGLView : public CView {
protected:
	DECLARE_DYNCREATE(COpenGLView)
	
	HGLRC			m_hGLRC;
	
public:
	COpenGLView( void );
	virtual ~COpenGLView( void );
	
#ifdef _DEBUG
	virtual void		AssertVialid() const;
	virtual void		Dump( CDumpContext& dc ) const;
#endif
	
protected:
	virtual BOOL	PreCreateWindow( CREATESTRUCT& cs );
	virtual void	OnDraw( CDC* pDC );

protected:
	DECLARE_MESSAGE_MAP()
	
	afx_msg int		OnCreate( LPCREATESTRUCT );
	afx_msg void	OnDestroy();
	afx_msg void	OnSize( UINT, int, int );
};
