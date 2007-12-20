//
// BulkExtraction.h
//
// $Id: //poco/Main/Data/include/Poco/Data/BulkExtraction.h#9 $
//
// Library: Data
// Package: DataCore
// Module:  BulkExtraction
//
// Definition of the BulkExtraction class.
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


#ifndef Data_BulkExtraction_INCLUDED
#define Data_BulkExtraction_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/AbstractExtraction.h"
#include "Poco/Data/Bulk.h"
#include <vector>


namespace Poco {
namespace Data {


template <class C>
class BulkExtraction: public AbstractExtraction
	/// Specialization for bulk extraction of values from a query result set.
	/// Bulk extraction support is provided only for following STL containers:
	/// - std::vector
	/// - std::deque
	/// - std::list
{
public:
	BulkExtraction(C& result, Poco::UInt32 limit): 
		AbstractExtraction(limit, 0, true),
		_rResult(result), 
		_default(1)
	{
		if (static_cast<Poco::UInt32>(result.size()) != limit)
			result.resize(limit);
	}

	BulkExtraction(C& result, const C& def, Poco::UInt32 limit): 
		AbstractExtraction(limit, 0, true),
		_rResult(result), 
		_default(def)
	{
		if (static_cast<Poco::UInt32>(result.size()) != limit)
			result.resize(limit);
	}

	virtual ~BulkExtraction()
	{
	}

	std::size_t numOfColumnsHandled() const
	{
		return TypeHandler<C>::size();
	}

	std::size_t numOfRowsHandled() const
	{
		return _rResult.size();
	}

	std::size_t numOfRowsAllowed() const
	{
		return getLimit();
	}

	bool isNull(std::size_t row) const
	{
		try
		{
			return _nulls.at(row);
		}catch (std::out_of_range& ex)
		{ 
			throw RangeException(ex.what()); 
		}
	}

	std::size_t extract(std::size_t col)
	{
		AbstractExtractor* pExt = getExtractor();
		TypeHandler<C>::extract(col, _rResult, _default, pExt);
		typename C::iterator it = _rResult.begin();
		typename C::iterator end = _rResult.end();
		for (int row = 0; it !=end; ++it, ++row)
			_nulls.push_back(pExt->isNull(col, row));

		return _rResult.size();
	}

	virtual void reset()
	{
	}

	AbstractPrepare* createPrepareObject(AbstractPreparation* pPrep, std::size_t col)
	{
		Poco::UInt32 limit = getLimit();
		if (limit != _rResult.size()) _rResult.resize(limit);
		pPrep->setLength(limit);
		return new Prepare<C>(pPrep, col, _rResult);
	}

protected:
	const C& result() const
	{
		return _rResult;
	}

private:
	C&               _rResult;
	C                _default;
	std::deque<bool> _nulls;
};


template <class C>
class InternalBulkExtraction: public BulkExtraction<C>
	/// Container Data Type specialization extension for extraction of values from a query result set.
	///
	/// This class is intended for PocoData internal use - it is used by StatementImpl 
	/// to automaticaly create internal BulkExtraction in cases when statement returns data and no external storage
	/// was supplied. It is later used by RecordSet to retrieve the fetched data after statement execution.
	/// It takes ownership of the Column pointer supplied as constructor argument. Column object, in turn
	/// owns the data vector pointer.
	///
	/// InternalBulkExtraction objects can not be copied or assigned.
{
public:
	typedef typename C::value_type T;

	explicit InternalBulkExtraction(C& result, Column<T,C>* pColumn, Poco::UInt32 limit): 
		BulkExtraction<C>(result, _default, limit), 
		_pColumn(pColumn)
		/// Creates InternalBulkExtraction.
	{
	}

	~InternalBulkExtraction()
		/// Destroys InternalBulkExtraction.
	{
		delete _pColumn;
	}

	void reset()
	{
		_pColumn->reset();
	}	

	const T& value(int index) const
	{
		try
		{ 
			return BulkExtraction<C>::result().at(index); 
		}
		catch (std::out_of_range& ex)
		{ 
			throw RangeException(ex.what()); 
		}
	}

	bool isNull(std::size_t row) const
	{
		return BulkExtraction<C>::isNull(row);
	}

	const Column<T,C>& column() const
	{
		return *_pColumn;
	}

private:
	InternalBulkExtraction();
	InternalBulkExtraction(const InternalBulkExtraction&);
	InternalBulkExtraction& operator = (const InternalBulkExtraction&);

	Column<T,C>* _pColumn;
	C            _default;
};


template <typename T> 
BulkExtraction<std::vector<T> >* into(std::vector<T>& t, const Bulk& bulk)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::vector bulk extraction support.
{
	return new BulkExtraction<std::vector<T> >(t, bulk.size());
}


template <typename T> 
BulkExtraction<std::vector<T> >* into(std::vector<T>& t, BulkFnType)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::vector bulk extraction support.
{
	Poco::UInt32 size = static_cast<Poco::UInt32>(t.size());
	if (0 == size) throw InvalidArgumentException("Zero length not allowed.");
	return new BulkExtraction<std::vector<T> >(t, size);
}


template <typename T> 
BulkExtraction<std::deque<T> >* into(std::deque<T>& t, const Bulk& bulk)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::deque bulk extraction support.
{
	return new BulkExtraction<std::deque<T> >(t, bulk.size());
}


template <typename T> 
BulkExtraction<std::deque<T> >* into(std::deque<T>& t, BulkFnType)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::deque bulk extraction support.
{
	Poco::UInt32 size = static_cast<Poco::UInt32>(t.size());
	if (0 == size) throw InvalidArgumentException("Zero length not allowed.");
	return new BulkExtraction<std::deque<T> >(t, size);
}


template <typename T> 
BulkExtraction<std::list<T> >* into(std::list<T>& t, const Bulk& bulk)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::list bulk extraction support.
{
	return new BulkExtraction<std::list<T> >(t, bulk.size());
}


template <typename T> 
BulkExtraction<std::list<T> >* into(std::list<T>& t, BulkFnType)
	/// Convenience function to allow for a more compact creation of an extraction object
	/// with std::list bulk extraction support.
{
	Poco::UInt32 size = static_cast<Poco::UInt32>(t.size());
	if (0 == size) throw InvalidArgumentException("Zero length not allowed.");
	return new BulkExtraction<std::list<T> >(t, size);
}


} } // namespace Poco::Data


#endif // Data_BulkExtraction_INCLUDED
