/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef HYPERLINK_H
#define HYPERLINK_H

#include <string_view>
#include "../color.h"
#include "../serialization/string_utils.h"

namespace font
{
	// Helper function for link-aware text feature
	
	inline bool looks_like_url(const std::string_view& str)
	{
		return (str.size() >= 8) && (str.substr(0, 7) == "http://") ||
			(str.substr(0, 8) == "https://");
	}

	inline std::string format_as_link(const std::string& link, Color color)
	{
		return "<span underline=\'single\' color=\'" + 
			color.to_hex_string() + "\'>" + link + "</span>";
	}
}

#endif
