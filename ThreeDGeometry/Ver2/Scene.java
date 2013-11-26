// Scene.java 

import	java.io.*;
import	java.util.*;


/***********************************************************
 *
 *
 */
class ThreeDParameter {
	public Vector3D	vEye		= new Vector3D();
	public Vector3D	vLookAt		= new Vector3D();
	public Vector3D	vUp			= new Vector3D();
	public Vector3D	vScale		= new Vector3D();
	public Vector3D	vRotate		= new Vector3D();
	public Vector3D	vTranslate	= new Vector3D();

	public float		fNear = 0.0f, fFar = 0.0f,  fFov = 0.0f, fAspect = 0.0f;
	public float		fYaw = 0.0f, fRolling = 0.0f, fPitch = 0.0f;

	public int			nWidth = 0, nHeight = 0;
	public float		fDepth = 0.0f;

	public int			nAzimuth = 0, nElevation = 0;



	private KeyValueClient		client = null;

	public void		createKeyValueClient( String strServer, int nPort ){
		try{
			client	= new KeyValueClient( strServer, nPort );
		}
		catch( IOException e ){
			client	= null;
		}
	}

	public void		writeInt( String strKey, int value ){
		System.out.println( "set " + strKey + value );
	}

	public void		writeFloat( String strKey, float value ){
		System.out.println( "set " + strKey + value );
	}

	public void		writeVector( String strKey, Vector3D vec ){
		System.out.println( String.format( "set %s_X %f", strKey, vec.x ) );
		System.out.println( String.format( "set %s_Y %f", strKey, vec.y ) );
		System.out.println( String.format( "set %s_Z %f", strKey, vec.z ) );
	}

	public void		write(){
		writeVector( "EYE"			, vEye			);
		writeVector( "LOOKAT"		, vLookAt		);
		writeVector( "UP"			, vUp			);
		writeVector( "SCALE"		, vScale		);
		writeVector( "ROTATE"		, vRotate		);
		writeVector( "TRANSLATE"	, vTranslate	);

		writeFloat( "NEAR"			, fNear			);
		writeFloat( "FAR"			, fFar			);
		writeFloat( "FOV"			, fFov			);
		writeFloat( "ASPECT"		, fAspect		);

		writeFloat( "YAW"			, fYaw			);
		writeFloat( "ROLLING"		, fRolling		);
		writeFloat( "PITCH"			, fPitch		);

		writeInt( "WIDTH"			, nWidth		);
		writeInt( "HEIGHT"			, nHeight		);
		
		writeFloat( "DEPTH"			, fDepth		);

		writeInt( "AZIMUTH"			, nAzimuth		);
		writeInt( "ELEVATION"		, nElevation	);

	//	writeMatirx4D( ""	, matx );
	}

	public int		readInt( String strKey ){
		System.out.println( "get " + strKey );
		return 0;
	}

	public float	readFloat( String strKey ){
		System.out.println( "get " + strKey );
		return 0;
	}

	public void		readVector( String strKey, Vector3D vec ){
		System.out.println( String.format( "get %s_X", strKey ) );
		System.out.println( String.format( "get %s_Y", strKey ) );
		System.out.println( String.format( "get %s_Z", strKey ) );
	}

	public void		read(){
	}

