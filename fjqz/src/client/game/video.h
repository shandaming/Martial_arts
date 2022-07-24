#ifndef _GAME_VIDEO_H
#define _GAME_VIDEO_H

#include "events.h"

#include <memory>

struct SDL_Renderer;
struct point;
class window;
class texture;

class video
{
public:
	static video* instance();

	void init_window();
	window* get_window();
	void set_fullscreen(bool ison);
	bool set_resolution(const point& resolution);

	SDL_Rect screen_area() const;

	void update_framebuffer();

	int get_width() const;
	int get_height() const;

	int current_refresh_rate() const
	{
		return refresh_rate_;
	}

	void flip();

	void clear_screen();

	bool is_fullscreen() const;
	point current_resolution();

	texture& get_texture() const;
	SDL_Renderer* get_renderer();

	static void delay(unsigned int milliseconds);

private:
	video();
	~video();
	video(const video&) = delete;
	video& operator=(const video&) = delete;

	void initSDL();

	enum MODE_EVENT { TO_RES, TO_FULLSCREEN, TO_WINDOWED, TO_MAXIMIZED_WINDOW };
	void set_window_mode(const MODE_EVENT mode, const point& size);

	class video_event_handler : public events::sdl_handler
	{
	public:
		video_event_handler() : sdl_handler(false) {}

		void handle_event(const SDL_Event&) override {}
		void handle_window_event(const SDL_Event& event) override;
	};

	video_event_handler event_handler_;

	int refresh_rate_;
	std::unique_ptr<window> window_;
};

#define VIDEO video::instance()

#endif
