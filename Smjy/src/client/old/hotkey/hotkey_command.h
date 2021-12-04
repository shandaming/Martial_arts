/*
 * Copyright (C) 2018
 */

#ifndef HOTKEY_COMMAND_H
#define HOTKEY_COMMAND_H

#include <vector>
#include <bitset>
#include "tooltips.h"

namespace hotkey
{
	// 可用的热键范围，该范围用于允许来自游戏非重叠区域的命令共享相同的按键
	enum Scope 
	{
		SCOPE_MAIN_MENU,
		SCOPE_GAME,
		SCOPE_COUNT
	};

	enum Hotkey_command
	{
	        HOTKEY_CYCLE_UNITS, HOTKEY_CYCLE_BACK_UNITS,
	        HOTKEY_UNIT_HOLD_POSITION,
	        HOTKEY_END_UNIT_TURN, HOTKEY_LEADER,
	        HOTKEY_UNDO, HOTKEY_REDO,
	        HOTKEY_ZOOM_IN, HOTKEY_ZOOM_OUT, HOTKEY_ZOOM_DEFAULT,
	        HOTKEY_FULLSCREEN, HOTKEY_SCREENSHOT, HOTKEY_MAP_SCREENSHOT, HOTKEY_ACCELERATED,
	        HOTKEY_TERRAIN_DESCRIPTION,
	        HOTKEY_UNIT_DESCRIPTION, HOTKEY_RENAME_UNIT, HOTKEY_DELETE_UNIT,
	        HOTKEY_SAVE_GAME, HOTKEY_SAVE_REPLAY, HOTKEY_SAVE_MAP, HOTKEY_LOAD_GAME,
	        HOTKEY_RECRUIT, HOTKEY_REPEAT_RECRUIT, HOTKEY_RECALL, HOTKEY_ENDTURN,
	        HOTKEY_TOGGLE_ELLIPSES, HOTKEY_TOGGLE_GRID, HOTKEY_STATUS_TABLE, HOTKEY_MUTE, HOTKEY_MOUSE_SCROLL,
	        HOTKEY_SPEAK, HOTKEY_CREATE_UNIT, HOTKEY_CHANGE_SIDE, HOTKEY_KILL_UNIT, HOTKEY_PREFERENCES,
	        HOTKEY_OBJECTIVES, HOTKEY_UNIT_LIST, HOTKEY_STATISTICS, HOTKEY_STOP_NETWORK, HOTKEY_START_NETWORK, HOTKEY_SURRENDER, HOTKEY_QUIT_GAME, HOTKEY_QUIT_TO_DESKTOP,
	        HOTKEY_LABEL_TEAM_TERRAIN, HOTKEY_LABEL_TERRAIN, HOTKEY_CLEAR_LABELS,HOTKEY_SHOW_ENEMY_MOVES, HOTKEY_BEST_ENEMY_MOVES,
	        HOTKEY_DELAY_SHROUD, HOTKEY_UPDATE_SHROUD, HOTKEY_CONTINUE_MOVE,
	        HOTKEY_SEARCH, HOTKEY_SPEAK_ALLY, HOTKEY_SPEAK_ALL, HOTKEY_HELP,
	        HOTKEY_CHAT_LOG, HOTKEY_LANGUAGE, HOTKEY_ANIMATE_MAP,

	        // Replay
	        HOTKEY_REPLAY_PLAY, HOTKEY_REPLAY_RESET, HOTKEY_REPLAY_STOP, HOTKEY_REPLAY_NEXT_TURN,
	        HOTKEY_REPLAY_NEXT_SIDE, HOTKEY_REPLAY_NEXT_MOVE, HOTKEY_REPLAY_SHOW_EVERYTHING,
	        HOTKEY_REPLAY_SHOW_EACH, HOTKEY_REPLAY_SHOW_TEAM1,
	        HOTKEY_REPLAY_SKIP_ANIMATION,
	        HOTKEY_REPLAY_EXIT,

