// ResultFrame.java 

import	java.io.IOException;
import	java.util.List;
import	java.util.ArrayList;
import	java.awt.*;
import	javax.swing.*;
import	java.awt.event.ActionListener;
import	java.awt.event.ActionEvent;

public class ResultFrame extends JFrame implements ActionListener {

	class LabelText extends JPanel {
		final JLabel		label	= new JLabel();
		final JTextField	text	= new JTextField();

		public LabelText(){
			initComponets();		
		}
		
		public LabelText( String str ){
			label.setText( str );
			initComponets();
		}

		public String	getLabelText(){
			return label.getText();
		}

		public String	getText(){
			return text.getText();
		}

		public void		setText( String str ){
			text.setText( str );
		}

		private void	initComponets(){
			setLayout( new BoxLayout( this, BoxLayout.X_AXIS ) );
			add( label );
			add( text );
		}
	}

	class MatrixPanel extends JPanel {
		final int		MAX_X = 4;
		final int		MAX_Y = 4;

		final LabelText[][]	texts = new LabelText[ MAX_Y ][ MAX_X ];

		String		strTitle;

		public MatrixPanel( String strTitle, int width, int height ){
			this.strTitle = strTitle;

			setBackground( Color.MAGENTA );
			setPreferredSize( new Dimension(width, height) );
			setBounds( 0, 0, width, height );
			setBorder( new javax.swing.border.TitledBorder( strTitle ) );
			setLayout( new GridLayout( MAX_Y, MAX_X ) );
		
			for( int y=0; y<MAX_Y; y++ ){
				for( int x=0; x<MAX_X; x++ ){
					texts[ y ][ x ]		= new LabelText( String.format("%dx%d", y, x) );
					add( texts[ y ][ x ] );
				}
			}
		}

		public ArrayList<LabelText>		getLabelTextComponents() {
			ArrayList<LabelText> list = new ArrayList<LabelText>();

			for( int y=0; y<MAX_Y; y++ ){
				for( int x=0; x<MAX_X; x++ ){
					list.add( texts[ y ][ x ] );
				}
			}

			return list;
		}
	}

	final MatrixPanel[]		mtxPanel = new MatrixPanel[ 4 ];

	public ResultFrame(){
		setSize(800,200);
		setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

		setLayout( new GridLayout( 2, 2 ) );

		mtxPanel[ 0 ]	= new MatrixPanel("MatirxView"		, 320, 80);
		mtxPanel[ 1 ]	= new MatrixPanel("MatirxProj"		, 320, 80);
		mtxPanel[ 2 ]	= new MatrixPanel("MatirxViewport"	, 320, 80);
		mtxPanel[ 3 ]	= new MatrixPanel("MatirxMaster"	, 320, 80);

		getContentPane().add( mtxPanel[ 0 ] );
		getContentPane().add( mtxPanel[ 1 ] );
		getContentPane().add( mtxPanel[ 2 ] );
		getContentPane().add( mtxPanel[ 3 ] );


		createKeyValueClient( "localhost", 12345 );


		new javax.swing.Timer( 1000, this ).start();
	}

	private KeyValueClient	client = null;

	private KeyValueClient	createKeyValueClient( String strServer, int nPort ){
		try{
			if( client == null )
				client	= new KeyValueClient( strServer, nPort );
			return client;
		}
		catch( IOException e ){
			return null;
		}
		catch( Exception e ){
			return null;
		}
	}

	@Override
	public void		actionPerformed( ActionEvent event ){
		System.out.println( "ResultFrame::OnTimer()" );

	//	try{
		//	KeyValueClient	client = new KeyValueClient( "localhost" , 12345 );

			for( int i=0; i<mtxPanel.length; i++ )
			{
				ArrayList<LabelText> list = mtxPanel[ i ].getLabelTextComponents();

				for( int j=0; j<list.size(); j++ )
				{
					String	strKey			= String.format( "%s%s", mtxPanel[ i ].strTitle, list.get( j ).getLabelText() );
					String	strKeyValueReq	= String.format( "get %s", strKey );

					String	strRet			= null;

					if( client != null ){
						strRet	= client.execute( "get", strKey );
					//	if( (strRet != null) && (strRet.length() != 0) )
						if( !strRet.equals("null") )
						{
							list.get( j ).setText( strRet );
						}
					}

					String	strTrace		= String.format( "%s = execute( get %s )", strRet, strKey );
					System.out.println( strTrace );
				}

				System.out.println();
			}
	//	}
	//	catch( IOException e ){
	//	//	e.printStackTrace();
	//	}
	}

	public static void	main( String[] args ){
		SwingUtilities.invokeLater( new Runnable() {
			public void	run(){
				new ResultFrame().setVisible( true );
			}
		});
	}
}