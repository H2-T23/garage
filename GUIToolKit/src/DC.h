#pragma once

class CDC {
public:
	HDC			m_hDC;
	
public:
	CDC( void ) : m_hDC(NULL) {}
	CDC( HDC hDC ) : m_hDC(hDC) {}
	CDC( CDC const& DC ) : m_hDC(DC.m_hDC) {}
	
	
public:
	void	MoveTo(int x, int y) const { ::MoveToEx(m_hDC,x,y,NULL); }
	void	LineTo(int x, int y) const { ::LineTo(m_hDC,x,y);		 }

};
