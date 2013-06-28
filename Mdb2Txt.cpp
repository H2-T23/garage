#include "Mdb2Txt.h"

/**************************************************************************
 *
 */
CMdb2Txt::CMdb2Txt(	LPCTSTR		lpszMDBFileName		
				,	LPCTSTR		lpszTXTFileName		/*= NULL*/
				,	HWND		hWnd				/*= NULL*/ )
{
	Construct( lpszMDBFileName, lpszTXTFileName, hWnd );
}

/**************************************************************************
 *
 */
BOOL	CMdb2Txt::Construct(	LPCTSTR		lpszMDBFileName	/*= NULL*/
							,	LPCTSTR		lpszTXTFileName	/*= NULL*/
							,	HWND		hWnd			/*= NULL*/ 
							)
{
	if( Setup( lpszMDBFileName, lpszTXTFileName, hWnd ) )
	{
		if( Mdb2Txt( m_strMDBFileName, m_strTXTFileName ) )
		{
			CString	strCmd;
			strCmd.Format( _T("explorer %s\n"), m_strTXTFileName );
			ExecuteProcess( strCmd );
			return TRUE;
		}
		else
		{
			::DeleteFile( m_strTXTFileName );
		}
	}
	return FALSE;
}

/**************************************************************************
 *
 */
inline
BOOL	CMdb2Txt::IsFileExist( LPCTSTR lpszFileName ) const
{
	if( DWORD(-1) == ::GetFileAttributes( lpszFileName ) ){
		return FALSE;
	}
	return TRUE;
}

/**************************************************************************
 *
 */
VOID	CMdb2Txt::ExecuteProcess( LPCTSTR lpszCmd )
{
	PROCESS_INFORMATION	pi;
	STARTUPINFO			si;
	
	ZeroMemory( &si, sizeof(si) );
	si.cb	= sizeof(si);
	::CreateProcess( NULL, (LPTSTR)lpszCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi );
}

/**************************************************************************
 *
 */
BOOL	CMdb2Txt::Setup( LPCTSTR lpszMDBFileName, LPCTSTR lpszTXTFileName, HWND hWnd )
{
	TCHAR	szPath[ 2048 ];
	ZeroMemory( szPath, sizeof(szPath) );

	::GetCurrentDirectory( 2048, szPath );
	_tcscat( szPath, _T("\\mdb.txt") );

	if( lpszMDBFileName == NULL )
	{
		OPENFILENAME	ofn;
		ZeroMemory( &ofn, sizeof(ofn) );

		ofn.lStructSize			= sizeof(ofn);
		ofn.hwndOwner			= hWnd;
		
		ofn.lpstrInitialDir		= szPath;
		ofn.lpstrFile			= szPath;
		ofn.nMaxFile			= sizeof(szPath);
		ofn.Flags				= 0;
		ofn.lpstrTitle			= _T("Open File");
		ofn.lpstrFilter			= _T("MDB files |*.mdb|\0*.mdb\0");
		ofn.lpstrDefExt			= _T("mdb");

		if( !GetOpenFileName( &ofn ) ){
			return FALSE;
		}

		if( !IsFileExist( szPath ) ){
			return FALSE;
		}

		m_strMDBFileName	= szPath;
	}
	else
	{
		if( !IsFileExist( lpszMDBFileName ) ){
			return FALSE;
		}

		m_strMDBFileName	= lpszMDBFileName;
	}

	if( lpszTXTFileName == NULL )
	{
		_tcscpy( szPath, m_strMDBFileName );
		_tcscat( szPath, _T(".txt") );

		OPENFILENAME	ofn;
		ZeroMemory( &ofn, sizeof(ofn) );

		ofn.lStructSize			= sizeof(ofn);
		ofn.hwndOwner			= hWnd;
		
		ofn.lpstrInitialDir		= szPath;
		ofn.lpstrFile			= szPath;
		ofn.nMaxFile			= sizeof(szPath);
		ofn.Flags				= OFN_OVERWRITEPROMPT;
		ofn.lpstrTitle			= _T("Save File");
		ofn.lpstrFilter			= _T("Text files |*.txt|\0*.txt\0");
		ofn.lpstrDefExt			= _T("txt");

		if( !GetSaveFileName( &ofn ) ){
		//	_tcscat( szPath, _T(".txt") );
			return FALSE;
		}

		m_strTXTFileName	= szPath;
	}
	else
	{
		m_strTXTFileName	= lpszTXTFileName;
	}

	return TRUE;
}

/**************************************************************************
 *
 */
BOOL	CMdb2Txt::Mdb2Txt( LPCTSTR lpszMDBFileName, LPCTSTR lpszTXTFileName )
{
	std::vector<CString>	vecTableNames;

	CStdioFile		file;
	CString			str;

	CDaoDatabase		db;
	CDaoTableDefInfo	tableInfo;

	try
	{
		db.Open( lpszMDBFileName );
		file.Open( lpszTXTFileName, CFile::modeCreate | CFile::modeWrite );
		file.WriteString( db.GetName() );
		file.WriteString( _T("\n") );

		short	cnt = db.GetTableDefCount();
		for( short i=0; i<cnt; i++ ){
			db.GetTableDefInfo( i, tableInfo );
			TRACE( _T("%s\n"), tableInfo.m_strName );
			if( !(tableInfo.m_lAttributes & dbSystemObject) )
			{
				vecTableNames.push_back( tableInfo.m_strName );
			}
		}
		
		CDaoRecordset		recordset( &db );
		CDaoTableDef		table( &db );
		CDaoFieldInfo		fieldInfo;

		CString				strSQL;

		std::vector<CString>::iterator it = vecTableNames.begin();
		while( it != vecTableNames.end() )
		{
			CString		strTableName( *it );
			str.Format( _T("[%s]\n"), strTableName );
			file.WriteString( str );

			table.Open( strTableName );

			if( table.IsOpen() )
			{
				CString	strFirstIndexFieldName;
				str = _T("");
				for( short i=0; i<table.GetFieldCount(); i++ ){
					table.GetFieldInfo( i, fieldInfo ); 
					str.AppendFormat( _T("\t%s,"), fieldInfo.m_strName );

					if( i == 0 )
						strFirstIndexFieldName = fieldInfo.m_strName;
				}
				str.Append( _T("\n") );
				file.WriteString( str );

				strSQL.Format( _T("SELECT * FROM %s ORDER BY %s ASC"), strTableName, strFirstIndexFieldName );

				recordset.Open( dbOpenSnapshot, strSQL, dbReadOnly );
				while( !recordset.IsEOF() )
				{
					str = _T("");
					for( short i=0; i<recordset.GetFieldCount(); i++ ){
						COleVariant var = recordset.GetFieldValue( i );
						VariantChangeType( &var, &var, 0, VT_BSTR );
						str.AppendFormat( _T("\t%s,"), var.bstrVal );
					}
					str.Append( _T("\n") );
					file.WriteString( str );

					recordset.MoveNext();
				}

				recordset.Close();
				table.Close();
			}

			it++;
		}

		db.Close();

		file.Flush();
		file.Close();
	}
	catch( CDaoException* pEx ){
		pEx->Delete();
		return FALSE;
	}
	catch( CMemoryException* pEx ){
		pEx->Delete();
		return FALSE;
	}
	catch( CFileException* pEx ){
		pEx->Delete();
		return FALSE;
	}
	catch( CException* pEx ){
		pEx->Delete();
		return FALSE;
	}

	return TRUE;
}
