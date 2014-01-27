#pragma once

#include "Panel.h"
#include "ComponentWnd.h"

/**********************************************************************************
 * 
 *
 *
 */
template<typename TYPE>
class TRange {
public:
	TYPE	Min, Max, Step;

	TRange( void )
		: Min(0), Max(0), Step(0) {
	}

	void	SetRange( TYPE min, TYPE max, TYPE step ){
		Min		= min;
		Max		= max;
		Step	= step;
	}

	int		SetPos( TYPE pos ){
		return Pos2Index(pos);
	}

	int		GetCount( void ){
		return int((Max - Min)/Step);
	}

	TYPE	GetPos( int idx ){
		return Index2Pos(idx);
	}

	int		Pos2Index( TYPE pos ) const {
		return int((pos - Min) / Step);
	}

	TYPE	Index2Pos( int idx ) const {
		return TYPE((idx * Step) + Min);
	}
};

/**********************************************************************************
 * 
 *
 *
 */
template<typename TYPE>
class TSpinCtrl : public CPanel {
public:
	enum { IDC_EDIT = 1001, IDC_UPDOWN };
	TRange<TYPE>		m_range;
	CNumberEdit			m_edt;
	CUpDown				m_updown;

public:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		int	w	= lpCreateStruct->cx;
		int	h	= lpCreateStruct->cy;
		if( !m_edt.Create(this, 0, 0, w, h, IDC_EDIT) ){
			return FALSE;
		}
		if( !m_updown.Create(this, w, h, 10, h, IDC_UPDOWN) ){
			return FALSE;
		}
		m_updown.SetBuddy( &m_edt );
		return TRUE;
	}

	void	SetRange( TYPE min, TYPE max, TYPE step ){
		m_range.SetRange(min, max, step);
		int	nCount = m_range.GetCount();
		m_updown.SetRange( 0, nCount );
		DBG::TRACE(_T("Range(0, %d)"), nCount);
	}

	void	SetPos( TYPE pos ){
		int	idx = m_range.Pos2Index(pos);
		m_updown.SetPos( idx );
		TString	str;
		str.Format( _T("%1.2f"), pos );
		m_edt.SetWindowText( str );
	}

	TYPE	GetPos( void ) const {
		return m_range.Index2Pos( m_updown.GetPos() );
	}

	void	OnVScroll			( HWND hWndCtl, UINT code, int idx ){
		if( m_updown.m_hWnd == hWndCtl && SB_THUMBPOSITION == code ){
			SetPos( m_range.Index2Pos(idx) );
		}
	}
};
