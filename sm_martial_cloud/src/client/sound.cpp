/*
 * Copyright (C) 2018 by Shan Daming
 */

#include <list>
#include <sstream>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include "sound.h"
#include "log.h"
#include "random.h"

static lg::Log_domain log_audio("audio");
#define DBG_AUDIO LOG_STREAM(lg::debug, log_audio)
#define LOG_AUDIO LOG_STREAM(lg::info, log_audio)
#define ERR_AUDIO LOG_STREAM(lg::err, log_audio)

namespace sound
{
	// Channel-chunk mapping lets us know, if we can safely free a given 
	// chunk
	static std::vector<Mix_chunk*> channel_chunks;

	// Channel-id mapping for use with sound sources (to check if given
	// source is playing on a channel for fading/panning)
	static std::vector<int> channel_ids;
}

namespace
{
	bool mix_ok = false;
	int music_start_time = 0;
	unsigned music_refresh = 0;
	unsigned music_refresh_rate = 20;
	bool want_new_music = false;
	int fadingout_time = 5000;
	bool no_fading = false;
	bool unload_music = false;

	// number of allocated channels;
	constexpr size_t n_of_channels = 32;

	constexpr size_t bell_channel = 0;
	constexpr size_t timer_channel = 1;

	// number of channels reserved for sound sources
	constexpr size_t source_channels = 8;
	constexpr size_t source_channel_start = timer_channel + 1;
	constexpr size_t source_channel_last = source_channel_start + 
											source_channels - 1;
	constexpr size_t UI_sound_channels = 2;
	constexpr size_t UI_sound_channel_start = source_channel_last + 1;
	constexpr size_t UI_sound_channel_last = UI_Sound_channel_start + 
											UI_sound_channels - 1;
	// sources, bell, timer and UI
	constexpr size_t n_reserved_channels = UI_sound_channel_last + 1;

	// Max number of sound chunks that we want to cache Keep this above
	// number of available channels to avoid busy-looping
	unsigned max_cached_chunks = 256;

	std::map<Mix_Chunk*, int> chunk_usage;
}

static void increment_chunk_usage(Mix_chunk* mcp)
{
	++(chunk_usage[mcp]);
}

static void decrement_chunk_usage(Mix_chunk* mcp)
{
	if(mcp == nullptr)
		return;

	std::map<Mix_Chunk*, int>::iterator this_usage = chunk_usage.find(mcp);
	assert(this_usage != chunk_usage.end());
	if(--(this_usage->second) == 0)
	{
		Mix_FreeChunk(mcp);
		chunk_usage.erase(this_usage);
	}
}

namespace 
{
	class Sound_cache_chunk
	{
		public:
			Sound_cache_chunk(const std::string& f) :
				group(sound::Channel_group::NULL_CHANNEL),
				file(f),
				data_(nullptr)
			{}

			Sound_cache_chunk(const Sound_cache_chunk& scc) :
				group(scc.group),
				file(scc.file),
				data_(scc.data_)
			{
				increment_chunk_usage(data_);
			}

			Sound_cache_chunk& operator=(const Sound_cache_chunk& scc)
			{
				file = scc.file;
				group = scc.group;
				set_data(scc.get_data());
				return *this;
			}

			~Sound_cache_chunk()
			{
				decrement_chunk_usage(data_);
			}

			void set_data(Mix_Chunk* d)
			{
				increment_chunk_usage(d);
				decrement_chunk_usage(data_);
				data_ = d;
			}

			Mix_Chunk* get_data() const { return data_; }

			bool operator==(const Sound_cache_chunk& scc) const
			{
				return file == scc.file;
			}

			bool operator!=(const Sound_cache_chunk& scc) const
			{
				return !=operator==(scc);
			}

			sound::Channel_group group;
			std::string file;
		private:
			Mix_Chunk* data_;
	};

	std::list<Sound_cache_chunk> sound_cache;
	typedef std::list<Sound_cache_chunk>::iterator sound_cache_iterator;
	std::map<std::string, std::shared_ptr<Mix_music>> music_cache;

	std::vector<std::string> played_before;

	/*
	 * FIXME: the first music_track may be initialized before main() is 
	 * reached. Using the logging facilities may lead to a SIGSEGV because
	 * it's not guaranteed that their objects are already alive.
	 *
	 * Use the music_track default constructor to avoid trying to invoke a 
	 * log object while resolving paths.
	 */
	std::vector<std::shared_ptr<sound::Music_track>> current_track_list;
	std::shared_ptr<sound::Music_track> current_track;
	unsigned int current_track_index = 0;
	std::shared_ptr<sound::Music_track> previous_track;

	std::vector<std::shared_ptr<sound::Music_track>>::const_iterator
		find_track(const sound::Music_track& track)
	{
		return std::find_if(current_track_list.begin(),
				current_track_list.end(),
				[&track]
				(const std::shared_ptr<const sound::Music_track>& ptr)
				{ return *ptr == track; });
	}
}

