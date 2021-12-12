/*
 * Copyright (C) 2020
 */

#ifndef _CHARACTER_TEMPLATE_DATA_STORE_H
#define _CHARACTER_TEMPLATE_DATA_STORE_H

#include <cstdint>
#include <vector>
#include <unordered_map>

struct character_template_class
{
	character_template_class(uint8_t faction_group_, uint8_t class_id_) :
		faction_group(faction_group_), class_id(class_id_) {}

	uint8_t faction_group;
	uint8_t class_id;
};

struct character_template
{
	uint32_t template_set_id;
	std::vector<character_template_class> classes;
	std::string name;
	std::string description;
	uint8_t level;
};

typedef std::unordered_map<uint32_t, character_template> character_template_container;

struct character_template_data_store
{
	void load_character_template();

	const character_template_container& get_character_templates() const;
	const character_template* get_character_template(uint32_t template_id) const;

	static character_template_data_store* instance();
};

#define CHARACTER_TEMPLATE_DATA_STORE character_template_data_store::instance()

#endif
