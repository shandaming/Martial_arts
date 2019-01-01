/*
 * Copyright (C) 2018
 */

#ifndef SM_JSON_H
#define SM_JSON_H

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <utility>

enum class Type : uint8_t
{
	null,
	boolean,
	number,
	string,
	Array,
	Object
};

enum class Format : uint8_t { indent_width = 2 };

struct Null {};

template<typename Iter>
void copy(const std::string& s, Iter it)
{
	st::copy(s.begin(), s.end(), it);
}

template<typename Iter>
struct Serialize
{
	void operator()(char c)
	{
		switch(c)
		{
			case '"':
				copy("\\\"", it);
				break;
			case '\\':
				copy("\\\\", it);
				break;
			case '/':
				copy("\\/", it);
				break;
			case '\b':
				copy("\\b", it);
				break;
			case '\f':
				copy("\\f", it);
				break;
			case '\n':
				copy("\\n", it);
				break;
			case '\r':
				copy("\\r", it);
				break;
			case '\t':
				copy("\\t", it);
				break;
			default:
				if(static_cast<uint8_t>(c) < 0x20 || c == 0x7f)
				{
					char buf[8];
					snprintf(buf, sizeof(buf), "\\u%04x", c & 0xff);
					copy(buf, buf + 6, it);
				}
				else
				{
					*it++ = c;
				}
				break;
		}
	}

	Iter it;
};

template<typename Iter>
void serialize_string(const std::string& s, Iter it)
{
	*it++ = '"';
	Serialize<Iter> process_char = {it};
	std::for_each(s.begin(), s.end(), process_char);
	*it++ = '"';
}

class Value
{
public:
	typedef std::vector<Value> Array;
	typedef std::map<std::string, Value> Object;
	union Storage 
	{
		bool boolean;
		double number;

		int64_t int64;

		std::string *string;
		Array *array;
		Object *object;
	};
protected:
	int type_;
	Storage u_;
public
	Value() : type_(Type::null), u_() {}

	Value(int type, bool) : type_(type), u_()
	{
		switch(type)
		{
			case Type::boolean:
				u_.boolean_ = false;
				break;
			case Type::number:
				u_.number = 0.0;
				break;
			case Type::int64:
				u_.int64 = 0;
				break;
			case Type::string:
				u_.string = new std::string();
				break;
			case Type::array:
				u_.array = new Array();
				break;
			case Type::object:
				u_.array = new Object();
			default:
				break;
		}
	}

	explicit Value(bool b) : type_(Type::boolean), u_() { u_.boolean = b; }

	explicit Value(int64_t v) : type_(Type::int64), u_() { u_.int64 = v; }

	explicit Value(double n) : type_(number), u_()
	{
		if(std::isnan(n) || std::isinf(n))
		{
			throw std::overflow_error();
		}
		u_.number = n;
	}

	Value(const std::string &s) : type_(Type::string), u_()
	{
		u_.string = new std::string(s);
	}

	Value(const Array &a) : type_(Type::array), u_()
	{
		u_.array = new Array(a);
	}

	Value(const Object &o) : type_(Type::object), u_()
	{
		u_.object = new Object(o);
	}

	Value(std::string &&s) : type_(Type::string), u_()
	{
		u_.string = new std::string(stds::move(s));
	}

	Value(Array &&a) : type_(Type::array), u_()
	{
		u_.array = new Array(std::move(a));
	}

	Value(Object &&o) : type_(Type::object), u_()
	{
		U_.object = new Object(std::move(o));
	}

	explicit Value(const char *s) : type_(Type::string), u_()
	{
		u_.string = new std::string(s);
	}

	explicit Value(const char *s, size_t len) : type_(Type::string), u_()
	{
		u_.string = new std::string(s, len);
	}

	~Value() { clear(); }

	Value(const Value &x) : type_(x.type_), u_()
	{
		switch(type_)
		{
			case Type::string:
				u_.string = new std::string(*x.u_.string);
				break;
			case Type::array:
				u_.array = new Array(*x.u_.array);
				break;
			case Type::object:
				u_.object = new Object(*x.u_.object);
				break;
			default:
				u_ = x.u_;
				break;
		}
	}

	Value& operator=(const Value& x)
	{
		if(this != &x)
		{
			Value v(x);
			swap(v);
		}
		return *this;
	}