	public void		get(){
	//	if( client != null )
		{
			for( ParamID id : ParamID.values() )
			{
				System.out.println( "get " + id );

				String strResult = "0";

				switch( id ){
				case EYE_X:
					vEye.x			= Float.valueOf( strResult ).floatValue();
					break;
				case EYE_Y:
					vEye.y			= Float.valueOf( strResult ).floatValue();
					break;
				case EYE_Z:
					vEye.z			= Float.valueOf( strResult ).floatValue();
					break;

				case LOOKAT_X:
					vLookAt.x		= Float.valueOf( strResult ).floatValue();
					break;
				case LOOKAT_Y:
					vLookAt.y		= Float.valueOf( strResult ).floatValue();
					break;
				case LOOKAT_Z:
					vLookAt.z		= Float.valueOf( strResult ).floatValue();
					break;

				case UP_X:
					vUp.x			= Float.valueOf( strResult ).floatValue();
					break;
				case UP_Y:
					vUp.y			= Float.valueOf( strResult ).floatValue();
					break;
				case UP_Z:
					vUp.z			= Float.valueOf( strResult ).floatValue();
					break;

				case SCALE_X:
					vScale.x		= Float.valueOf( strResult ).floatValue();
					break;
				case SCALE_Y:
					vScale.y		= Float.valueOf( strResult ).floatValue();
					break;
				case SCALE_Z:
					vScale.z		= Float.valueOf( strResult ).floatValue();
					break;

				case ROTATE_X:
					vRotate.x		= Float.valueOf( strResult ).floatValue();
					break;
				case ROTATE_Y:
					vRotate.y		= Float.valueOf( strResult ).floatValue();
					break;
				case ROTATE_Z:
					vRotate.z		= Float.valueOf( strResult ).floatValue();
					break;

				case TRANSLATE_X:
					vTranslate.x	= Float.valueOf( strResult ).floatValue();
					break;
				case TRANSLATE_Y:
					vTranslate.y	= Float.valueOf( strResult ).floatValue();
					break;
				case TRANSLATE_Z:
					vTranslate.z	= Float.valueOf( strResult ).floatValue();
					break;

				case NEAR:
					fNear			= Float.valueOf( strResult ).floatValue();
					break;
				case FAR:
					fFar			= Float.valueOf( strResult ).floatValue();
					break;
				case FOV:
					fFov			= Float.valueOf( strResult ).floatValue();
					break;
				case ASPECT:
					fAspect			= Float.valueOf( strResult ).floatValue();
					break;

				case WIDTH:
					nWidth			= Integer.valueOf( strResult ).intValue();
					break;
				case HEIGHT:
					nHeight			= Integer.valueOf( strResult ).intValue();
					break;
				case DEPTH:
					fDepth			= Float.valueOf( strResult ).floatValue();
					break;

				case YAW:
					fYaw			= Float.valueOf( strResult ).floatValue();
					break;
				case ROLLING:
					fRolling		= Float.valueOf( strResult ).floatValue();
					break;
				case PITCH:
					fPitch			= Float.valueOf( strResult ).floatValue();
					break;

				case AZIMUTH:
					nAzimuth		= Integer.valueOf( strResult ).intValue();
					break;
				case ELEVATION:
					nElevation		= Integer.valueOf( strResult ).intValue();
					break;
				}
			}
		}
	}

	public void		set(){
	//	if( client != null )
		{
			for( ParamID id : ParamID.values() )
			{
				String strValue = null;

				switch( id ){
				case EYE_X:
					strValue		= Float.toString( vEye.x );
					break;
				case EYE_Y:
					strValue		= Float.toString( vEye.y );
					break;
				case EYE_Z:
					strValue		= Float.toString( vEye.z );
					break;

				case LOOKAT_X:
					strValue		= Float.toString( vLookAt.x );
					break;
				case LOOKAT_Y:
					strValue		= Float.toString( vLookAt.y );
					break;
				case LOOKAT_Z:
					strValue		= Float.toString( vLookAt.z );
					break;

				case UP_X:
					strValue		= Float.toString( vUp.x );
					break;
				case UP_Y:
					strValue		= Float.toString( vUp.y );
					break;
				case UP_Z:
					strValue		= Float.toString( vUp.z );
					break;

				case SCALE_X:
					strValue		= Float.toString( vScale.x );
					break;
				case SCALE_Y:
					strValue		= Float.toString( vScale.y );
					break;
				case SCALE_Z:
					strValue		= Float.toString( vScale.z );
					break;

				case ROTATE_X:
					strValue		= Float.toString( vRotate.x );
					break;
				case ROTATE_Y:
					strValue		= Float.toString( vRotate.y );
					break;
				case ROTATE_Z:
					strValue		= Float.toString( vRotate.z );
					break;

				case TRANSLATE_X:
					strValue		= Float.toString( vTranslate.x );
					break;
				case TRANSLATE_Y:
					strValue		= Float.toString( vTranslate.y );
					break;
				case TRANSLATE_Z:
					strValue		= Float.toString( vTranslate.z );
					break;

				case NEAR:
					strValue		= Float.toString( fNear );
					break;
				case FAR:
					strValue		= Float.toString( fFar );
					break;
				case FOV:
					strValue		= Float.toString( fFov );
					break;
				case ASPECT:
					strValue		= Float.toString( fAspect );
					break;

				case WIDTH:
					strValue		= Integer.toString( nWidth );
					break;
				case HEIGHT:
					strValue		= Integer.toString( nHeight );
					break;
				case DEPTH:
					strValue		= Float.toString( fDepth );
					break;

				case YAW:
					strValue		= Float.toString( fYaw );
					break;
				case ROLLING:
					strValue		= Float.toString( fRolling );
					break;
				case PITCH:
					strValue		= Float.toString( fPitch );
					break;

				case AZIMUTH:
					strValue		= Integer.toString( nAzimuth );
					break;
				case ELEVATION:
					strValue		= Integer.toString( nElevation );
					break;
				}


				System.out.println( "set " + id +  " " + strValue );

			}
		}
	}
}


/***********************************************************
 *
 *
 */
public class Scene extends ThreeDParameter {


	public Scene() {
		createKeyValueClient( "localhost", 12345 );
		write();
	}

	public static void	main( String[] args ){
		new Scene();
	}
}