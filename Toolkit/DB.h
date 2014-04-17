// DB.h 
#pragma once

#include "Variant.h"

namespace DB {

using namespace VARIANT;

/****************************************************************************************
 *
 *
 *
 *
 *
 */
class CTable {
private:
	CTable(const CTable&);
	CTable&	operator=(const CTable&);

public:
	int							Rows;
	int							Cols;
	vector<vector<CVariant> >	Table;

	CTable(int row=1024,int col=256){
		CTable::Resize(row,col);
	}

	int		Resize(int row, int col){
		Rows = row;
		Cols = col;
		Table.resize(Rows);
		for( int i=0; i<Rows; i++ ){
			Table[ i ].resize(Cols);
		}
		return(Rows*Cols);
	}

	vector<CVariant>&	operator[](int i){
		return(Table[i]);
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
	CTable		tbl(10,10);
	
	tbl[ 0 ][ 0 ] = -1;
	tbl[ 0 ][ 1 ] = 2.1f;	
	tbl[ 0 ][ 2 ] = 3.5;
	tbl[ 0 ][ 3 ] = "value";
	tbl[ 0 ][ 4 ] = 'a';
	tbl[ 0 ][ 5 ] = (BYTE)0x12;
	tbl[ 0 ][ 6 ] = (WORD)0x3456;
	tbl[ 0 ][ 7 ] = (DWORD)0x789ABCDE;
	
	for( int y=0; y<tbl.Rows; y++ ){
		for( int x=0; x<tbl.Cols; x++ ){
			if( tbl[ y ][ x ].IsValid() ){
				cout << tbl[ y ][ x ].toString() << "(" << tbl[ y ][ x ].TypeString() << ") : " << ends;
			}
			tbl[ y ][ x ].Dump();
		}
	}
}

}// end of "namespace DB".