/*
 * Copyright (C) 2017 by Shan Daming
 */
#include <strings.h> // linux header file
#include <cfloat>
#include <climits>
#include "config.h"

template<typename T>
static T skip(const T& value)
{
	auto it = value.begin();
	while(it != value.end() && static_cast<unsigned char>(*it) <= 32) 
		++it;
	return T(it, value.end());
}

static bool is_float(double number)
{
	std::string val = std::to_string(number);
	std::string::size_type n = val.find('.');

	if(n == std::string::npos)
		return false;
	return true;
}

Config::Config() : root_(nullptr)
{}

Config::Config(const std::string& file) : root_(nullptr), file_(file)
{
	std::string text = read_file(file_);
	root_ = parse(text);
}

Config::~Config()
{
	std::string out = print_value(root_, 0, 0, 1);
	write_file(file_, out);

	json_delete(root_);
}

int Config::get_integer(const std::vector<std::string>& keys)
{
	Config::Node* n = root_;

	for(auto it = keys.begin(); it != keys.end() && n; ++it)
	{
		n = get_object_item(n, *it);
	}
	if(n->type == Type::INTEGER)
		return n->number;
}

double Config::get_float(const std::vector<std::string>& keys)
{
	Config::Node* n = root_;

	for(auto it = keys.begin(); it != keys.end() && n; ++it)
	{
		n = get_object_item(n, *it);
	}
	if(n->type == Type::FLOAT)
		return n->number;
}

std::string Config::get_string(const std::vector<std::string>& keys)
{
	Config::Node* n = root_;

	for(auto it = keys.begin(); it != keys.end() && n; ++it)
	{
		n = get_object_item(n, *it);
	}
	if(n->type == Type::STRING)
		return n->value;
}

Config::Node* Config::new_item()
{
	Config::Node* item = new Config::Node();
	assert(item);
	return item;
}

void Config::json_delete(Config::Node* j)
{
	while(j)
	{
		Config::Node* n = j->next;
		if(j->child)
			json_delete(j->child);
		delete j;
		j = n;
	}
}

Config::Node* Config::get_array_item(Config::Node* array, int item)
{
	Config::Node* c = array->child;
	while(c && item > 0)
		--item, c = c->next;
	return c;
}

Config::Node* Config::get_object_item(Config::Node* object,
		const std::string& str)
{
	Config::Node* c = object->child;
	while(c && strcasecmp(c->key.c_str(), str.c_str()))
		c = c->next;
	return c;
}

std::string Config::parse_number(Config::Node* item, const std::string& num)
{
	item->type = Type::INTEGER;
	double n = 0, scale = 0, sign = 1;
	auto it = num.begin();

	if(*it == '-')
		sign = -1, ++it; // Has sign?
	if(*it == '0')
		++it; // is zero

	if(*it >= '1' && *it <= '9')
	{
		do
		{
			n = (n * 10.0) + (*it++ - '0');
		}while(*it >= '0' && *it <='9'); // Number?
	}
        // Fractional part?
	if(*it == '.' && num[1] >= '0' && num[1] <= '9')
	{
		++it;
		do
		{
			n = n * 10.0 + (*it++ -'0'), --scale;
		}while(*it >= '0' && *it <= '9');

		item->type = Type::FLOAT;
	}
        
	int subscale = 0, signsubscale = 1;
        // Exponent?
	if(*it == 'e' || *it == 'E')
	{
		++it;
		if(*it == '+')
			++it;
		else if(*it == '-')
			signsubscale = -1, ++it; // with sign?
		while(*it >= '0' && *it <= '9')
		{
			subscale = subscale * 10 + (*it++ - '0'); // Number?
		}
	}
        // number = +/- number.fraction * 10^+/- exponent
	n = sign * n * pow(10.0, scale + subscale * signsubscale);

	item->number = n;
	return std::string(it, num.end());
}

std::string Config::parse_string(Config::Node* item, const std::string& str)
{
	if(str[0] != '\"')
	{
		printf("Error: not string!\n");
		return "";
	}

	std::string out;
    auto iter = str.begin() + 1;

	while(*iter != '\"' && *iter)
	{
		if(*iter != '\\')
			out.push_back(*iter++);
		else
		{
			++iter;
			switch(*iter)
			{
				case 'b':
					out.push_back('\b');
					break;
				case 'f':
					out.push_back('\f');
					break;
				case 'n':
					out.push_back('\n');
					break;
				case 'r':
					out.push_back('\r');
					break;
				case 't':
					out.push_back('\t');
					break;
				default:
					out.push_back(*iter);
					break;
			}
			++iter;
		}
	}

	if(*iter == '\"')
		++iter;
	item->value = out;
	item->type = Type::STRING;
	return std::string(iter, str.end());
}

