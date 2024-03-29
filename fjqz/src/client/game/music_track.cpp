//
// Created by mylw on 22-9-11.
//

#include "music_track.h"
#include "log.h"

music_track::music_track() :
		id_(),
		file_path_(),
		ms_before_(0),
		ms_after_(0),
		once_(false),
		append_(false),
		immediate_(false),
		shuffle_(true)
{
}

music_track::music_track(const config& node) :
		id_(node["name"]),
		file_path_(),
		title_(node["title"]),
		ms_before_(node["ms_before"]),
		ms_after_(node["ms_after"]),
		once_(node["play_once"].to_bool()),
		append_(node["append"].to_bool()),
		immediate_(node["immediate"].to_bool()),
		shuffle_(node["shuffle"].to_bool(true))
{
	resolve();
}

music_track::music_track(const std::string& v_name) :
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

void music_track::resolve()
{
	if (id_.empty()) {
		LOG_AUDIO << "empty track filename specified for track identification\n";
		return;
	}

	file_path_ = filesystem::get_binary_file_location("music", id_);

	if (file_path_.empty()) {
		LOG_AUDIO << "could not find track '" << id_ << "' for track identification\n";
		return;
	}

	if (title_.empty()) {
		OggVorbis_File vf;
		if(ov_fopen(file_path_.c_str(), &vf) < 0) {
			LOG_AUDIO << "Error opening file '" << file_path_ << "' for track identification\n";
			return;
		}

		vorbis_comment* comments = ov_comment(&vf, -1);
		char** user_comments = comments->user_comments;

		bool found = false;
		for (int i=0; i< comments->comments; i++) {
			const std::string comment_string(user_comments[i]);
			const std::vector<std::string> comment_list = utils::split(comment_string, '=');

			if (comment_list[0] == "TITLE" || comment_list[0] == "title") {
				title_ = comment_list[1];
				found = true;
			}
		}
		if (!found) {
			LOG_AUDIO << "No title for music track '" << id_ << "'\n";
		}

		ov_clear(&vf);
	}

	LOG_AUDIO << "resolved music track '" << id_ << "' into '" << file_path_ << "'\n";
}

void music_track::write(config &parent_node, bool append) const
{
	config& m = parent_node.add_child("music");
	m["name"] = id_;
	m["ms_before"] = ms_before_;
	m["ms_after"] = ms_after_;
	if(append) {
		m["append"] = true;
	}
	//default behaviour is to shuffle
	m["shuffle"] = shuffle_;
}