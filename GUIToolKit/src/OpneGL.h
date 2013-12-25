#pragma once

#include <Windows.h>
#include <GL/GL.h>
#include <GL/glu.h>
//#include <GL/glaux.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
//#pragma comment(lib,"glaux.lib")

class COpenGL {
public:
	HGLRC		m_hRC;

	COpenGL( void ) : m_hRC(NULL) {
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

		return ::wglMakeCurrent( hDC, m_hRC );
	}

	void	Cleanup( void ){
		::wglMakeCurrent( NULL, NULL );
		::wglDeleteContext( m_hRC );
	}
};