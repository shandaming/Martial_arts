/*
 * Copyright (C) 2018 by Shan Daming
 */

#include <random>
#include <cassert>
#include "random.h"
#include "log.h"

static lg::Log_domain log_random("random");
#define DBG_RND LOG_STREAM(lg::debug, log_random)
#define LOG_RND LOG_STREAM(lg::info, log_random)
#define WRN_RND LOG_STREAM(lg::warn, log_random)
#define ERR_RND LOG_STREAM(lg::err, log_random)

namespace
{
	class Rng_default : public randomness::Rng
	{
		public:
			Rng_default() : gen_()
			{
				std::random_device entropy_source;
				gen_.seed(entropy_source());
			}
		protected:
			virtual uint32_t next_random_impl()
			{
				return gen_();
			}
		private:
			std::mt19937 gen_;
	};
}

namespace randomness
{
	Rng* generator = &Rng::default_instance();

	Rng::Rng() : random_calls_(0) {}

	Rng::~Rng() {}

	Rng& Rng::default_instance()
	{
		static Rng* def = new Rng_default();
		return *def;
	}

	unsigned int Rng::get_random_calls() const
	{
		return random_calls_;
	}

	uint32_t Rng::next_random()
	{
		++random_calls_;
		return next_random_impl();
	}

	int Rng::get_random_int_in_range_zero_to(int max)
	{
		assert(max >= 0);
		return static_cast<int>(next_random() % 
				(static_cast<uint32_t>(max) + 1));
	}

	double Rng::get_random_double()
	{
		union
		{
			double floating_point_number;
			uint64_t integer;
		} number;

		number.integer = 0;

		// Exponent. Tt's set to zero.
		// Exponent bias is 1023 in double precision, and therefore the 
		// value 1023 needs to be encoded.
		number.integer |= static_cast<uint64_t>(1023) << 52;

		/*
		 * Significand. A double-precision floationg point number stores 52 
		 * significand bits. The underlying RNG only gives us 32 bits, so we
		 * need to shift the bits 20 positions to the left. The last 20
		 * significand bits we can leave at zero, we don't need the full 52
		 * bits of randomness allowed by the double-precision format.
		 */
		number.integer |= static_cast<uint64_t>(next_random()) << (52-32);

		/*
		 * At this point, the exponent is zero. The significand, taking into
		 * account the inplicit leading one bit, is at least wxactly one and
		 * at most almost two. In other words, interpreting the value as a 
		 * doublegives us a number int the range [1, 2]. Simply subtract one
		 * from that value and return it.
		 */
		return number.floating_point_number - 1.0;
	}

	bool Rng::get_random_bool(double probability)
	{
		assert(probability >= 0.0 && probability <= 1.0);
		return get_random_double() < probability;
	}
}
