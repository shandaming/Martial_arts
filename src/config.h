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

struct Node
{
	Node() : type(Type::NULLPTR), number(0), next(nullptr), 
		prev(nullptr), child(nullptr) {}

	Node* next, *prev, *child;

	Type type;
	std::string key, value;

	double number;
};

class Config
{
	public:
		Config();
		explicit Config(const std::string& file);
		~Config();

		int get_integer(const std::vector<std::string>& keys);
		double get_float(const std::vector<std::string>& key);
		std::string get_string(const std::vector<std::string>& key);

		void update_value(const std::vector<std::string>& keys, 
				const std::string& value)
		{
			Node* n = root_;

			for(auto it = keys.begin(); it != keys.end() && n; ++it)
				n = get_object_item(n, *it);

			n->value = value;
		}
		void update_value(const std::vector<std::string>& keys,
				double value)
		{
			Node* n = root_;

			for(auto it = keys.begin(); it != keys.end() && n; ++it)
				n = get_object_item(n, *it);

			n->number = value;
		}

		void json_minify(std::string& json);
	private:
		Node* root_;

		std::string file_;
};

#endif
