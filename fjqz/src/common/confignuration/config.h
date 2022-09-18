//
// Created by mylw on 2022/4/30.
//

#ifndef FJQZ_CONFIG_H
#define FJQZ_CONFIG_H

#include <string_view>

class config_mgr
{
public:
	static config_mgr* instance();

	bool load_init(std::string_view& file);
	void save_json();

	int get_window_value(std::string_view& key);
	void set_window_value(std::string_view& key, int value);

	std::string get_resource_path(std::string_view& key);
private:
	config_mgr(){}
	~config_mgr() { save_json(); }
	config_mgr(const config_mgr&) = delete;
	config_mgr& operator=(const config_mgr&) = delete;

	template<typename T>
	T get_value_default(std::string_view& name, T def) const;
};

#define CONFIG_MGR config_mgr::instance()

#endif //FJQZ_CONFIG_H
