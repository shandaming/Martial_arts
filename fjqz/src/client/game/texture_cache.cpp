//
// Created by mylw on 22-9-18.
//

#include "texture_cache.h"

texture_cache::texture_cache()
{}

texture_cache::texture_cache(texture_cache&& l):
	cache_(std::move(l.cache_))
{
	l.cache_.clear();
}

texture_cache& texture_cache::operator=(texture_cache&& l)
{
	if(&l != this)
	{
		cache_ = std::move(l.cache_);
		l.cache_.clear();
	}
	return *this;
}

void texture_cache::add(const std::string& key, texture&& t)
{
	cache_.emplace(key, std::forward<texture>(t));
}
void texture_cache::remove(const std::string& key)
{
	cache_.erase(key);
}

texture* texture_cache::get_texture(auto& key) const
{
	auto iter = cache_.find(key);
	if(iter != cache_.end())
		return &iter->second;
	return nullptr;
}

std::vector<texture*> texture_cache::get_all_textures()
{
	std::vector<texture*> result;
	for(auto& iter : cache_)
	{
		result.push_back(&iter.second);
	}
}