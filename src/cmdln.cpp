#include "cmdln.h"


int Commandline::exclusive = 0x0001;
int Commandline::internal  = 0x0002;

static constexpr int column_total_num = 73;

/*
 * Internal structure to store valid command line options.
 * Options are command line arguments to use optional functions and to 
 * specify optional properties of a program.
 * They are all starting with one or more special characters and can 
 * therefore be detected.
 */
struct Cmd_option
{
	Cmd_option(const std::string& long_opt, 
				const std::string& short_opt,
				const int value_count, 
				const std::string& value_descr,
				const std::string& opt_descr, 
				const int flags) :
				long_option(long_opt), 
				short_option(short_opt),
				value_count(value_count), 
				value_description(value_descr),
				option_description(opt_descr), 
				flags(flags),
				checked(false)
	{}

	Cmd_option(const Cmd_option&) = delete;
	Cmd_option& operator=(const Cmd_option&) = delete;

	// long option name
	std::string long_option;
	// short option name
	std::string short_option;
	// number of additional values
	int value_count;
	// description of optional values
	std::string value_description;
	// description of command line option
	std::string option_description;

	int flags;
	// true if find_option has been applied to this option
	bool checked;
};

/*
 * Internal structure to handle position of command line parameters.
 * Parameters are all command line arguments which are no options 
 * (e.g. file names).
 */
struct Cmd_param_pos
{
	Cmd_param_pos(const std::vector<std::string>::iterator& par_iter,
				const std::vector<std::string>::iterator& opt_iter,
				const int opt_count,
				const int direct_opt) :
				param_iter(par_iter),
				option_iter(opt_iter),
				option_count(opt_count),
				direct_option(direct_opt)
	{}

	Cmd_param_pos(const Cmd_param_pos&) = delete;
	Cmd_param_pos& operator=(const Cmd_param_pos&) = delete;

	// iterator pointing to a specific parameter
	std::vector<std::string>::iterator param_iter;
	// iterator pointing to first option iterator in front of the parameter
	std::vector<std::string>::iterator option_iter;
	// number of options in front of the parameter
	int option_count;
	// number of options which are direct predecessor in the argument list
	int direct_option;
};

Commandline::Commandline() : program_name_(), 
							valid_option_list_(),
							valid_param_list_(),
							argument_list_(),
							argument_iterator_(),
							param_pos_list_(),
							param_pos_number_(0),
							param_pos_iterator_(),
							option_pos_list_(),
							option_chars_("+-"),
							exclusive_option_(false),
							long_column_(0),
							short_column_(0),
							param_column_(0)
{}

Commandline::~Commandline()
{
        for(auto beg = valid_option_list_.begin(); beg != valid_option_list_.end();)
        {
                delete *beg;
                beg = valid_option_list_.erase(beg);
        }

        for(auto beg = valid_param_list_.begin(); beg != valid_param_list_.end();)
        {
                delete *beg;
                beg = valid_param_list_.erase(beg);
        }  
}

void Commandline::set_option_columns(const int long_cols, 
									const int short_cols)
{
	long_column_ = long_cols;
	short_column_ = short_cols;
}

bool Commandline::add_option(const std::string& long_opt, 
							const std::string& short_opt,
							const int value_count, 
							const std::string& value_descr,
							const std::string& opt_descr, 
							const int flags)
{
	if(check_option(long_opt) && check_option(short_opt))
	{
		Cmd_option* opt = new Cmd_option(long_opt, short_opt, 
										value_count, value_descr, 
										opt_descr, flags);
		if(opt != NULL)
		{
			valid_option_list_.push_back(opt);
			return true;
		}
	}
	return false;
}

bool Commandline::add_option(const std::string& long_opt, 
							const std::string& short_opt,
							const std::string& opt_descr, 
							const int flags)
{
	return add_option(long_opt, short_opt, 0, "", opt_descr, flags);
}

bool Commandline::add_option(const std::string& long_opt, 
							const int value_count,
							const std::string& value_descr, 
							const std::string& opt_descr,
							const int flags)
{
	return add_option(long_opt, "", value_count, 
					value_descr, opt_descr, flags);
}

bool Commandline::add_option(const std::string& long_opt, 
							const std::string& opt_descr,
							const int flags)
{
	return add_option(long_opt, "", 0, "", opt_descr, flags);
}

bool Commandline::check_option(const std::string& option, 
								const bool mode) const
{
	bool result = mode;
	size_t option_len = option.size();

	if(option_len > 0) // empty strings are allowed to support (sub)groups
	{
		result = false;
		if(option_len >= 2)
		{
			// options have to start with one of the defined chars
			if(option_chars_.find(option[0]) != std::string::npos)
			{
				// but when starting with sign character ...
				// ... don't allow a number as the following character
				if(((option[0] != '-') && (option[0] != '+')) ||
					(option[1] < '0') || (option[1] > '9'))
					result = true;
			}
		}
	}
	return result;
}

void Commandline::add_group(const std::string& name, 
							const int long_cols,
							const int short_cols)
{
	add_option("", "", pack_column_values(long_cols, short_cols), "", name);
}

void Commandline::add_subgroup(const std::string& name, 
								const int long_cols,
								const int short_cols)
{
	add_option("", "", pack_column_values(long_cols, short_cols), name, "");
}

int Commandline::pack_column_values(int long_cols, int short_cols) const
{
	if(long_cols < 0)
		long_cols = 0;
	if(short_cols < 0)
		short_cols = 0;
	return ((long_cols & 0xffff) << 16) | (short_cols & 0xffff);
}

void Commandline::unpack_column_values(const int value, 
									uint32_t& long_cols,
									uint32_t& short_cols) const
{
	long_cols = value == 0 ? long_column_ : (value >> 16) & 0xffff;
	short_cols = value == 0 ? short_column_ : value & 0xffff;
}

