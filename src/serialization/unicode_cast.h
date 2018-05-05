/*
 * Copyright (C) 2018
 */

#ifndef UNICODE_CAST_H
#define UNICODE_CAST_H

#include <iostream>
#include <iterator>
#include "ucs4_convert_impl.h"

namespace ucs4_convert_impl
{
	/*
	 * Transforms an output iterator to a writer for ucs4_convert_impl function
	 */
	template<typename Iter>
	struct Iterator_writer
	{
		Iterator_writer(Iter& out) : out_(out) {}

		bool can_push(size_t) { return true; }

		template<typename Value_type>
		void push(Value_type val)
		{
			*out_++ = val;
		}

		Iter& out_;
	};

	template<typename Ret, typename Check>
	struct Enableif
	{
		typedef Check Ignore;
		typedef Ret Type;
	};
}

/*
 * @param D	output	a collection type
 * @param S	input	a collection type
 */
template<typename D, typename S>
typename ucs4_convert_impl::Enableif<D, typename S::Value_type>::Type 
	unicode_cast(const S& source)
{
	using namespace ucs4_convert_impl;
	typedef typename Convert_impl<typename D::Value_type>::Type Impl_writer;
	typedef typename Convert_impl<typename S::Value_type>::Type Impl_reader;
	typedef typename std::back_insert_iterator<D> Output_iter;
	typedef typename S::const_iterator Input_iter;

	D res;
	try
	{
		Output_iter inserter(res);
		Iterator_writer<Output_iter> dst(inserter);
		Input_iter beg = source.begin();
		Input_iter end = source.end();

		while(beg != end)
			Impl_writer::write(dst, Impl_reader::read(beg, end));
	}
	catch(utf8::Invalid_utf8_exception&)
	{
		std::cerr << "Failed to convert a string from " << 
			Impl_reader::get_name() << " to " << 
			Impl_writer::get_name() << "\n";
	}
	return res;
}

template<typename D>
D unicode_cast(ucs4::Char onechar)
{
	using namespace ucs4_convert_impl;
	typedef typename Convert_impl<typename D::Value_type>::Type Impl_writer;
	typedef Convert_impl<ucs4::Char>::Type Impl_reader;
	typedef typename std::back_insert_iterator<D> Output_iter;

	D res;
	try
	{
		Output_iter inserter(res);
		Iterator_writer<Output_iter> dst(inserter);
		Impl_writer::write(dst, onechar);
	}
	catch(utf8::Invalid_utf8_exception&)
	{
		std::cerr << "Failed to convert a string from " <<
			Impl_reader::get_name() << " to " <<
			Impl_writer::get_name() << "\n";
	}
	return res;
}

#endif
