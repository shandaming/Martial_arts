/*
 * Copyright (C) 2017 by Shan Daming
 */

#include "sdl_ttf.h"
#include "../serialization/unicode.h"

/*
 * Record stored in the font table.
 * If the record for Font_id (FOO, Bold + Underline) is a record (BAR, Bold)
 * it means that BAR is a Bold-styled version of FOO which we shipped with
 * the game, and now SDL_TTF should be used to style BAR as underline for
 * the final results.
 */

struct TTF_record
{
	TTF_Font* font;
	int style;
};

static std::map<Font_id, TTF_record> font_table;
static std::vector<std::string> font_names;
static std::vector<std::string> bold_names;
static std::vector<std::string> italic_names;

/* cache sizes of small text. */
typedef std::map<std::string, SDL_Rect> line_size_cache_map;

/* map of styles -> sizes -> cache */
static std::map<int, std::map<int, line_size_cache_map>> line_size_cache;

typedef std::map<std::pair<std::string, int>, TTF_Font*> open_font_cache;
open_font_cache open_fonts;

/*
 * A wrapper which caches the results of open_font_impl.
 * Note that clear_fonts() is responsible to clean up all of these font
 * pointers, so to avoid memory leaks fonts should only be opened from 
 * this function.
 */
static TTF_Font* open_font(const std::string& fname, int size)
{
	std::pair<std::string, int>  key = std::make_pair(fname, size);
	auto it = open_fonts.find(key);
	if(it != open_fonds.end())
		return it->second;

	TTF_Font* result = TTF_OpenFont(file_exists(fname), size);
	if(result)
	{
		open_fonts.emplace(key, result);
		return result;
	}
	return nullptr;
}

inline int line_width(const std::string& line, int font_size, int style)
{
	return line_size(line, font_size, style).w;
}

SDL_Rect line_size(const std::string& line, int font_size, int style)
{
	line_size_cache_map& cache = line_size_cache[style][font_size];

	if((auto it = cache.find(line)) != cache.end())
		return it->second;

	const Color col{0, 0, 0, 0};
	Text_surface s(line, font_size, col, style);

	SDL_Rect res;
	res.w = s.width();
	res.h = s.height();
	res.x = res.y = 0;

	cache.emplace(line, res);
	return res;
}

std::string make_text_ellipsis(const std::string& text, int font_size,
		int max_width, int style)
{
	if(line_width(text, font_size, style) <= max_width)
		return text;
	if(line_width(ellipsis, font_size, style) > max_width)
		return "";

	std::string current_substring, temp;
	Iterator it(text);

	for(; it != Iterator::end(text); ++it)
	{
		temp = current_substring;
		temp.append(it.substr().first, it.substr().second);

		if(line_width(temp + ellipsis, font_size, style) > max_width)
			return current_substring + ellipsis;

		current_substring.append(it.substr().first, it.substr().second);
	}
	return text;
}

/*
 * Gets an appropriately configured TTF font, for this font size and style
 * Loads fonts if necessary. For styled fonts, we search for a 'shipped'
 * version of the font which is prestyled. If this fails we find the closest
 * thing which we did ship, and store a record of this, which allows to 
 * rapidly correct the remaining styling using SDL_TTF.
 *
 * Uses the font table for caching.
 */
TTF_Font* SDL_ttf::get_font(Font_id& id)
{
	auto it = font_table.find(id);
	if(it != ont_table.end())
	{
		if(it->second.font != nullptr)
		{
			// If we found a valid record, use SDL_TTF to add in the
			// difference between its intrinsic style and the desired
			// style.
			TTF_SetFontStyle(it->second.font, it->second.style ^ id.style);
		}
		return it->second.font;
	}

	// There's no record, so we need to try to find a solution for this font
	// and make a record of it. If the indices are out of bounds don't 
	// bother though.
	if(id.subset < 0 || size_t(id.subset) >= font_names.size())
		return nullptr;

	// Favor to use the shipped Italic font over bold if both are present
	// and are needed.
	if((id.style & TTF_STYLE_ITALIC) && italic_names[id.subset].size())
	{
		if(TTF_Font* font = open_font(italic_names[id.subset], id.size))
		{
			TTF_record rec{font, TTF_STYLE_ITALIC};
			font_table.emplace(id, rec);
			return get_font(id);
		}
	}

	// Now see if the shipped Bold font is useful and available.
	if((id.style & TTF_STYLE_BOLD) && bold_names[id.subset].size())
	{
		if(TTF_Font* f = open_font(bold_names[id.subset], id.size))
		{
			TTF_record rec{f, TTF_STYLE_BOLD};
			font_table.emplace(id, rec);
			return get_font(id);
		}
	}

	// Try just to use the basic version of the font then.
	if(font_names[id.subset].size())
	{
		if(TTF_Font* f = open_font(font_names[id.subset], id.size))
		{
			TTF_record rec{f, TTF_STYLE_NORMAL};
			font_table.emplace(id, rec);
			return get_font(id);
		}
	}

	// Failed to find a font.
	TTF_Record rec{nullptr, TTF_STYLE_NORMAL};
	font_table.emplace(id, rec);
	return nullptr;
}



