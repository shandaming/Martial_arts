/*
 * Copyright (C) 2018
 */

#ifndef GUI_CORE_PLACER_H
#define GUI_CORE_PLACER_H

namespace gui
{
class Placer_base
{
public:
	/*
	 * @param grow_direction	items的添加方向。
	 * @param parallel_items	
	 */
	static Placer_base* build(const Grow_direction grow_direction,
			const unsigned parallel_items);

	virtual ~Placer_base() {}

	virtual void initialize() = 0;

	virtual void add_item(const Point& size) = 0;

	virtual Point get_size() const = 0;

	virtual Point get_origin(const unsigned index) const = 0;
};

} // end namespace gui

#endif
