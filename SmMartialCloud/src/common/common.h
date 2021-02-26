/*
 * Copyright (C) 2020
 */

#ifndef _COMMON_H
#define _COMMON_H

enum time_constants
{
	MINUTE			= 60,
	HOUR			= MINUTE * 60,
	DAY				= HOUR * 24,
	WEEK			= DAY * 7,
	MONTH			= DAY * 30,
	YEAR			= MONTH * 12,
	IN_MILLISECONDS	= 1000
};

enum account_types
{
	SEC_PLAYER			= 0,
	SEC_MODERATOR		= 1,
	SEC_GAMEMASTER		= 2,
	SEC_ADMINISTRATOR	= 3,
	SEC_CONSOLE			= 4
};

enum locale_constant : uint8_t
{
	LOCALE_enUS = 0,
	LOCALE_koKR,
	LOCALE_frFR,
	LOCALE_deDE,
	LOCALE_zhCN,
	LOCALE_zhTW,
	LOCALE_esES,
	LOCALE_esMX,
	LOCALE_ruRU,
	LOCALE_none,
	LOCALE_ptBR,
	LOCALE_itIT,

	TOTAL_LOCALES
};

#define DEFAULT_LOCALE LOCALE_enUS
#define MAX_LOCALES 11

extern const char* locale_names[TOTAL_LOCALES];
locale_constant get_locale_by_name(const std::string& name);
#endif
