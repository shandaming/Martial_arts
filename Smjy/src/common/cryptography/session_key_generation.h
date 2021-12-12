/*
 * Copyright (C) 2020
 */

#ifndef SESSION_KEY_GENERATION_H
#define SESSION_KEY_GENERATION_H

template<typename Hash>
class session_key_generator
{
public:
	session_key_generator(uint8_t* buffer, uint32_t size)
	{
		uint32_t half_size = size / 2;

		sh_.initialize();
		sh_.update_data(buffer, half_size);
		sh_.finialize();

		memcpy(o1_, sh_.get_digest(), Hash::digest_length::value);

		sh_.initialize();
		sh_.update_data(buffer + half_size, size - half_size);
		sh_.finialize();

		memcpy(o2_, sh_.get_digest(), Hash::digest_length::value);
		memset(o0_, 0x00, Hash::digest_length::value);

		fill_up();
	}

	void generate(uint8_t* buffer, uint32_t size)
	{
		for(uint32_t i = 0; i < size; ++i)
		{
			if(taken_ == Hash::digest_length::value)
				fill_up();

			buffer[i] = o0_[taken_];
			++taken_;
		}
	}
private:
	void fill_up()
	{
		sh_.initialize();
		sh_.update_data(o1_, Hash::digest_length::value);
		sh_.update_data(o0_, Hash::digest_length::value);
		sh_.update_data(o2_, Hash::digest_length::value);
		sh_.finialize();

		memcpy(o0_, sh_.get_digest(), Hash::digest_length::value);
		taken_ = 0;
	}

	Hash sh_;
	uint32_t taken_;
	uint8_t o0_[Hash::digest_length::value];
	uint8_t o1_[Hash::digest_length::value];
	uint8_t o2_[Hash::digest_length::value];
};

#endif
