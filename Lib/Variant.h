// Variant.h 
#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdarg.h>

namespace VARIANT {

using namespace std;

/****************************************************************************************
 *
 *
 *
 *
 *
 */
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned int		DWORD;

/****************************************************************************************
 *
 *
 *
 *
 *
 */
typedef enum tagEVariantTyep {
	VAR_INVALID		= 0	,
	VAR_INT				,
	VAR_BOOL			,
	VAR_DOUBLE			,
	VAR_FLOAT			,
	VAR_BYTE			,
	VAR_WORD			,
	VAR_DWORD			,
	VAR_STRING
} EVariantType;

/****************************************************************************************
 *
 *
 *
 *
 *
 */
union UVariant {
	int			nVar;
	bool		bVar;
	double		dVar;
	float		fVar;
	BYTE		byVar;
	WORD		wVar;
	DWORD		dwVar;

	UVariant(int	var=0)	: nVar(var)	{}
	UVariant(bool	var)	: bVar(var)	{}	
	UVariant(double	var)	: dVar(var)	{}	
	UVariant(float	var)	: fVar(var)	{}	
	UVariant(BYTE	var)	: byVar(var){}	
	UVariant(WORD	var)	: wVar(var)	{}	
	UVariant(DWORD	var)	: dwVar(var){}

	int		operator=(int		var)	{ return(nVar=var); }
	bool	operator=(bool		var)	{ return(bVar=var); }
	double	operator=(double	var)	{ return(dVar=var); }
	float	operator=(float		var)	{ return(fVar=var); }
	BYTE	operator=(BYTE		var)	{ return(byVar=var); }
	WORD	operator=(WORD		var)	{ return(wVar=var); }
	DWORD	operator=(DWORD		var)	{ return(dwVar=var); }

	operator int()		{ return(nVar); }
	operator bool()		{ return(bVar); }
	operator double()	{ return(dVar); }
	operator float()	{ return(fVar); }
	operator BYTE()		{ return(byVar); }
	operator WORD()		{ return(wVar); }
	operator DWORD()	{ return(dwVar); }
} ;

/****************************************************************************************
 *
 *
 *
 *
 *
 */
class CVariant {
public:
	EVariantType	Type;
	UVariant		Var;
	string			strVar;

public:
	CVariant()				: Type(VAR_INVALID)	, Var(0) {}
	CVariant(int	var)	: Type(VAR_INT)		, Var(var) {}
	CVariant(bool	var)	: Type(VAR_BOOL)	, Var(var) {}
	CVariant(double	var)	: Type(VAR_DOUBLE)	, Var(var) {}
	CVariant(float	var)	: Type(VAR_FLOAT)	, Var(var) {}
	CVariant(BYTE	var)	: Type(VAR_BYTE)	, Var(var) {}
	CVariant(WORD	var)	: Type(VAR_WORD)	, Var(var) {}
	CVariant(DWORD	var)	: Type(VAR_DWORD)	, Var(var) {}

	CVariant(      char*	var)	: Type(VAR_STRING)	, strVar(var) {}
	CVariant(const char*	var)	: Type(VAR_STRING)	, strVar(var) {}

	CVariant(      string&	var)	: Type(VAR_STRING)	, strVar(var) {}
	CVariant(const string&	var)	: Type(VAR_STRING)	, strVar(var) {}

	int		operator=(int		var)	{ Type=VAR_INT;		return(Var=var); }
	bool	operator=(bool		var)	{ Type=VAR_BOOL;	return(Var=var); }
	double	operator=(double	var)	{ Type=VAR_DOUBLE;	return(Var=var); }
	float	operator=(float		var)	{ Type=VAR_FLOAT;	return(Var=var); }
	BYTE	operator=(BYTE		var)	{ Type=VAR_BYTE;	return(Var=var); }
	WORD	operator=(WORD		var)	{ Type=VAR_WORD;	return(Var=var); }
	DWORD	operator=(DWORD		var)	{ Type=VAR_DWORD;	return(Var=var); }

