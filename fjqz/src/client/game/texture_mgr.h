//
// Created by mylw on 22-9-17.
//

#ifndef FJQZ_TEXTURE_MGR_H
#define FJQZ_TEXTURE_MGR_H

#include <string_view>

class texture_mgr
{
public:
	static texture_mgr* instance();

	void load_mmap_textures();
private:
	texture_mgr() {}
	~texture_mgr() {}
	texture_mgr(const texture_mgr&) = delete;
	texture_mgr& operator=(const texture_mgr&) = delete;
};

#endif //FJQZ_TEXTURE_MGR_H
