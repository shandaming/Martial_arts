/*
 * Copyright (C) 2019
 */

#ifndef OPTIONS_DESCRIPTION_H
#define OPTIONS_DESCRIPTION_H

#include <memory>
#include <any>
#include <string>
#include <vector>
#include <map>

#include "value_semantic.h"
#include "errors.h"

class option_description 
{
public:
	option_description() {}

	option_description(const char* name, const value_semantic* s);

	option_description(const char* name, const value_semantic* s, 
		const char* description);

	virtual ~option_description() {}

	enum match_result { no_match, full_match, approximate_match };

    match_result match(const std::string& option) const;

    const std::string& key(const std::string& option) const;

	std::string canonical_display_name(int canonical_option_style = 0) const;

    const std::string& long_name() const;

    const std::pair<const std::string*, std::size_t> long_names() const;

	const std::string& description() const { return description_; }

	std::shared_ptr<const value_semantic> semantic() const { return value_semantic_; }
        
    std::string format_name() const;

    std::string format_parameter() const;
private:
    option_description& set_names(const char* name);

    std::string short_name_;
    std::vector<std::string> long_names_;

    std::string description_;

    std::shared_ptr<const value_semantic> value_semantic_;
};

class options_description;
class  options_description_easy_init 
{
public:
	options_description_easy_init(options_description* owner_) : owner_(owner_) {}
	
	options_description_easy_init& operator()(const char* name, 
			const char* description);
	options_description_easy_init& operator()(const char* name,
			const value_semantic* s);
    options_description_easy_init& operator()(const char* name,
			const value_semantic* s, const char* description);
private:
    options_description* owner_;
};

class options_description 
{
public:
    options_description(const std::string& caption);

    void add(std::shared_ptr<option_description> desc);

    options_description& add(const options_description& desc);

    uint32_t get_option_column_width() const;

	options_description_easy_init add_options()
	{
		return options_description_easy_init(this);
	}

    const option_description& find(const std::string& name) const;

    const option_description* find_nothrow(const std::string& name) const;

	const std::vector< std::shared_ptr<option_description> >& options() const 
	{
		return options_; 
	}

    friend  std::ostream& operator<<(std::ostream& os, const options_description& desc);

	void print(std::ostream& os, uint32_t width = 0) const;
private:
    typedef std::map<std::string, int>::const_iterator name2index_iterator;
    typedef std::pair<name2index_iterator, name2index_iterator> approximation_range;

    std::string caption_;
    const uint32_t line_length_ = 80;
    const uint32_t min_description_length_ = 40;
        
    std::vector<std::shared_ptr<option_description>> options_;

    std::vector<bool> belong_to_group_;

	std::vector<std::shared_ptr<options_description> > groups_;
};

    /** 找到重复选项描述时抛出的类。 */
    class  duplicate_option_error : public error {
    public:
        duplicate_option_error(const std::string& xwhat) : error(xwhat) {}
    };


#endif
