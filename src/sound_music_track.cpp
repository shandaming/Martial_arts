/*
 * Copyright (C) 2018 by Shan Daming
 */

#include "sound_music_track.h"

static lg::Log_domain log_audio("audio");
#define ERR_AUDIO LOG_STREAM(lg::err, log_audio)
#define LOG_AUDIO LOG_STREAM(lg::info, log_audio)

namespace sound
{
	Music_track::Music_track() : 
		id_(),
		file_path_(),
		ms_before_(0),
		ms_after_(0),
		once_(false),
		append_(false),
		immediate_(false),
		shuffle_(true)
	{}

	Music_track::Music_track(const Config& node) : 
		id_(node["name"]),
		file_path_(),
		title_(node["title"]),
		ms_before_(node["ms_before"]),
		ms_after_(node["ms_after"]),
		once_(node["play_once"]),
		append_(node["append"]),
		immediate_(node["immediate"]),
		shuffle_(node["shuffle"])
	{
		resolve();
	}

	Music_track::Music_track(const std::string& v_name) :
		id_(v_name),
		file_path_(),
		title_(),
		ms_before_(0),
		ms_after_(0),
		once_(false),
		append_(false),
		immediate_(false),
		shuffle_(true)
	{
		resolve();
	}

	void Music_track::resolve()
	{
		if(id_.empty())
		{
			LOG_AUDIO << "empty track filename specified for track \
				identification\n";
			return;
		}

		file_path_ = filesystem::get_binary_file_loaction("music", id);

		if(file_path_.empty())
		{
			LOG_AUDIO << "could not find track '" << id_ 
				<< "' for track identification\n";
			return;
		}

#if !define(_WIN32) && !define(__APPLE__)
		if(title_.empty())
		{
			FILE* f;
			f = fopen(file_path.c_str(), "r");
			if(f == nullptr)
			{
				LOG_AUDIO << "Error opening file '" << file_path_ 
					<< "' for track identification\n";
				return;
			}

			OggVorbis_File vf;
			if(ov_open(f, &vf, nullptr, 0) < 0)
			{
				LOG_AUDIO << "track does not appear to be an Ogg file '"
					<< id_ << "', cannot be identified\n";
				ov_clear(&vf);
				return;
			}

			vorbis_comment* comments = ov_comment(&vf, -1);
			char** user_comments = comments->user_comments;

			bool found = false;
			for(int i = 0; i != comments->comments; ++i)
			{
				std::string comment_string(user_comments[i]);
				std::vector<std::string> comment_list = 
					utils::split(comment_string, '=');

				if(comment_list[0] == "TITLE" || comment_list[0] == "title")
				{
					title_ = comment_list[1];
					found = true;
				}
			}
			if(!found)
				LOG_AUDIO << "No title for music track '" << id_ << "'\n";
			ov_clear(&vf);
		}
#endif
		LOG_AUDIO << "resolved music track '" << id_ << "' into '"
			<< file_path_ << "'\n";
	}

	void Music_track::write(Config& parent_node, bool append) const
	{
		// add music to the config file
	}
}
