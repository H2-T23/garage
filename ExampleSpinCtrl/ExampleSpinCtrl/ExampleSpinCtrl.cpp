// ExampleSpinCtrl.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleSpinCtrl.h"

#include "UseToolkit.h"

class CMainForm : public CForm {
public:
	enum {
		IDC_SPIN1 = 1001, IDC_SPIN2 = 1101, IDC_SPIN3 = 1201,
	};
protected:
	TSpinCtrl<double, IDC_SPIN1>			m_spinX;
	TSpinCtrl<int, IDC_SPIN2>				m_spinY;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_spinX.Create(this, 10, 10, 80, 20);
		m_spinX.SetRange( 1.0, 30.0, 0.5 );
		m_spinX.SetPos( 10.0 );

		m_spinY.Create(this, 10, 40, 80, 20);
		m_spinY.SetRange( 16, 64, 2 );
		m_spinY.SetPos( 32 );

		SetFontChildren();
		return TRUE;
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CMainForm	Form;
	if( Form.Create(_T(""), 0, 0, 100, 100, 300, 300) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