	        // Controls
	        HOTKEY_SELECT_HEX, HOTKEY_DESELECT_HEX,
	        HOTKEY_MOVE_ACTION, HOTKEY_SELECT_AND_ACTION,

	        // Camera movement
	        HOTKEY_SCROLL_UP, HOTKEY_SCROLL_DOWN, HOTKEY_SCROLL_LEFT, HOTKEY_SCROLL_RIGHT,

	        // Dialog control
	        HOTKEY_CANCEL, HOTKEY_OKAY,

	        // Whiteboard commands
	        HOTKEY_WB_TOGGLE,
	        HOTKEY_WB_EXECUTE_ACTION, HOTKEY_WB_EXECUTE_ALL_ACTIONS,
	        HOTKEY_WB_DELETE_ACTION,
	        HOTKEY_WB_BUMP_UP_ACTION, HOTKEY_WB_BUMP_DOWN_ACTION,
	        HOTKEY_WB_SUPPOSE_DEAD,

	        // Misc.
	        HOTKEY_USER_CMD,
	        HOTKEY_CUSTOM_CMD,
	        HOTKEY_AI_FORMULA,
	        HOTKEY_CLEAR_MSG,
	        HOTKEY_LABEL_SETTINGS,

	        // Minimap
	        HOTKEY_MINIMAP_CODING_TERRAIN, HOTKEY_MINIMAP_CODING_UNIT,
	        HOTKEY_MINIMAP_DRAW_UNITS, HOTKEY_MINIMAP_DRAW_VILLAGES, HOTKEY_MINIMAP_DRAW_TERRAIN,

	        /* Gui2 specific hotkeys. */
	        TITLE_SCREEN__RELOAD_WML,
	        TITLE_SCREEN__NEXT_TIP,
	        TITLE_SCREEN__PREVIOUS_TIP,
	        TITLE_SCREEN__TUTORIAL,
	        TITLE_SCREEN__CAMPAIGN,
	        TITLE_SCREEN__MULTIPLAYER,
	        TITLE_SCREEN__ADDONS,
	        TITLE_SCREEN__CORES,
	        TITLE_SCREEN__EDITOR,
	        TITLE_SCREEN__CREDITS,
	        TITLE_SCREEN__TEST,
	        GLOBAL__HELPTIP,
	        LUA_CONSOLE,

	        HOTKEY_WML,

	        /* Editor commands */
	        HOTKEY_EDITOR_CUSTOM_TODS,
	        HOTKEY_EDITOR_PARTIAL_UNDO,

	        // Palette
	        HOTKEY_EDITOR_PALETTE_ITEM_SWAP, HOTKEY_EDITOR_PALETTE_ITEMS_CLEAR,
	        HOTKEY_EDITOR_PALETTE_GROUPS, HOTKEY_EDITOR_PALETTE_UPSCROLL, HOTKEY_EDITOR_PALETTE_DOWNSCROLL,

	        HOTKEY_EDITOR_REMOVE_LOCATION,
	        HOTKEY_EDITOR_PLAYLIST,
	        HOTKEY_EDITOR_SCHEDULE,
	        HOTKEY_EDITOR_LOCAL_TIME,
	        HOTKEY_EDITOR_UNIT_FACING,

	        // Unit
	        HOTKEY_EDITOR_UNIT_TOGGLE_CANRECRUIT, HOTKEY_EDITOR_UNIT_TOGGLE_RENAMEABLE,
	        HOTKEY_EDITOR_UNIT_CHANGE_ID, HOTKEY_EDITOR_UNIT_TOGGLE_LOYAL,
	        HOTKEY_EDITOR_UNIT_RECRUIT_ASSIGN,

	        // Brushes
	        HOTKEY_EDITOR_BRUSH_NEXT, HOTKEY_EDITOR_BRUSH_DEFAULT,
	        HOTKEY_EDITOR_BRUSH_1, HOTKEY_EDITOR_BRUSH_2, HOTKEY_EDITOR_BRUSH_3, HOTKEY_EDITOR_BRUSH_SW_NE, HOTKEY_EDITOR_BRUSH_NW_SE,

