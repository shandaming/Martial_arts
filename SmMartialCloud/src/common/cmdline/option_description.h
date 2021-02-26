/*
 * Copyright (C) 2019
 */

#ifndef CMD_OPTION_DESCRIPTION_H
#define CMD_OPTION_DESCRIPTION_H

#include <memory>
#include <string>
#include <vector>
#include <exception>

class option_description
{
public:
	option_description() : value_semantic_(false) {}
	option_description(const std::string& name, bool vs);
	option_description(const std::string& name, bool vs,
			const std::string& description);

	bool match(const std::string& option) const;
	const std::string& key() const;

	bool semantic() const { return value_semantic_; }

	std::string format_name() const;
	std::string format_parameter() const;
	std::string description() const { return description_; }
private:
	// 设置选项名，判断长选项名和短选项名。e.g: help,h
	option_description& set_name(const std::string& name);

	std::string short_name_;
	std::vector<std::string> long_names_;
	std::string description_;

	bool value_semantic_;
};

class options_description;
class options_description_easy_init
{
public:
	options_description_easy_init(options_description* owner) : owner_(owner) {}

	// 添加选项-描述
	options_description_easy_init& operator()(const std::string& name, const std::string& description, const bool vs = false);
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

	const option_description* find(const std::string& name) const;
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

struct options_error : std::exception
{
	options_error(const std::string& str) : msg(str) {}

	const char* what() noexcept { return msg.c_str(); }

	std::string msg;
};

#endif
