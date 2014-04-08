#pragma once

#include "Panel.h"
#include "ComponentWnd.h"

/**********************************************************************************
 * 
 *
 *
 */
typedef enum {
	TYPE_INT		= 1	,	
	TYPE_FLOAT			,
	TYPE_DOUBLE			,
} VALUE_TYPE;
/**********************************************************************************
 * 
 *
 *
 */
template<typename TYPE>
class TRange {
public:
	TYPE	Min, Max, Step;
	int		nMaxIndex;

	TRange( void )
		: Min(0), Max(0), Step(0), nMaxIndex(0) {
	}

	void		SetRange( TYPE min, TYPE max, TYPE step ){
		Min			= min;
		Max			= max;
		Step		= step;
		nMaxIndex	= int((Max - Min)/Step);
	}

	int			GetCount( void ) const {
		return nMaxIndex;
	}
	int			GetMaxIndex( void ) const {
		return nMaxIndex;
	}

//	int			SetPos( TYPE pos ){
//		return Pos2Index(pos);
//	}

//	TYPE		GetPos( int idx ){
//		return Index2Pos(idx);
//	}

	int			Pos2Index( TYPE pos ) const {
		int idx = int((pos - Min) / Step);
		if( idx < 0 ){
			idx = 0;
		}else if( idx > nMaxIndex ){
			idx = nMaxIndex;
		}
		return idx;
	}

	TYPE		Index2Pos( int idx ) const {
		if( idx < 0 ){
			idx = 0;
		}else if( idx > nMaxIndex ){
			idx = nMaxIndex;
		}
		return TYPE((idx * Step) + Min);
	}

	int			IsType( void ) const {
		const type_info& info = typeid( Min );
		if( info == typeid(int) ){
			return int(TYPE_INT);
		}
		else if( info == typeid(float) ){
			return int(TYPE_FLOAT);
		}
		else if( info == typeid(double) ){
			return int(TYPE_DOUBLE);
		}
		return 0;
	}

	LPCTSTR		Format( void ){
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
	int					m_idx;

public:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		int	w	= lpCreateStruct->cx;
		int	h	= lpCreateStruct->cy;
		if( !m_edt.Create(this, 0, 0, w, h, IDC_EDIT) ){
			return FALSE;
		}

		DWORD	dwStyle	= (WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | /*UDS_HORZ | UDS_SETBUDDYINT | UDS_WRAP |*/ UDS_ARROWKEYS);

		if( !m_updown.Create(this, w, h, 10, h, IDC_SPIN, NULL, dwStyle) ){
			return FALSE;
		}
		m_updown.SetBuddy( &m_edt );

		return TRUE;
	}

	void	SetRange( TYPE min, TYPE max, TYPE step ){
		m_range.SetRange(min, max, step);
		int	nCount = m_range.GetCount();
		m_updown.SetRange( 0, nCount );
		m_updown.SetPos( 0 );
		DBG::TRACE(_T("SetRange(0, %d)"), nCount);
	}

	void	SetPos( TYPE pos ){
		m_idx = m_range.Pos2Index(pos);
		m_updown.SetPos( m_idx );
		DBG::TRACE(_T("SetPos(%d)(%d)"), m_idx, m_updown.GetPos());

		LPCTSTR lpszFmt = m_range.Format();
		TString	str;
		str.Format( lpszFmt, pos );
		m_edt.SetWindowText( str );
	}

	TYPE	GetPos( void ) const {
		return m_range.Index2Pos( m_updown.GetPos() );
	}

	void	OnVScroll			( HWND hWndCtl, UINT code, int idx ){
		if( m_updown.m_hWnd == hWndCtl && SB_THUMBPOSITION == code ){
			DBG::TRACE(_T("OnVScroll(%d)"), idx);
			SetPos( m_range.Index2Pos(idx) );
		}
	}

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		if( wParam == (WPARAM)IDC_SPIN )
		{
			LPNMUPDOWN lpNmUpDown = (LPNMUPDOWN)lParam;
			if( lpNmUpDown && lpNmUpDown->hdr.code == UDN_DELTAPOS )
			{
				TString	str;
				m_edt.GetWindowText( str );

				m_idx	= m_range.Pos2Index( str.ToInt() );
				
				if( m_range.IsType() == TYPE_DOUBLE )
				{
					m_idx	= m_range.Pos2Index( str.ToDouble() );
				}
				else
				if( m_range.IsType() == TYPE_FLOAT )
				{
					m_idx	= m_range.Pos2Index( float(str.ToDouble()) );
				}

				if( (0 <= m_idx) && (m_idx <= m_range.GetMaxIndex()) )
				{
					m_idx	+= lpNmUpDown->iDelta;
				}

				DBG::TRACE(_T("%s(%d)"), (LPCTSTR)str, m_idx);
				DBG::TRACE(_T("OnNotify(%d,%d,  %d)"), lpNmUpDown->iDelta, lpNmUpDown->iPos, m_idx);
			//	::SendMessage(GetParent(), WM_NOTIFY, wParam, lParam);
			}
		}
	}
};
