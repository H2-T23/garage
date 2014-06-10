#pragma once
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#include <math.h>
#include <Windows.h>
#include <GL/GL.h>
#include <GL/glu.h>

#include "Wnd.h"
#include "Form.h"
#include "Panel.h"

/**********************************************************************************
 *
 *
 *
 */
class COpenGL {
public:
	BOOL		m_IsValid;
	HGLRC		m_hRC;

	inline BOOL	IsValid(void) const { return(m_IsValid); }

	COpenGL( void ) : m_IsValid(FALSE), m_hRC(NULL) {
	}

	virtual ~COpenGL( void ){
		Cleanup();
	}

	BOOL	Startup( HDC hDC ){
		PIXELFORMATDESCRIPTOR	pfd	= {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,
			8,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		int nPixelFormat	= ::ChoosePixelFormat( hDC, &pfd );
		if( nPixelFormat == 0 )
			return FALSE;

		if( !::SetPixelFormat( hDC, nPixelFormat, &pfd ) )
			return FALSE;

		m_hRC	= ::wglCreateContext( hDC );
		if( m_hRC == NULL )
			return FALSE;

		return(m_IsValid = ::wglMakeCurrent( hDC, m_hRC ));
	}

	void	Cleanup( HDC hDC = NULL ){
		if( hDC ){
			::wglMakeCurrent( hDC, NULL );
		}

		if( m_hRC ){
			::wglDeleteContext( m_hRC );
			m_hRC	= NULL;
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
class IOpenGLView {
protected:
	COpenGL			gl;

public:
	virtual void	Init( void )						= 0;
	virtual void	Display( void )						= 0;
	virtual void	Resize( int nWidth, int nHeight )	= 0;
};
/**********************************************************************************
 *
 *
 *
 */
class IOpenGLPanel : public CPanel, public IOpenGLView {
protected:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		HDC	hDC = ::GetDC( m_hWnd );

		if( gl.Startup( hDC ) ){
			this->Init();
			return TRUE;
		}
		return FALSE;
	}

	void	OnDestroy( void ){
		if( !gl.IsValid() )
			return;
		HDC	hDC = ::GetDC(m_hWnd);
		gl.Cleanup(hDC);
	}

	void	OnPaint( HDC hDC ){
		if( !gl.IsValid() )
			return;
		this->Display();
		glFinish();
		SwapBuffers( hDC );
	}

	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;
		if( !gl.IsValid() )
			return;
		this->Resize( cx, cy );
	}
};
/**********************************************************************************
 *
 *
 *
 */
class IOpenGLForm : public CForm, public IOpenGLView {
protected:
    virtual DWORD				WindowStyle( void ) const		{return(CForm::WindowStyle() | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
	//	return TRUE;
		HDC	hDC = ::GetDC( m_hWnd );
		if( gl.Startup( hDC ) ){
			this->Init();
			return TRUE;
		}
		return FALSE;
	}

	void	OnDestroy( void ){
		if( !gl.IsValid() )
			return;
		HDC	hDC = ::GetDC(m_hWnd);
		gl.Cleanup(hDC);
	}

	void	OnPaint( HDC hDC ){
		if( !gl.IsValid() ){
			if( gl.Startup(hDC) ){
				this->Init();
			}
		}
		if( !gl.IsValid() )
			return;
		::wglMakeCurrent( hDC, gl.m_hRC );
		this->Display();
		glFinish();
		SwapBuffers( hDC );
		::wglMakeCurrent( NULL, NULL );
	}

	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;
		if( !gl.IsValid() )
			return;
		HDC hDC = ::GetDC(m_hWnd);
		::wglMakeCurrent(hDC, gl.m_hRC);
		this->Resize( cx, cy );
		::wglMakeCurrent(NULL, NULL);
		Invalidate();
	}

	void	OnWndMsg( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		Invalidate(FALSE);
	}
};
