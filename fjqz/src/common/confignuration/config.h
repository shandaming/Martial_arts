//
// Created by mylw on 2022/4/30.
//

#ifndef FJQZ_CONFIG_H
#define FJQZ_CONFIG_H

#include <string>

class config_mgr
{
public:
	static config_mgr* instance();

	bool load_init(const std::string& file);
	void save_json();

	int get_window_value(const std::string& key);
	void set_window_value(const std::string& key, int value);
private:
	config_mgr(){}
	~config_mgr() { save_json(); }
	config_mgr(const config_mgr&) = delete;
	config_mgr& operator=(const config_mgr&) = delete;

	template<typename T>
	T get_value_default(const std::string& name, T def) const;
};

#define CONFIG_MGR config_mgr::instance()

#endif //FJQZ_CONFIG_H
