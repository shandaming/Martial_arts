/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef TEXT_H
#define TEXT_H

#include <functional>
#include <pango-1.0/pango/pango.h>
#include <pango-1.0/pango/pangocairo.h>
#include "font_options.h"
#include "../color.h"
#include "../sdl/surface.h"
#include "../serialization/string_utils.h"
#include "../serialization/unicode_types.h"

namespace font
{
	class Pango_text
	{
		public:
			Pango_text();

			Pango_text(const Pango_text&) = delete;
			Pango_text& operator=(const Pango_text&) = delete;

			/*
			 * Returns the rendered text.
			 *
			 * Before rendering it tests whether a redraw is needed and if 
			 * so it first redraws the surface before returning it.
			 */
			Surface& render();

			int get_width() const;
			int get_height() const;

			// Returns the pixel size needed for the text.
			Point get_size() const;

			// Has the text been truncated? This happens if it exceeds max 
			// width or height.
			bool is_truncated() const;

			/*
			 * Inserts UTF-8 text
			 */
			unsigned insert_text(unsigned offset, const std::string& text);


			enum FONT_STYLE
			{
				STYLE_NORMAL = 0,
				STYLE_BOLD = 1,
				STYLE_ITALIC = 2,
				STYLE_UNDERLINE = 4,
				STYLE_LIGHT = 8
			};

			/*
			 * Gets the location for the cursor.
			 */
			Point get_cursor_position(unsigned column, unsigned line = 0) 
				const;

			/*
			 * Gets the largest collection of characters, including the 
			 * token at position, and not including any characters from 
			 * the delimiters set.
			 */
			Point get_token(const Point& position, const char* delimiters =
					"\r\n\t") const;

			/*
			 * Checks if position points to a character in a link in the 
			 * text, return it if so, empty string otherwise. 
			 * Link-awareness must be enabled to get results.
			 */
			std::string get_link(const Point& position) const;

			/*
			 * Gets the column of line of the character at the position.
			 */
			Point get_column_line(const Point& position) const;

			/*
			 * Gets the length of the text in bytes.
			 *
			 * The text set is UTF-8 so the length of the string might not 
			 * be the length of the text.
			 */
			size_t get_length() const { return length_; }

			/*
			 * Sets the text to render
			 */
			bool set_text(const std::string& text, bool markedup);

			/*------ ------ ------ Setters/ getters ------ ------ ------*/

			const std::string& text() const { return text_; }

			Pango_text& set_family_class(font::Family_class& fclass);

			Pango_text& set_font_size(unsigned font_size);

			Pango_text& set_font_style(const FONT_STYLE font_style);

			Pango_text& set_foreground_color(const Color& color);

			Pango_text& set_maximum_width(int width);

			Pango_text& set_characters_per_line(
					unsigned characters_per_line);

			Pango_text& set_maximum_height(int height, bool multiline);

			Pango_text& set_ellipse_mode(
					const PangoEllipsizeMode& ellipse_mode);

			Pango_text& set_aligment(const PangoAlignment& Alignment);

			Pango_text& set_maximum_length(size_t maximum_length);

			bool link_aware() const { return link_aware_; }

			Pango_text& set_link_aware(bool b);

			Pango_text& set_link_color(const Color& color);
		private:
			std::unique_ptr<PangoContext, 
				std::function<void(void*)>> context_;
			std::unique_ptr<PangoLayout, 
				std::function<void(void*)>> layout_;
			mutable PangoRectangle rect_;

			// Used if the text is too long to fit into a single Cairo 
			// surface
			std::vector<std::unique_ptr<PangoLayout, 
				std::function<void(void*)>>> sublayouts_;

			// The SDL surface to render upon used as a cache.
			mutable Surface surface_;

			std::string text_;

			// Does the text contain pango markup? If different render 
			// routinet must be used.
			bool markedup_text_;

			// Are hyperlinks in the text marked-up, and will get_link 
			// return them.
			bool link_aware_;

			// The color to render link in.
			// links are formatted using pango &lt;span> as follows;
			// &lt;span underline="single" color=" + link_color_ + ">
			Color link_color_;