	        // Tools
	        HOTKEY_EDITOR_TOOL_NEXT,
	        HOTKEY_EDITOR_TOOL_PAINT, HOTKEY_EDITOR_TOOL_FILL,
	        HOTKEY_EDITOR_TOOL_SELECT, HOTKEY_EDITOR_TOOL_STARTING_POSITION, HOTKEY_EDITOR_TOOL_LABEL,
	        HOTKEY_EDITOR_TOOL_UNIT, HOTKEY_EDITOR_TOOL_VILLAGE, HOTKEY_EDITOR_TOOL_ITEM, HOTKEY_EDITOR_TOOL_SOUNDSOURCE,

	        // Select
	        HOTKEY_EDITOR_SELECT_ALL, HOTKEY_EDITOR_SELECT_INVERSE,	HOTKEY_EDITOR_SELECT_NONE,
	        // Clipboard
	        HOTKEY_EDITOR_CLIPBOARD_PASTE,
	        HOTKEY_EDITOR_CLIPBOARD_ROTATE_CW, HOTKEY_EDITOR_CLIPBOARD_ROTATE_CCW,
	        HOTKEY_EDITOR_CLIPBOARD_FLIP_HORIZONTAL, HOTKEY_EDITOR_CLIPBOARD_FLIP_VERTICAL,
	        // Selection
	        HOTKEY_EDITOR_SELECTION_CUT, HOTKEY_EDITOR_SELECTION_COPY,
	        HOTKEY_EDITOR_SELECTION_ROTATE, HOTKEY_EDITOR_SELECTION_FLIP,
	        HOTKEY_EDITOR_SELECTION_FILL,
	        HOTKEY_EDITOR_SELECTION_EXPORT,
	        HOTKEY_EDITOR_SELECTION_GENERATE, HOTKEY_EDITOR_SELECTION_RANDOMIZE,

	        // Map
	        HOTKEY_EDITOR_MAP_NEW, HOTKEY_EDITOR_MAP_LOAD, HOTKEY_EDITOR_MAP_SAVE,
	        HOTKEY_EDITOR_MAP_SAVE_AS, HOTKEY_EDITOR_MAP_SAVE_ALL,
	        HOTKEY_EDITOR_MAP_REVERT, HOTKEY_EDITOR_MAP_INFO,
	        HOTKEY_EDITOR_MAP_CLOSE,
	        HOTKEY_EDITOR_MAP_SWITCH,
	        HOTKEY_EDITOR_MAP_RESIZE, HOTKEY_EDITOR_MAP_ROTATE,
	        HOTKEY_EDITOR_MAP_GENERATE, HOTKEY_EDITOR_MAP_APPLY_MASK,
	        HOTKEY_EDITOR_MAP_CREATE_MASK_TO,

	        // Transitions
	        HOTKEY_EDITOR_UPDATE_TRANSITIONS, HOTKEY_EDITOR_TOGGLE_TRANSITIONS,
	        HOTKEY_EDITOR_AUTO_UPDATE_TRANSITIONS, HOTKEY_EDITOR_PARTIAL_UPDATE_TRANSITIONS, HOTKEY_EDITOR_NO_UPDATE_TRANSITIONS,

	        // Refresh
	        HOTKEY_EDITOR_REFRESH,
	        HOTKEY_EDITOR_REFRESH_IMAGE_CACHE,

	        // Draw
	        HOTKEY_EDITOR_DRAW_COORDINATES, HOTKEY_EDITOR_DRAW_TERRAIN_CODES, HOTKEY_EDITOR_DRAW_NUM_OF_BITMAPS,

	        // Side
	        HOTKEY_EDITOR_SIDE_NEW,
	        HOTKEY_EDITOR_SIDE_EDIT,
	        HOTKEY_EDITOR_SIDE_REMOVE,

	        // Area
	        HOTKEY_EDITOR_AREA_REMOVE,
	        HOTKEY_EDITOR_AREA_ADD,
	        HOTKEY_EDITOR_AREA_SAVE,
	        HOTKEY_EDITOR_AREA_RENAME,

