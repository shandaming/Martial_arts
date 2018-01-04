/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

enum class Json_type
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
	Node() : prev(nullptr), next(nullptr), child(nullptr) {}

	Node* prev, next, child;
};

class Json_parser
{
	public:
		Json_parser();
	private:
		Node* node_;
};

#endif