	Value(Value &&x) : type_(Type::null), u_() { swap(x); }

	Value& operator=(Value&& x)
	{
		swap(x);
		return *this;
	}

	void swap(Value &x)
	{
		std::swap(type_, x.type_);
		std::swap(u_, x.u_);
	}

	template <typename T> 
	bool is() const
	{
		if()
	}

	template <typename T> 
	const T &get() const
	{
		if(is<T>())
		{
			return u_.
		}
	}

	const bool& get_boolean() const 
	{ if(is_boolean()) { return u_.boolean; } }
	const double& get_number() const 
	{ if(is_number()) { return u_.number; } }
	const int64_t& get_int64() const 
	{ if(is_int64()) { return u_.int64; } }
	const std::string& get_string() const 
	{ if(is_string()) { return u_.string; } }
	const Array& get_array() const 
	{ if(is_array()) { return u_.array; } }
	const Object& get_object() const 
	{ if(is_object()) { return u_.object; } }

	template <typename T> T &get();

	template <typename T> void set(const T &);

	template <typename T> void set(T &&);

	void set_boolean(const bool v) 
	{
		type_ = Type::boolean;
		u_.boolean = v; 
	}

	void set_number(const double v) 
	{
		type_ = Type::number;
		u_.number = v; 
	}

	void set_int64(const int64_t v) 
	{
		type_ = Type::int64;
		u_.int64 = v; 
	}

	void set_string(std::string&& v) 
	{
		type_ = Type::string;
		u_.array = std::move(v); 
	}

	void set_array(Array&& v) 
	{
		type_ = Type::array;
		u_.array = std::move(v); 
	}

	void set_object(Object&& v) 
	{
		type_ = Type::object;
		u_.object = std::move(v); 
	}

	bool evaluate_as_boolean() const;
	const Value &get(const size_t idx) const
	{
		static Value null;
		if(is_array() && idx < u_.array.size())
		{
			return (*u.array)[idx];
		}
		return null;
	}

	const Value &get(const std::string &key) const
	{
		static Value null;
		if(is_object())
		{
			auto it = u_.object.find(key);
			if(it != u_.object.end())
			{
				return it->second;
			}
		}
		return null;
	}

	Value &get(const size_t idx);
	Value &get(const std::string &key)
	{

	}

	bool contains(const size_t idx) const;
	bool contains(const std::string &key) const;
	std::string to_str() const
	{
		switch(type_)
		{
			case Type::null:
				return "nmull";
			case Type::boolean:
				return u_.boolean ? "true" : "false";
			case Type::int64:
				;
			case Type::number:
				;
			case Type::string:
				return *u_.string;
			case Type::array:
				return "array";
			case Type::object:
				return "object";
			default:
		}
		return std::string();
	}

	template <typename Iter> void serialize(Iter os, bool prettify = false) const;
	std::string serialize(bool prettify = false) const;

private:
	template <typename T> Value(const T *); // intentionally defined to block implicit conversion of pointer to bool
	template <typename Iter> static void _indent(Iter os, int indent)
	{
		*os++ = '\n';
		for(size_t = 0; i < indent * INDENT_WIDTH; ++i)
		{
			*os++ = ' ';
		}
	}


	template <typename Iter> void _serialize(Iter os, int indent) const;
	{
		switch(type_)
		{
			case Type::string:
				serialize_string(*u_.string, os);
				break;
			case Type::array:
				{
					*os++ = '[';
					if(indent != -1)
					{
						++indent;
					}
					for(auto it = u_.array->begin(); it != u_.array->end();
							++it)
					{
						if(it != u_.array->begin())
						{
							*os++ = ',';
						}
						if(indent != -1)
						{
							_indent(os, indent);
						}
					}
					if(indent != -1)
					{
						--indent;
						if(!u_.array->empyt())
						{
							_indent(os, indent);
						}
					}
					*os++ = ']';
					break;
				}
			case Type::object:
				{
					*os++ = '{';
					if(indent != -1)
					{
						++indent;
					}
					for(auto it = u_.object->begin(); 
							it != u_.object->end(); ++it)
					{
						if(it != u_.object->begin())
						{
							*os++ = ',';
						}
						if(indent != -1)
						{
							_indent(os, indent);
						}
						serialize_string(it->first, os);
						*os++ = ':';
						if(indent != -1)
						{
							*os++ = ' ';
						}
						it->second._serialize(os, indent);
					}
					if(indent != -1)
					{
						--indent;
						if(!u_.object->empty())
						{
							_indent(os, indent);
						}
					}
					*os++ = '}';
					break;
				}
			default:
				copy(to_string(), os);
				break;
		}
		if(indent == 0)
		{
			*os++ = '\n';
		}
	}

