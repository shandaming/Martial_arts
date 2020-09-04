/*
 * Copyright (C) 2020
 */

#include "common.h"

const char* locale_names[TOTAL_LOCALES] = 
{
	"enUS",
	"koKR",
	"frFR",
	"deDE",
	"zhCN",
	"zhTW",
	"esES",
	"esMX",
	"ruRU",
	"none",
	"ptBR",
	"itIT"
};

locale_constant get_locale_by_name(const std::string& name)
{
	for(uint32_t i = 0; i < TOTAL_LOCALES; ++i)
	{
		if(name == locale_names[i])
			return locale_constant(i);
	}

	return LOCALE_enUS;
}
