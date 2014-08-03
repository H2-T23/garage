// Graphics.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Graphics.h"

#include "UseToolkit.h"

#include "BitmapFile.h"


/*
[スケーリング]
拡大時：線形補間
縮小時：サンプリング

補間を行わない場合の拡大処理は出力する座標から入力画像内の座標を計算し、１画素ずつ転送を行う。
	→	単純、高速


点と点の間を直線で近似する補間方式
	直線上のP1とP2の間のD
		D	= (1 - d) x P1 + d x P2

		dはDがP1からP2の間でどの位置にあるのかを割合で表したもの

		d : 0-1023
			→	D	=  (P1 x (1023 - d)) / 1024 + (P2 x d) / 1024


 */

#include "ThreeDMath.h"
using namespace ThreeDMath;


namespace AFFINE {

	template<typename TYPE, int DIMENSION=3>
	class TMatrix {
	public:
		enum {
			dimension	= DIMENSION
		};

		TYPE			m[ DIMENSION ][ DIMENSION ];
	};


	double		Coef( int i, int j, const TMatrix<double>& mtx ){
		TMatrix<double>	wk;

		double	dCoef	= 0.0;

		for( int x=0; x<mtx.dimension; x++ ){
			for( int y=0; y<mtx.dimension; y++ )
			{
				if( (x < i) && (y < j) )
				{
					wk.m[ x ][ y ]		= mtx.m[ x ][ y ];
				}
				else
				if( (x < i) && (y < j) )
				{
					wk.m[ x ][ y - 1 ]	= mtx.m[ x ][ y ];
				}
				else
				if( (x > i) && (y < j) )
				{
					wk.m[ x - 1 ][ y ]	= mtx.m[ x ][ y ]; 
				}
				else
				if( (x > i) && (i < j) )
				{
					d
				}
				else
				if( 
			}
		}
	}

	bool		Inverse( TMatrix<double>& mtx ){
		TMatrix<double>	coef;

		for( int i=0; i<m.dimension; i++ ){
			for( int j=0; j<m.dimension; j++ )
			{
				coef.m[ i ][ j ]	= Coef(i, j, mtx);
			}
		}


	}


	class CMatrix : public TMatrix<double,3> {
	public:
		void		Set(	double d00, double d01, double d02
						,	double d10, double d11, double d12
						,	double d20, double d21, double d22	){
			m[ 0 ][ 0 ]	= d00;
			m[ 0 ][ 1 ]	= d01;
			m[ 0 ][ 2 ]	= d02;

			m[ 1 ][ 0 ]	= d10;
			m[ 1 ][ 1 ]	= d11;
			m[ 1 ][ 2 ]	= d12;

			m[ 2 ][ 0 ]	= d20;
			m[ 2 ][ 1 ]	= d21;
			m[ 2 ][ 2 ]	= d22;						
		}


		double		det( void ){
			return( m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[2][1]*m[1][0]
				-	m[0][0]*m[2][1]*m[1][2] - m[0][1]*m[1][0]*m[2][2] - m[2][0]*m[1][1]*m[0][2]);
		}
		
		CMatrix		inverse( void ){
			double	d	= det();

			if( d == 0.0 ){
				d	= 1.0;
			}
		}
	};
}

/**********************************************************************************
 */
typedef struct RGBA {
	BYTE	r,g,b,a;
} RGBA;
/**********************************************************************************
 */
typedef struct YUV {
	BYTE	y,u,v;
} YUV;
/**********************************************************************************
 */
YUV		RGBtoYUV(BYTE r,BYTE g,BYTE b){
	YUV		yuv;
	yuv.y	= ( 0.299 * r) + (0.587 * g) + (0.114 * b);
	yuv.u	= (-0.169 * r) - (0.331 * g) + (0.500 * b);
	yuv.v	= ( 0.500 * r) - (0.419 * g) - (0.081 * b);
	return yuv;
}
/**********************************************************************************
 */
RGBA	YUVtoRGB(BYTE y,BYTE u,BYTE v){
	RGBA	rgb;
	rgb.r	= (1.000 * y)               + (1.402 * v);
	rgb.g	= (1.000 * y) - (0.344 * u) - (0.714 * v);
	rgb.b	= (1.000 * y) + (1.772 * u);
	rgb.a	= 0;
	return rgb;
}
/**********************************************************************************
 */
typedef union {
	COLORREF	rgb;
	RGBA		rgba;
	YUV			yuv;
	void		convertRGB( void ){
		rgba	= YUVtoRGB(rgba.r, rgba.g, rgba.b); 
	}
	void		convertYUV( void ){
		yuv		= RGBtoYUV(yuv.y, yuv.u, yuv.v); 
	}
} PIXEL;
/**********************************************************************************

#define	Y_SIZE		(400)
#define	X_SIZE		(400)

typedef BYTE	BWImage[ Y_SIZE ][ X_SIZE ];
 */
/**********************************************************************************
 *
 *		0.0	｜ width　｜
 *		　―＋――――→Ｘ
 *			｜
 *	　Height｜
 *			｜
 *		　―↓
 *			Ｙ
 */
template<typename TYPE>
class TRawImage {
protected:
	int									m_nHeight;	// Y
	int									m_nWidth;	// X
	std::vector<std::vector<TYPE>>		m_Image;	// [ Y ][ X ]

