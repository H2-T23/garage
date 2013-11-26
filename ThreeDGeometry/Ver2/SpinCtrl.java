// SpinCtrl.java 

import	javax.swing.*;
import	javax.swing.event.*;
import	java.awt.Dimension;

/***********************************************************
 *
 *
 */
public class SpinCtrl extends JPanel {
	final public JLabel			label	= new JLabel();
	final public JSpinner		spin	= new JSpinner();
	private SpinnerNumberModel	model	= null;

	public SpinCtrl( String str, SpinnerNumberModel model ){
		this.init( str, model );
	}

	public SpinCtrl( String str, SpinnerNumberModel model, String strEditor, ChangeListener listener ){
		this( str, model );
		this.setEditor( strEditor );
		this.addChangeListener( listener );
	}

	public void		init( String str, SpinnerNumberModel model ){
		setLayout( new BoxLayout(this, BoxLayout.LINE_AXIS) );			
		add( label );
		add( spin );

		this.setSpinSize( 100, 20 );

		label.setText( str );
		spin.setModel( model );
		this.model = model;
	}

	public void		setEditor( String str ){
		this.spin.setEditor( new JSpinner.NumberEditor( this.spin, str ) );
	}

	public void		addChangeListener( ChangeListener listener ){
		this.spin.addChangeListener( listener );
	}

	public void		setSpinSize( int w, int h ){
		if( (w + h) == 0 )
		{
			spin.setPreferredSize( null );
			spin.setMaximumSize( null );
			spin.setMinimumSize( null );
		}
		else
		{
			Dimension sz = new Dimension( w, h );
			spin.setPreferredSize( sz );
			spin.setMaximumSize( sz );
			spin.setMinimumSize( sz );
		}
	}
}