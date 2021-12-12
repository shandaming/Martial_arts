/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef CMDLN_H
#define CMDLN_H

#include <string>
#include <vector>
#include <algorithm>

struct Cmd_option;
struct Cmd_param_pos;

/*
 * Internal structure to store valid command line parameters.
 * Parameters are all command line arguments which are no options
 * (e.g. file names).
 */
struct Cmd_param
{
	// mode specifying parameter's cardinality
	enum class Param_mode
	{
		// parameter is required (# = 1) "option"
		mandatory,
		// parameter is optional (# = 0..1), "[option]"
		optional,
		// parameter is required, more than one value is allowed 
		// (# = 1..n), "option..."
		multi_mandatory,
		// parameter is optional, more than one value is allowed 
		// (# = 0..n), "[option...]"
		multi_optional
	};

	Cmd_param(const std::string& param, 
			const std::string& descr, 
			const Param_mode& mode) :
			param_name(param), 
			param_description(descr), 
			param_mode(mode)
	{}

        Cmd_param(const Cmd_param&) = delete;
	Cmd_param& operator=(const Cmd_param&) = delete;

	std::string param_name;
	std::string param_description;

	Param_mode param_mode;
};

/*
 * handles command line arguments.
 * This class is the interface to this module.
 * All methods which can be used from outside are defined here.
 */
 class Commandline
 {
	public:
		// status of command line parsing
		enum class Parse_status
		{
			// normal, no errors
			normal,
			// no arguments to be parsed
			no_arguments,
			// exclusive option used
			exclusive_option,
			// unknown option detected
			unknown_option,
			// missing value(s) for an option
			missing_value,
			// missing parameter
			missing_parameter,
			// too many parameters
			too_many_parameters,
			// cannot open command file
			cannot_open_command_file
		};

		// status of converting string option value to value field
		enum class Value_status
		{
			// normal, no errors
			normal,
			// argument contains invalid char(s)
			invalid,
			// no more arguments to be converted
			no_more,
			// empty string argument
			empty,
			// converted value falls below minimum
			underflow,
			// converted value exceeds maximum
			overflow
		};

		// status of converting string parameter to value field
		enum class Param_value_status
		{
			// normal, no errors
			normal,
			// argument contains invalid char(s)
			invalid,
			// specified parameter doesn't exist
			cant_find,
			// empty string argument
			empty,
			// converted value falls below minimum
			underflow,
			// converted value exceeds maximum
			overflow
		};

		// mode for find_option() function
		enum class Find_option_mode
		{
			// normal find
			normal,
			// find first option
			first,
			// find next option
			next,
			// find first option (from left to right)
			first_from_left,
			// find next option (from left to right)
			next_from_left
		};

		Commandline();

		~Commandline();

		// sets default width of option colums
		void set_option_columns(const int long_cols, const int short_cols);

		// adds an item to the list of valid options
		bool add_option(const std::string& long_opt, 
						const std::string& short_opt,
						const int value_count, 
						const std::string& value_descr,
						const std::string& opt_descr, 
						const int flags = 0);
		bool add_option(const std::string& long_opt, 
						const std::string& short_opt,
						const std::string& opt_descr, 
						const int flags = 0);
		bool add_option(const std::string& long_opt, 
						const int value_count,
						const std::string& value_descr, 
						const std::string& opt_descr,
						const int flags = 0);
		bool add_option(const std::string& long_opt, 
						const std::string& opt_descr,
						const int flags = 0);

		// adds a new group (top-level). all following options belong to 
		// this group
		void add_group(const std::string& name, 
					const int long_cols = 0,
					const int short_cols = 0);

		// adds a new subgroup (beyond group-level). all following options 
		// belong to this subgroup
		void add_subgroup(const std::string& name, 
						const int long_cols = 0,
						const int short_cols = 0);


		// gets number of command line arguments
		int get_arg_count() const { return argument_list_.size(); }


		// gets number of parameters in the parsed command line. A parameter
		// is an argument which is no option (e.g. a filename).
		int get_param_count() const { return param_pos_list_.size(); }


		// checks vhether the parsed command line contains any "exclusive" 
		// option which does not require any mandatory parameter. Examples 
		// for typical exclusive options are "--help" and "--version".
		bool has_exclusive_option() const { return exclusive_option_; }

		// checks whether specified parameter exists in the command line.
		bool find_param(const int pos);


		// checks whether specified option exists in the command line.
		// The search process starts from the last command line argument 
		// (which is a direct predecessor of the optional defined reference
		// parameter 'pos') and goes to the first one.
		bool operator[](const std::string& long_opt);

		// parses specified command line arguments (argc, argv).
		// Additionally, create internal structures for evaluation and 
		// return status indicating any errors that occurred during the 
		// parse process.
		Parse_status parse_line(const std::vector<std::string>& argv, 
								const int flag = 0,
								const int start_pos = 1);

		// returns command line syntax as a C++ string (single text line).
		// Text consists of "[options]" (if defined by addOption()) and 
		// all parameters defined by addParam().
		void get_syntax_string(const std::string& syntax_str) const;

		// returns description of all options as a C++ string (multiple 
		// text lines). Text consists of group headings, short and long 
		// option names, option descriptions and additional option values.
		void get_option_string(std::string& option_str);

		// returns description of all parameters as a C++ string (multiple 
		// text lines). Text consists of heading "parameters:" and parameter
		// names and descriptions.
		void get_param_string(const std::string& param_str) const;

		// returns status of parseLine() as a C++ string
		void get_status_string(const Parse_status& status, 
								const std::string& status_str);
		// returns status of getParam() as a C++ string
		void get_status_string(const Param_value_status& status,
								const std::string& status_str);
		// returns status of getValue() as a C++ string
		void get_status_string(const Value_status& status, 
								const std::string& status_str);

                std::string console_auto_wraps(const int row_bytes, const std::string& val, const int start_pos = 0);

		// exclusive option that overrides any other option (e.g.\ "--help")
		static int exclusive;
		// internal option that is not shown in the syntax usage output
		static int internal;
	protected:
		// checks whether given option is valid (starting with an option 
		// char and not followed by a number)
		bool check_option(const std::string& option, 
						const bool mode = true) const;

		// finds specified parameter and sets given iterator to this 
		// position
		bool find_param(const int pos, std::vector<Cmd_param_pos*>::iterator& pos_iter);

		// finds specified option and returns reference to its describing 
		// structure
		const Cmd_option* find_cmd_option(const std::string& option) const;

		// parse the given argument list and check it for known 
		// options/parameters
		Parse_status parse_argument_list(std::vector<std::string>& arg_list, 
										const int flags);

		// packs the two 16 bit values into one 32 bit value
		int pack_column_values(int long_cols, int short_cols) const;
		// unpacks two 16 bit values from one 32 bit value
		void unpack_column_values(const int value, 
								uint32_t& long_cols, 
								uint32_t& short_cols) const;

		// checks whether number of parameters in parsed command line is 
		// within the range
		// of min/max (see below)
		Parse_status check_param_count();
	private:
		// program name, i.e. the value of argv[0]
		std::string program_name_;

		// list of valid/defined command line options
		std::vector<Cmd_option*> valid_option_list_;
		// list of valid/defined command line parameters
		std::vector<Cmd_param*> valid_param_list_;

		// list of command line arguments (after parsing)
		std::vector<std::string> argument_list_;
		// current position in argument list
		std::vector<std::string>::iterator argument_iterator_;

		// list of parameter positions within argument list
		std::vector<Cmd_param_pos*> param_pos_list_;
		// current position in param position list (numeric value)
		int param_pos_number_;
		// current position in param position list (iterator)
		std::vector<Cmd_param_pos*>::iterator param_pos_iterator_;

		// list of option positions within argument list
		std::vector<std::vector<std::string>::iterator> option_pos_list_;
		// current position in option position list
		std::vector<std::vector<std::string>::iterator> option_pos_iterator_;

		// C++ string consisting of all valid characters introducing an option
		std::string option_chars_;

		// OFTrue if an "exclusive" option is used in the command line, OFFalse otherwise
		bool exclusive_option_;

		// width of column for long option names
		int long_column_;
		// width of column for short option names
		int short_column_;
		// width of column for parameter names
		int param_column_;

		// minimum number of parameters which should be accepted
		int min_param_count_;
		// maximum number of parameter which should be accepted
		int max_param_count_;
 };

                using PM = Cmd_param::Param_mode;
                using PS = Commandline::Parse_status;
                using VS = Commandline::Value_status;
                using PVS = Commandline::Param_value_status;
                using FOM = Commandline::Find_option_mode;

#endif
