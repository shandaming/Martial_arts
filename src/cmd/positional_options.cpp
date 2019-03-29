
#define BOOST_PROGRAM_OPTIONS_SOURCE

#include <cassert>
#include "positional_options.h"


    positional_options_description& positional_options_description::add(const char* name, int max_count)
    {
        assert(max_count != -1 || trailing_.empty());

        if (max_count == -1)
            trailing_ = name;
        else {
            names_.resize(names_.size() + max_count, name);
        }
        return *this;
    }

    uint32_t positional_options_description::max_total_count() const
    {
        return trailing_.empty() ? 
          static_cast<uint32_t>(names_.size()) : (std::numeric_limits<uint32_t>::max)();
    }
    
    const std::string&   positional_options_description::name_for_position(uint32_t position) const
    {
        assert(position < max_total_count());

        if (position < names_.size())
            return names_[position];
        else
            return trailing_;
    }