	std::string _serialize(int indent) const
	{
		std::string s;
		_serialize(std::back_inserter(s), indent);
		return s;
	}

	bool is_null() const { return type_ == Type::null; }
	bool is_boolean() const { return type_ == Type::boolean; }
	bool is_number() const { return type_ == Type::number; }
	bool is_int64() const { return type_ == Type::int64; }
	bool is_string() const { return type_ == Type::string; }
	bool is_array() const { return type_ == Type::array; }
	bool is_object() const { return type_ == Type::object; }

	void clear()
	{
		switch(type_)
		{
			case Type::string:
				delete u_.string;
				break;
			case Type::array:
				delete u_.array;
				break;
			case Type::object:
				delete u_.object;
				break;
			default:
				break;
		}
	}
};

typedef value::array array;
typedef value::object object;

template <typename Iter> 
class Input 
{
protected:
	Iter cur_, end_;
	bool consumed_;
	int line_;

public:
	Input(const Iter& first, const Iter& last) : cur_(first), end_(last), consumed_(false), line_(1) {}

	int getc() 
	{
		if(consumed_) 
		{
			if (*cur_ == '\n') 
			{
				++line_;
			}
			++cur_;
		}
		if (cur_ == end_) 
		{
			consumed_ = false;
			return -1;
		}
		consumed_ = true;
		return *cur_ & 0xff;
	}

	void ungetc() {  consumed_ = false; }

	Iter cur() const 
	{
		if (consumed_) 
		{
			Input<Iter> *self = const_cast<Input<Iter> *>(this);
			self->consumed_ = false;
			++self->cur_;
		}
		return cur_;
	}

	int line() const { return line_; }

	void skip_ws() 
	{
		while (1) 
		{
			int ch = getc();
			if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) 
			{
				ungetc();
				break;
			}
		}
	}

	bool expect(const int expected) 
	{
		skip_ws();
		if (getc() != expected) 
		{
			ungetc();
			return false;
		}
		return true;
	}

	bool match(const std::string &pattern) 
	{
		for (std::string::const_iterator pi(pattern.begin()); pi != pattern.end(); ++pi) 
		{
			if (getc() != *pi) 
			{
				ungetc();
				return false;
			}
		}
		return true;
	}
};

template <typename Iter> inline int _parse_quadhex(Input<Iter> &in) 
{
	int uni_ch = 0, hex;
	for (int i = 0; i < 4; i++) 
	{
		if ((hex = in.getc()) == -1) 
		{
			return -1;
		}
		if ('0' <= hex && hex <= '9') 
		{
			hex -= '0';
		}
		else if ('A' <= hex && hex <= 'F') 
		{
			hex -= 'A' - 0xa;
		}
		else if ('a' <= hex && hex <= 'f') 
		{
			hex -= 'a' - 0xa;
		} 
		else 
		{
			in.ungetc();
			return -1;
		}
		uni_ch = uni_ch * 16 + hex;
	}
	return uni_ch;
}

template <typename String, typename Iter> inline bool _parse_codepoint(String &out, Input<Iter> &in) 
{
	int uni_ch;
	if ((uni_ch = _parse_quadhex(in)) == -1) 
	{
		return false;
	}
	if (0xd800 <= uni_ch && uni_ch <= 0xdfff) 
	{
		if (0xdc00 <= uni_ch) 
		{
			// a second 16-bit of a surrogate pair appeared
			return false;
		}
		// first 16-bit of surrogate pair, get the next one
		if (in.getc() != '\\' || in.getc() != 'u') 
		{
			in.ungetc();
			return false;
		}
		int second = _parse_quadhex(in);
		if (!(0xdc00 <= second && second <= 0xdfff)) 
		{
			return false;
		}
		uni_ch = ((uni_ch - 0xd800) << 10) | ((second - 0xdc00) & 0x3ff);
		uni_ch += 0x10000;
	}
	if (uni_ch < 0x80) 
	{
		out.push_back(static_cast<char>(uni_ch));
	}
	else 
	{
		if (uni_ch < 0x800) 
		{
			out.push_back(static_cast<char>(0xc0 | (uni_ch >> 6)));
		} 
		else 
		{
			if (uni_ch < 0x10000) 
			{
				out.push_back(static_cast<char>(0xe0 | (uni_ch >> 12)));
			}
			else 
			{
				out.push_back(static_cast<char>(0xf0 | (uni_ch >> 18)));
				out.push_back(static_cast<char>(0x80 | ((uni_ch >> 12) & 0x3f)));
			}
			out.push_back(static_cast<char>(0x80 | ((uni_ch >> 6) & 0x3f)));
		}
		out.push_back(static_cast<char>(0x80 | (uni_ch & 0x3f)));
	}
	return true;
}

