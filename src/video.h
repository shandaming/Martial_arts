/*
 * Copyright (C) 2017 by Shan Daming <shandaming@hotmail.com>
 */

#ifndef VIDEO_H
#define VIDEO_H

#include "sdl/window.h"

class Video
{
	public:
		enum class FAKE_TYPES
		{
			NO_FAKE,
			FAKE,
			FAKE_TEST
		};
		enum class MODE_EVENT
		{
			TO_RES,
			TO_FULLSCREEN,
			TO_WINDOWED,
			TO_MAXIMIZED_WINDOW
		};

		explicit Video( FAKE_TYPES type = FAKE_TYPES::NO_FAKE);
		~Video();

		Video(const Video&) = delete;
		Video& operator=(const Video&) = delete;

		static Video& get_singletion() { return *singleton_; }

		bool non_interactive() const;

		/*
		 * Initializes a new window, taking into account any preiously
		 * saved states.
		 */
		void init_window();

		void set_mode(int x, int y, Mode_EVENT mode); // ??

		void set_fullscreen(bool ison);

		/*
		 * Set the resolution.
		 *
		 * @return             The status true if width and height are the 
		 *                     size of the framebuffer, false otherwise.
		 */
		void set_resolution(const std::pair<int, int>& res);
		void set_resolution(unsigned int ,width, unsigned int height);

		std::pair<int, int> current_resolution();

		/*
		 *  Functions to get the dimensions of the current video-mode
		 */
		int getx() const;
		int gety() const;

		std::pair<float, float> get_dpi_scale_factor() const;

		/*
		 * blits a texture with black as alpha
		 */
		void blit_texture(int x, int y, Texture& t, SDL_Rect* src_rect =
				nullptr, SDL_Rect* clip_rect = nullptr);
		void flip();

		static void delay(unsigned int milliseconds);

		Texture& get_texture();

		bool is_fullscreen() const;

		/*
		 * Functions to allow changing video modes when 16BPP is emulated
		 */
		void make_fake();

		/*
		 * Creates a fake frame buffer for the unit tests.
		 *
		 * @return                        The bpp of the buffer.
		 */
		void make_test_fake(unsigned int width = 1024, unsigned int height
				= 268);
		bool faked() const { return fake_screen_; }

		/*
		 * Functions to set and clear 'help strings'. A 'help strings' is
		 * like a tooltip, but it appears at the bottom of the screen, so
		 * as to not be intrusive. Setting a help string sets what is 
		 * currently displayed there.
		 */
		int set_help_string(const std::string& str);
		void clear_help_string(int handle);
		void clear_all_help_string();

		/*
		 * function to stop the screen being redrawn. Anything that happens
		 * while the update is locked will be hidden from the user's view.
		 * note that this function is re-entrant, meaning that if 
		 * lock_updates(true) is called twice, lock_updates(false) must be
		 * called twice to unlock updates.
		 */
		void lock_updates(bool value);
		bool update_locked() const;

		/*
		 * This needs to be invoked immediately after a resize event or the
		 * game will crash.
		 */
		void update_framebuffer();

		void set_window_title(const std::string& title);

		void set_window_icon(SDL_Surface* icon);

		void clear_screen();

		Window* get_window();

		/*
		 * Returns the list of available screen resolutions.
		 */
		std::vector<std::pair<int ,int>> get_available_resolutions(
				bool include_current = false);

		void lock_filps(bool);
	private:
		void init_sdl();

		std::unique_ptr<Window> window;

		static Video* singleton_;

		// if ther is no display at all, but we 'fake' it for clients
		bool fake_screen_;

		// variables for help strings.
		int help_string_;

		int updates_locked_;
		int flip_locked_;
};

#endif
