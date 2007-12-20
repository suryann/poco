//
// ODBCSQLServerTest.h
//
// $Id: //poco/Main/Data/ODBC/testsuite/src/ODBCSQLServerTest.h#4 $
//
// Definition of the ODBCSQLServerTest class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef ODBCSQLServerTest_INCLUDED
#define ODBCSQLServerTest_INCLUDED


#include "Poco/Data/ODBC/ODBC.h"
#include "ODBCTest.h"


// uncomment to use native SQL Server ODBC driver
// #define POCO_ODBC_USE_SQL_NATIVE


class ODBCSQLServerTest: public ODBCTest
	/// SQLServer ODBC test class
	/// Tested:
	/// 
	/// Driver			|	DB							| OS
	/// ----------------+-------------------------------+------------------------------------------
	/// 2000.86.1830.00 | SQL Server Express 9.0.2047	| MS Windows XP Professional x64 v.2003/SP1
	/// 2005.90.2047.00 | SQL Server Express 9.0.2047	| MS Windows XP Professional x64 v.2003/SP1
	///

{
public:
	ODBCSQLServerTest(const std::string& name);
	~ODBCSQLServerTest();

	void testBareboneODBC();

	void testBLOB();
	void testNull();
	void testBulk();

	void testStoredProcedure();
	void testCursorStoredProcedure();
	void testStoredProcedureAny();
	void testStoredProcedureDynamicAny();
	
	void testStoredFunction();

	static CppUnit::Test* suite();

private:
	void dropObject(const std::string& type, const std::string& name);
	void recreatePersonTable();
	void recreatePersonBLOBTable();
	void recreatePersonDateTimeTable();
	void recreatePersonDateTable() { /* no-op */ };
	void recreatePersonTimeTable() { /* no-op */ };
	void recreateStringsTable();
	void recreateIntsTable();
	void recreateFloatsTable();
	void recreateTuplesTable();
	void recreateVectorTable();
	void recreateVectorsTable();
	void recreateAnysTable();
	void recreateNullsTable(const std::string& notNull = "");
	void recreateBoolTable();
	void recreateMiscTable();

	static SessionPtr  _pSession;
	static ExecPtr     _pExecutor;
	static std::string _driver;
	static std::string _dsn;
	static std::string _uid;
	static std::string _pwd;
	static std::string _db;
	static std::string _connectString;
};


#endif // ODBCSQLServerTest_INCLUDED
