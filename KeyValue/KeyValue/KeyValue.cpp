// KeyValue.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "KeyValue.h"

#include "Inet.h"
#include "Thread.h"
#include "IOCP.h"

#include "ISocket.h"
#include "IEventHandler.h"

#include "KeyValueClient.h"
#include "KeyValueServerForm.h"

#include "SessionProtocol.h"

#include "MessageClient.h"

using namespace INET;
using namespace MT;
using namespace GENERIC;
using namespace IOCP;

/**********************************************************************************
 *
 *
 *
 */
class CKeyValueProtocol : public IEventHandler {
public:
	CBuffer				m_RecvBuffer;
	CBuffer				m_SendBuffer;

	CMessageClient		m_msg;

	CKeyValueProtocol( void )
	{
		m_msg.Connect("127.0.0.1", 5511);
	}

	void	Done( std::string& str ){
		char	strCmd[256], strKey[256], strValue[256];

		str.append( "\0" );
		sscanf( str.c_str(), "%s %s %s\n", strCmd, strKey, strValue );
		if( m_msg.IsConnected() ){
			m_msg.Send( str.c_str() );
			TRACE( str.c_str() );
		}

		if( str.compare(0, 3, "Set") == 0 )
		{
			CKeyValue::Instance().Set( strKey, strValue );
		}
		else 
		if( str.compare(0, 3, "Get") == 0 )
		{
			std::string	str( CKeyValue::Instance().Get( strKey ) );
			str.append( "\n" );
			m_SendBuffer.Set( (LPBYTE)str.c_str(), str.length() );
		}
	}

	int		OnRead( LPVOID lpParam, DWORD dwSize ){

		CBuffer*	pBuff	= (CBuffer*)(lpParam);
		if( pBuff )
		{
			//BYTE* p = pBuff->At( 0 );
			//while( *p ){
			//	TRACE("%c ",(char)*p);
			//	p++;
			//}
			//TRACE("\n");
		}

		m_RecvBuffer.Append( *pBuff );


		int	iStart	= 0;
		int iEnd	= m_RecvBuffer.Find( (BYTE)'\n', iStart );
		if( iEnd < 0 )
		{
			return 0;
		}
		else
		{
			std::string	strTotal( (char*)m_RecvBuffer.At( 0 ) );
			m_RecvBuffer.Clear();

			int	len	= strTotal.length();

			std::string	str;
			do{
				int	sz	= (iEnd + 1) - iStart;
				str.resize( sz );
				memcpy( &str.at( 0 ), &strTotal.at( iStart ), sz );

				Done( str );

				iStart = iEnd + 1;
				len -= sz;
				iEnd	= strTotal.find( '\n', iStart );
			}while( iEnd > 0 );

			if( len > 0 )
			{
				int next	= strTotal.length() - len;
				m_RecvBuffer.Set( (LPBYTE)&strTotal.at( next ), len );
			}
		
			return 1;
		}
	}

	int		OnWrite( LPVOID lpParam ){
		CBuffer* pBuff = (CBuffer*)lpParam;
		if( pBuff )
		{
			if( m_SendBuffer.Size() > 0 )
			{
				pBuff->Set( m_SendBuffer );
			}

			m_SendBuffer.Clear();
			return 1;
		}

		return 0;
	}

	void	OnClose( void ){
	}
};
/**********************************************************************************
 *
 *
 *
 */
//class CIOCPServer	: public IRunnable {
//public:
//	CIOCP				m_IOCP;
//	CEvent				m_evntShutdown;
//	CListenSocket		m_sockListen;
//
//	CThread*			m_pThreads[ 4 ];
//
//	void	run( void )
//	{
//		m_evntShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);
//
//		m_IOCP.Create();
//
//		for( int i=0; i<4; i++ ){
//			m_pThreads[ i ]	= new CThread( new CIOCPWorker(&m_IOCP, &m_evntShutdown) );
//			m_pThreads[ i ]->Start();
//		}
//
//		if( m_sockListen.Create( 54321 ) )
//		{
//			while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_evntShutdown.m_hObject, 0) )
//			{
//				SOCKADDR_IN		addr;
//				int	len	= sizeof(addr);
//				CSocket	sock	= m_sockListen.Accept( (LPSOCKADDR)&addr, len );
//				if( sock.IsValid() )
//				{
//					SOCKET	s	= sock.Detach();
//					CSession*	pSession	= (CSession*)CSessionManager::Instance().New<CKeyValueProtocol>( (DWORD)s, (LPVOID)&s );
//					if( pSession )
//					{
//						m_IOCP.IoCompletionPort( s, (ULONG_PTR)pSession );
//
//						pSession->OnEvent( OP_CONNECT );
//					}
//				}
//			}
//		}
//
//		for( int i=0; i<4; i++ ){
//			m_pThreads[ i ]->exit();
//		}
//	}
//};
/**********************************************************************************
 *
 *
 *
 */
class CKeyValueClient	: public IRunnable {
protected:
	void		run( void )
	{
		CClientForm	Form;
		if( Form.Create(_T("KeyValue Client"), 0, 0, 100, 100, 600, 200) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
	}
};
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CThread*	pKeyValue	= new CThread( new CKeyValueServerFormThread() );
	if( pKeyValue )
	{
		pKeyValue->Start();
	}


//	CThread*	pThreadServer	= new CThread( new CIOCPServer() );
	CThread*	pThreadServer	= new CThread( new TIOCPServer<CKeyValueProtocol, 54321>() );
	if( pThreadServer )
	{
		pThreadServer->Start();

		Sleep( 500 );

		CThread*	pThreadClient	= new CThread( new CKeyValueClient() );
		pThreadClient->Start();
		pThreadServer->Wait();
	}

	INET::WinInet::Cleanup();
}
