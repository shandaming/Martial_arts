void font_test()
{
	font_property property(18, normal);
	Font font("font", property);
	font_cache.add(font);
	font_table.add("bold_style", font_cache);
}
