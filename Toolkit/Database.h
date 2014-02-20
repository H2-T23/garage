#pragma once

namespace DB {
	using namespace std;

	struct SchemaEntity {
		TString		name;
	};

	typedef vector<SchemaEntity>		Schema;
	typedef vector<vector<TString> >	Table;


	class ITable {
	public:
		virtual void	Append();
		virtual void	AppendHead();
		virtual void	AppendTail();

		virtual void	Find();

		virtual void	Remove();
		virtual void	RemoveHead();
		virtual void	RemoveTail();
	};


	class CTable {
	protected:
		Schema			m_Schema;
		Table			m_Table;

	public:
		
	};

	class CDatabase {
	public:
		vector<CTable>		m_Tables;
	};
}