/* Parser core - when encountering text, process appropriately. */
std::string Config::parse_value(Config::Node* item, const std::string& value)
{
	if(value.empty())
		return value;

	auto it = value.begin();
	if(value.substr(0, 4) == "null")
	{
		item->type = Type::NULLPTR;
		return std::string(it + 4, value.end());
	}
	if(value.substr(0, 5) == "false")
	{
		item->type = Type::BOOLEAN;
		return std::string(it + 5, value.end());
	}
	if(value.substr(0, 4) == "true")
	{
		item->type = Type::BOOLEAN;
		item->number = 1;
		return std::string(it + 4, value.end());
	}
	if(*it == '\"')
		return parse_string(item, value);
	if(*it == '-' || (*it >= '0' && *it <= '9'))
		return parse_number(item, value);
	if(*it == '[')
		return parse_array(item, value);
	if(*it == '{')
		return parse_object(item, value);
}

/* Build and array from input text. */
std::string Config::parse_array(Config::Node* item, const std::string& value)
{
	auto it = value.begin();
	item->type = Type::ARRAY;

	std::string temp_val = skip(value.substr(1));
	if(temp_val[0] == ']')
		return ""; // emtpy array.

	Config::Node* child;
	item->child = child = new_item();

	// skip any spacing, get the value.
	temp_val = skip(parse_value(child, skip(temp_val))); 
	if(temp_val.empty())
		return temp_val;
	
	while(temp_val[0] == ',')
	{
		Config::Node* new_item = new_item;
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		temp_val = skip(parse_value(child, skip(temp_val.substr(1))));
		if(temp_val.empty())
			return temp_val;
	}

	if(temp_val[0] == ']')
		return temp_val.substr(1);
}

/* Build an object from the text. */
std::string Config::parse_object(Config::Node* item, const std::string& value)
{
	item->type = Type::OBJECT;
	std::string val = skip(value.substr(1));
	if(val[0] == '}')
		return "";

	Config::Node* child;
	item->child = child = new_item();

	val = skip(parse_string(child, skip(val)));
	if(val.empty())
		return val;

	child->key = child->value;
	child->value = "";
	if(val[0] != ':')
		return val;

	val = skip(parse_value(child, skip(val.substr(1))));
	if(val.empty())
		return val;

	while(val[0] == ',')
	{
		Config::Node* n = new_item();
		child->next = n;
		n->prev = child;
		child = n;

		val = skip(parse_string(child, skip(val.substr(1))));
		if(val.empty())
			return val;

		child->key = child->value;
		child->value = "";
		if(val[0] != ':')
			return val;

		val = skip(parse_value(child, skip(val.substr(1))));
		if(val.empty())
			return val;
	}

	if(val[0] == '}')
		return val.substr(1);
}

Config::Node* Config::parse(const std::string& value)
{
	Config::Node* c = new_item();

	std::string val = parse_value(c, skip(value));
	if(val.empty())
	{
		delete c;
		return nullptr;
	}

	return c;
}

void Config::suffix_object(Config::Node* prev, Config::Node* item)
{
	prev->next = item;
	item->prev = prev;
}

/* Add item to array/object. */
void Config::add_item_to_array(Config::Node* array, Config::Node* item)
{
	assert(item);
	Config::Node* c = array->child;
	if(!c)
		array->child = item;
	else
	{
		while(c && c->next)
			c = c->next;
		suffix_object(c, item);
	}
}

void Config::add_item_to_object(Config::Node* object, const std::string& str, 
		Config::Node* item)
{
	item->key = str;
	add_item_to_array(object, item);
}

Config::Node* Config::detach_item_from_array(Config::Node* array, int which)
{
	Config::Node* c = array->child;
	while(c && which > 0)
		c = c->next, --which;

	if(!c)
		return 0;
	if(c->prev)
		c->prev->next = c->next;
	if(c->next)
		c->next->prev = c->prev;
	if(c == array->child)
		array->child = c->next;
	c->prev = c->next = 0;

	return c;
}

inline void Config::delete_item_from_array(Config::Node* array, int which)
{
	json_delete(detach_item_from_array(array, which));
}

Config::Node* Config::detach_item_from_object(Config::Node* object, 
		const std::string& str)
{
	Config::Node* c = object->child;
	int i =0;
	while(c && strcasecmp(c->key.c_str(), str.c_str()))
		++i, c = c->next;
	if(c)
		return detach_item_from_array(object, i);
	return 0;
}

