#pragma once

/****************************************************************************************
 *
 *
 *
 *
 *
 */
class CFile {
protected:
	HANDLE					m_hFile;

	virtual	DWORD					DesiredAccess( void ) const			{ return(GENERIC_READ|GENERIC_WRITE);	}
	virtual DWORD					ShareMode( void ) const				{ return(0);	}
	virtual LPSECURITY_ATTRIBUTES	SecurityAttributes( void ) const	{ return(NULL);	}
    virtual DWORD					CreationDisposition( void ) const	{ return(CREATE_ALWAYS);	}
    virtual DWORD					FlagsAndAttributes( void ) const	{ return(0);	}
    virtual HANDLE					TemplateFile( void ) const			{ return(NULL);	}

public:
	operator HANDLE () { return(m_hFile); }

	CFile( void ) : m_hFile(NULL) {
	}

	CFile( LPCTSTR lpszFilename ) : m_hFile(NULL) {
		Create(lpszFilename);
	}

	virtual ~CFile( void ){
		Close();
	}

	inline BOOL			IsOpen( void ) const {
		return(NULL!=m_hFile?TRUE:FALSE);
	}

	BOOL			Create( LPCTSTR lpszFilename ){
		Close();
		m_hFile	= ::CreateFile(lpszFilename
							, DesiredAccess()
							, ShareMode()
							, SecurityAttributes()
							, CreationDisposition()
							, FlagsAndAttributes()
							, TemplateFile() 
							);
		if( m_hFile == INVALID_HANDLE_VALUE ){
			Close();
		}
		return IsOpen();
	}

	virtual BOOL		Open( LPCTSTR lpszFilename ){
		return Create(lpszFilename);
	}

	virtual void		Close( void ){
		if(IsOpen())
			::CloseHandle(m_hFile);
		m_hFile	= NULL;
	}

	virtual DWORD		Write( LPVOID lpData, DWORD dwDataSize ){
		if( IsOpen() )
		{
			DWORD	dwSize	= 0;
			if( ::WriteFile(m_hFile, lpData, dwDataSize, &dwSize, NULL) )
			{
				return dwSize;
			}
		}
		return 0;
	}

	virtual DWORD			Read( LPVOID lpData, DWORD dwDataSize ){
		if( IsOpen() )
		{
			DWORD	dwSize	= 0;
			if( ::ReadFile(m_hFile, lpData, dwDataSize, &dwSize, NULL) )
			{
				return dwSize;
			}
		}
		return 0;
	}

	virtual DWORD			Seek( LONG lOffset, DWORD dwMove ){
		return ::SetFilePointer(m_hFile, lOffset, NULL, dwMove);
	}

	virtual DWORD			SeekBegin( void ){
		return Seek(0, FILE_BEGIN);
	}

	virtual DWORD			SeekEnd( void ){
		return Seek(0, FILE_END);
	}

	virtual BOOL			MoveEndOfFile( void ){
		return ::SetEndOfFile(m_hFile);
	}

	virtual	BOOL			Flush( void ){
		return ::FlushFileBuffers(m_hFile);
	}

	DWORD					GetFileSize( LPDWORD lpFileSizeHigh = NULL ){
		return ::GetFileSize(m_hFile, lpFileSizeHigh);
	}

	DWORD					GetFileType( void ){
		return ::GetFileType(m_hFile);
	}
};
/****************************************************************************************
 *
 *
 *
 *
 *
 */
class CReadOnlyFile : public CFile {
protected:
 	virtual DWORD			DesiredAccess( void ) const			{ return(GENERIC_READ);		}
    virtual DWORD			CreationDisposition( void ) const	{ return(OPEN_EXISTING);	}

public:
 	CReadOnlyFile( LPCTSTR lpszFilename ) : CFile() {
		Create( lpszFilename );
	}

	virtual BOOL		Create( LPCTSTR lpszFilename ){
		Close();
		m_hFile	= ::CreateFile(lpszFilename
							, DesiredAccess()
							, ShareMode()
							, SecurityAttributes()
							, CreationDisposition()
							, FlagsAndAttributes()
							, TemplateFile() 
							);
		if( m_hFile == INVALID_HANDLE_VALUE ){
			Close();
		}
		return IsOpen();
	}
};
/****************************************************************************************
 *
 *
 *
 *
 *
 */
class CTxtFile : public CFile {
public:
	CTxtFile( LPCTSTR lpszFilename ) : CFile(lpszFilename) {
	}

	int			WriteString( LPCTSTR lpLineBuff ){
		TString	str(lpLineBuff);
		return CTxtFile::WriteString( str );
	}

	int			WriteString( const TString& str ){
		return CFile::Write( (LPVOID)(LPCTSTR)str, str.Length()*sizeof(TCHAR) );
	}
};