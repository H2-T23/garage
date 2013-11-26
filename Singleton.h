#ifndef __SINGLETON_H__
#define __SINGLETON_H__
//------------------------------------------------------------------
/**
 *
 */
template<class T>
class TSingleton
{
public:
	static T&	Instance()
	{
		static T _ins ;
		return _ins ;
	}
} ;
#endif//__SINGLETON_H__