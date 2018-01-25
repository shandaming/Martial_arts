/*
 * Copyright (C) 2017 by Shan Daming
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include "io.h"

enum class Type : uint8_t
{
	NULLPTR,
	BOOLEAN,
	INTEGER,
	FLOAT,
	STRING,
	ARRAY,
	OBJECT
};

class Config
{
	public:
		Config();
		explicit Config(const std::string& file);
		~Config();

		template<typename... Args>
		int get_integer(Args&&... keys);
		template<typename... Args>
		double get_float(Args&&... key);
		template<typename... Args>
		std::string get_string(Args&&... key);

		template<typename... Args>
		void update_value(Args&&... keys, const std::string& value);
		template<typename... Args>
		void update_value(Args&&... keys, double value);

		struct Node
		{
			Node() : type(Type::NULLPTR), number(0), next(nullptr), 
				prev(nullptr), child(nullptr) {}

			Node* next, *prev, *child;

			Type type;
			std::string key, value;

			double number;
		};

		Node* new_item();
		void json_delete(Node* j);

		int get_node_count(Node* item)
		{
			int i = 0;

			while(item)
			{
				++i;
				Node* c = item->next;
				if(item->child)
				{
					++i;
					i += get_node_count(item->child);
				}

				item = c;
			}
			return i;
		}

		void suffix_object(Node* prev, Node* item);

		Node* get_array_item(Node* array, int item);
		Node* get_object_item(Node* object, 
				const std::string& str);

		std::string json_print(Node* item);
		std::string json_print_unformatted(Node* item)
		{
			print_value(item, 0, 0, 0);
		}
		std::string json_print_buffered(Node* item, int prebuffer,
				int fmt);
		std::string print_value(Node* item, bool b, int depth, int fmt);
		std::string print_number(Node* item);
		std::string print_array(Node* item, int depth, int fmt);
		std::string print_object(Node* item, int depth, int fmt);
		std::string print_string_value(const std::string& str);
		std::string print_string(Node* item)
		{
			return print_string_value(item->value);
		}

		std::string parse_number(Node* item, const std::string& num);
		std::string parse_string(Node* item, const std::string& str);
		std::string parse_value(Node* item, const std::string& value);
		std::string parse_array(Node* item, const std::string& value);
		std::string parse_object(Node* item, const std::string& value);
		Node* parse(const std::string& value);

		// Append item to the specified array/object.
		void add_item_to_array(Node* array, Node* item);
		void add_item_to_object(Node* object, const std::string& str,
				Node* item);
		void add_item_to_ObjectCS(Node* object,
				const std::string& str,
				Node* item);

		/* Remove/Detach items from Arrays/Objects. */
		Node* detach_item_from_array(Node* array, int which);
		void delete_item_from_array(Node* array, int which);
		Node* detach_item_from_object(Node* object, 
				const std::string& str);
		void delete_item_from_object(Node* object,
				const std::string& str);

		// Update array items.
		void insert_item_in_array(Node* array, int which, 
				Node* new_item);//Shifts pre-existing items to the right
		void replace_item_in_array(Node* array, int which,
				Node* new_item);
		void replace_item_in_object(Node* object, 
				const std::string& str, Node* new_item);

		/*
		 * These calls create a Json item of the appropriate type.
		 */
		Node* create_null();
		Node* create_bool(bool b);
		Node* create_number(double num);
		Node* create_string(const std::string& str);
		Node* create_array();
		Node* create_object();

		// this utilities create an Array of count items.
		Node* create_number_array(const std::vector<int>& numbers, 
				int count);
		Node* create_string_array(const std::vector<std::string>& str, 
				int count);

		void json_minify(std::string& json);
	private:
		Node* root_;

		std::string file_;
};

namespace detail
{
	template<typename C = Config, typename T>
	struct Config_unpacker
	{
		void visit(C& c, Node* n, T& val)
		{
			n = c.get_object_item(n, val);
		}
	};

	template<typename C = Config, typename... Args>
	struct Config_unpacker
	{
		void visit(C& c, Node* n, Args&&... args)
		{
			Config_unpacker<C, Args...> unpack;
			unpack.visit(c, n, std::forward<Args>(vals)...);
		}
	};
}

template<typename... Args>
int Config::get_integer(Args&&... keys)
{
	detail::Config_unpacker<Config, Args...> unpack;
	Config::Node* n = root_;

	unpack.visit(*this, n, std::forward<Args>(keys)...);

	if(n->type == Type::INTEGER)
		return n->number;
}

template<typename... Args>
double Config::get_float(Args&&... keys)
{
	detail::Config_unpacker<Config, Args...> unpack;
	Config::Node* n = root_;

	unpack.visit(*this, n, std::forward<Args>(keys)...);

	if(n->type == Type::FLOAT)
		return n->number;
}

template<typename... Args>
std::string Config::get_string(Args&&... keys)
{
	detail::Config_unpacker<Config, Args...> unpack;
	Config::Node* n = root_;

	unpack.visit(*this, n, std::forward<Args>(keys)...);

	if(n->type == Type::STRING)
		return n->value;
}

template<typename... Args>
void Config::update_value(Args&&... keys, const std::string& value)
{
	Config::Node* n = root_;
	detail::Config_unpacker<Config, Args...> unpack;

	unpack.visit(*this, n, std::forward<Args>(keys)...);
	n->value = value;
}

template<typename... Args>
void Config::update_value(Args&&... keys, double value)
{
	Config::Node* n = root_;
	detial::Config_unpacker<Config, Args...> unpack;

	unpack.visit(*this, n, std::forward<Args>(keys)...);
	n->number = value;
}

#endif