	typedef	TRawImage<TYPE>				SUPER;

public:
	int			HEIGHT(void){return(m_nHeight);}
	int			WIDTH(void){return(m_nWidth);}

	int			Y_SIZE(void){return(m_nHeight);}
	int			X_SIZE(void){return(m_nWidth);}

public:
	BOOL	Create( const int nWidth, const int nHeight ){
		BOOL	bResult	= FALSE;
		try
		{
			m_nHeight	= nHeight;
			m_nWidth	= nWidth;

			m_Image.resize( m_nHeight );
			for( int y=0; y<m_nHeight; y++ ){
				m_Image[ y ].resize( m_nWidth );
			}

		//	m_Image[ m_nHeight ][ m_nWidth ]; 

			bResult	= TRUE;
		}
		catch( ... )
		{
			bResult	= FALSE;
			Destroy();
		}
		return bResult;
	}

	void	Destroy( void ){
		if( (m_nHeight > 0) && (m_nWidth > 0) )
		{
			for( int y=0; y<m_nHeight; y++ ){
				m_Image[ y ].clear();
			}
			m_Image.clear();
		}
		m_nHeight	= 0;
		m_nWidth	= 0;
	}

public:
	TRawImage( void ) : m_nWidth(0), m_nHeight(0){
	}
	TRawImage( const int nWidth, const int nHeight ){
		Create(nWidth, nHeight);
	}

	virtual ~TRawImage( void ){
		Destroy();
	}

public:
	virtual TYPE&		GetPixel( const int x, const int y ){
		return m_Image[ y ][ x ];	
	//	return m_Image[ x ][ y ];	
	}

public:
	std::vector<TYPE>&	operator[]( const int y ){
		return m_Image[ y ];
	}
};
/**********************************************************************************
 */
typedef	TRawImage<BYTE>	CBWImage;
/**********************************************************************************
 */
class CRGBImage : public TRawImage<PIXEL> {
public:
	CRGBImage( void ) : SUPER() {
	}
	CRGBImage( const int nWidth, const int nHeight, const BYTE* pRawImage ){
		Create( nWidth, nHeight, pRawImage );
	}

	BOOL	Create( const int nWidth, const int nHeight, const BYTE* pRawImage ){
		BOOL	bResult	= SUPER::Create( nWidth, nHeight );

		BYTE	r,g,b;

		unsigned ofs	= 0;
		for( int y=0; y<nHeight; y++ ){
			for( int x=0; x<nWidth; x++ )
			{
				int	X	= (nWidth - 1  - x);
				int	Y	= (nHeight - 1 - y);

				r	= pRawImage[ ofs += 1 ];
				g	= pRawImage[ ofs += 1 ];
				b	= pRawImage[ ofs += 1 ];
	
			//	m_Image[ X ][ Y ].rgb	= RGB(r,g,b);
				m_Image[ Y ][ X ].rgb	= RGB(r,g,b);
			}
		}

		return bResult;
	}
};
/**********************************************************************************
 */
class CScene {
public:
	CRect		m_rcScreen;
	CRect		m_rcViewport;

public:
	void	Screen( int nWidth, int nHeight ){
		m_rcScreen.Set( 0, 0, nWidth, nHeight );
	}

