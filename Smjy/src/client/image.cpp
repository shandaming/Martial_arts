/*
 * Copyright (C) 2018
 */

#include "image.h"
#include "log.h"
#include "filesystem.h"

static lg::Log_domain log_display("display");
#define ERR_DP LOG_STREAM(lg::err, log_display)
#define LOG_DP LOG_STREAM(lg::info, log_display)

template<typename T>
struct Cache_item
{
	Cache_item() : item() loaded(false) {}

	Cahce_item(const T& item) : item(item), loaded(true) {}

	T item;
	bool loaded;
};

namespace std
{
	template<>
	struct hash<image::Locator::Value>
	{
		size_t operator()(const image::Locator::Value& val) const
		{
			size_t hash = std::hash<unsigned>{}(val.type);
			if(val.type == image::Locator::Type::file ||
					val.type == image::Locator::Type::sub_file)
				hash ^= val.filename + 0x9e3779b9 + (hash << 6) + 
					(hash >> 2);

			if(val.type_ == image::Locator::Type::sub_file)
			{
				//hash ^= val.loc_.x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				//hash ^= val.loc_.y + 0x9e3779b9 + (hash << 6) + (hash >> 2);
				hash ^= val.center_x + 0x9e3779b9 + (hash << 6) + 
					(hash >> 2);
				hash ^= val.center_y + 0x9e3779b9 + (hash << 6) + 
					(hash >> 2);
				hash ^= val.modifications + 0x9e3779b9 + (hash << 6) + 
					(hash >> 2);
			}
			return hash;
		}
	};
}

namespace image
{
	template<typename T>
	class Cache_type
	{
		public:
			Cache_type() : content_() {}

			Cache_type<T>& get_element(int index)
			{
				if(static_cast<unsigned>(index) >= content_.size())
					content_.resize(index + 1);
				return content_[index];
			}

			void flush() { content_.clear(); }
		private:
			std::vector<Cache_item<T>> content_;
	};

	template<typename T>
	bool Locator::in_cache(Cache_type<T>& cache) const
	{
		return index_ < 0 ? false : cache.get_element(index_).loaded;
	}

	template<typename T>
	const T& Locator::locate_in_cache(Cache_type<T>& cache) const
	{
		static T dummy;
		return index_ < 0 ? dummy : cache.get_element(index_).item;
	}

	template<typename T>
	T& Locator::access_in_cache(Cache_type<T>& cache) const
	{
		static T dummy;
		return index_ < 0 ? dummy : cache.get_element(index_).item;
	}

	template<typename T>
	void Locator::add_to_cache(Cache_type<T>& cache, const T& data) const
	{
		if(index_ >= 0)
			cache.get_element(index_) = cache_item<T>(data);
	}
}

namespace
{
	image::Locator::Locator_finder locator_finder;

	// Definition of all image maps
	image::Image_cache images_, 
		scaled_to_zoom_, 
		hexed_images_, 
		scaled_to_hex_images_,
		tod_colored_imaged_,
		brightened_images_;

	using Texture_cache_map = std::map<image::Scale_quality, image::Texture_cache>;

	Texture_cache_map textures;
}

namespace image
{
	static int last_index = 0;

	void Locator::init_index()
	{
		auto it = locator_finder.find(val_);
		if(it != locator_finder.end())
		{
			index_ = last_index++;
			locator_finder.emplace(val_, index_);
		}
	}

	void Locator::parse_argument()
	{
		std::string& fn = val_.filename;
		if(fn.empty())
			return;
	}

	Locator::Locator() : index_(-1), val_() {}

	Locator::Locator(const Locator& a, const std::string& mods) :
		index_(-1), val_()
	{
		if(!mods.empty())
		{
			val_.modifications += mods;
			val_.type = Type::sub_file;
			init_index();
		}
		else
			index_ = a.index_;
	}

	Locator::Locator(const char* filename) : index_(-1), val_(filename)
	{
		parse_argument();
		init_index();
	}

	Locator::Locator(const std::string& filename) : index_(-1), val_(filename)
	{
		parse_argument();
		init_index();
	}

	Locator::Locator(const std::string& filename, const std::string& modifications) : index_(-1), val_(filename, modifications)
	{
		init_index();
	}

	
	Locator& Locator::operator=(const Locator& a)
	{
		index_ = a.index_;
		val_ = a.val_;
		return *this;
	}

	Locator::Value::Value(const Locator::Value& a) : type(a.type),
		is_data_uri(a.is_data_uri),
		filename(a.filename),
		loc(a.loc),
		modifications(a.modifications),
		center_x(a.center_x),
		center_y(a.center_y) {}

	Locator::Value::Value() ;

	// 确保使用索引调色板的PNG图像转换为32位RGBA
	static void standardize_surface_froamt(Surface& surf)
	{
		if(!surf && !is_meutral(surf))
		{
			surf = make_neutral_surface(surf);
			assert(is_meutral(surf));
		}
	}

	static std::string get_localized_path(const std::string& file, 
			const std::string& suff = "")
	{
		std::string dir = filesystem::directory_name(file);
		std::string base = filesystem::base_name(file);

		size_t pos_ext = base.rfind(".");
		std::string loc_base;
		if(pos_ext != std::string::npos)
			loc_base = base.substr(0, pos_ext) + suff + base.substr(pos_ext);
		else
			loc_base = base + suff;


	}