			font::Family_class font_class_;

			unsigned font_size_;

			FONT_STYLE font_style_;

			Color foreground_color_;

			/*
			 * The maximum width of the text
			 *
			 * Values less or equal to 0 mean no maximum and are internally 
			 * stored as -1, since that's the value pango uses for it.
			 */
			int maximum_width_;

			/*
			 * The number of characters per line.
			 *
			 * This can be used as an alternative of @ref maximum_width_. 
			 * The  user can select a number of characters on a line for 
			 * wrapping. When the value is non-zero it determines the 
			 * maximum width based
			 * on the average character width
			 *
			 * If both @ref maximum_width_ and @ref 
			 * characters_per_line_ are set the minimum of the two will be 
			 * thw maximum.
			 *
			 * @note long lines are often harder to read, setting this 
			 * value can automatically wrap on a number of characters 
			 * regardless of the font size. Often 66 characters is 
			 * comsidered the optimal value  for a one column text.
			 */
			unsigned characters_per_line_;

			/*
			 * The maximum height of the text
			 * 
			 * Values less or equal to 0 mean no maximum and are internally
			 * stored as -1, since that's the value pango uses for it.
			 */
			int maximum_height_;

			PangoEllipsizeMode ellipse_mode_;

			PangoAlignment alignment_;

			size_t maximum_length_;

			/*
			 * The text has two dirty states:
			 * -The setting of the state and the size calculations.
			 * -The rendering of the surface.
			 */

			// the dirty state of the calculation.
			mutable bool calculation_dirty_;

			mutable size_t length_;

			/*
			 * Recalculates the text layout
			 *
			 * when the text is recalculated the surface is dirty
			 */
			void recalculate(bool force = false) const;

			PangoRectangle calculate_size(PangoLayout& layout) const;

			mutable bool surface_dirty_;

			/*
			 * Render the text
			 *
			 * it will do a recalculation first so no nedd to call both.
			 */
			void rerender(bool force = false);

			void render(PangoLayout& layout, const PangoRectangle& rect,
					size_t surface_buffer_offset, unsigned stride);

			/*
			 * Buffer to store the image on.
			 *
			 * We use a catro surface to draw on this buffer and then use 
			 * the buffer as data source for the SDL_surface. THis means 
			 * the buffer needs to be stored in the object, since 
			 * SDL_Surface doesn;t own its buffer.
			 */
			mutable std::vector<uint8_t> surface_buffer_;

			/*
			 * Creates a new buffer.
			 *
			 * If needed frees the other surface and then crates a new 
			 * buffer and initializes the entire buffer with values 0.
			 *
			 * NOTE eventhought we're clearly modifying function we don't 
			 * change the state of the object. The const is needed so other
			 * functions can also be marked const (those also don't change 
			 * the state of the object)
			 */
			void create_surface_buffer(size_t size) const;

			/*
			 * Sets the markup'ed text.
			 *
			 * It tries to set the text as markup. If the markup is 
			 * invalid it will try a bit harder to recover from the errors 
			 * an still set the markup.
			 */
			bool set_markup(const std::string_view& text, PangoLayout& layout);

			bool validate_markup(const std::string_view& text,
					char** raw_tex, std::string& semi_escaped) const;

			/*
			 * Splits the text to two cairo surfaces.
			 *
			 * The implementation isn't recursive: the function only splits
			 * the text once. As a result, it only doubles the maximum 
			 * surface  height to 64,000 pixels or so.
			 * The reason for this is that a recursive implementation 
			 * would be more complex and it's unnecessary for now, as the 
			 * longest  surface in the game
			 * (end credits) is only about 40,000 pixels heigh width the 
			 * default large widget definition.
			 * If we need event larger surfaces in the future, the 
			 * implemenation can be made recursive.
			 */
			void split_surface();

			bool is_surface_split() const { return sublayouts_size() > 0; }

			static void copy_layout_properties(PangoLayout& src, 
					PangoLayout& dst);

			std::vector<std::string> find_links(
					const std::string_view& text) const;

			void format_links(std::string& text, 
					const std::vector<std::string>& links) const;
	};
}

#endif