	const char*	operator=(const char* var) { Type=VAR_STRING; strVar=var; return(strVar.c_str()); }
	const char*	operator=(      char* var) { Type=VAR_STRING; strVar=var; return(strVar.c_str()); }

	const string&	operator=(const string& var) { Type=VAR_STRING; strVar=var; return(strVar); }
	const string&	operator=(      string& var) { Type=VAR_STRING; strVar=var; return(strVar); }

	operator int()		{ return(Var); }
	operator bool()		{ return(Var); }
	operator double()	{ return(Var); }
	operator float()	{ return(Var); }
	operator BYTE()		{ return(Var); }
	operator WORD()		{ return(Var); }
	operator DWORD()	{ return(Var); }

	operator const char*() { return(strVar.c_str()); }
	operator string() { return(strVar); }

	bool			IsValid(void) const { return(Type==VAR_INVALID?false:true); }

	EVariantType	TYPE(void) const { return(Type); }

	string			toString(void){
		switch(Type){
		case VAR_INT:		return Format(GetFmt(),(int)Var);
		case VAR_BOOL:		return Format("%s",Var.bVar==false?"false":"true");
		case VAR_DOUBLE:	return Format(GetFmt(),(double)Var);
		case VAR_FLOAT:		return Format(GetFmt(),(float)Var);
		case VAR_BYTE:		return Format(GetFmt(),(BYTE)Var);
		case VAR_WORD:		return Format(GetFmt(),(WORD)Var);
		case VAR_DWORD:		return Format(GetFmt(),(DWORD)Var);
		case VAR_STRING:	return Format(GetFmt(),strVar.c_str());
		default:			return "invalid.";
		}
	}

	const char*		TypeString(void) const {
		switch(Type){
		case VAR_INT:		return "VAR_INT";
		case VAR_BOOL:		return "VAR_BOOL";
		case VAR_DOUBLE:	return "VAR_DOUBLE";
		case VAR_FLOAT:		return "VAR_FLOAT";
		case VAR_BYTE:		return "VAR_BYTE";
		case VAR_WORD:		return "VAR_WORD";
		case VAR_DWORD:		return "VAR_DWORD";
		case VAR_STRING:	return "VAR_STRING";
		default:			return "VAR_INVALID";
		}
	}

	const char*		GetFmt(void) const {
		switch(Type){
		case VAR_INT:		return "%d";
		case VAR_BOOL:		return "%s";
		case VAR_DOUBLE:	return "%f";
		case VAR_FLOAT:		return "%f";
		case VAR_BYTE:		return "0x%01x";
		case VAR_WORD:		return "0x%02x";
		case VAR_DWORD:		return "0x%04x";
		case VAR_STRING:	return "%s";
		default:			return "%s";
		}
	}

	const char*		Format(const char* fmt, ...){
		static char	buf[1024];
		va_list	ap;
		va_start(ap,fmt);
		vsprintf(buf,fmt,ap);
		va_end(ap);
		return buf;
	}

	void			Dump(void) const {
		if( Type == VAR_INVALID ){
			printf("invalid.\n");
		}else if( Type == VAR_STRING ){
			const char* p = strVar.c_str();
			while( *p ){
				printf("%02X ",*p);
				p++;
			}
			printf("\n");
		}else{
			size_t	sz = sizeof(Var);
			for( size_t i=0; i<sz; i++ ){
				printf("%02X ", ((BYTE*)&Var)[ i ]);
			}
			printf("\n");
		}
	}

protected:
	const CVariant&	Copy(const CVariant& rThis){
		if( this != &rThis ){
			Type	= rThis.Type;
			Var		= rThis.Var;
			strVar	= rThis.strVar;
		}
		return(*this);
	}

public:
	CVariant(const CVariant& rThis){
		CVariant::Copy(rThis);
	}

	const CVariant&	operator=(const CVariant& rThis){
		return CVariant::Copy(rThis);
	}

	virtual ~CVariant(){
	}
};

/****************************************************************************************
 *
 *
 *
 *
 *
 */
void	UnitTest(void)
{
	CVariant	var(1.122);
	cout << var.toString() << ends;
	cout << var.TypeString() << ends;
	var.Dump();
}

}// end of "namespace VARIANT".
