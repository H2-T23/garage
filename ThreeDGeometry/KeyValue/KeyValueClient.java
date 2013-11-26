// KeyValueClient.java 

//package keyvalue;

import	java.io.*;
import	java.net.*;
import	java.util.*;

public class KeyValueClient {
	private Socket				sock = null;
	private BufferedReader		in	= null;
	private PrintWriter			out	= null;

	private String				strServer	= null;
	private int					nPort		= 0;


	public KeyValueClient( String strServer, int nPort ) throws IOException {
		this.strServer = strServer;
		this.nPort = nPort;
		newConnection( strServer, nPort );
	}

	protected void	newConnection( String strServer, int nPort ) throws IOException {
		System.out.println( "new Socket" );
		sock = new Socket( strServer, nPort );
		System.out.println( "new Socket success" );

		in	= new BufferedReader( new InputStreamReader( sock.getInputStream() ) );
		out	= new PrintWriter( sock.getOutputStream(), true );
	}

	public String		execute( String strCmd, String strKey, String strValue ){
		try{
			out.printf( "%s %s %s\n", strCmd, strKey, strValue );
			return in.readLine();
		}
		catch( IOException e ){
			e.printStackTrace();
			return null;
		}
	}

	public String		execute( String strCmd, String strKey ){
		try{
			out.printf( "%s %s\n", strCmd, strKey );
			return in.readLine();
		}catch( IOException e ){
			e.printStackTrace();
			return null;
		}
	}

	public void			Console(){
		try{
			BufferedReader cin = new BufferedReader( new InputStreamReader( System.in ) );

			System.out.print( "client> " );

			String	str = null;
			while( (str = cin.readLine()).length() > 0 )
			{
				System.out.printf( "(echo : %s)\n", str );

				String[] params = splitCmdLine( str );

				String strResult = null;
				if( params.length == 2 ){
					strResult	= execute( params[ 0 ], params[ 1 ] );
					System.out.printf( "%s = execute(%s, %s)\n", strResult, params[ 0 ], params[ 1 ] );
				}else
				if( params.length == 3 ){
					strResult	= execute( params[ 0 ], params[ 1 ], params[ 2 ] );
					System.out.printf( "%s = execute(%s, %s)\n", strResult, params[ 0 ], params[ 1 ], params[ 2 ] );
				}

				if( strResult == null ){
					System.out.println( "Usage: cmd key [value]" );
				}

				System.out.print( "\nclient> " );
			}
		}
		catch( IOException e ){
			e.printStackTrace();
		}
	}

	public String[]		splitCmdLine( String strCmdLine ){
		List<String> listString = new ArrayList<String>();

		StringTokenizer st = new StringTokenizer( strCmdLine );
		while( st.hasMoreTokens() )
		{
			listString.add( st.nextToken() );
		}

		String[] ret = listString.toArray( new String[ listString.size() ] );
		return ret;
	}
	
	public static void	main( String[] args ){
		try{
			KeyValueClient client = new KeyValueClient( "localhost", 12345 );
			client.Console();	
		}
		catch( IOException e ){
			e.printStackTrace();
		}
	}
}
