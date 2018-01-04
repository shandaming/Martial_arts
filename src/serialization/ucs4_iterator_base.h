/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef UCS4_ITERATOR_BASE_H
#define UCS4_ITERATOR_BASE_H

#include <iterator>
#include <utility>
#include <cassert>

template<typename S, typename U>
class Iterator_base
{
	public:
		Iterator_base(const S& str) : current_char(0), end(str.end()),
			current_substr(std::make_pair(str.begin(), str.end()))
		{
			update();
		}

		Iterator_base(typename S::const_iterator const& begin,
				typename S::const_iterator const& end) : current_char(0),
				end(end), current_substr(std::make_pair(begin, end))
		{
			update();
		}

		static Iterator_base begin(const S& str)
		{
			return Iterator_base(str.begin(), str.end());
		}

		static Iterator_base end(const S& str)
		{
			return Iterator_base(str.end(), str.end());
		}

		bool operator==(const Iterator_base& a) const
		{
			return current_substr.first == a.current_substr.first;
		}

		bool operator!=(const Iterator_base& a) const
		{
			return !(*this == a);
		}

		Iterator_base& operator++()
		{
			current_substr.first = current_substr.second;
			update();
			return *this;
		}

		uint32_t operator*() const { return current_char; }

		bool next_is_end() const
		{
			if(current_substr.second == end)
				return true;
			return false;
		}

		const std::pair<typename S::const_iterator, 
			  typename S::const_iterator>& substr() const
		{
			return current_substr;
		}
	private:
		void update()
		{
			assert(current_substr.first == current_substr.second);
			if(current_substr.first == end)
				return;
			current_char = U::read(current_substr.second, end);
		}

		uint32_t current_char;
		typename S::const_iterator end;
		std::pair<typename S::const_iterator, typename S::const_iterator>
			current_substr;
};

#endif
