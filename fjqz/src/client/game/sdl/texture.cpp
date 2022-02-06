#include <SDL2/SDL_render.h>
#include "texture.h"
#include "video.h"

texture::texture(SDL_Texture* t) : texture_(t)
{
}

texture::~texture()
{
    if(texture_)
        SDL_DestroyTexture(texture_);
}

texture& texture::operator=(const SDL_Texture* r)
{
    texture_ = r;
    return *this;
}

void texture::set_alpha_mod(uint8_t a)
{
    SDL_SetTextureAlphaMod(texture_, a);
}

void texture::set_blend_mod()
{
	SDL_SetTextureBlendMod(texture_, SDL_BLENDMODE_BLEND);
}

void texture::set_color_mod(uint8_t r, uint8_t b, uint8_t g)
{
    SDL_SetTextureColorMod(texture_, r, b, g);
}

texture::info::info(SDL_texture* t) : format(0), access(0), w(0), h(0)
{
	SDL_QueryTexture(t, &format, &w, &h);
}

SDL_Texture* make_texture(int w, int h)
{
    SDL_Renderer* r = VIDEO->get_renderer();
    return (r? SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h) : nullptr);
}
