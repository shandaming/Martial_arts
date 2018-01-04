/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef FONT_H
#define FONT_H

class Font_property
{
	public:
		explicit Font_property(int size) : size_(size), 
			style_(TTF_STYLE_NORMAL) {}

		explicit Font_property(int size, int style) : size_(size), 
			style_(style) {}

		void set_size(int size) { size_ = size; }
		void set_style(int style_) { style_ = style; }

		int get_size() const { return size_; }
		int get_style() const { return style_; }
	private:
		int size_;
		int style_;
};

class Font
{
	public:
		explicit Font(const std::string& name);
		explicit Font(const std::string& name, 
				const Font_property& property);
		
		Font(const Font& font) : 
			fonts_(font.get_font_table()) {}

		Font& operator=(const Font& font);

		void add_font(const std::string& name, 
				const Font_property& property)
		{
			fonts_.emplace(property, font);
		}

		std::map<Font_property, TTF_Font*> get_font_table() const 
		{
			return fonts_;
		}

		TTF_Font* get_font(const Font_property& property);
	private:
		std::map<Font_property, TTF_Font*> font_table;
};

#endif