template <typename String, typename Iter> inline bool _parse_string(String &out, Input<Iter> &in) 
{
	while (1) 
	{
		int ch = in.getc();
		if (ch < ' ') 
		{
			in.ungetc();
			return false;
		}
		else if (ch == '"') 
		{
			return true;
		}
		else if (ch == '\\') 
		{
			if ((ch = in.getc()) == -1) 
			{
				return false;
			}
			switch (ch) 
			{
#define MAP(sym, val)                                                                                                              \
  case sym:                                                                                                                        \
    out.push_back(val);                                                                                                            \
    break
        MAP('"', '\"');
        MAP('\\', '\\');
        MAP('/', '/');
        MAP('b', '\b');
        MAP('f', '\f');
        MAP('n', '\n');
        MAP('r', '\r');
        MAP('t', '\t');
#undef MAP
      case 'u':
        if (!_parse_codepoint(out, in)) {
          return false;
        }
        break;
      default:
        return false;
      }
    } else {
      out.push_back(static_cast<char>(ch));
    }
  }
  return false;
}

template <typename Context, typename Iter> 
inline bool _parse_array(Context &ctx, Input<Iter> &in) 
{
	if (!ctx.parse_array_start()) 
	{
		return false;
	}
	size_t idx = 0;
	if (in.expect(']')) 
	{
		return ctx.parse_array_stop(idx);
	}
	do 
	{
		if (!ctx.parse_array_item(in, idx)) 
		{
			return false;
		}
		idx++;
	} while (in.expect(','));
	return in.expect(']') && ctx.parse_array_stop(idx);
}

template <typename Context, typename Iter> inline bool _parse_object(Context &ctx, Input<Iter> &in) 
{
	if (!ctx.parse_object_start()) 
	{
		return false;
	}
	if (in.expect('}')) 
	{
		return true;
	}
	do 
	{
		std::string key;
		if (!in.expect('"') || !_parse_string(key, in) || !in.expect(':')) 
		{
			return false;
		}
		if (!ctx.parse_object_item(in, key)) 
		{
			return false;
		}
	} while (in.expect(','));
	return in.expect('}');
}

