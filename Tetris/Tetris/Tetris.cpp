// Tetris.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Tetris.h"

#include "UseToolkit.h"

typedef struct {
	int			rotate;
	POINT		pt[ 3 ];
} BLOCK;

BLOCK	block[]	= {
		{	1,	{{0, 0}, { 0,0}, { 0,0}}	}
	,	{	2,	{{0,-1}, { 0,1}, { 0,2}}	}
	,	{	4,	{{0,-1}, { 0,1}, { 1,1}}	}
	,	{	4,	{{0,-1}, { 0,1}, {-1,1}}	}
	,	{	2,	{{0,-1}, { 1,0}, { 1,1}}	}
	,	{	2,	{{0,-1}, {-1,0}, {-1,1}}	}
	,	{	1,	{{0, 1}, { 1,0}, { 1,1}}	}
	,	{	4,	{{0,-1}, { 1,0}, {-1,0}}	}
};

typedef struct {
	int		x, y;
	int		type;
	int		rotate;
} STATUS;

class CMainForm : public CForm {
protected:
	HDC			m_hMemDC;
	HDC			m_hBlockDC;

	HBITMAP		m_hMemPrev;
	HBITMAP		m_hBlockPrev;

	int			board[ 12 ][ 25 ];
	STATUS		current;

protected:
	inline int	random( int max ){
		return((int)(rand() / (RAND_MAX + 1.0)*max));
	}

	bool putBlock(STATUS s, bool action = false) {
		if(board[s.x][s.y] != 0) {
			return false;
		}


		if(action) {
			board[s.x][s.y] = s.type;
		}


		for(int i = 0; i < 3; i++) {
			int dx = block[s.type].pt[i].x;
			int dy = block[s.type].pt[i].y;
			int r = s.rotate % block[s.type].rotate;
			for(int j = 0; j < r; j++) {
				int nx = dx, ny = dy;
				dx = ny; dy = -nx;
			}
			if(board[s.x + dx][s.y + dy] != 0) {
				return false;
			}
			if(action) {
				board[s.x + dx][s.y + dy] = s.type;
			}
		}
		if(!action) {
			putBlock(s, true);
		}
		return true;
	}

	bool deleteBlock(STATUS s) {
		board[s.x][s.y] = 0;

		for(int i = 0; i < 3; i++) {
			int dx = block[s.type].pt[i].x;
			int dy = block[s.type].pt[i].y;
			int r = s.rotate % block[s.type].rotate;
			for(int j = 0; j < r; j++) {
				int nx = dx, ny = dy;
				dx = ny; dy = -nx;
			}
			board[s.x + dx][s.y + dy] = 0;
		}

		return true;
	}

	void showBoard() {
		for(int x = 1; x <= 10; x++) {
			for(int y = 1; y <= 20; y++) {
				BitBlt(m_hMemDC, (x - 1) * 24, (20 -y) * 24, 24, 24, m_hBlockDC, 0, board[x][y] * 24, SRCCOPY);
			}
		}
	}

	bool processInput() {
		bool ret = false;
		STATUS n = current;
		if(GetAsyncKeyState(VK_LEFT)) {
				n.x--;
			} else if(GetAsyncKeyState(VK_RIGHT)) {
				n.x++;
			} else if(GetAsyncKeyState(VK_UP)) {
				n.rotate++;
			} else if(GetAsyncKeyState(VK_DOWN)) {
			n.y--;
			ret = true;
		}

		if(n.x != current.x || n.y != current.y || n.rotate != current.rotate) {
			deleteBlock(current);
			if(putBlock(n)) {
				current = n;
			} else {
				putBlock(current);
			}
		}
    
		return ret;
	}

	void gameOver() {
		KillTimer(100);
		for(int x = 1; x <= 10;x++) {
			for(int y = 1; y <= 20; y++) {
				if(board[x][y] != 0) {
					board[x][y] = 1;
				}
			}
		}
		Invalidate(FALSE);
	}

	void deleteLine() {
		for(int y = 1; y < 23; y++) {
			bool flag = true;
			for(int x = 1;x <= 10; x++) {
				if(board[x][y] == 0) {
					flag = false;
				}
			}
        
			if(flag) {
				for(int j = y; j < 23; j++) {
					for(int i = 1; i <= 10; i++) {
						board[i][j] = board[i][j + 1];
					}
				}
				y--;
			}
		}
	}

	void blockDown() {
		deleteBlock(current);
		current.y--;
		if(!putBlock(current)) {
			current.y++;
			putBlock(current);
        
			deleteLine();
        
			current.x = 5;
			current.y = 21;
			current.type = random(7) + 1;
			current.rotate = random(4);
			if(!putBlock(current)) {
				gameOver();
			}
		}
	}

public:
	CMainForm( void ){
		for( int x=0; x<12; x++ ){
			for( int y=0; y<25; y++ )
			{
				if( x == 0 || x == 11 || y == 0 ){
					board[ x ][ y ]	= 1;
				}else{
					board[ x ][ y ]	= 0;
				}
			}
		}

		current.x		= 5;
		current.y		= 21;
		current.type	= random( 7 ) + 1;
		current.rotate	= random( 4 );

		putBlock( current );
	}

protected:
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		HDC	hDC	= ::GetDC(m_hWnd);

		m_hMemDC		= ::CreateCompatibleDC( hDC );
		HBITMAP	hBmp	= ::CreateCompatibleBitmap( hDC, 24*10, 24*20 );
		m_hMemPrev		= (HBITMAP)::SelectObject( m_hMemDC, hBmp );

		m_hBlockDC		= ::CreateCompatibleDC( hDC );
	//	hBmp			= ::LoadBitmapA( ::GetModuleHandle(NULL), "BLCOK" );

		hBmp			= (HBITMAP)::LoadImageA( NULL, "block.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
		m_hBlockPrev	= (HBITMAP)::SelectObject( m_hBlockDC, hBmp );

		::BitBlt( m_hMemDC, 0, 0, 24, 24, m_hBlockDC, 0, 0, SRCCOPY );

		ReleaseDC( m_hWnd, hDC );
		::DeleteObject( hBmp );

		SetTimer( 100, 1000/10, NULL );

		return TRUE;
	}

	void	OnTimer( UINT uID ){
		static int	w = 0;
		if( (w % 2) == 0 ){
			if( processInput() ){
				w	= 0;
			}
		}

		if( (w % 5) == 0 ){
			blockDown();
		}

		w++;

		Invalidate( FALSE );
	}	
	
	void	OnPaint( HDC hDC ){
		showBoard();
		::BitBlt(hDC, 0, 0, 24*10, 24*20, m_hMemDC, 0, 0, SRCCOPY);
	}

	void	OnDestroy( void ){
		HBITMAP	hBmp;
		hBmp	= (HBITMAP)::SelectObject( m_hMemDC, m_hMemPrev );
		::DeleteObject( hBmp );
		::DeleteObject( m_hMemDC );

		hBmp	= (HBITMAP)::SelectObject( m_hBlockDC, m_hBlockPrev );
		::DeleteObject( hBmp );
		::DeleteObject( m_hBlockDC );
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
	if( Form.Create(_T(APPLICATION_NAME), 0, 0, 100, 100, 24*10, 24*20) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
