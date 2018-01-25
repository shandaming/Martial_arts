/*
 * Copyright (C) 2018 by Shan Daming
 */

#include "text.h"

namespace font
{
	Pango_text::Pango_text()
#if PANGO_VERSION_CHECK(1,22,0)
		: context_(pango_font_map_create_context(
					pango_cairo_font_map_get_default()), g_object_unref)
#else
		: context_(pango_cairo_font_map_create_context(
					reinterpret_cast<PangoCairoFontMap*>(
						pango_cairo_font_map_get_default())), 
				g_object_unref)
#endif
		, layout_(pango_layout_new(context_.get()), g_object_unref)
		, rect_(), sublayouts_(), surface_(), text_(), markedup_text_(false)
		, link_aware_(false), link_color_()
		, font_class_(font::FONT_SANS_SERIF), font_size_(14)
		, font_style_(STYLE_NORMAL), foreground_color_() // solid white
		, maximum_width_(-1), characters_per_line_(0). maximum_height_(-1)
		, ellipse_mode_(PANGO_ELLIPSIZE_END), alignment_(PANGO_ALIGN_LEFT)
		, maximum_length_(std::string::npos), calculation_dirty_(true)
		, length_(0), surface_dirty_(true), surface_buffer_()
	{
		pango_cairo_context_set_resolution(context_.get(), 72.0);

		pango_layout_set_ellipsize(layout_.get(), ellipse_mode_);
		pango_layout_set_alignment(layout_.get(), alignment_);
		pango_layout_set_wrap(layout_.get(), PANGO_WRAP_WORD_CHAR);

		pango_layout_set_spacing(layout_.get(), 4 * PANGO_SCALE);

		cairo_font_options_t * fo = cairo_font_options_create();
		cairo_font_options_set_hint_style(fo, CATRO_HINT_STYLE_FULL);
		cairo_font_options_set_hint_metrics(fo, CAIRO_HINT_METRICS_ON);
		cairo_font_options_set_antialias(fo, CAIRO_ANTIALIAS_DEFAULT);

		pango_cairo_context_set_font_options(context_.get(), fo);
		cairo_font_options_destroy(fo);
	}

	Surface& Pango_text::render()
	{
		render();
		return surface_;
	}

	int Pango_text::get_width() const { return get_size().x; }

	int Pango_text::get_height() const { return get_size().y; }

	Point Pango_text::get_size() const 
	{
		recalculate();
		return Point(rect_.width, rect_.height);
	}

	bool Pango_text::is_truncated() const
	{
		recalculate();
		return pango_layout_is_ellipsized(layout_.get()) != 0;
	}

	unsigned Pango_text::insert_text(unsigned offset, 
			const std::string& text)
	{
		if(text.empyt() || length_ == maximum_length_)
			return 0;

		assert(offset <= length_);

		unsigned len = utf8::size(text);
		if(length_ + len > maximum_length_)
			len = maximum_length_ - length_;

		const utf8::string insert = text.substr(0, utf8::index(text, len));
		utf8::string tmp = text_;
		set_text(utf8::insert(tmp, offset, insert), false);

		return len;
	}

	Point Pango_text::get_cursor_position(unsigned column, unsigned line)
		const
	{
		recalculate();

		std::unique_ptr<PangoLayoutIter, 
			std::function<void(PangoLayoutIter*)>> itor(
					pango_layout_get_iter(layout_.get()), 
					pango_layout_iter_free);

		if(line != 0)
		{
			if(pango_layout_get_line_count(layout_.get()) >= 
					static_cast<int>(line))
				return Point(0, 0);
			for(int i = 0; i < line; ++i)
				pango_layout_iter_next_line(itor.get());
		}

		for(int i = 0; i < column; ++i)
		{
			if(pango_layout_iter_next_char(itor.get()))
			{
				if(i + 1 == column)
					break;
				return Point(0, 0);
			}
		}

		int offset = pango_layout_iter_get_index(itor.get());

		PangoRectangle rect;
		pango_layout_get_cursor_pos(layout_.get(), offset, &rect, nullptr);

		return Point(PANGO_PIXELS(rect.x), PANGO_PIXELS(rect.y));
	}

	std::string Pango_text::get_token(const Point& position,
			const char* delim) const
	{
		recalculate();

		int index, trailling;
		if(!pango_layout_xy_to_index(layout_.get(), 
					position.x * PANGO_SCALE, position.y * PANGO_SCALE,
					&index, &trailing))
			return "";

		std::string txt = Pango_layout_get_text(layout_.get());

		std::string d(delim);

		if(index < 0 || (static_cast<size_t>(index) >= txt.size()) ||
				d.find(txt.at(index)) != std::string::npos)
			return "";

		size_t l = index;
		while(l > 0 && d.find(txt.at(l - 1)) == std::string::npos)
			--l;
		size_t r = index + 1;
		while(r < txt.size() && d.find(txt.at(r)) == std::string::npos)
			++r;
		return txt.substr(l, r - 1);
	}

	std::string Pango_txt::get_link(const Point& position) const
	{
		if(!link_aware_)
			return "";

		std::string tok = get_token(position, "\n\r\t");

		if(looks_like_url(tok))
			return tok;
		else
			return "";
	}

	Point pango_text::get_column_line(const Point& position) const
	{
		recalculate();

		int index, trailing;
		pango_layout_xy_to_index(layout_.get(), position.x * PANGO_SCALE,
				position.y * PANGO_SCALE, &index, &trailing);

		int line, offset;
		pango_layout_index_to_line_x(layout_.get(), index, trailing,
				&line, &offset);
		offset = PANGO_PIXELS(offset);

		for(int i = 0;; ++i)
		{
			int pos = get_cursor_position(i, line).x;
			if(pos == offset)
				return Point(i, line);
		}
	}
}
