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

	int		GetRange( void ){
		return int(Max-Min/Step);
	}

	int		GetPos( TYPE pos ){
		return int((pos - GetRange()) / Step);
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
		m_updown.SetRange( 0, m_range.GetRange() );
	}

	void	SetPos( TYPE pos ){
		m_updown.SetPos( m_range.GetPos( pos ) );
	}

	void	OnVScroll			( HWND hWndCtl, UINT code, int pos ){
		if( SB_THUMBPOSITION == code ){
			TString	str;
			str.Format( _T("%1.1f"), double(pos) );
			m_edt.SetWindowText( str );
		}
	}
};
