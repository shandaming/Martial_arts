/*
 * Copyright (C) 2019
 */

#ifndef OPTION_DESCRIPTION_H
#define OPTION_DESCRIPTION_H

#include <memory>
#include <string>
#include <vector>

#include "value_semantic.h"

class option_description
{
public:
	option_description() = default;
	option_description(const std::string& name, const value_semantic* vs);
	option_description(const std::string& name, const value_semantic* vs,
			const std::string& description);

	std::string format_name() const;
	std::string format_parameter() const;
	std::string description() const { return description_; }
private:
	// 设置选项名，判断长选项名和短选项名。e.g: help,h
	option_description& set_name(const std::string& name);

	std::string short_name_;
	std::vector<std::string> long_names_;
	std::string description_;

	std::shared_ptr<const value_semantic> value_semantic_;
};

class options_description;
class options_description_easy_init
{
public:
	options_description_easy_init(options_description* owner) : owner_(owner) {}

	// 添加选项-描述
	options_description_easy_init& operator()(const std::string& name, 
			const std::string& description);
	options_description_easy_init& operator()(const std::string& name, const value_semantic* vs);
	options_description_easy_init& operator()(const std::string& name, const value_semantic* vs, 
			const std::string& description);
private:
	options_description* owner_;
};

class options_description
{
	friend std::ostream& operator<<(std::ostream& os, const options_description& desc);
public:
	options_description(const std::string& caption);

	options_description_easy_init add_options() { return options_description_easy_init(this); }

	// 添加一个options_description，且添加一个不是组的标志
	void add(const std::shared_ptr<option_description>& description);
private:
	uint32_t get_option_column_width() const;

	void print(std::ostream& os) const;

	std::string caption_;
	const uint32_t line_length_ = 80;
	const uint32_t min_description_length_ = 40;

	std::vector<std::shared_ptr<option_description>> options_;

	std::vector<bool> belong_to_group_;

	std::vector<std::shared_ptr<options_description>> groups_;
};



#endif
