//
// Extractor.cpp
//
// $Id: //poco/Main/Data/ODBC/src/Extractor.cpp#5 $
//
// Library: ODBC
// Package: ODBC
// Module:  Extractor
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


#include "Poco/Data/ODBC/Extractor.h"
#include "Poco/Data/ODBC/ODBCColumn.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/ODBC/ODBCException.h"
#include "Poco/Data/BLOB.h"
#include "Poco/Exception.h"


namespace Poco {
namespace Data {
namespace ODBC {


const std::string Extractor::FLD_SIZE_EXCEEDED_FMT = "Specified data size (%z bytes) "
	"exceeds maximum value (%z).\n"
	"Use Session.setProperty(\"maxFieldSize\", value) "
	"to increase the maximum allowed data size\n";


Extractor::Extractor(const StatementHandle& rStmt, 
	Preparation& rPreparation): 
	_rStmt(rStmt), 
	_rPreparation(rPreparation),
	_dataExtraction(rPreparation.getDataExtraction())
{
}


Extractor::~Extractor()
{
}


template<>
bool Extractor::extractBoundImpl<std::string>(std::size_t pos, std::string& val)
{
	if (isNull(pos)) return false;

	std::size_t dataSize = _rPreparation.actualDataSize(pos);
	char* sp = AnyCast<char*>(_rPreparation[pos]);
	std::size_t len = strlen(sp);
	if (len < dataSize) dataSize = len;
	checkDataSize(dataSize);
	val.assign(sp, dataSize);

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<std::string> >(std::size_t pos, 
	std::vector<std::string>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = columnSize(pos);
	std::vector<std::string>::iterator it = values.begin();
	std::vector<std::string>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assign(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<std::string> >(std::size_t pos, 
	std::deque<std::string>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = columnSize(pos);
	std::deque<std::string>::iterator it = values.begin();
	std::deque<std::string>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assign(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<std::string> >(std::size_t pos, 
	std::list<std::string>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = columnSize(pos);
	std::list<std::string>::iterator it = values.begin();
	std::list<std::string>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assign(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImpl<Poco::Data::BLOB>(std::size_t pos, Poco::Data::BLOB& val)
{
	if (isNull(pos)) return false;

	std::size_t dataSize = _rPreparation.actualDataSize(pos);
	checkDataSize(dataSize);
	char* sp = AnyCast<char*>(_rPreparation[pos]);
	val.assignRaw(sp, dataSize);

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<Poco::Data::BLOB> >(std::size_t pos, 
	std::vector<Poco::Data::BLOB>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = _rPreparation.maxDataSize(pos);
	std::vector<Poco::Data::BLOB>::iterator it = values.begin();
	std::vector<Poco::Data::BLOB>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assignRaw(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<Poco::Data::BLOB> >(std::size_t pos, 
	std::deque<Poco::Data::BLOB>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = _rPreparation.maxDataSize(pos);
	std::deque<Poco::Data::BLOB>::iterator it = values.begin();
	std::deque<Poco::Data::BLOB>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assignRaw(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<Poco::Data::BLOB> >(std::size_t pos, 
	std::list<Poco::Data::BLOB>& values)
{
	char** pc = AnyCast<char*>(&_rPreparation[pos]);
	poco_assert_dbg (pc);
	poco_assert_dbg (_rPreparation.bulkSize() == values.size());
	std::size_t colWidth = _rPreparation.maxDataSize(pos);
	std::list<Poco::Data::BLOB>::iterator it = values.begin();
	std::list<Poco::Data::BLOB>::iterator end = values.end();
	for (int row = 0; it != end; ++it, ++row)
		it->assignRaw(*pc + row * colWidth, _rPreparation.actualDataSize(pos, row));

	return true;
}


template<>
bool Extractor::extractBoundImpl<Poco::Data::Date>(std::size_t pos, Poco::Data::Date& val)
{
	if (isNull(pos)) return false;
	SQL_DATE_STRUCT& ds = *AnyCast<SQL_DATE_STRUCT>(&_rPreparation[pos]);
	Utility::dateSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<Poco::Data::Date> >(std::size_t pos, 
	std::vector<Poco::Data::Date>& val)
{
	std::vector<SQL_DATE_STRUCT>& ds = RefAnyCast<std::vector<SQL_DATE_STRUCT> >(_rPreparation[pos]);
	Utility::dateSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<Poco::Data::Date> >(std::size_t pos, 
	std::deque<Poco::Data::Date>& val)
{
	std::vector<SQL_DATE_STRUCT>& ds = RefAnyCast<std::vector<SQL_DATE_STRUCT> >(_rPreparation[pos]);
	Utility::dateSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<Poco::Data::Date> >(std::size_t pos, 
	std::list<Poco::Data::Date>& val)
{
	std::vector<SQL_DATE_STRUCT>& ds = RefAnyCast<std::vector<SQL_DATE_STRUCT> >(_rPreparation[pos]);
	Utility::dateSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImpl<Poco::Data::Time>(std::size_t pos, Poco::Data::Time& val)
{
	if (isNull(pos)) return false;

	std::size_t dataSize = _rPreparation.actualDataSize(pos);
	checkDataSize(dataSize);
	SQL_TIME_STRUCT& ts = *AnyCast<SQL_TIME_STRUCT>(&_rPreparation[pos]);
	Utility::timeSync(val, ts);

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<Poco::Data::Time> >(std::size_t pos, 
	std::vector<Poco::Data::Time>& val)
{
	std::vector<SQL_TIME_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIME_STRUCT> >(_rPreparation[pos]);
	Utility::timeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<Poco::Data::Time> >(std::size_t pos, 
	std::deque<Poco::Data::Time>& val)
{
	std::vector<SQL_TIME_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIME_STRUCT> >(_rPreparation[pos]);
	Utility::timeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<Poco::Data::Time> >(std::size_t pos, 
	std::list<Poco::Data::Time>& val)
{
	std::vector<SQL_TIME_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIME_STRUCT> >(_rPreparation[pos]);
	Utility::timeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImpl<Poco::DateTime>(std::size_t pos, Poco::DateTime& val)
{
	if (isNull(pos)) return false;

	std::size_t dataSize = _rPreparation.actualDataSize(pos);
	checkDataSize(dataSize);
	SQL_TIMESTAMP_STRUCT& tss = *AnyCast<SQL_TIMESTAMP_STRUCT>(&_rPreparation[pos]);
	Utility::dateTimeSync(val, tss);

	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<Poco::DateTime> >(std::size_t pos, 
	std::vector<Poco::DateTime>& val)
{
	std::vector<SQL_TIMESTAMP_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIMESTAMP_STRUCT> >(_rPreparation[pos]);
	Utility::dateTimeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<Poco::DateTime> >(std::size_t pos, 
	std::deque<Poco::DateTime>& val)
{
	std::vector<SQL_TIMESTAMP_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIMESTAMP_STRUCT> >(_rPreparation[pos]);
	Utility::dateTimeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<Poco::DateTime> >(std::size_t pos, 
	std::list<Poco::DateTime>& val)
{
	std::vector<SQL_TIMESTAMP_STRUCT>& ds = RefAnyCast<std::vector<SQL_TIMESTAMP_STRUCT> >(_rPreparation[pos]);
	Utility::dateTimeSync(val, ds);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::vector<bool> >(std::size_t pos, 
	std::vector<bool>& val)
{
	std::size_t length = _rPreparation.getLength();
	bool** p = AnyCast<bool*>(&_rPreparation[pos]);
	val.assign(*p, *p + length);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::deque<bool> >(std::size_t pos, 
	std::deque<bool>& val)
{
	std::size_t length = _rPreparation.getLength();
	bool** p = AnyCast<bool*>(&_rPreparation[pos]);
	val.assign(*p, *p + length);
	return true;
}


template<>
bool Extractor::extractBoundImplContainer<std::list<bool> >(std::size_t pos, 
	std::list<bool>& val)
{
	std::size_t length = _rPreparation.getLength();
	bool** p = AnyCast<bool*>(&_rPreparation[pos]);
	val.assign(*p, *p + length);
	return true;
}


template<>
bool Extractor::extractManualImpl<std::string>(std::size_t pos, std::string& val, SQLSMALLINT cType)
{
	std::size_t maxSize = _rPreparation.getMaxFieldSize();
	std::size_t fetchedSize = 0;
	std::size_t totalSize = 0;
	
	SQLLEN len;
	std::auto_ptr<char> apChar(new char[CHUNK_SIZE]);
	char* pChar = apChar.get();
	SQLRETURN rc = 0;
	
	val.clear();
	resizeLengths(pos);

	do
	{
		memset(pChar, 0, CHUNK_SIZE);
		len = 0;
		rc = SQLGetData(_rStmt, 
			(SQLUSMALLINT) pos + 1, 
			cType, //C data type
			pChar, //returned value
			CHUNK_SIZE, //buffer length
			&len); //length indicator

		if (SQL_NO_DATA != rc && Utility::isError(rc))
			throw StatementException(_rStmt, "SQLGetData()");

		if (SQL_NO_TOTAL == len)//unknown length, throw
			throw UnknownDataLengthException("Could not determine returned data length.");

		if (isNullLengthIndicator(len))
		{
			_lengths[pos] = len;
			return false;
		}

		if (SQL_NO_DATA == rc || !len)
			break;

		_lengths[pos] += len;
		fetchedSize = _lengths[pos] > CHUNK_SIZE ? CHUNK_SIZE : _lengths[pos];
		totalSize += fetchedSize;
		if (totalSize <= maxSize) 
			val.append(pChar, fetchedSize);
		else 
			throw DataException(format(FLD_SIZE_EXCEEDED_FMT, fetchedSize, maxSize));
	}while (true);

	return true;
}


template<>
bool Extractor::extractManualImpl<Poco::Data::BLOB>(std::size_t pos, 
	Poco::Data::BLOB& val, 
	SQLSMALLINT cType)
{
	std::size_t maxSize = _rPreparation.getMaxFieldSize();
	std::size_t fetchedSize = 0;
	std::size_t totalSize = 0;

	SQLLEN len;
	std::auto_ptr<char> apChar(new char[CHUNK_SIZE]);
	char* pChar = apChar.get();
	SQLRETURN rc = 0;
	
	val.clear();
	resizeLengths(pos);

	do
	{
		memset(pChar, 0, CHUNK_SIZE);
		len = 0;
		rc = SQLGetData(_rStmt, 
			(SQLUSMALLINT) pos + 1, 
			cType, //C data type
			pChar, //returned value
			CHUNK_SIZE, //buffer length
			&len); //length indicator
		
		_lengths[pos] += len;

		if (SQL_NO_DATA != rc && Utility::isError(rc))
			throw StatementException(_rStmt, "SQLGetData()");

		if (SQL_NO_TOTAL == len)//unknown length, throw
			throw UnknownDataLengthException("Could not determine returned data length.");

		if (isNullLengthIndicator(len))
			return false;

		if (SQL_NO_DATA == rc || !len)
			break;

		fetchedSize = len > CHUNK_SIZE ? CHUNK_SIZE : len;
		totalSize += fetchedSize;
		if (totalSize <= maxSize) 
			val.appendRaw(pChar, fetchedSize);
		else 
			throw DataException(format(FLD_SIZE_EXCEEDED_FMT, fetchedSize, maxSize));

	}while (true);

	return true;
}


template<>
bool Extractor::extractManualImpl<Poco::Data::Date>(std::size_t pos, 
	Poco::Data::Date& val, 
	SQLSMALLINT cType)
{
	SQL_DATE_STRUCT ds;
	resizeLengths(pos);

	SQLRETURN rc = SQLGetData(_rStmt, 
		(SQLUSMALLINT) pos + 1, 
		cType, //C data type
		&ds, //returned value
		sizeof(ds), //buffer length
		&_lengths[pos]); //length indicator
	
	if (Utility::isError(rc))
		throw StatementException(_rStmt, "SQLGetData()");

	if (isNullLengthIndicator(_lengths[pos])) 
		return false;
	else 
		Utility::dateSync(val, ds);

	return true;
}


template<>
bool Extractor::extractManualImpl<Poco::Data::Time>(std::size_t pos, 
	Poco::Data::Time& val, 
	SQLSMALLINT cType)
{
	SQL_TIME_STRUCT ts;
	resizeLengths(pos);

	SQLRETURN rc = SQLGetData(_rStmt, 
		(SQLUSMALLINT) pos + 1, 
		cType, //C data type
		&ts, //returned value
		sizeof(ts), //buffer length
		&_lengths[pos]); //length indicator
	
	if (Utility::isError(rc))
		throw StatementException(_rStmt, "SQLGetData()");

	if (isNullLengthIndicator(_lengths[pos])) 
		return false;
	else 
		Utility::timeSync(val, ts);

	return true;
}


template<>
bool Extractor::extractManualImpl<Poco::DateTime>(std::size_t pos, 
	Poco::DateTime& val, 
	SQLSMALLINT cType)
{
	SQL_TIMESTAMP_STRUCT ts;
	resizeLengths(pos);

	SQLRETURN rc = SQLGetData(_rStmt, 
		(SQLUSMALLINT) pos + 1, 
		cType, //C data type
		&ts, //returned value
		sizeof(ts), //buffer length
		&_lengths[pos]); //length indicator
	
	if (Utility::isError(rc))
		throw StatementException(_rStmt, "SQLGetData()");

	if (isNullLengthIndicator(_lengths[pos])) 
		return false;
	else 
		Utility::dateTimeSync(val, ts);

	return true;
}


bool Extractor::extract(std::size_t pos, Poco::Int32& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_SLONG);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Int32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Int32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Int32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Int64& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_SBIGINT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Int64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Int64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Int64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


#ifndef POCO_LONG_IS_64_BIT
bool Extractor::extract(std::size_t pos, long& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_SLONG);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<long>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<long>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<long>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}
#endif


bool Extractor::extract(std::size_t pos, double& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_DOUBLE);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<double>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<double>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<double>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::string& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_CHAR);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<std::string>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<std::string>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<std::string>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Data::BLOB& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_BINARY);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Data::BLOB>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Data::BLOB>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Data::BLOB>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Data::Date& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_DATE);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Data::Date>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Data::Date>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Data::Date>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Data::Time& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_TIME);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Data::Time>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Data::Time>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Data::Time>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::DateTime& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_TIMESTAMP);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::DateTime>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::DateTime>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::DateTime>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Int8& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_STINYINT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Int8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Int8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Int8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::UInt8& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_UTINYINT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::UInt8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::UInt8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::UInt8>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Int16& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_SSHORT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Int16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Int16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Int16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::UInt16& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_USHORT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::UInt16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::UInt16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::UInt16>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::UInt32& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_ULONG);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::UInt32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::UInt32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::UInt32>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::UInt64& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_SBIGINT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::UInt64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::UInt64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::UInt64>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, bool& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_BIT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<bool>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<bool>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<bool>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, float& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_FLOAT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<float>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<float>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<float>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, char& val)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
		return extractManualImpl(pos, val, SQL_C_STINYINT);
	else
		return extractBoundImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<char>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<char>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<char>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImplContainer(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::Any& val)
{
	return extractImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::Any>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::Any>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::Any>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, Poco::DynamicAny& val)
{
	return extractImpl(pos, val);
}


bool Extractor::extract(std::size_t pos, std::vector<Poco::DynamicAny>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::deque<Poco::DynamicAny>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::extract(std::size_t pos, std::list<Poco::DynamicAny>& val)
{
	if (Preparation::DE_BOUND == _dataExtraction)
		return extractBoundImpl(pos, val);
	else
		throw InvalidAccessException("Direct container extraction only allowed for bound mode.");
}


bool Extractor::isNull(std::size_t col, std::size_t row)
{
	if (Preparation::DE_MANUAL == _dataExtraction)
	{
		try
		{
			return isNullLengthIndicator(_lengths.at(col));
		}catch (std::out_of_range& ex)
		{ 
			throw RangeException(ex.what()); 
		}
	}
	else
		return SQL_NULL_DATA == _rPreparation.actualDataSize(col, row);
}


void Extractor::checkDataSize(std::size_t size)
{
	std::size_t maxSize = _rPreparation.getMaxFieldSize();
	if (size > maxSize)
		throw DataException(format(FLD_SIZE_EXCEEDED_FMT, size, maxSize));
}


} } } // namespace Poco::Data::ODBC
