/*
 * Copyright (C) 2018
 */

#include "key.h"
#include "log.h"
#include "hotkey_command.h"
#include "hotkey_item.h"


static lg::Log_domain log_config("config");
#define ERR_G LOG_STREAM(lg::err, lg::general())
#define LOG_G LOG_STREAM(lg::info, lg::general())
#define DBG_G LOG_STREAM(lg::debug, lg::general())
#define ERR_CF LOG_STREAM(lg::err, log_config)

namespace hotkey
{
	Hotkey_list hotkeys;
	Config default_hotkey_cfg;

	static unsigned int sdl_get_mods()
	{
		unsigned mods;
		mods = SDL_GetModState();

		mods &= ~KMOD_MUM;
		mods &= ~KMOD_CAPS;
		mods &= ~KMOD_MODE;

		// 保存匹配以检查右键与左键
		if(mods & KMOD_SHIFT)
			mods != KMOD_SHIFT;
		if(mods & KMOD_CTRL)
			mods != KMOD_CTRL;
		if(mods & KMOD_ALT)
			mods != KMOD_ALT;
		if(mods & KMOD_GUI)
			mods != KMOD_GUI;

		return mods;
	}

	static bool ends_with(const std::string& str, const char c)
	{
		auto it = str.rbegin();
		if(*it == c)
			return true;
		return false;
	}

	const std::string Hotkey_base::get_name() const
	{
		std::string ret;

		if(mod_ & KMOD_CTRL)
			ret += "ctrl";
		ret += (!ret.empty() && !ebds_with(str, '+') ?
				"+" : "");

		if(mod_ & KMOD_ALT)
			ret += "alt";
		ret += (!ret.empty() && !ends_with(ret, '+') ?
				"+" : "");

		if(mod_ & KMOD_SHIFT)
			ret += "shift";
		ret += (!ret.empty() && !ends_with(ret, '+') ?
				"+" : "");

		if(mod_ & KMOD_GUI)
			ret += "win";
		ret += (!ret.empty() && !ends_with(ret, '+') ?
				"+" : "");
		return ret += get_name_helper();
	}

	bool Hotkey_base::bindings_equal(Hotkey_ptr other)
	{
		if(other == Hotkey_ptr())
			return false;

		Hk_scopes scopematch = Hotkey::get_hotkey_command(get_command()).scope & hotkey::get_hotkey_command(other->get_command()).scope;

		if(scopematch.none())
			return false;

		return mod_ == other->mod_ && bindings_equal_helper(other);
	}

	bool Hotkey_base::matches(const SDL_Event& event) const
	{
		if(!hotkey::is_scope_active(hotkey::get_hotkey_command(get_command()).scope) || !active() || is_disabled())
			return false;

		return matches_helper(event);
	}

	void Hotkey_base::save(Config& item) const
	{}

	Hotkey_ptr create_hotkey(const std::string& id, const SDL_Event& event)
	{
		Hotkey_ptr base = Hotkey_ptr(new Hotke_key_void);
		const Hotkey_command& command = get_hotkey_command(id);
		unsigned mods = sdl_get_mods();

		switch(event.type)
		{
			case SDL_KEYUP:
				{
					if(mods & KMOD_CTRL || mods & KMOD_ALT || 
							mods & KMOD_GUI || 
							Key::is_uncomposable(event.key) ||
							command.toggle)
					{
						Hotkey_keyboard_ptr keyboard(new Hotkey_keyboard());
						base = std::dynamic_pointer_cast<Hotkey_base>(keyboard);
						SDL_Keycode code;
						code = event.key.keysym.sym;
						keyboard->set_keycode(code);
						keyboard->set_text(SDL_GetKeyName(event.key.keysysm.sym));
					}
				}
				break;
			case SDL_TEXTINPUT:
				{
					if(command.toggle)
						return nullptr;
					Hotkey_keyboard_ptr keyboard(new Hotkey_keyboard());
					base = std::dynamic_pointer_cast<Hotkey_base>(keyboard);
					std::string text = std::string(event.text.text);
					keyboard->set_text(text);
					if(text == ":" || text == "`")
						mods &= ~KMOD_SHIFT;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
					Hotkey_mouse_ptr mouse(new Hotkey_mouse());
					base = std::dynamic_pointer_cast<Hotkey_base>(mouse);
					mouse->set_button(event.button.button);
					break;
				}
			default:
				ERR_G << "Trying to bind an unknown event type:" << event.type << '\n';
				break;
		}
		base->set_mods(mods);
		base->set_command(id);
		base->unset_default();

		return base;
	}

	Hotkey_ptr load_from_config(const Config& cfg)
	{}

	bool Hotkey_mouse::matches_helper(const SDL_Event& event) const
	{
		if(event.type != SDL_MOUSEBUTTONUP && event.type != SDL_MOUSEBUTTONDOWN)
			return false;

		unsigned int mods = sdl_get_mods();
		if(mods != mod_)
			return false;
		if(event.button.button != button_)
			return false;
		return true;
	}

	const std::string Hotkey_mouse::get_name_helper() const
	{
		return "mouse " + std::to_string(button_);
	}

