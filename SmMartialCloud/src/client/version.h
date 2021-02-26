/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <vector>
#include "serialization/string_utils.h"

class Version_info
{
	public:
		Version_info();
		Version_info(const std::string&);
		Version_info(unsigned int major, unsigned int minor, 
				unsigned int revision_level, char special_separator = '\0',
				const std::string& special = std::string());

		// Whether the version number is considered canonical for mainline .
		//
		// Mainline version numbers have at most three components, so this
		// check is equivalent to components() >= 3
		bool is_canonical() const;

		// Serializes the version number into string form.
		//
		// The result is in the format <x1.x2.x3.[.x4[.x5[...]]]>, followed
		// by the special version suffix separatoer (if not null) and the 
		// suffix itself (if not empty)
		std::string str() const;

		// Syntactic shortcut for str()
		operator std::string() const { return str(); }

		// Retrieves the major number (x1 in "x1.x2.x3")
		unsigned int major_version() const;

		// Retrieves the minor version number (x2 in "x1.x2.x3")
		unsigned int minor_version() const;

		// Retrieves the revison level (x3 in "x1.x2.x3")
		unsigned int revision_level() const;

		// Retrieves the special version separator (e.g. '+' in "0.1 + dev")
		//
		// The special version spearator is the first non-alphanumberical 
		// character preceding the special version suffix and following the
		// last numeric component. If missing, the null character is 
		// returned instead.
		char special_version_separator() const 
		{
			return special_separator_; 
		}

		// Retrieves the special version suffix (e.g. "dev" in "0.1 + dev")
		const std::string& special_version() const { return special_; }

		// Sets the major version number
		void set_major_version(unsigned int);

		// Sets the minor version number
		void set_minor_version(unsigned int);

		// Sets the revision level
		void set_revision_level(unsigned int);

		// Sets the special version suffix
		void set_special_version(const std::string& str) { special_ = str; }

		// Returns any numeric component from a version number.
		//
		// The index may be in the (0.3) range, yielding the same results as
		// major_version(), minor_version(), and revision_level().
		//
		// @throw std::out_of_range If the number of components is less than
		// <Index - 1>
		unsigned int get_component(size_t index) const 
		{
			return nums_.at(index);
		}

		// Sets any numeric component from a version number.
		//
		// The index may be in the [0, 3] range, resulting in the same 
		// effect as set_major_version(), set_minor_version(), and 
		// set_revision_level().
		void set_comonent(size_t index, unsigned int value)
		{
			nums_.at(index) = value;
		}

		// Read-only access to all numeric components.
		const std::vector<unsigned int>& components() const 
		{
			return nums_; 
		}
	private:
		std::vector<unsigned int> nums_;
		std::string special_;
		char special_separator_;
};

bool operator==(const Version_info&, const Version_info&);
bool operator!=(const Version_info&, const Version_info&);
bool operator>(const Version_info&, const Version_info&);
bool operator<(const Version_info&, const Version_info&);
bool operator>=(const Version_info&, const Version_info&);
bool operator<=(const Version_info&, const Version_info&);

enum VERSION_COMP_OP
{
	OP_INVALID,
	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_LESS,
	OP_LESS_OR_EQUAL,
	OP_GREATER,
	OP_GREATER_OR_EQUAL
};

VERSION_COMP_OP parse_version_op(const std::string& op_str);
bool do_version_check(const Version_info& a, VERSION_COMP_OP op, 
		const Version_info& b);

#endif
