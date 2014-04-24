#pragma once

#include "Form.h"
#include "ComponentWnd.h"
#include "GridPanel.h"
#include "Handler.h"

#include "Thread.h"

#include "KeyValue.h"

using namespace MT;


class CKeyValueServerForm : public CForm {
protected:
	enum {
		IDC_GRID_PANEL	= 1001	,
	};

	CGridPanel		m_wndGrid;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_wndGrid.Create(this, 0, 0, 0, 0);

		CListView&	ListView = m_wndGrid.GetListView();
		ListView.SetExtendedLitViewStyle( LVS_EX_GRIDLINES );
		ListView.InsertColumn( 0, _T("Key") );
		ListView.InsertColumn( 1, _T("Value") );

		for( int i=0; i<3201; i++ ){
			ListView.InsertItem( i, _T("") );
			ListView.SetItem( i, 1, _T("-") );
		}

		SetTimer(1234, 1000, NULL);

		return TRUE;
	}

	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;

		m_wndGrid.MoveWindow( 0, 0, cx, cy );
	}

	void	OnTimer( UINT nID ){
		CListView&	ListView	= m_wndGrid.GetListView();
		CKeyValue&	KeyValue	= CKeyValue::Instance();

		int	i	= 0;

		TCHAR	buf[256];

		const char*	pKey	= NULL;
		const char*	pValue	= NULL;

		for( pKey=KeyValue.Begin(); KeyValue.hasNext(); pKey=KeyValue.Next() ){
			
			pValue	= KeyValue.Get( pKey );
			if( pValue == NULL )
				continue;

			int	len	= strlen( pKey );
			TString::A2W(pKey, strlen(pKey), buf, 256);
			buf[ len ] = _T('\0');
			ListView.SetItem( i, 0, buf );

			len	= strlen( pValue );
			TString::A2W(pValue, strlen(pValue), buf, 256);
			buf[ len ] = _T('\0');
			ListView.SetItem( i, 1, buf );

			i++;
		}
	}
};

class CKeyValueServerFormThread : public IRunnable {
public:
	virtual void	run(){
		CKeyValueServerForm	Form;
		if( Form.Create(_T("KeyValue Viewer"), 0, 0, 100, 300, 600, 600) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};
