/*
 * Copyright (C) 2017 by Shan Daming
 */

#ifndef GENERAL_H
#define GENERAL_H

int font_scaling();
void set_font_scaling(int scale);
int font_scaled(int size);

namespace preferences
{
	unsigned int sample_rate();
	void save_sample_rate(const unsigned int rate);

	size_t sound_buffer_size();
	void save_sound_buffer_size(const size_t size);
}

#endif
