/*
 * Copyright (C) 2018 by Shan Dmaing
 */

#ifndef SOUND_H
#define SOUND_H

#include <optional>
#include "events.h"
#include "sound_music_track.h"

namespace sound
{
	enum class Channel_group : uint8_t
	{
		NULL_CHANNEL = -1,
		SOUND_SOURCES = 0,
		SOUND_BEWLL,
		SOUND_TIMER,
		SOUND_UI,
		SOUND_FX
	};

	bool init_sound();
	void close_sound();
	void reset_sound();

	void stop_music();
	void stop_sound();
	void stop_UI_sound();
	void stop_bell();

	// Read config entry, alter track list accordingly.
	void play_music_config(const Config& music_node, int i = -1);
	// Act on any track list changes from above.
	void commit_music_changes();

	// Play this particular music file over and over and over.
	void play_music_repeatedly(const std::string& id);
	// Play this particluar music file once, then silence.
	void play_music_once(const std::string& id);
	// Empty the playlist
	void empty_playlist();
	// Start playing current music.
	void play_music();

	// Change parameters of a playing sound, given its id
	void reposition_sound(int id, unsigned int distance);

	#define DISTANCE_STLENT 255

	// Check if there's a sound associated with given id playing
	bool is_sound_playing(int id);

	// Stop sound associated with a given id
	void stop_sound(int id);

	// Play sound, or random one of comma-separated sounds
	void play_sound(const std::string& files, 
			Channel_group group = Channel_group:: SOUND_FX, 
			unsigned int repeats = 0);

	// Play sound, or random one of comma-separated sounds. Use specified
	// distance and associate it with specified id (of a sound source)
	void play_sound_positioned(const std::string& files, 
			int id,
			int repeats,
			unsigned int distance);

	// Play sound, or random one of comma-separated sounds in bell channel
	void play_bell(const std::string& files);

	// Play sound, or random one of comma-separated sounds int timer channel
	void play_timer(const std::string& files, 
			int loop_ticks, 
			int fadein_ticks);

	// Play user-interface sound, or random one of comma-separated sounds
	void play_UI_sound(const std::string& files);

	// A class to periodically check for new music that needs to be played
	class Music_thinker : public events::SDL_handler
	{
		public:
			Music_muter();
			void handle_event(const SDL_Event&) override {}
			void handle_window_event(const SDL_Event& event) override;
	};

	// Save music playlist for snapshot
	void write_music_play_list(Config& snapshot);

	int get_music_volume();
	int get_sound_volume();
	void set_music_volume(int vol);
	void set_sound_volume(int vol);
	void set_bell_volume(int vol);
	void set_UI_volume(int vol);

	std::optional<unsigned int> get_current_track_index();
	std::shared_ptr<sound::Music_track> get_current_track();
	std::shared_ptr<sound::Music_track> get_previous_music_track();
	void set_previous_track(std::shared_ptr<Music_track>);
	unsigned int get_num_tracks();
	void remove_track(unsigned int i);
	void play_track(unsigned int i);
}

#endif