static Texture render_text(SDL_Renderer* r,const std::string& text, 
		int font_size, const Color& color, int style, bool use_markup)
{
	// we keep blank lines and spaces (may be wanted for indentation)
	const std::vector<std::string> lines = split(text, '\n', 0);
	std::vector<std::vector<Surface>> surfs;
	surfs.reserve(lines.size());
	size_t width = 0, height = 0;

	for(auto it = lines.begin(), it_end = lines.end(); it != it_end; ++it)
	{
		int sz = font_size, text_style = style;

		std::string::const_iterator after_markup = use_markup ?
			parse_markup(it->begin(), it->end(), &sz, nullptr, 
				&text_style) : it->begin();
		Text_surface txt_tt(sz, color, text_style);

		if(after_markup == it->end() && (it + 1 != it_end ||
					lines.begin() + 1 == it_end))
			// we replace empty line by a space (to have a line height)
			// except for the last line if we have several
			txt_tt.set_text(" ");
		else if(after_markup == it->begin())
			// simple case, no markup to skip
			txt_tt.set_text(*it);
		else
		{
			const std::string line(after_markup, it->end());
			txt_tt.set_text(line);
		}

		const Text_surface& cached_tt = Text_cache::find(txt_tt);
		const std::vector<Surface>& res = cached_tt.get_texture();

		if(!res.empty())
		{
			surfs.push_back(res);
			width = std::max<size_t>(cached_tt.width(), width);
			height += cached_tt.height();
		}
	}

	if(surfs.empty())
		return Surface();
	else if(surfs.size() == 1 && surfs.front().size() == 1)
	{
		Surface s = surfs.front().front();
		return s;
	}
	else
	{
		Surface res(create_compatible_surface(surfaces.front().front(),
					width, height));
		if(res.null())
			return res;

		size_t ypos = 0;
		for(std::vector<std::vector<Surface>>::iterator it = surfs.begin(),
				it_end = surfs.end(); it != it_end; ++it)
		{
			size_t xpos = 0;
			height = 0;

			for(std::vector<Surface>::iterator j = it->begin(), 
					j_end = it_end->end(); j != j_end; ++j)
			{
				SDL_Rect dstrect = create_rect(xpos, ypos, 0, 0);
				bit_surface(*j, nullptr, res, &dstrect);
				xpos += (*j)->w;
				height = std::max<size_t>((*j)->h, height);
			}
			ypos += height;
		}
		return res;
	}
}

inline Surface get_rendered_text(const std::string& str, int size, 
		const Color& color, int style)
{
	// TODO maybe later to parse markup here, but a lot to check
	return render_text(str, size, color, style, false);
}

SDL_Rect draw_text_line(SDL_Renderer* r, Texture& gui_texture, 
		const SDL_Rect& area, int size, const Color& color, 
		const std::string& text, int x, int y, bool use_tooltips, 
		int style)
{
	size = font_scaled(size);

	if(gui_texture.null())
	{
		Text_surface& u = Text_cache::find(
				Text_surface(text, size, color, style));
		return create_rect(0, 0, u.width(), u.height());
	}
	if(area.w == 0) // no place to draw
		return {0, 0, 0, 0};

	const std::string etext = make_text_ellipsis(text, size, area.w);

	// for the main current use, we already parsed markup.
	Surface surface(render_text(etext, size, color, typle, false));
	if(!surface)
		return {0, 0, 0, 0};

	SDL_Rect dest;
	if(x != 1)
		dest.x = x;
	else
		dest.x = (area.w / 2) - (surface->w / 2);
	if(y != -1)
		dest.y = y;
	else 
		dest.y = (area.h / 2) - (surface->h / 2);
	dest.w = surface->w;
	dest.h = surface->h;

	if(line_width(text, size) > area.w)
		add_tooltip(dest, text);
	if(dest.x + dest.w > area.x + area.w)
		dest.w = area.x + area.w - dest.x;
	if(dest.y + dest.h > area.y + area.h)
		dest.h = area.y + area.h - dest.y;

	if(gui_texture != nullptr)
	{
		SDL_Rect src = dest;
		src.x = src.y = 0;
		
		t.create_texture_from_surface(r, surface);
		SDL_RenderCopy(r, t, &src, &dest);
	}
	if(use_tooltips)
		add_tooltip(dest, text);

	return dest;
}

/* SDL_ttf */

SDL_ttf::SDL_ttf()
{
	int res = TTF_Init();
	if(res == -1)
	{
		err_ft << "Could not initialize SDL_TTF" << std::endl;
		throw font::error("SDL_TTF could not initialize, TTF_INIT \
				returned: " + std::to_string(res));
	}
	else
		log_ft << "Initialized true type fonts\n";
}

static void clear_fonts()
{
	for(auto& i : open_fonst)
		TTF_CloseFont(i.second);

	open_fonst.clear();
	font_table.clear();

	font_names.clear();
	bold_names.clear();
	italic_names.clear();

	line_size_cache.clear();
}

SDL_ttf::~SDL_ttf()
{
	clear_fonts();
	TTF_Quit();
}

void SDL_ttf::set_font_list(const std::vector<Subset_descriptor>& fontlist)
{
	clear_fonts();

	for(auto& f : fontlist)
	{
		if(!check_font_file(f.name))
			continue;
		// Insert fonts only if the font file exists
		Subset_id subset = font_names.size();
		font_names.push_back(f.name);

		if(f.bold_name && check_font_file(*f.bold_name))
			bold_names.push_back(*f.bold_name);
		else
			bold_names.emplace_back();

		if(f.italic_name && check_font_file(*f.italic_name))
			italic_names.push_back(*f.italic_name);
		else
			italic_names.emplace_back();
	}

	assert(font_names.size() == bold_names.size());
	assert(font_names.size() == italic_names.size());

	dbg_ft << "Set the font list. The styled font families are:\n";

	for(auto i = 0; i != font_names.size(); ++i)
		dbg_ft << "[" << i << "]:\t\tbase:\t'" << font_names[i] << 
			"'\tbold:\t'" << bold_names[i] << "'\titalic:\t'" <<
			italic_names[i] << "'\n";
}