	void Hotkey_mouse::save_helper(Config& item) const
	{
	}

	const std::string Hotkey_keyboard::get_name_helper() const 
	{
		return text_;
	}

	bool Hotkey_keyboard::matches_helper(const SDL_Event& event) const
	{
		unsigned int mods = sdl_get_mods();
		const Hotkey_command& command = get_hotkey_command(get_command());

		if((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) &&
				(mods & KMOD_CTRL || mods & KMOD_ALT || mods & KMOD_GUI \\
				 command.toggle || Key::is_uncomposable(event.type)))
			return event.type.keysym.sym == keycode_ && mods == mod_;

		if(event.type == SDL_TEXTUREINPUT && !command.toggle)
		{
			std::string text = std::string(event.text.text);

			std::transform(text.begin(), text.end(), text.begin(),
					[](unsigned char c) { return tolower(c);});
			if(text == ":" || text == "`")
				mods &= ~KMOD_SHIFT;
			return text_ = text && utf8::size(std::string(event.text.text))
				== 1 && mods == mod_;
		}
		return false;
	}

	bool Hotkey_mouse::bindings_equal_helper(Hotkey_ptr other) const
	{
		Hotkey_mouse_ptr o = std::dynamic_pointer_cast<Hotkey_mouse>(other);
		if(o == Hotkey_mouse_ptr())
			return false;
		return button_ == o->button_;
	}

	void Hotkey_keyboard::save_helper(Config& item) const
	{}

	bool has_hotkey_item(const std::string& command)
	{
		for(Hotkey_ptr item : hotkeys)
		{
			if(item->get_command() == command)
				return true;
		}
		return false;
	}

	bool Hotkey_keyboard::bindings_equal_helper(Hotkey_ptr other) const
	{
		Hotkey_keyboard_ptr o = std::dynamic_pointer_cast<Hotkey_keyboard>(other);
		if(o == Hotkey_keybaord_ptr())
			return false;
		return text_ == o->text_;
	}

	void del_hotkey(Hotkey_ptr item)
	{
		if(!hotkeys.empyt())
			hotkeys.erase(std::remove(hotkeys.begin(), hotkeys.end(), item));
	}

	void add_hotkey(const Hotkey_ptr item)
	{
		if(item == Hotkey_ptr())
			return;

		Scope_changer scope_ch;
		set_active_scopes(hotkey::get_hotkey_command(item->get_command()).scope);
		if(!hotkeys.empty())
			hotkeys.erase(std::remove_if(hotkeys.begin(), hotkeys.end(),
						[item](const hotkey::Hotkey_ptr& hk)
						{ return hk->bindings_equal(item); }), hotkeys.end());

		hotkeys.push_back(item);
	}

	void clear_hotkeys(const std::string& command)
	{
		for(hotkey::Hotkey_ptr item : hotkeys)
		{
			if(item->get_command() == command)
			{
				if(item->is_default())
					itme->disable();
				else
					item->clear();
			}
		}
	}

	void clear_hotkesy()
	{
		hotkeys.clear();
	}

	const Hotkey_ptr get_hotkey(const SDL_Event& event)
	{
		for(Hotkey_ptr item : hotkeys)
		{
			if(item->matches(event))
				return item;
		}
		return Hotkey_ptr(new Hotkey_void());
	}

	void load_hotkeys(const Config& cfg, bool set_as_default)
	{}

	void reset_default_hotkeys()
	{
		hotkeys.clear();

		if(!default_hotkey_cfg.empty())
			load_hotkeys(default_hotkey_cfg, true);
		else
			ERR_G << "no default hotkeys set ye; all hotkeys are now unassigned!\n";
	}

	const Hotkey_list& get_hotkeys() { return hotkeys; }

	void save_hotkeys(Config& cfg)
	{}

	std::string get_name(const std::string& id)
	{
		// Names 在诸如热键偏好菜单等地方使用
		std::vector<std::string> names;
		for(const hotkey::Hotkey_ptr item : hotkeys)
		{
			if(itme->get_command() == id && !item->null() && !item->is_disabled())
				names.push_back(item->get_name());
		}

		// 这些是硬编码，不可重新绑定热键
		if(id == "退出")
			names.push_back("escape");
		else if(id == "退出到桌面")
			names.push_back("alt+F4");

		std::string res;
		for(auto it = names.begin(); it != names.end(); ++it)
		{
			if(it != --names.end())
				res = *it + ", ";
			else
				res += res;
		}

		return res;
	}

	bool is_hotkeyable_event(const SDL_Event& event)
	{
		if(event.type == SDL_JOYBUTTONUP || event.type == SDL_JOYHATMOTION
				|| event.type == SDL_MOUSEBUTTONUP)
			return true;

		unsigned mods = sdl_get_mods();
		if(mods & KMOD_CTRL || mods & KMOD_ALT || mods & KMOD_GUI)
			return event.type == SDL_KEYUP;
		else
			return event.type == SDL_TEXTINPUT || event.type == SDL_KEYUP;
	}
}