	void	Viewport( int x, int y, int nWidth, int nHeight ){
		m_rcViewport.Set( x, y, nWidth, nHeight );
	}
};
/**********************************************************************************
 */
void	ScaleNear( CBWImage& in, CBWImage& out, float zoomX, float zoomY )
{
	int	half_y	= in.Y_SIZE() / 2;	// half_y
	int	half_x	= in.X_SIZE() / 2;	// half_x

	for( int y=-half_y; y<half_y; y++ ){
		for( int x=-half_x; x<half_x; x++ )
		{
			int	yy	= (y > 0 ? y / zoomY + 0.5 : y / zoomY - 0.5);
			int	xx	= (x > 0 ? x / zoomX + 0.5 : x / zoomX - 0.5);

			if( (-half_y <= yy) && (yy < half_y)
			&&	(-half_x <= xx) && (xx < half_x) )
			{
				out[ y + half_y ][ x + half_x ]	= in[ yy + half_y ][ xx + half_x ];
			}
			else
			{
				out[ y + half_y ][ x + half_x ]	= 0;
			}
		}
	}
}
/**********************************************************************************
 */
void	ScaleLine( CBWImage& in, CBWImage& out, float zoomX, float zoomY )
{
	int	half_y	= in.Y_SIZE() / 2;	// half_y
	int	half_x	= in.X_SIZE() / 2;	// half_x

	for( int y=-half_y; y<half_y; y++ ){
		for( int x=-half_x; x<half_x; x++ )
		{
		//	float	y	= float(i / zoomY);
		//	float	x	= float(j / zoomX);

			int		yy	= (float(y / zoomY) > 0 ? float(y / zoomY) : float(y / zoomY)-1);
			int		xx	= (float(x / zoomX) > 0 ? float(x / zoomX) : float(x / zoomX)-1);

			float	q	= float(y / zoomY) - yy;
			float	p	= float(x / zoomX) - xx;

			int		d	= 0;
			if( (-half_y <= yy) && (yy < half_y) 
			&&	(-half_x <= xx) && (xx < half_x) )
			{
				d	= (1.0 - q)	* ((1.0 - p) * in[ yy     + half_y ][ xx     + half_x ]
								+		  p  * in[ yy     + half_y ][ xx + 1 + half_x ])
					+		 q	* ((1.0 - p) * in[ yy + 1 + half_y ][ xx     + half_x ]
								+		  p  * in[ yy + 1 + half_y ][ xx + 1 + half_x ]);
			}

			if( d < 0 )
				d	= 0;
			if( d > 255 )
				d	= 255;

			out[ y + half_y ][ x + half_x ]	= d;
		}
	}
}
/**********************************************************************************
 */
class CMainForm : public CForm {
protected:
	CBitmapFile			m_bmpfile;

	CRGBImage			m_rgbImage;
	CBWImage			m_bwImage;

	BOOL		OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_bmpfile.LoadImage( _T("Happy.bmp") );

		int	nWidth			= m_bmpfile.Width();
		int	nHeight			= m_bmpfile.Height();
		const BYTE*	pImage	= m_bmpfile.RawImage();

		m_rgbImage.Create( nWidth, nHeight, pImage );
		m_bwImage.Create( nWidth, nHeight );

		for( int y=0; y<nHeight; y++ ){
			for( int x=0; x<nWidth; x++ )
			{
				PIXEL	pix	= m_rgbImage.GetPixel(x, y);
				pix.convertYUV();
				m_bwImage[ y ][ x ]	= pix.yuv.y;
			}
		}

		return TRUE;
	}