inline void Config::delete_item_from_object(Config::Node* object, 
		const std::string& str)
{
	json_delete(detach_item_from_object(object, str));
}

/* Replace array/object items with new ones. */
void Config::insert_item_in_array(Config::Node* array, int which, 
		Config::Node* new_item)
{
	Config::Node* c = array->child;
	while(c && which > 0)
		c = c->next, --which;
	if(!c)
	{
		add_item_to_array(array, new_item);
		return;
	}

	new_item->next = c;
	new_item->prev = c->prev;
	c->prev = new_item;
	if(c == array->child)
		array->child = new_item;
	else
		new_item->prev->next = new_item;
}

void Config::replace_item_in_array(Config::Node* array, int which,
		Config::Node* new_item)
{
	Config::Node* c = array->child;
	while(c && which > 0)
		c = c->next, --which;
	if(!c)
		return;
	new_item->next = c->next;
	new_item->prev = c->prev;
	if(new_item->next)
		new_item->next->prev = new_item;
	c->next = c->prev = 0;
	json_delete(c);
}

void Config::replace_item_in_object(Config::Node* object, 
		const std::string& str, Config::Node* new_item)
{
	Config::Node* c = object->child;
	int i = 0;
	while(c && strcasecmp(c->key.c_str(), str.c_str()))
		++i, c = c->next;
	if(c)
	{
		new_item->key = str;
		replace_item_in_array(object, i, new_item);
	}
}

/* Create basic types */
inline Config::Node* Config::create_null()
{
	Config::Node* item = new_item();
	assert(item);
	item->type = Type::NULLPTR;

	return item;
}

inline Config::Node* Config::create_bool(bool b)
{
	Config::Node* item = new_item();
	assert(item);
	item->type = Type::BOOLEAN;

	return item;
}

inline Config::Node* Config::create_number(double num)
{
	Config::Node* item = new_item();
	assert(item);

	if(is_float(num))
		item->type = Type::FLOAT;
	else
		item->type = Type::INTEGER;
	item->number = num;

	return item;
}

inline Config::Node* Config::create_string(const std::string& str)
{
	Config::Node* item = new_item();
	assert(item);

	item->type = Type::STRING;
	item->value = str;

	return item;
}

inline Config::Node* Config::create_array()
{
	Config::Node* item = new_item();
	assert(item);

	item->type = Type::ARRAY;
	return item;
}

inline Config::Node* Config::create_object()
{
	Config::Node* item = new_item();
	assert(item);

	item->type = Type::OBJECT;
	return item;
}

