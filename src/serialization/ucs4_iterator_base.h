/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef UCS4_ITERATOR_BASE_H
#define UCS4_ITERATOR_BASE_H

#include <iterator>
#include <utility>
#include <cassert>
#include "unicode_type.h"

namespace ucs4
{
        template<typename String_type, typename Update_implementation>
        class Iterator_base
        {
	        public:
		        Iterator_base(const String_type& str) : 
			        current_char_(0), 
			        string_end_(str.end()),
			        current_substr(std::make_pair(str.begin(), str.begin()))
		        {
			        update();
		        }

		        Iterator_base(typename String_type::const_iterator const& begin, 
				        typename String_type::const_iterator const& end) : 
			        current_char_(0),
			        string_end_(end), 
			        current_substr_(std::make_pair(begin, begin))
		        {
			        update();
		        }

		        static Iterator_base begin(const String_type& str)
		        {
			        return Iterator_base(str.begin(), str.end()); // ???
		        }

		        static Iterator_base end(const String_type& str)
		        {
			        return Iterator_base(str.end(), str.end()); // ???
		        }

		        bool operator==(const Iterator_base& a) const
		        {
			        return current_substr_.first == a.current_substr_.first;
		        }

		        bool operator!=(const Iterator_base& a) const
		        {
			        return !(*this == a);
		        }

		        Iterator_base& operator++()
		        {
			        current_substr_.first = current_substr_.second;
			        update();
			        return *this;
		        }

		        ucs4::Char operator*() const { return current_char_; }

		        bool next_is_end() const
		        {
			        if(current_substr_.second == string_end_)
				        return true;
			        return false;
		        }

		        const std::pair<typename String_type::const_iterator, 
			          typename String_type::const_iterator>& substr() const
		        {
			        return current_substr_;
		        }
	        private:
		        void update()
		        {
			        assert(current_substr_.first == current_substr_.second);

			        if(current_substr_.first == string_end_)
				        return;

			        current_char_ = Update_implementation::read(
					        current_substr_.second, string_end_);
		        }

		        ucs4::Char current_char_;
		        typename String_type::const_iterator string_end_;
		        std::pair<typename String_type::const_iterator, 
			        typename String_type::const_iterator> current_substr_;
        };
}

#endif
