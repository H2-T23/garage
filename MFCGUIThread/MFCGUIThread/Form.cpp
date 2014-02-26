// Form.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCGUIThread.h"
#include "Form.h"


#define MIN_ROI_HEIGHT		64	
#define MIN_ROI_WIDTH		64	

#define	IN_RANGE(VALUE,MIN,MAX)		((MIN<=VALUE)&&(VALUE<=MAX))

void	TraceROI( const CRect& rc ){
	CPoint	pt	= rc.CenterPoint();
	TRACE(_T("center(%d, %d)\n"), pt.x, pt.y);
	TRACE(_T("width: %d\n"), rc.Width());
	TRACE(_T("height: %d\n"), rc.Height());
}

template<typename TYPE>
inline bool		InRange(TYPE value, TYPE min, TYPE max){
	return((min<=value) && (value<=max));
}

bool	PtInRectEx( const CRect& rc, const CPoint& pt ){
//	return( IN_RANGE(pt.x, rc.left, rc.right) && IN_RANGE(pt.y, rc.top, rc.bottom) );
	return( InRange(pt.x, rc.left, rc.right) && InRange(pt.y, rc.top, rc.bottom) );
}

bool	IsWithinArea( const CRect& rcArea, const CRect& rcROI ){
//	return(rcArea.PtInRect(rcROI.TopLeft()) && rcArea.PtInRect(rcROI.BottomRight()));
	return( PtInRectEx(rcArea, rcROI.TopLeft()) && PtInRectEx(rcArea, rcROI.BottomRight()) );
}

bool	CorrectSize( const CRect& rcArea, CRect& rcROI )
{
//	const int	MIN_WIDTH	= (rcArea.Width()	/ 2);
//	const int	MIN_HEIGHT	= (rcArea.Height()	/ 2);

	int	nNewWidth	= rcROI.Width();
	if( rcROI.Width() > rcArea.Width() )
	{
		nNewWidth	= rcArea.Width();
	}
	else 
	if( rcROI.Width() < MIN_ROI_HEIGHT )
	{
		nNewWidth	= MIN_ROI_HEIGHT;
	}

	int	nNewHeight	= rcROI.Height();
	if( rcROI.Height() > rcArea.Height() )
	{
		nNewHeight	= rcArea.Height();
	}
	else 
	if( rcROI.Height() < MIN_ROI_HEIGHT )
	{
		nNewHeight	= MIN_ROI_HEIGHT;
	}


	bool	bModify	= false;
	int		nDiff	= 0;

	if( nDiff = (nNewWidth - rcROI.Width()) )
	{
		bModify	= true;

		if( (nDiff % 2) == 0 )
			rcROI.InflateRect( nDiff/2, 0, nDiff/2, 0 );
		else
			rcROI.InflateRect( nDiff/2, 0, nDiff/2 + 1, 0 );
	}

	if( nDiff = (nNewHeight - rcROI.Height()) )
	{
		bModify	= true;

		if( (nDiff % 2) == 0 )
			rcROI.InflateRect( 0, nDiff/2, 0, nDiff/2 );
		else
			rcROI.InflateRect( 0, nDiff/2, 0, nDiff/2 + 1 );
	}

	return(bModify);
}


bool	CorrectPosition( const CRect& rcArea, CRect& rcROI )
{
	bool	bModify	= false;

	if(	!IsWithinArea(rcArea, rcROI) )
	{
		int	nDiffX	= 0;
		int	nDiffY	= 0;

		if( rcArea.left > rcROI.left ){
			nDiffX	= rcArea.left - rcROI.left;
		}
		else
		if( rcArea.right < rcROI.right ){
			nDiffX	= rcArea.right - rcROI.right;
		}

		if( rcArea.top > rcROI.top ){
			nDiffY	= rcArea.top - rcROI.top;
		}
		else
		if( rcArea.bottom < rcROI.bottom ){
			nDiffY	= rcArea.bottom - rcROI.bottom;
		}

		rcROI.OffsetRect( nDiffX, nDiffY );
		bModify	= true;
	}

	return(bModify);
}

void	ROICorrect( const CRect& rcArea, CRect& rcROI )
{
	TraceROI(rcROI);
	CorrectSize(rcArea, rcROI);
	TraceROI(rcROI);
	CorrectPosition(rcArea, rcROI);
	TraceROI(rcROI);
}


// CForm

IMPLEMENT_DYNAMIC(CForm, CWnd)

CForm::CForm()
{
	TRACE(_T("CForm::CForm().\n"));

	CRect	rcArea(CPoint(100,100), CSize(500,500));
	CRect	rcROI(CPoint(-100, 700), CSize(800, 50));

	TraceROI(rcROI);
	CorrectSize(rcArea, rcROI);
	TraceROI(rcROI);
	CorrectPosition(rcArea, rcROI);
	TraceROI(rcROI);
	CorrectPosition(rcArea, rcROI);
	TraceROI(rcROI);
}

CForm::~CForm()
{
	TRACE(_T("CForm::~CForm().\n"));
}


BEGIN_MESSAGE_MAP(CForm, CWnd)
END_MESSAGE_MAP()



// CForm メッセージ ハンドラ



