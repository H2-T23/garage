#pragma once
#include <map>


enum COLOR_TYPE {
	COLOR_WHITE			= RGB(0xFF,0xFF,0xFF),
	COLOR_BLACK			= RGB(0x00,0x00,0x00),

	COLOR_RED			= RGB(0xFF,0x00,0x00),
	COLOR_GREEN			= RGB(0x00,0xFF,0x00),
	COLOR_BLUE			= RGB(0x00,0x00,0xFF),

	COLOR_CYAN			= RGB(0x00,0xFF,0xFF),
	COLOR_MAGENTA		= RGB(0xFF,0x00,0xFF),
	COLOR_YELLOW		= RGB(0xFF,0xFF,0x00),

	COLOR_GLAY			= RGB(0x80,0x80,0x80),
	COLOR_DARKGRAY		= RGB(0xA9,0xA9,0xA9),
	COLOR_SILVER		= RGB(0xC0,0xC0,0xC0),
	COLOR_LIGHTGRAY		= RGB(0xD3,0xD3,0xD3),
	
	COLOR_ROYALBLUE		= RGB(0x41,0x69,0xE1),
	COLOR_MIDNIGHTBLUE	= RGB(0x19,0x19,0x70),
	COLOR_NAVY			= RGB(0x00,0x00,0x80),
	COLOR_DARKBLUE		= RGB(0x00,0x00,0x8B),
	COLOR_MEDIUMBLUE	= RGB(0x00,0x00,0xCD),

	COLOR_PINK			= RGB(0xFF,0xC0,0xCB),
};

typedef struct COLORINFO {
	COLORREF	rgbText;
	COLORREF	rgbBk;
} COLORINFO, *LPCOLORINFO;

class ColorInfoCollection {
protected:
	typedef std::pair<DWORD, COLORINFO>	PAIR;
	typedef std::map<DWORD, COLORINFO> MAP;
	typedef MAP::iterator ITERATOR;
	typedef MAP::const_iterator CONST_ITERATOR;
	MAP items;

public:
	BOOL		Find( DWORD key ) const;
	VOID		Set(DWORD key, COLORINFO& clr);
	COLORINFO*	Get(DWORD key);
	VOID		Remove(DWORD key);
	VOID		RemoveAll(void);
};

template<typename CONTROL>
class TColorControl : public CONTROL {
	typedef TColorControl<CONTROL> BASE;
private:
	ColorInfoCollection	m_items;
	COLORREF			m_rgbText;
	COLORREF			m_rgbBk;

public:
	TColorControl(COLORREF rgbText=COLOR_BLACK, COLORREF rgbBk=COLOR_MAGENTA)
		: m_rgbText(rgbText)
		, m_rgbBk(rgbBk)
	{
	}
	virtual ~TColorControl(VOID)
	{
	}
public:
	void SetDefaultTextColor(COLORREF rgb)
	{
		m_rgbText = rgb;
	}
	void SetDefaultBkColor(COLORREF rgb)
	{
		m_rgbBk = rgb;
	}
	COLORREF GetDefaultTextColor(VOID) const
	{
		return m_rgbText;
	}
	COLORREF GetDefaultBkColor(VOID) const
	{
		return m_rgbBk;
	}
	VOID SetTextColor(DWORD key, COLORREF rgb)
	{
		COLORINFO* pColorInfo = m_items.Get( key );
		if( pColorInfo ){
			pColorInof->rgbText	= rgb;
			return;
		}

		COLORINFO colorInfo = {0};
		colorInfo.rgbText	= rgb;
		colorInfo.rgbBk		= GetDefaultBkColor();
		m_items.Set( key, colorInfo );
	}
	VOID SetTextColor(WORD col, WORD row, COLORREF rgb)
	{
		SetTextColor( (DWORD)MAKELONG(col,row), rgb );
	}
	VOID SetBkColor(DWORD key, COLORREF rgb)
	{
		COLORINFO* pColorInfo = m_items.Get( key );
		if( pColorInfo ){
			pColorInfo->rgbBk	= rgb;
			return;
		}

		COLORINFO colorInfo = {0};
		colorInfo.rgbText	= GetDefaultTextColor();
		colorInfo.rgbBk		= rgb;
		m_items.Set( key, colorInfo );
	}
	VOID SetBkColor(WORD col, WORD row, COLORREF rgb)
	{
		SetBkColor( (DWORD)MAKELONG(col,row), rgb );
	}
	COLORREF GetTextColor(DWORD key) /*const*/
	{
		COLORINFO* pColorInfo = m_items.Get( key );
		if( pColorInfo ){
			return pColorInfo->rgbText;
		}
		return GetDefaultTextColor();
	}
	COLORREF GetTextColor(WORD col, WORD row)
	{
		return GetTextColor( (DWORD)MAKELONG(col,row) );
	}
	COLORREF GetBkColor(DWORD key) /*const*/
	{
		COLORINFO* pColorInfo = m_items.Get( key );
		if( pColorInfo ){
			return pColorInfo->rgbBk;
		}
		return GetDefaultBkColor();
	}
	COLORREF GetBkColor(WORD col, WORD row)
	{
		return GetBkColor( (DWORD)MAKELONG(col,row) );
	}
};
