#ifndef __ACCESSOR_H__
#define __ACCESSOR_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#define		ACCESSOR(TYPE, NAME)					\
	protected:	TYPE	NAME ;						\
	public: inline void	Set##NAME( ##TYPE value ) {	\
		NAME = value ;								\
	}												\
	public: inline TYPE	Get##NAME(void) const {		\
		return NAME ;								\
	}

#define		SETTER(TYPE,NAME)		protected:TYPE	NAME;public:inline void Set##NAME(##TYPE param){NAME=param;}
#define		GETTER(TYPE,NAME)		protected:TYPE	NAME;public:inline void Get##NAME(void)const{return NAME;} 
#define		GETTERSETTER(TYPE,NAME)		ACCESSOR(TYPE,NAME)


#endif//__ACCESSOR_H__