	        // Scenario
	        HOTKEY_EDITOR_SCENARIO_EDIT,
	        HOTKEY_EDITOR_SCENARIO_NEW,
	        HOTKEY_EDITOR_SCENARIO_SAVE_AS,

	        /* This item must stay at the end since it is used as terminator for iterating. */
                HOTKEY_NULL
	};

	enum Hotkey_category
	{
		HKCAT_GENERAL,
	        HKCAT_SAVING,
	        HKCAT_MAP,
	        HKCAT_UNITS,
	        HKCAT_CHAT,
	        HKCAT_REPLAY,
	        HKCAT_WHITEBOARD,
	        HKCAT_SCENARIO,
	        HKCAT_PALETTE,
	        HKCAT_TOOLS,
	        HKCAT_CLIPBOARD,
	        HKCAT_DEBUG,
	        HKCAT_CUSTOM,
                HKCAT_PLACEHOLDER // Keep this one last
	};

	typedef std::bitset<Scope_count> Hk_scopes;

	// 不要在hotkey.cpp外使用这个
	struct Hotkey_command_temp
	{
		Hotkey_command id;
		std::string command;
		std::string description; 
		bool hidden;
		Hk_scopes scope;
		Hotkey_category category;
		std::string tooltip;
	};

	// 存储与可以绑定到热键的功能相关的所有信息。这当前是一个半结构体，他有
	// 一个构造函数，但只有const-publicc成员
	struct Hotkey_command
	{
		Hotkey_command() = delete;
		Hotkey_command(const Hotkey_command_temp& temp_command);
		Hotkey_command(Hotkey_command cmd, const std::string& id, 
				const std::string& desc, bool hidden, bool toggle, 
				Hk_scopes scope, Hotkey_category category, 
				const std::string& tooltip);

		Hotkey_command(const Hotkey_command&) = default;
		Hotkey_command& operator=(const Hotkey_command&) = default;

		Hotkey_command id;
		std::string command; // 该命令是唯一的
		std::string description;
		bool hidden; // 如果隐藏则需要显示

		// 切换热键对可绑定的内容有一些限制，他们需要一个具有2种状态的绑定
		// ‘按下’和‘释放’
		bool toggle;

		Hk_scopes scope; // 命令的可见范围

		Hotkey_category category; // 命令类别

		std::string tooltip;

		bool null() const;

		// 返回被视为null的命令
		static const Hotkey_command& null_command();

		static const Hotkey_command& get_command_by_command(Hotkey_command command);
	};

	class Scope_change
	{
		public:
			Scope_change();
			~Scope_change();
		private:
			Hk_scopes prev_scope_active_;
	};

	// 返回一个包含所有当前后动的Hotkey_commands的容器，所有想要热键的东西
	// 都必须在这容器内
	const std::vector<Hotkey_command>& get_hotkey_command();

	// 返回给定名称的Hotkey_command
	const Hotkey_command& get_hotkey_command(const std::string& command);

	// 返回被视为null的Hotkey_command
	const Hotkey_command& get_hotkey_null();

	void deactivate_all_scopes();
	void set_scope_active(Scope s, bool set = true);
	void set_active_scope(Hk_scopes s);
	bool is_scope_active(Scope s);
	bool is_scope_active(Hk_scopes s);

	bool has_hotkey_command(const std::string& id);

	// 将新的wml热键添加到列表中，但只有在列表中没有带该标志的热键时才会
	// 显示，此处创建的对象将在'delete_all_wml_hotkeys()'中删除
	void add_wml_hotkey(const std::string& id, const std::string& description, const Config& defualt_hotkey);

	// 输出所有wml热键，在游戏结束后调用
	void delete_all_wml_hotkeys();
	bool remove_wml_hotkey(const std::string& id);

	const std::string& get_description(const std::string& command);
	const std::string& get_tooltip(const std::string& command);

	void init_hotkey_commands();
	void clear_hotkey_commands();

	// 返回get_hotkey_command(command).id
	Hotkey_command get_id(const std::string& command);
}

#endif
