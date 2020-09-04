/*
 * Copyright (C) 2020
 */

#include "character_template_data_store.h"

namespace
{
character_template_container character_template_store;
}
/*
void character_template_data_store::load_character_templates()
{
	uint32_t old_ms_time = get_ms_time();
	character_template_store.clear();

	std::unordered_map<uint32_t, std::vector<character_template_class>> character_template_classes;

	if(query_result classes_result = world_database.query("SELECT template_id, faction_group, class FROM character_template_class"))
	{
		do
		{
			field* fields = classes_result->fetch();

			uint32_t template_id = fields[0].get_uint32();
			uint8_t faction_group = fields[1]/get_uint8();
			uint8_t class_id = fields[2].get_uint8();

			if(!((faction_group & (f))))
		}
	}
}
*/
