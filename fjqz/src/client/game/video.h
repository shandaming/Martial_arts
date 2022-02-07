#ifndef _GAME_VIDEO_H
#define _GAME_VIDEO_H

//#include "events.h"
#include <SDL2/SDL.h>

#include <memory>

struct point;
class window;

class video
{
public:
	static video* instance();

	void init_window();
	window* get_window() const;
	void set_fullscreen(bool ison);

	SDL_Rect screen_area() const;

	int get_width() const;
	int get_height() const;


	int current_refresh_rate() const
	{
		return refresh_rate_;
	}


	void flip();

	void clear_screen();
	static void delay(unsigned int milliseconds);

private:
	video();
	~video();
	video(const CVideo&) = delete;
	video& operator=(const CVideo&) = delete;

	void initSDL();

#if 0
	class video_event_handler : public sdl_handler
	{
	public:
		video_event_handler() : sdl_handler(false) {}

		void handle_event(const SDL_Event&) override {}
		virtual void handle_window_event(const SDL_Event& event);
	};

	video_event_handler event_handler_;
#endif
	int refresh_rate_;
	std::unique_ptr<window> window_;
};

#define VIDEO video::instance()

#endif