bool Commandline::operator[](const std::string& long_opt)
{
	// normal direction (right to left)
	auto iter = option_pos_list_.end();
	auto beg = option_pos_list_.begin();

	while(iter != beg)
	{
		auto it = *(--iter);
		if(*it == long_opt)
			return true;
	}
	return false;
}

Commandline::Parse_status Commandline::parse_argument_list(
											std::vector<std::string>& args,
											const int flags)
{
	argument_list_.clear();
	option_pos_list_.clear();
	exclusive_option_ = false;
	int direct_option = 0;
	int i = args.size();

	for(auto beg = args.begin(), end = args.end(); beg != end; ++beg)
	{
			const Cmd_option* opt = find_cmd_option(*beg);
			if(opt != nullptr)
			{
				argument_list_.push_back(opt->long_option);
				option_pos_list_.push_back(--argument_list_.end());

				// check for an "exclusive" option
				if(opt->flags & exclusive)
					exclusive_option_ = true;
				++direct_option;
				int j = opt->value_count; // number of expected values
				if(j >= i)
					// expecting more values than present
					return PS::missing_value;
				while(j-- > 0)
				{
					argument_list_.push_back(*(++beg));
					--i;
				}
			}
			else
			{
				argument_list_.push_back(*beg);
				return PS::unknown_option;
			}
		--i;
	}
	return check_param_count();
}

Commandline::Parse_status Commandline::check_param_count()
{
	if(has_exclusive_option())
	        return PS::exclusive_option;
	return PS::normal;
}

Commandline::Parse_status Commandline::parse_line(const std::vector<std::string>& argv,
									const int flags, 
									const int start_pos)
{
        int argc = argv.size();
	if(argc > 0)
		program_name_ = argv[0];
	else
		program_name_.clear(); 

	std::vector<std::string> args;

	if(argc > start_pos)
	{
		// expand command files (if any)
		for(int i = start_pos; i != argc; ++i)
			args.push_back(std::string(argv[i]));
	}
	// call the real function after the preparatory work is done
	return parse_argument_list(args, flags);
}

const Cmd_option* Commandline::find_cmd_option(
							const std::string& option) const
{
	for(auto beg = valid_option_list_.begin();
		beg != valid_option_list_.end();
		++beg)
	{
		if(((*beg)->long_option == option) ||
			((*beg)->short_option == option))
			return *beg;
	}
	return nullptr;
}

static void trim_escape_chars(std::string& str)
{
	for(auto beg = str.begin(); beg != str.end(); ++beg)
	{
		if(*beg == '\t' || *beg == '\n')
			beg = str.erase(beg);
	}
}

void Commandline::get_option_string(std::string& option_str)
{
	option_str.clear();

	if(!valid_option_list_.empty())
	{
		std::string str;
		int new_grp = 1;
		uint32_t short_size = 0;
		uint32_t long_size = 0;
		int long_option_size = 0;
		std::string::size_type line_indent = 0;
		std::string::size_type group_indent = 2;
		std::string::size_type subgroup_indent = 4;
		std::string::size_type column_space = 1;

		for(auto beg = valid_option_list_.begin(), 
			end = valid_option_list_.end();
			beg != end; ++beg)
		{
				if(new_grp)
				{
					// determine column width per group
					
					for(auto it = beg; it != end && 
							!(*it)->long_option.empty(); ++it)
					{
							if((*it)->short_option.size() > short_size)
								short_size = (*it)->short_option.size();
							if((*it)->long_option.size() > long_size)
								long_size = (*it)->long_option.size();
					}
					new_grp = 0;
				}
				if((*beg)->long_option.empty())
				{
					// group entry
					new_grp = 1;
                                        option_str += "\n";

					if(!(*beg)->option_description.empty()) // new group
					{
						option_str += (*beg)->option_description;
						line_indent = group_indent;
					}
					else // new subgroup
					{
						option_str.append(group_indent, ' ');
						option_str += (*beg)->value_description;
						line_indent = subgroup_indent;
					}
					option_str += "\n";
				}
				else
				{
					// regular option
					option_str.append(line_indent, ' ');
					if(short_size > 0 && (*beg)->short_option.size() > 0)
					{
						str = (*beg)->short_option;
						str.resize(short_size, ' ');
						option_str += str + ",";
						option_str.append(column_space, ' ');
					}
					else
					{
						option_str.append(column_space + short_size + 1, 
										' ');
					}
					str = (*beg)->long_option;
					str.resize(long_size, ' ');
					option_str += str;
					option_str.append(column_space + 3, ' ');
					if(!(*beg)->value_description.empty())
					{
						option_str += (*beg)->value_description;
						option_str += "\n";
						option_str.append(line_indent + short_size + 
										long_size + column_space, ' ');
						if(short_size > 0)
							option_str.append(column_space, ' ');
					}

                    str = (*beg)->option_description;
					trim_escape_chars(str);
                    if(str.size() > column_total_num)
					{
						str = console_auto_wraps(column_total_num, str, 
							(line_indent + short_size + long_size + 4));
					}
					size_t pos = 0;
					option_str += str;
					option_str += "\n";
				}
		}
	}
}

std::string Commandline::console_auto_wraps(const int row_bytes, const std::string& val, const int start_pos)
{
	std::string str = val;
	int spaces = row_bytes - start_pos;

	for(auto beg = str.begin(), it = str.begin() + spaces; it > beg; --it)
	{
		if(isspace((*it)))
		{
			it = str.insert(++it, '\n');
			beg = str.insert(++it, start_pos + 2, ' ');

			if(str.end() - beg <= 0)
				break;
		}
	}
	return str;
}
