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

	std::string get_string_default(const std::string& name, const std::string& def) const;
	bool get_bool_default(const std::string& name, bool def) const;
	int get_int_default(const std::string& name, int def) const;
	float get_float_default(const std::string& name, float def) const;

private:
	config_mgr(){}
	config_mgr(const config_mgr&) = delete;
	config_mgr& operator=(const config_mgr&) = delete;

	template<typename T>
	T get_value_default(const std::string& name, T def) const;
};

#define CONFIG_MGR config_mgr::instance()

#endif //FJQZ_CONFIG_H
