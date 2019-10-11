/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef SOUND_MUSIC_TRACK_H
#define SOUND_MUSIC_TRACK_H

#include <string>
#include <memory>
#include "config.h"

namespace sound
{
	class Music_track
	{
		public:
			Music_track();
			Music_track(const Config& node);
			explicit Music_track(const std::string& v_name);

			void write(Config& parent_node, bool append) const;

			bool valid() const { return file_path_.empty() != true; }

			bool append() const { return append_; }
			bool immediate() const { return immediate_; }
			bool shuffle() const { return shuffle_; }
			bool play_once() const { return once_; }
			int ms_before() const { return ms_before_; }
			int ms_after() const { return ms_after_; }

			const std::string& file_path() const { return file_path_; }
			const std::string& id() const { return id_; }
			const std::string& title() const { return title_; }

			void set_play_once(bool v) { once_ = v; }
			void set_shuffle(bool v) { shuffle_ = v; }
			void set_ms_before(int v) { ms_before_ = v; }
			void set_ms_after(int v) { ms_after_ = v; }
		private:
			void resolve();

			std::string id_;
			std::string file_path_;
			std::string title_;

			int ms_before, ms_after_;

			bool once_;
			bool append_;
			bool immediate_;
			bool shuffle_;
	};

	std::shared_ptr<Music_track> get_track(unsigned int i);
	void set_track(unsigned int i, const std::shared_ptr<Music_track>& to);
}

inline bool operator==(const sound::Music_track& a, 
					const sound::Music_track& b)
{
	return a.file_path() == b.file_path();
}

inline bool operator!=(const sound::Music_track& a,
					const sound::Music_track& b)
{
	return a.file_path() != b.file_path();
}

#endif
