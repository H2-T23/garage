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

	LPCTSTR	Format( void ){
		const type_info& info = typeid( Min );
		if( info == typeid(int) ){
			return _T("%d");
		}
		else if( info == typeid(float) ){
			return _T("%0.2f");
		}
		else if( info == typeid(double) ){
			return _T("%0.3f");
		}
		return _T("%d");
	}
};

/**********************************************************************************
 * 
 *
 *
 */
template<typename TYPE, int IDC_SPIN = 1000>
class TSpinCtrl : public CPanel {
public:
	enum { IDC_EDIT = (IDC_SPIN + 1),  };
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
		if( !m_updown.Create(this, w, h, 10, h, IDC_SPIN) ){
			return FALSE;
		}
		m_updown.SetBuddy( &m_edt );

		SetFont();
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
		LPCTSTR lpszFmt = m_range.Format();
		m_updown.SetPos( idx );
		TString	str;
		str.Format( lpszFmt, pos );
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

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		if( wParam == (WPARAM)IDC_SPIN ){
			LPNMUPDOWN lpNmUpDown = (LPNMUPDOWN)lParam;
			if( lpNmUpDown && lpNmUpDown->hdr.code == UDN_DELTAPOS )
			{
				::SendMessage(GetParent(), WM_NOTIFY, wParam, lParam);
			}
		}
	}
};