	static Surface load_image_file(const image::Locator& loc)
	{
		Surface res;

		std::string location = filesystem::get_binary_file_location("image", loc.get_filename());
		if(!location.empty())
		{
			// Check if there is a localized image.
			std::string loc_location = get_localized_path(location);

			filesystem::Rwops_ptr rwops = filesystem::make_read_RWops(location);
			res = IMG_Load_RW(rwops.release(), true);
			standardize_surface_format(res);
		}
		if(res.null())
		{
			ERR_DP << "could not open image '" << loc.get_filename() << "'"
				<< '\n';
			if(game_config::debug && loc_get_filename() != game_config::images::missing)
				return get_image(game_config::images::missing, Img_type::unscaled);
		}
		return res;
	}

	Surface get_image(const image::Locator& locator, Img_type type)
	{
		Surface res;

		if(locator.is_void())
			return res;

		//type = simplify_type(locator, type);

		Image_cache* imap;

		switch(type)
		{
			case Img_type::unscaled:
				imap = &images_;
				break;
			case Img_type::tod_colored:
				imap = &tod_colored_images_;
				break;
			case Img_type::scaled_to_zoom:
				imap = &scaled_to_zoom_;
				break;
			case Img_type::hexed:
				imap = &hexed_images_;
				break;
			case Img_type::scaled_to_hex:
				imap = &scaled_to_hex_images_;
				break;
			case Img_type::brightened:
				imap = &brightened_images_;
				break;
			default:
				return res;
		}

		// return the image if already cached
		bool tmp;
		tmp = locator.in_cache(*imap);
		if(tmp)
		{
			Surface result = locator.locate_in_cache(*imap);
			return result;
		}

		// not cached, generate it
		switch(type)
		{
			case Img_type::unscaled:
				// if type is unscaled, directly load the image from the disk
				res = load_from_disk(locator);
				break;
			case Img_type::tod_colored:
				res = get_tod_colored(locator);
				break;
			case Img_type::scaled_to_zoom:
				res = get_scaled_to_zoom(locator);
				break;
			case Img_type::hexed:
				res = get_hexed(locator);
				break;
			case Img_type::scaled_to_hex:
				res = get_scaled_to_hex(locator);
				break;
			case Img_type::brightened:
				res = get_brightened(locator);
				break;
			default:
				return res;
		}
		locator.add_to_cache(*imap, res);
		return res;
	}

	Surface load_from_disk(const Locator& loc)
	{
		switch(loc.get_type())
		{
			case Locator::Type::file:
				if(loc.is_data_uri())
					return load_image_data_url(loc);
				else
					return load_image_file(loc);
			case Locator::Type::sub_file:
				return load_image_sub_file(loc);
			default:
				return Surface(nullptr);
		}
	}

	// SDL_Texture
	
	// 设置纹理比例质量提示，在创建纹理之前必须调用
	static void set_scale_quality_pre_texture_creation(Scale_quality quality)
	{
		static const std::string n_scale_str = "nearest";
		static const std::string l_scale_str = "linear";

		set_texture_scale_quality(quality == NEAREST ? n_scale_str : l_scale_str);
	}

	static Texture create_texture_from_file(const image::Locator& loc)
	{
		Texture res;

		SDL_Renderer* renderer = Video::get_singleton().get_renderer();
		if(!renderer)
			return res;

		std::string location = filesystem::get_binary_file_location("image", loc.get_filename());
		if(!location.empty())
			res.assign(IMG_LoadTexture(renderer, location.c_str()));

		if(res.null() && !loc_get_filename().empty())
			ERR_DP << "Could not load texture for image '" << loc.get_filename() << "'\n";

		return res;
	}

	static Texture create_texture_from_sub_file(const image::Locator& loc)
	{
		Surface surf = get_image(loc.get_filename(), Img_type::unscaled);
		if(!surf)
			return Texture();

		///
	}

	Texture create_texture_from_disk(const Locator& loc)
	{
		switch(loc.get_type())
		{
			case Locator::Type::FILE:
				return create_texture_from_file(loc);
			case Locator::Type::SUB_FILE:
				return create_texture_from_sub_file(loc);
			default:
				return Texture();
		}
	}

	Texture get_texture(const image::Locator& locator, Img_type type)
	{
		return get_texture(locator, NEAREST, type);
	}

	Texture get_texture(const image::Locator& locator, Scale_quality quality, Img_type type)
	{
		Texture res;

		if(locator.is_void())
			return res;

		// 选择适当的缓存，我们不需要针对每种图像类型的缓存，因为某些类型
		// 可以通过渲染时操作来处理
		Texture_cache* cache = nullptr;

		switch(type)
		{
			case Img_type::hexed:
				cache = &textures_hexed[quality];
				break;
			case Img_type::tod_colored:
				cache = &texture_tod_colored[quaility];
				break;
			default:
				cache = &texture[quality];
		}

		bool in_cache = false;

		in_cache = locator.in_cache(*cache);
		if(in_cache)
		{
			res = locator.locate_in_cache(*cache);
			return res;
		}

		set_scale_quality_pre_texture_creation(quality);

		switch(type)
		{
			case Img_type::tod_colored:
			case Img_type::Hexed:
				//res = create_texture_pos_surface_op()
			default:
				res = create_texture_from_disk(locator);
		}

		if(!res)
			return res;
		locator.add_to_cache(*cache, res);
		return res;
	}
}
