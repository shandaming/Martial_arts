/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Node
{
	Node() : prev(nullptr), next(nullptr), child(nullptr), 
		num(-1), key(), value(), {}

	Node(const Node& n);
	Node& operator=(const Node& n);

	Node* prev, *next, *child;
	double num;
	std::string key, value;
};

template<typename T>
struct Node_iterator
{
	typedef Node_iterator<T> Self;
	typedef ptrdiff_t Difference_type;
	typedef std::bidirectional_iterator_tag Iterator_category;
	typedef T Value_type;
	typedef T* Pointer;
	typedef T& Reference;

	Node_iterator() : node() {}

	explicit Node_iterator(const Node* n) : node(n) {}

	Self node_const_cast() const { return *this; }

	Reference operator*() const { return *static_cast<Node*> }

	bool operator==(const Node* n) const;
	bool operator!=(const Node* n) const;

	operator*() const;
	operator->() const;

	operator++() { node = node->next; return *this; }
	operator++(int) { temp = this; node = node->next; return *self; }

	operator--();
	operator--(int);

	Node* node;
	Node_iterator* temp;
};

class Config
{
	public:
		enum class Type
		{
			NULLPTR,
			BOOLEAN,
			INTEGER,
			FLOAT,
			STRING,
			ARRAY,
			OBJECT
		};

		struct Node
		{
		};

		Config();
		Config(const Config& cfg);

		void check_valid() const;
		void check_valid(const Config& cfg) const;
	private:
};

#endif