/* Create Arrays. */
Config::Node* Config::create_number_array(const std::vector<int>& numbers, 
		int count)
{
	Config::Node* arr = create_array(), *n, *p;
	for(int i = 0; arr && i < count; ++i)
	{
		n = create_number(numbers[i]);
		if(!i)
			arr->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return arr;
}

Config::Node* Config::create_string_array(const std::vector<std::string>& str, 
		int count)
{
	Config::Node* n, *p, *a = create_array();
	for(int i = 0; a && i < count; ++i)
	{
		n = create_string(str[i]);
		if(!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}

void Config::json_minify(std::string& json)
{
	auto it = json.begin(), iter = it;
	while(*it)
	{
		if(*it == ' ')
			++it;
		else if(*it == '\t')
			++it; // whitespace characters.
		else if(*it == '\r')
			++it;
		else if(*it == '\n')
			++it;
		else if(*it == '/' && *(it + 1) == '/')
		{
			while(*it && *it != '\n')
				++it; // double-slash comments, to end of line.
		}
		else if(*it == '/' && *(it + 1) == '*')
		{
			while(*it && !(*it == '*' && *(it + 1) == '/'))
				++it;
			it += 2;
		} // multiline comments.
		else if(*it == '\"')
		{
			*iter++ = *it++;
			while(*it && *it != '\"')
			{
				if(*it == '\\')
					*iter = *it++;
				*iter++ = *it++;
			}
			*iter++ = *it++;
		} // string literals, which are \" sensitive
		else
			*iter++ = *it++;
	}
	*iter = 0; // and null-terminate.
}

// Render the number nicely from the given item into a string.
std::string Config::print_number(Config::Node* item)
{
	double d = item->number;
	std::string str;
	std::stringstream ss;
	if(d == 0)
	{
		str = "0";
	}
	else if(fabs(item->number - d) <= DBL_EPSILON &&
			d <= INT_MAX && d >= INT_MIN)
	{
		ss << item->number;
		ss >> str;
	}
	else
	{
		if(fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60)
		{
			ss << d;
			ss >> str;
		}
		else if(fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
		{
			ss << d;
			ss >> str;
		}
		else
		{
			ss << d;
			ss >> str;
		}
	}
	return str;
}

/* Render the string provided to an escaped version that can be printed. */
std::string Config::print_string_value(const std::string& str)
{
	int flag = 0;
	std::string out;

	for(auto it = str.begin(); it != str.end(); ++it)
	{
		flag |= ((*it > 0 && *it < 32) || 
			*it == '\"' || *it == '\\') ? 1 : 0;
	}
	if(!flag)
	{
		out = "\"" + str + "\"";
		return out;
	}
	if(str.empty())
	{
		out = "\"\"";
		return out;
	}

	std::stringstream ss;
	std::string hex;

	out = "\"";

	for(int i = 0; i < str.size(); ++i)
	{
		if(static_cast<unsigned char>(str[i]) > 31 && str[i] != '\"' &&
				str[i] != '\\')
			out.push_back(str[i]);
		else
		{
			out.push_back('\\');
			switch(str[i])
			{
				case '\\':
					out.push_back('\\');
					break;
				case '\"':
					out.push_back('\"');
					break;
				case '\b':
					out.push_back('b');
					break;
				case '\f':
					out.push_back('f');
					break;
				case '\n':
					out.push_back('n');
					break;
				case '\r':
					out.push_back('r');
					break;
				case '\t':
					out.push_back('t');
					break;
				default:
					ss.str("");
					ss << std::hex << static_cast<unsigned int>(str[i]);
					ss >> hex;
					out.append(hex);
					break; // escape and print.
			}
		}
	}
	out.push_back('\"');
	return out;
}

/* Render a value to text. */
std::string Config::print_value(Config::Node* item, bool b, int depth, int fmt)
{
	assert(item);
	std::string out;

	switch(item->type)
	{
		case Type::NULLPTR:
			out = "null";
			break;
		case Type::BOOLEAN:
			if(b)
				out = "true";
			else
			    out = "false";
			break;
		case Type::INTEGER:
		case Type::FLOAT:
			out = print_number(item);
			break;
		case Type::STRING:
			out = print_string(item);
			break;
		case Type::ARRAY:
			out = print_array(item, depth, fmt);
			break;
		case Type::OBJECT:
			out = print_object(item, depth, fmt);
			break;
	}
	return out;
}

/* Reander an array to text. */
std::string Config::print_array(Config::Node* item, int depth, int fmt)
{
	// How many entries in the array?
	int numentries = get_node_count(item);

	std::string out;
	// Explicitly handle numentries == 0
	if(!numentries)
	{
		out = "[]";
		return out;
	}

	std::string val;
	std::vector<std::string> entries;

	// Retrieve all the results.
	for(Config::Node* child = item->child; child; child = child->next)
	{
		val = print_value(child, 0, depth + 1, fmt);
		entries.push_back(val);
	}

	// Compose the output array.
	out = "[";
	for(int i = 0; i < numentries; ++i)
	{
		out.append(entries[i]);
		if(i != numentries - 1)
		{
			out.push_back(',');
			if(fmt)
				out.push_back(' ');
		}
	}
	out.push_back(']');
	return out;
}

/* Render an object to text. */
std::string Config::print_object(Config::Node* item, int depth, int fmt)
{
	std::string out;
	int numentries = get_node_count(item);

	if(!numentries)
	{
		out = "{";
		if(fmt)
		{
			out.push_back('\n');
			for(int i = 0; i < depth - 1; ++i)
				out.push_back('\t');
		}
		out.push_back('}');
		return out;
	}

	++depth;
	int len = 7;
	if(fmt)
		len += depth;

	std::string val;
	std::vector<std::string> names, entries;
	Config::Node* child = item->child;

	for(int i = 0; child; child = child->next)
	{
		names.push_back(print_string_value(child->key));
		entries.push_back(print_value(child, 0, depth, fmt));
	}
	// Compose the output.
	out = "{";
	if(fmt)
		out.push_back('\n');
	for(int i = 0; i < numentries; ++i)
	{
		if(fmt)
		{
			for(int j = 0; j < depth; ++j)
			{
				out.push_back('\t');
			}
		}

		out.append(names[i]);
		out.push_back(':');
		if(fmt)
			out.push_back('\t');
		out.append(entries[i]);
		if(i != numentries - 1)
			out.push_back(',');
		if(fmt)
			out.push_back('\n');
	}

	if(fmt)
	{
		for(int i = 0; i < depth - 1; ++i)
		{
			out.push_back('\t');
		}
	}
	out.push_back('}');
	return out;
}
