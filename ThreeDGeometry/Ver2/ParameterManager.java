// ParameterManager.java 

import	java.util.*;
import	javax.swing.*;
import	javax.swing.event.*;

/***********************************************************
 *
 *
 */
enum ParamID {
	EYE_X, EYE_Y, EYE_Z,

	LOOKAT_X, LOOKAT_Y, LOOKAT_Z,

	UP_X, UP_Y, UP_Z,

	SCALE_X, SCALE_Y, SCALE_Z,

	ROTATE_X, ROTATE_Y, ROTATE_Z,

	TRANSLATE_X, TRANSLATE_Y, TRANSLATE_Z, 

	NEAR, FAR, FOV, ASPECT,

	WIDTH, HEIGHT, DEPTH,

	YAW, ROLLING, PITCH,

	AZIMUTH, ELEVATION,
}

/***********************************************************
 *
 *
 */
class Parameter {
	public ParamID		id;
	public SpinCtrl		spin = null;
	
	public Parameter( ParamID id ){
		this.id = id;
	}

	public Parameter( ParamID id, String strLabel, int value, int min, int max, int step, String strEditor, ChangeListener listener ){
		this( id );
		spin = new SpinCtrl( strLabel, new SpinnerNumberModel( value, min, max, step ), strEditor , listener );
	}

	public Parameter( ParamID id, String strLabel, float value, float min, float max, float step, String strEditor, ChangeListener listener ){
		this( id );
		spin = new SpinCtrl( strLabel, new SpinnerNumberModel( value, min, max, step ), strEditor , listener );
	}

	public Object	getValue(){
		return spin.spin.getValue();
	}

	public void		setValue( Object value ){
		spin.spin.setValue( value );
	}
}

/***********************************************************
 *
 *
 */
public class ParameterManager extends EventHandler.Subject implements ChangeListener {
	private static ParameterManager	ins = new ParameterManager();
	public static ParameterManager	getInstance(){ return ins; }

	private ParameterManager(){
		map.put( ParamID.EYE_X			, new Parameter(ParamID.EYE_X, "X", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.EYE_Y			, new Parameter(ParamID.EYE_Y, "Y", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.EYE_Z			, new Parameter(ParamID.EYE_Z, "Z", 100, 0, 999, 1, "###0", this) );

		map.put( ParamID.LOOKAT_X		, new Parameter(ParamID.LOOKAT_X, "X", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.LOOKAT_Y		, new Parameter(ParamID.LOOKAT_Y, "Y", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.LOOKAT_Z		, new Parameter(ParamID.LOOKAT_Z, "Z", 100, 0, 999, 1, "###0", this) );

		map.put( ParamID.UP_X			, new Parameter(ParamID.UP_X, "X", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.UP_Y			, new Parameter(ParamID.UP_Y, "Y", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.UP_Z			, new Parameter(ParamID.UP_Z, "Z", 100, 0, 999, 1, "###0", this) );

		map.put( ParamID.SCALE_X		, new Parameter(ParamID.SCALE_X, "X", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.SCALE_Y		, new Parameter(ParamID.SCALE_Y, "Y", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.SCALE_Z		, new Parameter(ParamID.SCALE_Z, "Z", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.ROTATE_X		, new Parameter(ParamID.ROTATE_X, "X", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.ROTATE_Y		, new Parameter(ParamID.ROTATE_Y, "Y", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.ROTATE_Z		, new Parameter(ParamID.ROTATE_Z, "Z", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.TRANSLATE_X	, new Parameter(ParamID.TRANSLATE_X, "X", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.TRANSLATE_Y	, new Parameter(ParamID.TRANSLATE_Y, "Y", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.TRANSLATE_Z	, new Parameter(ParamID.TRANSLATE_Z, "Z", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.NEAR			, new Parameter(ParamID.NEAR, "Near", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.FAR			, new Parameter(ParamID.FAR, "Far", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.FOV			, new Parameter(ParamID.FOV, "Fov", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.ASPECT			, new Parameter(ParamID.ASPECT, "Aspect", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.WIDTH			, new Parameter(ParamID.WIDTH, "Width", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.HEIGHT			, new Parameter(ParamID.HEIGHT, "Height", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.DEPTH			, new Parameter(ParamID.DEPTH, "Depth", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.YAW			, new Parameter(ParamID.YAW, "Yaw", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.ROLLING		, new Parameter(ParamID.ROLLING, "Rolling", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );
		map.put( ParamID.PITCH			, new Parameter(ParamID.PITCH, "Pitch", 1.0f, 0.0f, 10.0f, 0.25f, "0.00", this) );

		map.put( ParamID.AZIMUTH		, new Parameter(ParamID.AZIMUTH, "Azimuth", 100, 0, 999, 1, "###0", this) );
		map.put( ParamID.ELEVATION		, new Parameter(ParamID.ELEVATION, "Elevation", 100, 0, 999, 1, "###0", this) );
	}

	private Map<ParamID, Parameter>	map = new HashMap<ParamID, Parameter>();

	public synchronized Collection		getParameterAll(){
		return map.values();
	}
		
	public synchronized Parameter		getParameter( ParamID id ){
		Parameter param = map.get( id );
		if( param == null ){
			param	= new Parameter( id );
			map.put( id, param );
		}
		return param;
	}

	public void		stateChanged( ChangeEvent change ){
		for( Map.Entry<ParamID, Parameter> item : map.entrySet() ){

			ParamID id = item.getKey();
			Parameter param = item.getValue();

			System.out.println( "set " + id.toString() + " " + param.getValue().toString() );
		}

		notifyObserver();
	}
}