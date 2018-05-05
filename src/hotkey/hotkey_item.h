/*
 * Copyright (C) 2018
 */

#ifndef HOTKEY_ITEM_H
#define HOTKEY_ITEM_H

#include <memory>
#include <vector>
#include <string>
#include <SDL2/SDL.h>

namespace hotkey
{
	class Hotkey_base;
	class Hotkey_mouse;
	class Hotkey_keyboard;

	typedef std::shared_ptr<Hotkey_base> Hotkey_ptr;
	typedef std::shared_ptr<Hotkey_mouse> Hotkey_mouse_ptr;
	typedef std::shared_ptrr<Hotkey_keyboard> Hotkey_keyboard_ptr;

	typedef std::vector<hotkey::Hotkey_ptr> Hotkey_list;
	typedef std::vector<hotkey::Hotkey_ptr>::iterator Hotkey_list_iter;

	// 热键事件匹配的基类
	class Hotkey_base
	{
		public:
			// 初始化一个将被禁用的新空的热键
			Hotkey_base() : command_("null"), is_default_(true), 
				is_disabled_(false), mod_(0) {}

			void set_command(const std::string& command) { command_ = command; }

			// 设置键盘修改器
			// @param mods SDL_Keymod的位掩码
			void set_mods(unsigned int mods) { mods_ = mods; }

			const std::string get_command() const { return command_; }

			// 这控制项目是否应该出现在热键偏好中
			virtual bool hidden() const { return false; }

			// 这表明热键是来自默认配置还是用户首选项
			bool is_default() const { return is_default_; }

			// 用于指示热键被覆盖并应被视为用户设置的热键
			void unset_default() { is_defualt_ = false; }

			bool is_disabled() const { return is_disabled_; }

			void disable() { is_disabled_ = true; }

			void enable() { is_disabled_ = false; }

			// 通过将该热键链接到空命令解除绑定
			void clear() { command_ = "null"; }

			// 返回这命令是否指向空命令
			bool null() const { return command_ == "null"; }

			/*
			 * 返回是否有关联的Hotkey_command.如果没有一个Hotkey_commands
			 * 符合这个Hotkey_item,那么@param get_hotkey_command将返回
			 * hotkey_command::null_command().
			 */
			bool active() const { return command_ != "null"; }

			// 评估热键绑定是否有效
			virtual bool valid() const = 0;

			// 将热键保存到配置对象中
			void save(Config& cfg) const;

			// 返回热键名，例如"ctrl+alt+g"
			const std::string get_name() const;

			/*
			 * 用于评估：1 键盘在当前范围内是否有效，2 键盘修改器与
			 * SDL_Event是否匹配此热键
			 */
			bool matches(const SDL_Event& event) const;

			// 检测热键绑定和范围是否相等
			virtual bool bindings_equal(Hotkey_ptr other);

			virtual ~Hotkey_base() {}
		protected:
			/*
			 * 这由Hotkey_base::get_name调用，并且必须由子类实现，键盘修饰符
			 * 在这个类中处理，其他热键在相应的类中处理
			 */
			virtual const std::string get_name_helper() const = 0;

			// 这由Hotkey_base::matches作为具体类的帮助器来调用，实现类应该
			// 只检查他们的热键部分
			virtual bool matchers_helper(const SDL_Event& event) const = 0;

			// 这由Hotkey_base::bindings_equal作为具体类的帮助器来调用，实现
			// 类应该只检查他们的热键部分
			virtual bool bindings_equal_helper(Hotkey_ptr other) const = 0;

			// 应该执行的命令后"null"
			std::string command_;

			/*
			 * 如果热键是data/core/hotkeys.cfg中定义的默认热键列表的一部分，
			 * 则is_default为true。is_default_如果不是，则为false，在这种
			 * 情况下，他将在用户的首选文件中定义。
			 */
			bool is_default_;

			/*
			 * 关于何时将用户热键保存到首选项以及何时应将默认热键标记为禁
			 * 用，使用'null'命令来指示禁用的热键的原始设计是不明确的，因此
			 * 引入一个单独的禁用标志来解决歧义，如果标志为真，则不应将热键
			 * 写入偏好，除非他是默认的热键。
			 */
			bool is_disabled_;

			// 键盘修饰符，视为不透明，只做比较
			unsigned int mod_;
	};

	class Hotkey_keyboard : public Hotkey_base
	{
		public:
			Hotkey_keyboard() : Hotkey_base(), keycode_(SDLK_UNKNOWN), text_("") {}

			void set_keycode(SDL_Keycode keycode) { keycode_ = keycode; }

			void set_text(const std::string& text)
			{
				text_ = text;

				std::transform(text_.begin(), text_.end(), text_.end(), [](unsigned char& c) { return tolower(c); })
			}

			virtual bool valid() const 
			{
				return keycode_ != SDLK_UNKNOWN && !text_.empty();
			}
		protected:
			virtual void save_helper(Config& cfg) const;
			virtual const std::string get_name_helper() const;
			virtual bool matches_helper(const SDL_Event& event) const;
			virtual bool bindings_equal_helper(Hotkey_ptr other) const;

			SDL_Keyboard keycode_;
			std::string text_;
	};

	// 该类用于返回无效结果，以便以空检查中保存他人
	class Hotkey_void : public Hotkey_base
	{
		public:
			Hotke_void() : Hotkey_base() {}
			virtual bool valid() const { return false; }
		protected:
			virtual void save_helper(Config&) const {}
			virtual const std::string get_name_helper() const { return ""; }
			virtual bool matches_helper(const SDL_Event&) const { return false; }
			virtual bool bindings_equal_helper(Hotkey_ptr) const { return false; }
	};

	// 这类负责处理鼠标按键
	class Hotkey_mouse : public Hotkey_base
	{
		public:
			Hotkey_mouse() : Hotkey_base(), button_(0) {}

			// 如果热键有个与之关联的有效鼠标按钮则返回true
			virtual bool valid() const { return button_ != 0; }

			void set_button(int button) { button_ = button; }
		protected:
			virtual void save_helper(Config& cfg) const;
			virtual const std::string get_name_helper() const;
			virtual bool matches_helper(const SDL_Event& event) const;
			virtual bool bindings_equal_helper(Hotkey_ptr other) const;

			int button_;
	};
}

#endif
