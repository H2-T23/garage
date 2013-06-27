// Range.cpp 
#include <iostream>

using namespace std;

namespace RANGE1 {
	
	class Iterator {
	public:
		virtual void		First( void ) = 0;
		virtual void		Next( void ) = 0;
		virtual bool		hasNext( void ) const = 0;
	};
	
	class Aggregate {
	public:
		virtual Iterator*	newIterator( void ) = 0;
	};

	template<typename T>
	class TIterator : public Iterator {
	private:
		T		curr,MIN,MAX,STEP;
		
	public:
		TIterator( T min, T max, T step ) 
		: curr(min), MIN(min), MAX(max), STEP(step){
		} 
		void	First( void ){
			curr=MIN;
		}
		void	Next( void ){
			curr += STEP;
		}
		bool	hasNext( void ) const {
			return(curr<=MAX);
		}
		T		Current( void ) const {
			return(curr);
		}
	};
	
	
	// Built-in Types
	typedef TIterator<int>			IteratorInt;
	typedef TIterator<float>		IteratorFloat;
	typedef	TIterator<double>		IteratorDouble;

	template<typename T>
	class TRange : public Aggregate {
	public:
		T					MIN, MAX;
	//	TIterator<T>*		pIT;
		TIterator<T>		IT;
		
		TRange( T min, T max, T step )
	//		: MIN(min), MAX(max), pIT(new TIterator<T>(min,max,step)) {
			: MIN(min), MAX(max), IT(min,max,step) {
		}
		
		bool		inRange( T v ) const {
			return((MIN<=v)&&(v<=MAX));
		}
		
		bool		outRange( T v ) const {
			return((v<MIN)||(v>MAX));
		}
		
		bool		isUnder( T v ) const {
			return(v<MIN);
		}
		
		bool		isOver( T v ) const {
			return(v>MAX);
		}
		
	public:
		Iterator*		newIterator( void ){
	//		return(pIT);
			return(&IT);
		}
	};
	
	class CRange {
	public:
		typedef enum {
			INT = 1, FLOAT, DOUBLE
		} TYPE ;
	
	private:
		CRange( void );
		
		TYPE				Type;
		IteratorInt*		pItI;
		IteratorFloat*		pItF;
		IteratorDouble*		pItD;
		
		CRange( int min, int max, int step )
		: Type(INT), pItI(new IteratorInt(min,max,step)), pItF(NULL), pItD(NULL) {
		}
		CRange( float min, float max, float step )
		: Type(FLOAT), pItI(NULL), pItF(new IteratorFloat(min,max,step)), pItD(NULL) {
		}
		CRange( double min, double max, double step )
		: Type(DOUBLE), pItI(NULL), pItF(NULL), pItD(new IteratorDouble(min,max,step)) {
		}
	
	public:
		template<typename T>
		bool		InRange( T value ) const {
			
		}
		
	public:
		Iterator*	Iterator( void ) const {
			switch( Type ){
				case INT:return(pItI);
				case FLOAT:return(pItF);
				case DOUBLE:return(pItD);
			}
			return(NULL);
		}

	public:
		template<typename T>
		static CRange*		Create( T min, T max, T step ){
			return(new CRange(min,max,step));
		}
		
	};
	
	
	void	main( void ) 
	{
		CRange*	pRange	= CRange::Create( -5.0, 10.0, .5 );
		
		IteratorInt* pIt = (IteratorInt*)pRange->Iterator();
		
		for( pIt->First(); pIt->hasNext(); pIt->Next() ){
			cout << pIt->Current() << endl;
		}
		
		TRange<int>		intrange(0,10,1);
		pIt = (IteratorInt*)intrange.newIterator();
		
		pIt->First();
		while( pIt->hasNext() ){
			cout << pIt->Current() << endl;
			pIt->Next();
		}
	}
	
}// End of "RANGE1".

void	main( void )
{
	RANGE1::main();
}
