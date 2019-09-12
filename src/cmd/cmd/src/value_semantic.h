/*
 * Copyright (C) 2019
 */

#ifndef VALUE_SEMANTIC_H
#define VALUe_SEMANTIC_H

#include <string>
#include <cstdint>

class value_semantic
{
public:
	virtual ~value_semantic() {}

	virtual uint32_t min_tokens() const = 0;
	virtual uint32_t max_tokens() const = 0;

	virtual std::string name() const = 0;
};

class value_semantic_codecvt_helper : public value_semantic
{
public:
	value_semantic_codecvt_helper() {}
	uint32_t min_tokens() const override {}
	uint32_t max_tokens() const override {}
	std::string name() const override {}
};

// options_description中 只有选项名和描述，没有值。
class untyped_value : public value_semantic_codecvt_helper
{
public:
	untyped_value(const bool zero_tokens = false) : zero_tokens_(zero_tokens) {}

	uint32_t min_tokens() const override { return zero_tokens_ ? 0 : 1; }
	uint32_t max_tokens() const override { return zero_tokens_ ? 0 : 1; }

	std::string name() const override;
private:
	bool zero_tokens_;
};

class typed_value_base
{
public:
	typed_value_base() {}
	virtual ~typed_value_base() {}
};

template<typename T>
class typed_value : public value_semantic_codecvt_helper, public typed_value_base
{
public:
	typed_value(T* store_to) : store_to_(store_to) {}

	uint32_t min_tokens() const override {}
	uint32_t max_tokens() const override {}
	std::string name() const override {}
private:
	T* store_to_;
};


// 构造存储类型
template<typename T>
typed_value<T>* value(T* v)
{
	typed_value<T>* r = new typed_value<T>(v);
	return r;
}

template<typename T>
typed_value<T>* value() { return value<T>(0); }

#endif
