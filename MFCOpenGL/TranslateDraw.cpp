// TranslateDraw.cp 

void	DrawCube( void );


void	TranslateDraw( void )
{
	::wglMakeCurrent( hdc, glrc );
	
	::glEnable( GL_DEPTH_TEXT );
	::glEnable( GL_CULL_FACE );
	
	::glClearColor( ZERO, ZEOR, ZERO, ONE );
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	::glPushMatrix();
	::glTranslatef( -2.0, ZERO, ZERO );
	
	DrawCube();
	
	::glPopMatrix();
	
	::glMakeCurrent( NULL, NULL );
}

void	RotateDraw( void )
{
	::wglMakeCurrent( hdc, glrc );
	
	::glEnable( GL_DEPTH_TEST );
	::glEnable( GL_CULL_FACE );
	
	::glClearColor( ZERO, ZERO, ZERO, ONE );
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	::glPushMatrix();
	::glRotatef( 30.0, ZERO, ONE, ZERO );
	
	DrawCube();
	
	::glPopMatrix();
	
	::glMakeCurrent( NULL, NULL );
}

void	ScaleDraw( void )
{
	::glMakeCurrent( hdc, glrc );
	
	::glEnable( GL_DEPTH_TEST );
	::glEnable( GL_CULL_FACE );
	
	::glClearColor( ZERO, ZERO, ZERO, ONE );
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	::glPushMatrix();
	::glScalef( 2.0, 0.5, 1.0 );
	
	DrawCube();
	
	::glPopMatrix();
	
	::wglMakeCurrent( NULL, NULL );
}

void	CompundDraw( void )
{
	::wglMakeCurrent( hdc, glrc );
	
	::glEnable( GL_DEPTH_TEST );
	::glEnable( GL_CULL_FACE );
	
	::glClearColor( ZERO, ZERO, ZERO, ONE );
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	::glPushMatrix();
	::glTranslatef( -2.0, 0, 0 );
	::glRotatef( 30, 0, 1, 0 );
	::glScalef( 2, 0.5, 1 );
	
	DrawCube();
	
	::glPopMatrix();
	
	::wglMakeCurrent( NULL, NULL );
}


