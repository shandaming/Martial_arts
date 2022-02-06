#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdint>

struct SDL_Texture;

class texture
{
public:
    explicit texture(SDL_Texture* t);
    ~texture();

	texture(const texture&) = delete;
	texture& operator=(const SDL_Texture* r);

    void set_alpha_mod(uint8_t a);
    void set_blend_mod();
    void set_color_mod(uint8_t r, uint8_t b, uint8_t g);

	struct info
	{
		explicit info(SDL_texture* t);

		uint32_t format;
		int access;
		int w;
		int h;
	};

	const info get_info() const { return info(*this); }

	operator SDL_Texture*() const { return texture_; }

	bool null() const { return texture_ == nullptr; }
private:
	const SDL_Texture* texture_;
};

SDL_Texture* make_texture(int w, int h);

#endif
