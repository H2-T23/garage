#pragma once

#include "Types.h"

class CDC {
public:
	HDC			m_hDC;
	
public:
	CDC( void ) : m_hDC(NULL) {}
	CDC( HDC hDC ) : m_hDC(hDC) {}
	CDC( CDC const& DC ) : m_hDC(DC.m_hDC) {}
	
	
public:
	HGDIOBJ		SelectObject( HGDIOBJ hGDI ) const {
		return ::SelectObject(m_hDC, hGDI);
	}

public:
	void	MoveTo(int x, int y) const		{ ::MoveToEx(m_hDC,x,y,NULL);		}
	void	MoveTo(POINT const& pt) const	{ ::MoveToEx(m_hDC,pt.x,pt.y,NULL);	}
	void	LineTo(int x, int y) const		{ ::LineTo(m_hDC,x,y);				}
	void	LineTo(POINT const& pt) const	{ ::LineTo(m_hDC,pt.x,pt.y);		}



	void	DrawLine(POINT const& ptA, POINT const& ptB) const {
		MoveTo(ptA);
		LineTo(ptB);
	}
	void	DrawLine(POINT const& ptA, POINT const& ptB, COLORREF rgb, int nWidth = 1, int nPenStyle = PS_SOLID) const {
		HPEN	hPen = ::CreatePen(nPenStyle, nWidth, rgb);
		HPEN	hOldPen = (HPEN)SelectObject( hPen ); 
		DrawLine(ptA, ptB);
		SelectObject( hOldPen );
		DeleteObject( hPen );
	}

	void	DrawRect(CRect const& rc) const {
		POINT	pt[] = {
			{ rc.X()             , rc.Y() }
		,	{ rc.X() + rc.Width(), rc.Y() }
		,	{ rc.X() + rc.Width(), rc.Y() + rc.Height() }
		,	{ rc.X()             , rc.Y() + rc.Height() }
		};
		DrawLine(pt[0], pt[1]);
		DrawLine(pt[1], pt[2]);
		DrawLine(pt[2], pt[3]);
		DrawLine(pt[3], pt[0]);
	}
	void	DrawRect(CRect const& rc, COLORREF rgb, int nWidth = 1, int nPenStyle = PS_SOLID) const {
		HPEN	hPen = ::CreatePen(nPenStyle, nWidth, rgb);
		HPEN	hOldPen = (HPEN)SelectObject( hPen ); 
		DrawRect( rc );
		SelectObject( hOldPen );
		DeleteObject( hPen );
	}


	void	FillRect(CRect const& rc, HBRUSH hBr) const { ::FillRect(m_hDC, &rc, hBr); }
	void	FillRect(CRect const& rc, COLORREF rgb) const {
		HBRUSH	hBr		= CreateSolidBrush( rgb );
		HBRUSH	hOldBr	= (HBRUSH)SelectObject( hBr );
		FillRect(rc, hBr);
		SelectObject(hOldBr);
		DeleteObject(hBr);
	}
	void	FrameRect(CRect const& rc, HBRUSH hBr) const { ::FrameRect(m_hDC, &rc, hBr); }
	void	FrameRect(CRect const& rc, COLORREF rgb) const {
		HBRUSH	hBr		= CreateSolidBrush( rgb );
		HBRUSH	hOldBr	= (HBRUSH)SelectObject( hBr );
		FrameRect(rc, hBr);
		SelectObject( hOldBr );
		DeleteObject( hBr );
	}
	void	Rectangle(CRect const& rc) const { ::Rectangle(m_hDC, rc.left, rc.top, rc.right, rc.bottom);	}
	void	RoundRect(CRect const& rc, int nWidth, int nHeight) const { ::RoundRect(m_hDC, rc.left, rc.top, rc.right, rc.bottom, nWidth, nHeight);	}
};
