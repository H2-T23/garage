#pragma once

#include "Types.h"


#define	COPY_OPERATOR(TYPE,VAR)				public:TYPE operator=(TYPE& rType){return(VAR=rType);}
#define	CAST_OPERATOR(TYPE,VAR)				public:operator TYPE(){return(VAR);}
#define	COPY_AND_CAST_OPERATOR(TYPE,VAR)	COPY_OPERATOR(TYPE,VAR) CAST_OPERATOR(TYPE,VAR)
#define	COPY_AND_CAST(TYPE,VAR)				COPY_AND_CAST_OPERATOR(TYPE,VAR)
#define	BIND_TYPE_VARIAVLE

/**********************************************************************************
 *
 *
 *
 */
union UGdiOobject {
	HGDIOBJ				hGdiobj;
	HBITMAP				hBitmap;
	HBRUSH				hBrush;
	HFONT				hFont;
	HPEN				hPen;
	HRGN				hReg;

	COPY_AND_CAST(HGDIOBJ, hGdiobj);
	COPY_AND_CAST(HBITMAP, hBitmap);
	COPY_AND_CAST(HBRUSH, hBrush);
	COPY_AND_CAST(HFONT, hFont);
	COPY_AND_CAST(HPEN, hPen);
	COPY_AND_CAST(HRGN, hReg);
};

/**********************************************************************************
 *
 *
 *
 */
class CGdiObject {
public:
	UGdiOobject			m_hGdiObj;

	COPY_AND_CAST(HGDIOBJ, m_hGdiObj.hGdiobj);
};

/****************************************************************************************
 *
 *
 *
 */
class CFont : public CGdiObject {
public:
	HFONT&			m_hFont;
	LOGFONT			m_LogFont;

//	CFont( void ) : CGdiObject() {}
//	CFont( HFONT hFont ) : m_hFont(CGdiObject::m_hGdiObj.hFont) {}
//	CFont( CFont const& rFont ) : CGdiObject(rFont) {}
//	CFont( CFont const* pFont ) : CGdiObject(*pFont) {}


	BOOL	CreateFont(	int		nHeight
					,	int		nWidth
					,	int		nEscapement
					,	int		nOrientation
					,	int		fnWeight
					,	DWORD	fdwItalic
					,	DWORD	fdwUnderline
					,	DWORD	fdwStrikeOut
					,	DWORD	fdwCharSet
					,	DWORD	fdwOutputPresisin
					,	DWORD	fdwClipPrecisin
					,	DWORD	fdwQuality
					,	DWORD	fdwPitchAndFamily
					,	LPCTSTR	lpszFace
					){
		m_LogFont.lfHeight				= nHeight;
		m_LogFont.lfWidth				= nWidth;
		m_LogFont.lfEscapement			= nEscapement;
		m_LogFont.lfOrientation			= nOrientation;
		m_LogFont.lfWeight				= fnWeight;
		m_LogFont.lfItalic				= (BYTE)fdwItalic;
		m_LogFont.lfUnderline			= (BYTE)fdwUnderline;
		m_LogFont.lfStrikeOut			= (BYTE)fdwStrikeOut;
		m_LogFont.lfCharSet				= (BYTE)fdwCharSet;
		m_LogFont.lfOutPrecision		= (BYTE)fdwOutputPresisin;
		m_LogFont.lfClipPrecision		= (BYTE)fdwClipPrecisin;
		m_LogFont.lfQuality				= (BYTE)fdwQuality;
		m_LogFont.lfPitchAndFamily		= (BYTE)fdwPitchAndFamily;
		_tcscpy(m_LogFont.lfFaceName, lpszFace);
	}

	BOOL	CreateFontIndirect( const LOGFONT* lplf ){
		m_hFont	= ::CreateFontIndirect( lplf );
		if( m_hFont == NULL )
		{
			memset(&m_LogFont, 0, sizeof(LOGFONT));
			return FALSE;
		}

		memcpy(&m_LogFont, lplf, sizeof(LOGFONT));
		return TRUE;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CBitmap : public CGdiObject {

};


/**********************************************************************************
 *
 *
 *
 */
class CDC {
public:
	HDC			m_hDC;

	CDC( void ) : m_hDC(NULL) {}
	CDC( HDC hDC ) : m_hDC(hDC) {}

public:
	/**********************************************************************************
	 *
	 */
	HGDIOBJ		SelectObject( HGDIOBJ hGDI ) const	{ return ::SelectObject(m_hDC, hGDI);	}

	/**********************************************************************************
	 *
	 */
	void		MoveTo(int x, int y) const	{ ::MoveToEx(m_hDC, x, y, NULL);	}
	/**********************************************************************************
	 *
	 */
	void		LineTo(int x, int y) const	{ ::LineTo(m_hDC, x, y);			}

	/**********************************************************************************
	 *
	 */
	void		DrawLine( POINT const& ptBegin, POINT const& ptEnd ) const {
		MoveTo(ptBegin.x, ptBegin.y);
		LineTo(ptEnd  .x, ptEnd  .y);
	}

	/**********************************************************************************
	 *
	 */
	void		DrawLine( POINT const& ptBegin, POINT const& ptEnd, COLORREF rgb, int nWidth=1, int nPenStyle=PS_SOLID  ) const {
		HPEN	hPen	= CreatePen(nPenStyle,nWidth,rgb);
		HPEN	hOldPen	= (HPEN)SelectObject( hPen );
		MoveTo(ptBegin.x, ptBegin.y);
		LineTo(ptEnd  .x, ptEnd  .y);
		SelectObject( hOldPen );
		DeleteObject( hPen );
	}

	/**********************************************************************************
	 *
	 */
	void		DrawRect( CRect& rect, COLORREF rgb=RGB(0,0,0), int nWidth=1, int nPenStyle=PS_SOLID ) const {
		POINT	pt[] = {
			{ rect.X()					, rect.Y()					}	// TopLeft.
		,	{ rect.X() + rect.Width()	, rect.Y()					}	// TopRight.
		,	{ rect.X() + rect.Width()	, rect.Y() + rect.Height()	}	// BottomRight.
		,	{ rect.X()					, rect.Y() + rect.Height()	}	// BottomLeft.
		};
		HPEN	hPen	= CreatePen(nPenStyle,nWidth,rgb);
		HPEN	hOldPen	= (HPEN)SelectObject( hPen );
		DrawLine( pt[ 0 ], pt[ 1 ] );
		DrawLine( pt[ 1 ], pt[ 2 ] );
		DrawLine( pt[ 2 ], pt[ 3 ] );
		DrawLine( pt[ 3 ], pt[ 0 ] );
		SelectObject( hOldPen );
		DeleteObject( hPen );
	}
};


/**********************************************************************************
 *
 *
 *
class CVDC {
protected:
	CDC			m_DC;
	CBitmap		m_Bitmap;
	CBitmap*	m_pOldBitmap;
	CRect		m_Rect;

public:
	operator HDC()	{ return m_DC.m_hDC; }

	BOOL	CreateCompatibleVDC( CDC* pDC, CRect* pRect ){
		if( m_DC.CreateCompatibleDC( pDC ) == FALSE )
			return FALSE;

		if( m_Bitmap.CreateCompatibleBitmap( pDC, pRect->Width(), pRect->Height() ) == FALSE )
			return FALSE;

		m_pOldBitmap	= m_DC.SelectObject( &m_Bitmap );

	}

	VOID	DeleteVDC( VOID ){
		m_DC.SelectObject( m_pOldBitmap );

		m_Bitmap.DeleteObject();

		m_DC.DeleteDC();
	}
};
 */
