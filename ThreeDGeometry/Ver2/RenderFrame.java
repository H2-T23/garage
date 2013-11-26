// RenderFrame.java 

import	java.util.*;
import	java.lang.Math;
import	java.awt.*;
import	java.awt.event.*;
import	javax.swing.*;
import	javax.swing.border.*;
import	javax.swing.event.*;

/***********************************************************
 *
 *
 */
public class RenderFrame extends JFrame implements EventHandler.Observer {

	/***********************************************************
	 *
	 *
	 */
	public void		OnUpdate(){
		System.out.println( "RenderFrame::OnUpdate()" );
	}

	/***********************************************************
	 *
	 *
	 */
	class CanvasPanel extends JPanel implements ComponentListener, MouseListener, MouseMotionListener {
		private boolean 	initialized = false;
		private	Image		memImage;
		private	Graphics	memGraphics;

		public int			nWidth	= 0;
		public int			nHeight = 0;

		public Rect			rect = new Rect();

	//	World world = new World();

		Point		ptCur	= new Point(0,0);

		public CanvasPanel(){
			Dimension dim = new Dimension(600, 600);
			setPreferredSize( dim );
			setMaximumSize( dim );
			setMinimumSize( dim );
			
			addComponentListener( this );
			addMouseListener( this );
			addMouseMotionListener( this );
		}

		@Override
		public void		componentResized( ComponentEvent e ){
			Resize();
			System.out.println( "componentResize" );
		}
		
		@Override public void		componentHidden( ComponentEvent e ){}
		@Override public void		componentShown( ComponentEvent e ){}
		@Override public void		componentMoved( ComponentEvent e ){}

		@Override
		public void		paintComponent( Graphics g ){
			super.paintComponent( g );
			if( initialized == false ){
				initialize();
			}

			if( Render( memGraphics ) )
			{
				g.drawImage( memImage, 0, 0, this );
			}

			System.out.println( "paintComponent" );
		}

		@Override public void	mouseEntered( MouseEvent e ){}
		@Override public void	mouseExited( MouseEvent e ){}
		@Override public void	mouseClicked( MouseEvent e ){}
		@Override public void	mousePressed( MouseEvent e ){
			ptCur.x	= e.getX();
			ptCur.y	= e.getY();
			e.consume();
		}
		@Override public void	mouseReleased( MouseEvent e ){}
		@Override public void	mouseMoved( MouseEvent e ){}
		@Override public void	mouseDragged( MouseEvent e ){
			int	x	= e.getX();
			int y	= e.getY();

			System.out.println( String.format( "[%d, %d][%d, %d]\n", ptCur.x, ptCur.y, x, y ) );

		//	world.azimuth		-= x - ptCur.x;
		//	world.elevation		+= y - ptCur.y;
			
		//	Parameter.getInstance().Panel = this;
		//	Parameter.getInstance().vRotation.x = (float)(Math.PI * world.azimuth   / 180.0);
		//	Parameter.getInstance().vRotation.y = (float)(Math.PI * world.elevation / 180.0);
		//	Parameter.getInstance().updateUIfromValue();

			ParameterManager.getInstance().getParameter( ParamID.AZIMUTH ).setValue( (x - ptCur.x) );
			ParameterManager.getInstance().getParameter( ParamID.ELEVATION ).setValue( (x - ptCur.x) );

			ptCur.x	= x;
			ptCur.y = y;

			repaint();
			e.consume();
		}

		protected void	initialize(){
			initialized = true;
			Resize();
			
		//	Parameter.getInstance().InitValue();
		}

		protected boolean	Render( Graphics g ){
			g.setColor( Color.black );
			g.fillRect( 0, 0, nWidth, nHeight );

		//	world.Render( g, rect );

			return true;
		}

		protected void		Resize(){
			nWidth	= getSize().width;
			nHeight	= getSize().height;

			rect.Set( 0, 0, nHeight, nWidth );
			
		//	Parameter.getInstance().Panel = this;
		//	Parameter.getInstance().nHeight	= nHeight;
		//	Parameter.getInstance().nWidth	= nWidth;
		//	Parameter.getInstance().updateUIfromValue();
			
			memImage	= createImage( nWidth, nHeight );
			memGraphics	= memImage.getGraphics();
			
			repaint();
		}
	}

	/***********************************************************
	 *
	 *
	 */
	public RenderFrame( int x, int y, int w, int h ){
		setBounds( x, y, w, h );
		setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

		add( new CanvasPanel(), BorderLayout.CENTER );

		ParameterManager.getInstance().addObserver( this );
	}
}

