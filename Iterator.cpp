// Iteratorcpp 
#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

template<class COLLECTION, typename T>
class ITRATOR {
private:
	ITRATOR(void);
	ITRATOR(ITRATOR&);
	ITRATOR& operator=(ITRATOR&);

public:
	ITRATOR( COLLECTION<T>* p ){
		this->p = p;
		it = c.begin();
	}
//	 : collection(c), it(c.being()) {}

	void	Begin(void){
		it=p->begin();
	}

	bool	hasNext(void){
		return(it!=p->end());
	}

	void	Next(void){
		it++;
	}

	T		Current(void){
		return(*it);
	}

private:
	COLLECTION<T>&			collection;
	COLLECTION<T>*			p;
	COLLECTION<T>::iterator	it;
};


int main( void )
{
	int	num	= 100;
	vector<int>		ary;
	for( int i=0; i<10; i++ )
		ary.push_back( num++ );	

	ITRATOR<vector, int>	it(ary);
	for( it.Begin(); it.hasNext(); it.Next() ){
		cout << it.Current() << endl;
	}
}

