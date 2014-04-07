#pragma once


namespace GENERIC 
{
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<typename TYPE>
	class TNonCopyable {
	protected:
		TNonCopyable( void ){}
		~TNonCopyable( void ){}
	private:
		TNonCopyable(const TNonCopyable&);
		TNonCopyable(const TYPE&);
		TNonCopyable&	operator=(const TNonCopyable&);
		TYPE&			operator=(const TYPE&);
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<typename TYPE>
	class TSingleton : public TNonCopyable<TYPE> {
	public:
		static TYPE&	Instance( void ){
			static TYPE ins;
			return(ins);
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<typename TYPE, TYPE DEFAULT = 0>
	class TValueObject {
	private:
		TYPE	m_value;

	public:
		typedef TValueObject<TYPE,DEFAULT>	SUPER;

		TValueObject( void ) : m_value(DEFAULT) {
		}
		TValueObject( const TValueObject& obj ) : m_value(DEFAULT) {
			SetValue( obj );
		}
		explicit TValueObject( const TYPE& value ) : m_value(DEFAULT) {
			SetValue( value );
		}

		TYPE	GetValue( void ) const {
			return(m_value);
		}
		void	SetValue( const TYPE& value ) {
			m_value = value;
		}
		void	SetValue( const TValueObject& obj ) {
			if( this != &obj )
				m_value = obj.m_value;
		}

		TValueObject&	operator=( const TYPE& value ){
			SetValue( value );
			return (*this);
		}
		TValueObject&	operator=( const TValueObject& obj ){
			SetValue( obj );
			return (*this);
		}

		bool	operator==( const TValueObject& obj ) const {
			return(m_value == obj.m_value);
		}
		bool	operator!=( const TValueObject& obj ) const {
			return !(*this == obj);
		}

		operator TYPE(){ return(m_value); }
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<class CLASS, typename TYPE, TYPE DEFAULT>
	class TBasicDataType : public TNonCopyable<TBasicDataType<CLASS,TYPE,DEFAULT> > {
	private:
		TYPE	m_value;

	public:
		typedef TBasicDataType<CLASS,TYPE,DEFAULT>	SUPER;

		TBasicDataType( void ) : m_value(DEFAULT) {
		}
		explicit TBasicDataType( const CLASS& obj ) : m_value(DEFAULT) {
			SetValue( obj );
		}
		explicit TBasicDataType( const TYPE& value ) : m_value(DEFAULT) {
			SetValue( value );
		}

		TYPE	GetValue( void ) const {
			return(m_value);
		}
		void	SetValue( const TYPE& value ) {
			m_value = value;
		}
		void	SetValue( const CLASS& obj ) {
			if( this != &obj )
				m_value = obj.m_value;
		}

		CLASS&	operator=( const TYPE& value ){
			SetValue( value );
			return (CLASS)(*this);
		}
		CLASS&	operator=( const CLASS& obj ){
			SetValue( obj );
			return (CLASS)(*this);
		}

		bool	operator==( const CLASS& obj ) const {
			return(m_value == obj.m_value);
		}
		bool	operator!=( const CLASS& obj ) const {
			return !(*this == obj);
		}

		operator TYPE(){ return(m_value); }
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<class CLASS, typename TYPE, TYPE DEFAULT>
	class THandle : public TNonCopyable<THandle<CLASS,TYPE,DEFAULT> > {
	protected:
	public:
		TYPE		m_hHandle;

		typedef THandle<CLASS,TYPE,DEFAULT>	SUPER;

		THandle( void ) : m_hHandle(DEFAULT) {
		}
		explicit THandle( const CLASS& obj ) : m_hHandle(DEFAULT) {
			SetHandle( obj );
		}
		explicit THandle( const TYPE& handle ) : m_hHandle(DEFAULT) {
			SetHandle( handle );
		}

		TYPE	Attach( const TYPE& handle ){
			TYPE	prev = m_hHandle;
			m_hHandle	= handle;
			return prev;
		}

		TYPE	Detach( void ){
			TYPE	prev = m_hHandle;
			m_hHandle	= NULL;
			return prev;
		}

		TYPE	GetHandle( void ) const {
			return(m_hHandle);
		}
		void	SetHandle( const TYPE& handle ) {
			m_hHandle = handle;
		}
		void	SetHandle( const CLASS& obj ) {
			if( this != &obj )
				m_hHandle = obj.m_hHandle;
		}

		CLASS&	operator=( const TYPE& handle ){
			SetHandle( handle );
			return (CLASS)(*this);
		}
		CLASS&	operator=( const CLASS& obj ){
			SetHandle( obj );
			return (CLASS)(*this);
		}

		bool	operator==( const CLASS& obj ) const {
			return(m_hHandle == obj.m_hHandle);
		}
		bool	operator!=( const CLASS& obj ) const {
			return !(*this == obj);
		}

		operator TYPE(){ return(m_hHandle); }
		operator HANDLE() { return (HANDLE)m_hHandle; }
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<typename TYPE>
	class TBindData : public GENERIC::TNonCopyable<TBindData<TYPE> > {
	private:
		const TYPE	&A, &B;

	public:
		TBindData( TYPE& a, TYPE& b ) : A(a), B(b) {
		}

		bool	Comp( void ){
			return(A==B);		
		}
	};
}
