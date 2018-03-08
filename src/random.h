/*
 * Copyright (C) 2018 by Shan Daming
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <limits>

namespace randomness
{
	class Rng : private std::numeric_limits<uint32_t>
	{
			using base = std::numeric_limits<uint32_t>;
		public:
			Rng();

			// Provides the next random draw. This is raw RPG output
			uint32_t next_random();

			virtual ~Rng();

			/*
			 * Provides the number of random calls to the rng in this 
			 * context. Note that this may be different from the number of 
			 * random calls to the underlying rng, and to the random_calls
			 * number in save files!
			 */
			unsigned int get_random_calls() const;

			/*
			 * This helper method provides a random int from the underlying
			 * generator, using results of next_random in a manner 
			 * guaranteed to be cross platform.
			 * The result will be random in range [min, max] inclusive.
			 */
			int get_random_int(int min, int max)
			{
				return min + get_random_int_in_range_zero_to(max - min);
			}

			/*
			 * This helper method returns true with the probability supplied
			 * as a parameter.
			 */
			bool get_random_bool(double probability);

			/*
			 * This helper method return a floatin-point number in the range
			 * [0, 1]
			 */
			double get_random_double();

			/*
			 * This helper method selects a random element from a container
			 * of floating-point numbers. Every number has a probability to
			 * be selected equal to the number itself (e.g. a number of 0.1
			 * is selected with a probability of 0.1). The sum of numbers
			 * should be one.
			 */
			template<typename T>
			unsigned int get_random_element(T first, T last);

			// For compatibility with the c++ UniformRandomBitGenerator
			// concept
			using result_type = uint32_t;
			using base::min;
			using base::max;

			uint32_t operator()() { return next_random(); }

			static Rng& default_instance();
		protected:
			virtual uint32_t next_random_impl() = 0;
			unsigned int random_calls_;
		private:
			/*
			 * Does the hard work of get_random_int.
			 * The result will be random in range [0, max] inclusive.
			 */
			int get_random_int_in_range_zero_to(int max);
	};

	/*
	 * This generator is automatically synced during synced context.
	 * Calling this Rng during a synced context automatically makes undoing
	 * impossible. Outside a synced context this has the same effect as 
	 * rand().
	 */
	extern Rng* generator;

	template<typename T>
	unsigned int Rng::get_random_element(T frist, T last)
	{
		double target = get_random_double();
		double sum = 0.0;
		T it = first;
		sum += *it;
		while(sum <= target)
		{
			if(++it != last)
				sum += *it;
			else
				break;
		}
		return std::distance(first, it);
	}
}

#endif