	void		OnPaint( HDC hDC ){
		if( hDC != NULL )
		{
			CMatrix3x3		mtxScreen;
			mtxScreen.Set(	0, 0, 0
						,	0, 0, 0
						,	0, 0, 0	);

			int	nWidth			= m_bmpfile.Width();
			int	nHeight			= m_bmpfile.Height();
			const BYTE*	pImage = m_bmpfile.RawImage();

		#if 1
			for( int y=0; y<nHeight; y++ ){
				for( int x=0; x<nWidth; x++ )
				{
					::SetPixel(hDC, x+100, y+10, m_rgbImage[ y ][ x ].rgb);
				}
			}


			CBWImage		out(nWidth, nHeight);

			ScaleLine(m_bwImage, out, 2.0f, 2.0f);

			for( int y=0; y<nHeight; y++ ){
				for( int x=0; x<nWidth; x++ )
				{
					::SetPixel(hDC, x+500, y+10, RGB(out[y][x],out[y][x],out[y][x]) );
				//	::SetPixel(hDC, x+600, y+100, RGB(bwImage[y][x],bwImage[y][x],bwImage[y][x]) );
				}
			}

			ScaleNear(m_bwImage, out, 2.0f, 2.0f);

			for( int y=0; y<nHeight; y++ ){
				for( int x=0; x<nWidth; x++ )
				{
					::SetPixel(hDC, x+900, y+10, RGB(out[y][x],out[y][x],out[y][x]) );
				//	::SetPixel(hDC, x+600, y+100, RGB(bwImage[y][x],bwImage[y][x],bwImage[y][x]) );
				}
			}
		#else
			BYTE	r,g,b;
			unsigned ofs	= 0;
			for( int y=0; y<nHeight; y++ ){
				for( int x=0; x<nWidth; x++ )
				{
					r	= pImage[ ofs += 1 ];
					g	= pImage[ ofs += 1 ];
					b	= pImage[ ofs += 1 ];
					COLORREF	rgb = RGB(r,g,b);
					::SetPixel( hDC, (nWidth - x), (nHeight - y), rgb );
				//	::SetPixel( hDC, x, y, pImage[ ofs += 1 ] );
				//	::SetPixel( hDC, x, y, pImage[ ofs += 1 ] );
				//	::SetPixel( hDC, x, y, pImage[ ofs += 1 ] );
				}
			}
		#endif
		}
	}
};
/**********************************************************************************
 */
class CMD5 {
public:
	std::vector<BYTE>		Target;
	std::vector<BYTE>		TargetEx;
	DWORD					A, B, C, D;

	void	Init( void ){
		A	= 0x67452301;
		B	= 0xefcdab89;
		C	= 0x98badcfe;
		D	= 0x10325476;
		Target.clear();
		TargetEx.clear();
	}

	BOOL	MD5( std::vector<BYTE>&	vData ){
		unsigned Len	= vData.size();
		unsigned n		= 1;
		if( (Len / 64) >= 1 ){
			n	= (Len / 64);
		}

		unsigned LenEx	= n * 64 + 64;

		TargetEx.resize( LenEx, 0x00 );

		int	shift = 0;
		for( int i=TargetEx.size()-1; i>=TargetEx.size()-sizeof(Len); i-- ){
			TargetEx[ i ]	= (Len >> shift) & 0xFF;
			shift += 8;
		}

		memcpy( &TargetEx.at(0), &vData.at(0), Len );
		TargetEx.at( Len )	= 0x80;

		return TRUE;
	}

	BOOL	MD5( const BYTE* pData, const unsigned nLen ){
		Init();
		SetTarget( pData, nLen );
		TRACE("%s", &Target.at(0) );
		return MD5( Target );
	}

	BOOL	MD5( const CHAR* pString, const unsigned nLen ){
		Init();
		SetTarget( pString, nLen );
		TRACE("%s", &Target.at(0) );
		return MD5( Target );
	}

	BOOL	MD5( const WCHAR* pString, const unsigned nLen ){
		Init();
		SetTarget( pString, nLen );
		TRACE(L"%s", &Target.at(0) );
		return MD5( Target );
	}

	template<typename TYPE>
	void	SetTarget( const TYPE* pData, const unsigned nNum ){
		Target.resize( nNum * sizeof(TYPE)/* + sizeof(TYPE)*/ );
		memcpy( &Target.at(0), (BYTE*)pData, Target.size() );
	}

	//template<typename TYPE>
	//BOOL	MD5( const TYPE* pData, const unsigned nNum ){
	//	Target.resize( nNum * sizeof(TYPE) + sizeof(TYPE) );
	//	memcpy( &Target.at(0), (BYTE*)pData, Target.size() );
	//	TRACE(L"%s", &Target.at(0) );
	//	return PrepareMD5( Target );
	//}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AString	strA("abdcdefg123456789");
	WString	strW(L"abdcdefg123456789");

	CMD5	md5;
	md5.MD5( (CHAR*)strA, strA.Length() );

	CMainForm	Form;
	if( Form.Create(_T(""), 0, 0, 10, 100, 1400, 600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}