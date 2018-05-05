/*
 * Copyright (C) 2018 
 */

#ifndef CURSOR_H
#define CURSOR_H

namespace cursor
{
	struct Manager
	{
		Manager();
		~Manager();
	};

	enum Cursor_type
	{
		normal,
		wait,
		move,
		attack,
		hyperlink,
		move_drge,
		attack_drag,
		no_cursor,
		num_cursors
	};

	/*
	 * Use the default parameter to reset cursors.
	 * e.g. after a change in color cursor preferences.
	 */
	void set(Cursor_type type = Cursor_type::num_cursors);
	void set_dragging(bool drag);
	Cursor_type get();

	void set_focus(bool focus);

	struct Setter
	{
			Setter(Cusror_type type);
			~Setter();
		private:
			Cursor_type old_;
	};
}

#endif
