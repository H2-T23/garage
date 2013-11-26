// ParameterFrame.java 

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
public class ParameterFrame extends JFrame implements EventHandler.Observer {

	/***********************************************************
	 *
	 */
	public void		OnUpdate(){
		System.out.println( "ParmeterFrame::OnUpdate()" );
	}

	/***********************************************************
	 *
	 *
	 */
	class VectorPanel extends JPanel {
		public SpinCtrl	spinX	= null;
		public SpinCtrl	spinY	= null;
		public SpinCtrl	spinZ	= null;

		public VectorPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );
		}

		public VectorPanel( String strTitle, ParamID x, ParamID y, ParamID z ){
			this( strTitle );
			this.setSpinCtrl( x, y, z );
		}

		public void		setSpinCtrl( ParamID x, ParamID y, ParamID z ){
			this.setSpinCtrl(	ParameterManager.getInstance().getParameter( x ).spin
							,	ParameterManager.getInstance().getParameter( y ).spin
							,	ParameterManager.getInstance().getParameter( z ).spin	);
		}

		public void		setSpinCtrl( SpinCtrl spinX, SpinCtrl spinY, SpinCtrl spinZ ){
			this.spinX	= spinX;
			this.spinY	= spinY;
			this.spinZ	= spinZ;

			setLayout( new BoxLayout(this, BoxLayout.PAGE_AXIS) );
			add( spinX );
			add( spinY );
			add( spinZ );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class CameraPanel extends JPanel {
		public CameraPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( new VectorPanel( "Eye"			, ParamID.EYE_X		, ParamID.EYE_Y		, ParamID.EYE_Z		 ) );
			add( new VectorPanel( "LookAt"		, ParamID.LOOKAT_X	, ParamID.LOOKAT_Y	, ParamID.LOOKAT_Z	 ) );
			add( new VectorPanel( "Up"			, ParamID.UP_X		, ParamID.UP_Y		, ParamID.UP_Z		 ) );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class ModelPanel extends JPanel {
		public ModelPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( new VectorPanel( "Scale"		, ParamID.SCALE_X		, ParamID.SCALE_Y		, ParamID.SCALE_Z		 ) );
			add( new VectorPanel( "Rotate"		, ParamID.ROTATE_X		, ParamID.ROTATE_Y		, ParamID.ROTATE_Z		 ) );
			add( new VectorPanel( "Translate"	, ParamID.TRANSLATE_X	, ParamID.TRANSLATE_Y	, ParamID.TRANSLATE_Z	 ) );			
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class ProjectionPanel extends JPanel {
		public ProjectionPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( ParameterManager.getInstance().getParameter( ParamID.NEAR ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.FAR ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.FOV ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.ASPECT ).spin );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class ViewportPanel extends JPanel {
		public ViewportPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( ParameterManager.getInstance().getParameter( ParamID.WIDTH ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.HEIGHT ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.DEPTH ).spin );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class PitchingPanel extends JPanel {
		public PitchingPanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( ParameterManager.getInstance().getParameter( ParamID.YAW ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.ROLLING ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.PITCH ).spin );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	class PlanePanel extends JPanel {
		public PlanePanel( String strTitle ){
			setBorder( new TitledBorder( strTitle ) );

			setAlignmentX( JComponent.LEFT_ALIGNMENT );
			setAlignmentY( JComponent.TOP_ALIGNMENT );

			setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );
			add( ParameterManager.getInstance().getParameter( ParamID.AZIMUTH ).spin );
			add( ParameterManager.getInstance().getParameter( ParamID.ELEVATION ).spin );
		}
	}

	/***********************************************************
	 *
	 *
	 */
	public ParameterFrame( int x, int y, int w, int h ){
		setBounds( x, y, w, h );
		setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

		JPanel panel = new JPanel();
		panel.setLayout( new BoxLayout( panel, BoxLayout.PAGE_AXIS ) );
		panel.add( new CameraPanel		( "Camera"		) );
		panel.add( new ModelPanel		( "Model"		) );
		panel.add( new ProjectionPanel	( "Projection"	) );
		panel.add( new ViewportPanel	( "Viewport"	) );
		panel.add( new PitchingPanel	( "Pitching"	) );
		panel.add( new PlanePanel		( "Plane"		) );

	//	add( panel, BorderLayout.CENTER );
		add( panel, BorderLayout.WEST );

		new javax.swing.Timer( 1000, new ActionListener() {
			public void		actionPerformed( ActionEvent e ){
			//	Parameter a  = ParameterManager.getInstance().getParameter( ParamID.AZIMUTH );
			//	int	value = (int)Integer.valueOf( a.getValue().toString() );
			//	value++;
			//	a.setValue( value );
			}
		}).start();

		ParameterManager.getInstance().addObserver( this );
	}

	/***********************************************************
	 *
	 *
	 */
	public static void	main( String[] args ){
		new ParameterFrame( 100, 100, 500, 500 ).setVisible( true );
	}
}