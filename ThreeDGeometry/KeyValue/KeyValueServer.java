// KeyValueServer.java 

//package keyvalue;

import	java.io.*;
import	java.net.*;
import	java.util.*;


public class KeyValueServer {
	
	enum SessionState {
		PAHSE_1, PAHSE_2
	}
	
	interface IFKeyValue {
		public String		setKey( String strKey, String strValue );
		public String		getKey( String strKey );
		public String		removeKey( String strKey );
	}

	class KeyValueStore implements IFKeyValue {
		public Map<String,String>	map = Collections.synchronizedMap(new HashMap<String,String>());
		
		@Override public String		setKey( String strKey, String strValue ){
			map.put( strKey, strValue );
			return strValue;
		}
		
		@Override public String		getKey( String strKey ){
			if( map.containsKey( strKey ) ){
				return map.get( strKey );
			}
			return null;
		}
		
		@Override public String		removeKey( String strKey ){
			String str = getKey( strKey );
			if( str != null ){
				map.remove( strKey );
			}
			return str;
		}
	}

	class Session extends Thread implements IFKeyValue {
		private BufferedReader			in	= null;
		private PrintWriter				out	= null;
		private KeyValueStore			store = null;
		
		private SessionState			state	= SessionState.PAHSE_1;
		
		
		public Session( Socket sock, KeyValueStore store ) throws IOException {
			this.store	= store;
			this.in		= new BufferedReader( new InputStreamReader( sock.getInputStream() ) );
			this.out	= new PrintWriter( sock.getOutputStream(), true );
		}

		@Override public void			run() {
			try{
				String	str;
				while( (str = recive()) != null );
			}
			catch( IOException e ){
			}
			catch( NullPointerException e ){
			}

			System.out.println( "terminate thread" );
		}

		@Override protected void	finalize() throws Throwable {
			try{
				super.finalize();
			}
			catch( Exception e ){
			}
			finally{
				System.out.println( "destory thread instance" );
			}
		}

		public String		recive() throws IOException, NullPointerException {
			if( in	== null ) return null;
			if( out	== null ) return null;
			
			String		str = null;
			
			str = in.readLine();
			System.out.println( "> " + str );
			
			switch( state ){
			case PAHSE_1:
				{
					out.println( dispach( str ) );
				}
				break;
				
			case PAHSE_2:
				{
				}
				break;
			}
		
			return str;
		}
		
		private String		dispach( String strCmdline ){
			String strCmd = null, strKey = null, strValue = null, strResult = null;
			
			StringTokenizer tokenizer = new StringTokenizer( strCmdline );
			int idx = 0;
			while( tokenizer.hasMoreTokens() ){
				switch( idx ){
				case 0: 
					strCmd		= tokenizer.nextToken(); 
					break;
				case 1:
					strKey		= tokenizer.nextToken();
					break;
				case 2:
					strValue	= tokenizer.nextToken();
					break;
				}
				
				idx++;
				
				if( idx >= 3 )
					break;
			}
			
			System.out.printf( "CommandLine : %s %s %s\n", strCmd, strKey, strValue );

			if( strCmd != null )
			{
				if( strCmd.compareToIgnoreCase( "set" ) == 0 ){
					strResult = setKey( strKey, strValue );
					System.out.printf( "set( %s, %s ) : %s\n", strKey, strValue, strResult );
				}
				else
				if( strCmd.compareToIgnoreCase( "get" ) == 0 ){
					strResult = getKey( strKey );
					System.out.printf( "get( %s ) : %s\n", strKey, strResult );
				}
				else
				if( strCmd.compareToIgnoreCase( "replace" ) == 0 ){
					strResult = setKey( strKey, strValue );
					System.out.printf( "replace( %s, %s ) : %s\n", strKey, strValue, strResult );
				}
				else
				if( strCmd.compareToIgnoreCase( "delete" ) == 0 ){
					strResult = removeKey( strKey );
					System.out.printf( "delete( %s ) : %s\n", strKey, strResult );
				}
			}
			
			return strResult;
		}
		
		@Override public String	setKey( String strKey, String strValue ){
			return store.setKey( strKey, strValue );
		}
		
		@Override public String	getKey( String strKey ){
			return store.getKey( strKey );
		}
		
		@Override public String	removeKey( String strKey ){
			return store.removeKey( strKey );
		}
	}
	
	public KeyValueServer( int nPort ){
		ServerSocket	sockServer = null;
		
		try{
			sockServer = new ServerSocket( nPort );
			System.out.println( sockServer.getInetAddress().getHostAddress() );
			System.out.println( "port: " + nPort );
			
			KeyValueStore store = new KeyValueStore();
			
			while( true )
			{
				try{
					Socket sock = sockServer.accept();
					System.out.println( "accepted at: " + sock.getRemoteSocketAddress() );
					Session session = new Session( sock, store );
					session.start();
				}
				catch( IOException e ){
				}
				catch( NullPointerException e ){
				}
			}
		}
		catch( IOException e ){
		}
	}

	public static void	main( String[] args ){
		Thread thread = new Thread(){
			public void run(){
				new KeyValueServer( 12345 );
			}
		};
		thread.start();
		try{
			thread.join();
		}
		catch( InterruptedException e ){}
	}
}