template <typename Iter> inline std::string _parse_number(Input<Iter> &in) 
{
	std::string num_str;
	while (1) 
	{
		int ch = in.getc();
		if (('0' <= ch && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E') 
		{
			num_str.push_back(static_cast<char>(ch));
		}
		else if (ch == '.') 
		{
#if PICOJSON_USE_LOCALE
			num_str += localeconv()->decimal_point;
#else
			num_str.push_back('.');
#endif
		} 
		else 
		{
			in.ungetc();
			break;
		}
	}
	return num_str;
}

template <typename Context, typename Iter> 
inline bool _parse(Context &ctx, Input<Iter> &in) 
{
  in.skip_ws();
  int ch = in.getc();
  switch (ch) {
#define IS(ch, text, op)                                                                                                           \
  case ch:                                                                                                                         \
    if (in.match(text) && op) {                                                                                                    \
      return true;                                                                                                                 \
    } else {                                                                                                                       \
      return false;                                                                                                                \
    }
    IS('n', "ull", ctx.set_null());
    IS('f', "alse", ctx.set_bool(false));
    IS('t', "rue", ctx.set_bool(true));
#undef IS
  case '"':
    return ctx.parse_string(in);
  case '[':
    return _parse_array(ctx, in);
  case '{':
    return _parse_object(ctx, in);
  default:
    if (('0' <= ch && ch <= '9') || ch == '-') {
      double f;
      char *endp;
      in.ungetc();
      std::string num_str(_parse_number(in));
      if (num_str.empty()) {
        return false;
      }
#ifdef PICOJSON_USE_INT64
      {
        errno = 0;
        intmax_t ival = strtoimax(num_str.c_str(), &endp, 10);
        if (errno == 0 && std::numeric_limits<int64_t>::min() <= ival && ival <= std::numeric_limits<int64_t>::max() &&
            endp == num_str.c_str() + num_str.size()) {
          ctx.set_int64(ival);
          return true;
        }
      }
#endif
      f = strtod(num_str.c_str(), &endp);
      if (endp == num_str.c_str() + num_str.size()) {
        ctx.set_number(f);
        return true;
      }
      return false;
    }
    break;
  }
  in.ungetc();
  return false;
}

class deny_parse_context 
{
public:
  bool set_null() {
    return false;
  }
  bool set_bool(bool) {
    return false;
  }
#ifdef PICOJSON_USE_INT64
  bool set_int64(int64_t) {
    return false;
  }
#endif
  bool set_number(double) {
    return false;
  }
  template <typename Iter> bool parse_string(Input<Iter> &) {
    return false;
  }
  bool parse_array_start() {
    return false;
  }
  template <typename Iter> bool parse_array_item(Input<Iter> &, size_t) {
    return false;
  }
  bool parse_array_stop(size_t) {
    return false;
  }
  bool parse_object_start() {
    return false;
  }
  template <typename Iter> bool parse_object_item(Input<Iter> &, const std::string &) {
    return false;
  }
};



class Default_parse_context 
{
protected:
	value *out_;

public:
	Default_parse_context(value *out) : out_(out) {}

	bool set_null() 
	{
		*out_ = value();
		return true;
	}

	bool set_bool(bool b) 
	{
		*out_ = value(b);
		return true;
	}

	bool set_int64(int64_t i) 
	{
		*out_ = value(i);
		return true;
	}

	bool set_number(double f) 
	{
		*out_ = value(f);
		return true;
	}

	template <typename Iter> 
	bool parse_string(input<Iter> &in) 
	{
		*out_ = value(string_type, false);
		return _parse_string(out_->get<std::string>(), in);
	}

	bool parse_array_start() 
	{
		*out_ = value(array_type, false);
		return true;
	}

	template <typename Iter> 
	bool parse_array_item(input<Iter> &in, size_t) 
	{
		array &a = out_->get<array>();
		a.push_back(value());
		Default_parse_context ctx(&a.back());
		return _parse(ctx, in);
	}

	bool parse_array_stop(size_t) 
	{
		return true;
	}

	bool parse_object_start() 
	{
		*out_ = value(object_type, false);
		return true;
	}

	template <typename Iter> 
	bool parse_object_item(input<Iter> &in, const std::string &key) 
	{
		object &o = out_->get<object>();
		Default_parse_context ctx(&o[key]);
		return _parse(ctx, in);
	}

private:
	Default_parse_context(const Default_parse_context &);
	Default_parse_context &operator=(const Default_parse_context &);
};



template <typename Context, typename Iter> 
inline Iter _parse(Context &ctx, const Iter &first, const Iter &last, std::string *err) 
{
	input<Iter> in(first, last);
	if (!_parse(ctx, in) && err != NULL) 
	{
		char buf[64];
		SNPRINTF(buf, sizeof(buf), "syntax error at line %d near: ", in.line());
		*err = buf;
		while (1) 
		{
			int ch = in.getc();
			if (ch == -1 || ch == '\n') 
			{
				break;
			}
			else if (ch >= ' ') 
			{
				err->push_back(static_cast<char>(ch));
			}
		}
	}
	return in.cur();
}

template <typename Iter> 
inline Iter parse(value &out, const Iter &first, const Iter &last, std::string *err) 
{
	Default_parse_context ctx(&out);
	return _parse(ctx, first, last, err);
}

inline std::string parse(Value& out, const std::string& s) 
{
	std::string err;
	parse(out, s.begin(), s.end(), &err);
	return err;
}

inline std::string parse(Value& out, std::istream& is) 
{
	std::string err;
	parse(out, std::istreambuf_iterator<char>(is.rdbuf()), 
			std::istreambuf_iterator<char>(), &err);
	return err;
}


#endif
