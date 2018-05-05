/*
 * Copyright (C) 2018
 */

#ifndef IMAGE_H
#define IMAGE_H

namespace image
{
	template<typename T>
	class Cache_type;

	// a generic image locator. Abstracts the location of an image.
	class Locator
	{
		public:
			enum class Type
			{
				none,
				file,
				sub_file
			};
		private:
			// called by each constructor after actual construction to 
			// initialize the index_field.
			void init_index();
			void parse_arguments();

			struct Value
			{
				Value();
				Value(const Value&);
				Value(const char* filename);
				Value(const std::string& filename);
				Value(const std::string& filename, 
						const std::string& modifications);

				bool operator==(const Value& a) const;
				bool operator!=(const Value& a) const;

				Type type;
				bool is_data_url;
				std::string filename;
				std::string modifications;
				int center_x;
				int center_y;
			};

			friend struct std::hash<Value>;
		public:
			typedef std::unordered_map<Value, int> Locator_finder;

			/*
			 * Constructing locators is somewhat slow, accessing image
			 * throught locators is fast. The idea is that calling functions
			 * should store locators, and not strings to construct locators
			 * (the second will work, of course, but will be slower)
			 */
			Locator();
			Locator(const Locator& a, const std::string& mods = "");
			Locator(const char* filename);
			Locator(const std::string& filename);
			Locator(const std::string& filename, 
					const std::string& modifications);

			Locator& operator=(const Locator& a);
			bool operator==(const Locator& a) const 
			{
				return index_ == a.index_; 
			}

			bool operator!=(const Locator& a) const 
			{
				return index_ != a.index_;
			}

			bool operator<(const Locator& a) const
			{
				return index_ < a.index_;
			}

			const std::string& get_filename() const 
			{
				return val_filename_; 
			}

			bool is_data_url() const { return val_.is_data_url_; }
			int get_center_x() const { return val_.center_x_; }
			int get_center_y() const { return val_.center_y_; }

			const std::string& get_modifications() const 
			{
				return val_.modifications_;
			}

			Type get_type() const { return val_.type_; }

			// returns true if the locator does not correspond to any 
			// actual image
			bool is_void() const { return val_.type_ == Type;;NONE; }

			/*
			 * Tests whether the file the locater points at exists.
			 *
			 * is_void doen't seem to work before the image is loaded and 
			 * also in debug mode a placeholder is returned. So it;s not
			 * possible to test for the existence of a file. So this 
			 * function does that. (Note it tests for existence not whether
			 * or not it's a valid image.)
			 */
			bool file_exits() const;

			template<typename T>
			bool in_cache(Cache_type<T>& cache) const;
			template<typename T>
			T& access_in_cache(Cache_type<T>& cache) const;
			template<typename T>
			const T& locate_in_cache(Cache_type<T>& cache) const;
			template<typename T>
			void add_to_cache(Cache_type<T>& cache, const T& data) const;
		private:
			int index_;
			Value val_;
	};

	Surface load_from_disk(const Locator& loc);
	Texture create_texture_from_disk(const Locator& loc);

	typedef Cache_type<Surface> Image_cache;
	typedef Cache_type<Texture> Texture_cache;
	typedef Cache_type<bool> Bool_cache;

	enum Img_type{unscaled, scaled_to_zoom, hexed, scaled_to_hex, tod_colored, brightened};

	enum Scale_quality {NEAREST, LINEAR};

	Surface get_image(const Locator& locator, Img_type = Img_type::unscaled);

	Texture get_texture(const image::Locator& locator, Img_type type = Image_type::unscaled);
	Textire get_textire(const image::Locator& locator, Scale_quality quality, Img_type type = Img_type::unscaled);
}

#endif