namespace sound
{
	std::optional<unsigned int> get_current_track_index()
	{
		if(current_track_index >= current_track_list.size())
			return {};
		return current_track_index;
	}

	std::shared_ptr<Music_track> get_current_track()
	{
		return current_track;
	}

	std::shared_ptr<Music_track> get_previous_music_track()
	{
		return previous_track;
	}

	void set_previous_track(std::shared_ptr<Music_track> track)
	{
		previous_track = track;
	}

	unsigned int get_num_tracks()
	{
		return current_track_list.size();
	}

	std::shared_ptr<Music_track> get_track(unsigned int i)
	{
		if(i < current_track_list.size())
			return current_track_list[i];
		if(i == current_track_list.size())
			return current_track;
		return nullptr;
	}

	void set_track(unsigned int i, const std::shared_ptr<Music_track>& to)
	{
		if(i < current_track_list.size() && 
				find_track(*to) != current_track_list.end())
			current_track_list[i] = std::make_shared<Music_track>(*to);
	}

	void remove_track(unsigned int i)
	{
		if(i >= current_track_list.size())
			return;
		if(i == current_track_index)
		{
			// Let the track finish playing
			if(current_track)
				current_track->set_play_once(true);
			// Set current index to the new size of the list
			current_track_index = current_track_list.size() - 1;
		}
		else if(i < current_track_index)
			--current_track_index;

		current_track_list.erase(current_track_list.begin() + i);
	}
}

static bool track_ok(const std::string& id)
{
	LOG_AUDIO << "Considering " << id << "\n";

	if(!current_track)
		return true;

	// If they committed changes to list, we forget previous plays, but 
	// still "never" repeat same track twice if we have an option.
	if(id == current_track->file_path())
		return false;
	if(current_track_list.size() <= 3)
		return true;

	unsigned int num_played = 0;
	std::set<std::string> played;
	std::vector<std::string>::reverse_iterator i;

	for(i = played_before.rbegin(); i != played_before.rend(); ++i)
	{
		if(*i == id)
		{
			if(++num_played == 2)
				break;
		}
		else
			played.insert(*i);
	}

	// If we played this twice, must have played every other track
	if(num_played == 2 && played.size() != current_track_list.size() - 1)
	{
		LOG_AUDIO << "Played twice with only " << played.size(
				) << "tracks between\n";
		return false;
	}

	// Check previous previous track not same.
	i = played_before.rbegin();
	if(i != played_before.rend())
	{
		if(++i != played_before.rend())
		{
			if(*i == id)
			{
				LOG_AUDIO << "Played just before previous\n";
				return false;
			}
		}
	}
	return true;
}

static std::shared_ptr<sound::Music_track> choose_track()
{
	assert(!current_track_list.empty());

	if(current_track_index >= current_track_list.size())
		current_track_index = 0;
	if(current_track_list[current_track_index]->shuffle())
	{
		unsigned int track = 0;

		if(current_track_list.size() > 1)
		{
			do
			{
				track = randomness::Rng::default_instance().get_random_int(
						0, current_track_list.size() -1);
			}while(!track_ok(current_track_list[track]->file_path()));
		}
		current_track_index = track;
	}

	DBG_AUDIO << "Next track will be " 
		<< current_track_list[current_track_index]->file_path() << "\n";
	played_before.push_back(
			current_track_list[current_track_index]->file_path());
	return current_track_list[current_track_index];
}

static std::string pick_one(const std::string& files)
{
	std::vector<std::string> ids = 
		utils::square_parenthetical_split(files, ',', "[", "]");
	if(ids.empty())
		return "";
	if(ids.size() == 1)
		return ids[0];

	// We avoid returning same choice twice if we can avoid it.
	static std::map<std::string, unsigned int> prev_choices;
	unsigned choice;

	if(prev_choices.find(files) != prev_choices.end())
	{
		choice = randomness::Rng::default_instance().get_random_int(
				0, ids.size() - 2);
		if(choice >= prev_choices[files])
			++choice;
		prev_choices[files] = choice;
	}
	else
	{
		choice = randomness::Rng::default_instance().get_random_int(
				0, ids.size() -1);
		prev_choices.emplace(files, choice);
	}
	return ids[choice];
}

namespace
{
	struct Audio_lock
	{
		Audio_lock
		{
			SDL_LockAudio();
		}
		~Audio_lock
		{
			SDL_UnlockAudio();
		}
	};
}

namespace sound
{
	// Removes channel-chunk and channel-id mapping
	static void channel_finished_hook(int channel)
	{
		channel_chunks[channel] = nullptr;
		channel_ids[channel] = -1;
	}

	bool init_sound()
	{
		LOG_AUDIO << "Initializing audio...\n";

		if(SDL_WasInit(SDL_INIT_AUDIO) == 0)
		{
			if(SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
				return false;
		}

		if(!mix_ok)
		{
			if(Mix_OpenAudio())
		}
	}
}
