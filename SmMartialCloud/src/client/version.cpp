/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "version.h"

Version_info::Version_info() : nums_(3, 0), special_(""), 
	special_separator_('\0') {}

Version_info::Version_info(unsigned int major, unsigned int minor,
		unsigned int revision_level, char special_separator, 
		const std::string& special) : nums_(3, 0), special_(special),
	special_separator_(special_separator)
{
	nums_[0] = major;
	nums_[1] = minor;
	nums_[2] = revision_level;
}

Version_info::Version_info(const std::string& str) : nums_(3, 0),
	special_(""), special_separator_('\0')
{
	std::string v = str;
	utils::trim(v);
	if(v.empty())
		return;

	// The breakpoint is where the "special" version component begins.
	// For 1.1.2a it would at the index of the char 'a', For 1.1.4+dev it
	// is at '+'.
	//
	// For 1.5.2 it is at npos.
	const std::string::size_type breakpoint_pos = 
		v.find_first_not_of(".0123456789");
	std::string left_side;

	if(breakpoint_pos != std::string::npos)
	{
		const std::string right_side = v.substr(breakpoint_pos);
		assert(right_side.empty() == false);

		if((right_side[0] >= 'A' && right_side[0] <= 'Z') ||
				(right_side[0] >= 'a' && right_side[0] <= 'z'))
		{
			special_separator_ = '\0';
			special_ = right_side;
		}
		else
		{
			special_separator_ = right_side[0];
			if(right_side.size() > 1)
				special_ = right_side.substr(1);
		}
		left_side = v.substr(0, breakpoint_pos);
	}
	else
		left_side = v;

	const std::vector<std::string> components = utils::split(left_side, '.');
	size_t s = components.size();
	if(s == 0)
		return;
	else if(s > 3)
		nums_.resize(s, 0);

	for(size_t i = 0; i < s; ++i)
		//nums_[i] = static_cast<unsigned int>(components[i]);
                nums_[i] = atol(components[i].c_str());
}

std::string Version_info::str() const
{
	size_t s = nums_.size();
	std::ostringstream o;

	for(int i = 0; i < s; ++i)
	{
		o << nums_[i];
		if(1 + i != s)
			o << '.';
	}

	if(!special_.empty())
	{
		if(special_separator_ != '\0')
			o << special_separator_;

		o << special_;
	}
	return o.str();
}

void Version_info::set_major_version(unsigned int v)
{
	nums_[0] = v;
}

void Version_info::set_minor_version(unsigned int v)
{
	nums_[1] = v;
}

void Version_info::set_revision_level(unsigned int v)
{
	nums_[2] = v;
}

unsigned int Version_info::major_version() const
{
	return nums_[0];
}

unsigned int Version_info::minor_version() const
{
	return nums_[1];
}

unsigned int Version_info::revision_level() const
{
	return nums_[2];
}

bool Version_info::is_canonical() const
{
	return nums_.size() <= 3;
}

enum COMP_TYPE
{
	EQUAL,
	NOT_EQUAL,
	LT,
	GT
};

/*
 *					x > y
 * x0.x1.x2.x3.[...].xN > y0.y1.y2.y3.[...].yN iff
 * x0 > y0 || (x0 == yo && (x1 > y1 || (x1 == y1 && (x2 > y2 || 
 * (x2 >= y2 ||)))))
 */
template<typename Toperator, typename Tfallback_operator>
bool recursive_order_operation(const std::vector<unsigned int>& l,
		const std::vector<unsigned int>& r, size_t k)
{
	if(k >= l.size() || k >= r.size())
		return false;

	auto lvalue = l[k];
	auto rvalue = r[k];

	Toperator o;
	Tfallback_operator fallback_o;

	bool ret = o(lvalue, rvalue);
	if(!ret && fallback_o(lvalue, rvalue))
		ret = recursive_order_operation<Toperator, Tfallback_operator>(
				l, r, ++k);
	return ret;
}

bool version_numbers_comparison_internal(const Version_info& l,
		const Version_info& r, COMP_TYPE o)
{
	std::vector<unsigned int> lc = l.components();
	std::vector<unsigned int> rc = r.components();

	size_t lsize = lc.size();
	size_t rsize = rc.size();
	size_t csize = std::max(lsize, rsize);

	// make compatible, missing items default to zero
	if(lsize < csize) lc.resize(csize, 0);
	if(rsize < csize) rc.resize(csize, 0);

	bool result = true;

	const std::vector<unsigned int>& lcc = lc;
	const std::vector<unsigned int>& rcc = rc;

	switch(o)
	{
		case EQUAL:
		case NOT_EQUAL:
			for(size_t i = 0; i < csize; ++i)
			{
				auto lvalue = lc[i], rvalue = rc[i];
				if(o == NOT_EQUAL)
				{
					if(result = (lvalue != rvalue))
						return true;
					continue;
				}
				else
				{
					result = result && lvalue == rvalue;
					if(!result)
						break;
				}
			}
			break;
		case LT:
			result = recursive_order_operation<std::less<unsigned int>,
				   std::equal_to<unsigned int>>(lcc, rcc, 0);
			break;
		case GT:
			result = recursive_order_operation<std::greater<unsigned int>,
				   std::equal_to<unsigned int>>(lcc, rcc, 0);
			break;
		default:
			assert(false);
			break;
	}
	return result;
}

bool operator==(const Version_info& l, const Version_info& r)
{
	return version_numbers_comparison_internal(l, r, EQUAL) &&
		l.special_version() == r.special_version();
}

bool operator!=(const Version_info& l, const Version_info& r)
{
	return version_numbers_comparison_internal(l, r, NOT_EQUAL) ||
		l.special_version() != r.special_version();
}

bool operator<(const Version_info& l, const Version_info& r)
{
	return version_numbers_comparison_internal(l, r, LT) ||
		(version_numbers_comparison_internal(l, r, EQUAL) &&
		 ((l.special_version().empty() && !r.special_version().empty()) ||
		  (l.special_version() < r.special_version())));
}

bool operator>(const Version_info& l, const Version_info& r)
{
	return version_numbers_comparison_internal(l, r, GT) ||
		(version_numbers_comparison_internal(l, r, EQUAL) &&
		 ((r.special_version().empty() && !l.special_version().empty()) ||
		  (l.special_version() > r.special_version())));
}

bool operator<=(const Version_info& l, const Version_info& r)
{
	return l < r || l == r;
}

bool operator >=(const Version_info& l, const Version_info& r)
{
	return l > r || l == r;
}

VERSION_COMP_OP parse_version_op(const std::string& op_str)
{
	if(op_str == "==")
		return OP_EQUAL;
	else if(op_str == "!=")
		return OP_NOT_EQUAL;
	else if(op_str == "<")
		return OP_LESS;
	else if(op_str == "<=")
		return OP_LESS_OR_EQUAL;
	else if(op_str == ">")
		return OP_GREATER;
	else if(op_str == ">=")
		return OP_GREATER_OR_EQUAL;

	return OP_INVALID;
}

bool do_version_check(const Version_info& a, VERSION_COMP_OP op, 
		const Version_info& b)
{
	switch(op)
	{
		case OP_EQUAL:
			return a == b;
		case OP_NOT_EQUAL:
			return a != b;
		case OP_LESS:
			return a < b;
		case OP_LESS_OR_EQUAL:
			return a <= b;
		case OP_GREATER:
			return a > b;
		case OP_GREATER_OR_EQUAL:
			return a >= b;
		default:
			;
	}
	return false;
}
