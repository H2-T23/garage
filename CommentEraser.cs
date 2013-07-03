// CommentEraser.cs 
using  System;
using	System.IO;


/*
	// ?????
	hello world C#.
*/


//********************//

public class CommentEraser {
	public static void	Main( string[] args ){

		int	argc = args.Length;
		for( int i=0; i<argc; i++ ){
			Console.Write( "No.{0} args:{1}\n", i, args[ i ] );
		}

		if( argc <= 0 )
			return;

		bool		comment_c			= false;
		bool		comment_cplusplus	= false;


		int	lineNo = 1;
		string	str;

		StreamReader r = new StreamReader( args[ 0 ] );
		while( (str = r.ReadLine()) != null )
		{

			int i = 0;
			while( i < str.Length )
			{
				if( comment_c )
				{
					if( str[ i ] == '*' )
					{
						if( str[ i + 1 ] == '/' )
						{
							comment_c		= false;
							i += 2;
							continue;
						}
					}
				}
				else
				{
					if( str[ i ] == '/' )
					{
						if( str[ i + 1 ] == '/' )
						{
							comment_cplusplus	= true;
							break;
						}
						else
						if( !comment_cplusplus && str[ i + 1 ] == '*' )
						{
							comment_c			= true;
							i += 2;
							continue;
						}
					}
				}

				if( (comment_c == false) && (comment_cplusplus == false) )
				{
					Console.Write( "{0}", (char)str[ i ] );
				}
				
				i += 1;
			}
			// end of while.


		//	if( (comment_c == false) && (comment_cplusplus == false) )
				Console.Write( "\n" );

			comment_cplusplus	= false;

			lineNo++;
		}

		r.Close();
	}
}
