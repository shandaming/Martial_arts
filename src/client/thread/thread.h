/*
 * Copyright (C) 2017 by Shan Daming
 */

#include <pthread.h>
#include <ostream>
#include <memory>
#include <time.h>
#include <utility>
#include <chrono>
#include <functional>

/*
 * Stores a tuple of indices. Used by tuple and pair, and by bind() to
 * extract the elements in a tuple.
 */
template<size_t... I> struct Index_tuple {};

/* Concatenates two Index_tuples. */
template<typename I1, typename I2> struct Itup_cat;

template<size_t... I1, size_t... I2>
struct Itup_cat<Index_tuple<I1...>, Index_tuple<I2...>>
{
	using type = Index_tuple<I1..., (I2 + sizeof...(I1))...>;
};

/* Builds an Index_tuple<0, 1, 2, ..., Num - 1> */
template<size_t Num>
struct Build_index_tuple : 
	Itup_cat<typename Build_index_tuple<Num / 2>::type,
	typename Build_index_tuple<Num - Num / 2>::type>
{};

template<>
struct Build_index_tuple<1>
{
	typedef Index_tuple<0> type;
};

template<>
struct Build_index_tuple<0>
{
	typedef Index_tuple<> type;
};

class Thread
{
	public:
		// Abstract base class for types that wrap arbitrary functions to be
		// invoked in the new thread of execution.
		struct State
		{
			virtual ~State();
			virtual void run() = 0;
		};
		using State_ptr = std::unique_ptr<State>;

		typedef pthread_t Native_handle_type;

		// Thread::Id
		class Id
		{
			public:
				Id() : thread_() {}
				explicit Id(Native_handle_type id): thread_(id) {}
			private:
				friend class Thread;
				
				friend class std::hash<Thread::Id>;

				friend bool operator==(const Thread::Id& l, 
						const Thread::Id& r);
				friend bool operator<(const Thread::Id& l, 
						const Thread::Id& r);

				template<typename C, typename T>
				friend std::ostream& operator<<(std::ostream& out,
						const Thread::Id& id);

				Native_handle_type thread_;
		};
		
		Thread() = default;
		Thread(Thread&) = delete;
		Thread(const Thread&) = delete;
		Thread(const Thread&&) = delete;

		Thread(Thread&& t) { swap(t); }

		Thread& operator=(const Thread&) = delete;

		template<typename Callable, typename... Args>
		explicit Thread(Callable& f, Args&&... args)
		{
			start_thread(make_state(make_invoker(std::forward<Callable>(f),
							std::forward<Args>(args)...)), nullptr);
		}

		~Thread()
		{
			if(joinable())
				std::terminate();
		}

		Thread& operator=(Thread&& t)
		{
			if(joinable())
				std::terminate();
			swap(t);
			return *this;
		}

		void swap(Thread& t) { std::swap(id_, t.id_); }

		bool joinable() const { return !(id_ == Id()); }

		void join();

		void detach();

		Thread::Id get_id() const { return id_; }

		Native_handle_type native_handle() { return id_.thread_; }

		// Returns a value that hints at the number of hardware thread 
		// contexts
		static unsigned int hardware_concurrency();
	private:
		template<typename Callable>
		struct State_impl : public State
		{
			Callable func;
			State_impl(Callable&& c) : 
				func(std::forward<Callable>(c)) {}

			void run() { func(); }
		};

		void start_thread(State_ptr, void (*)());

		template<typename Callable>
		static State_ptr make_state(Callable&& f)
		{
			using Impl = State_impl<Callable>;
			return State_ptr{new Impl{std::forward<Callable>(f)}};
		}

		// A call wrapper that does INVOKE(forwarded tuple elements...)
		template<typename T>
		struct Invoker
		{
			T t;

			template<std::size_t I, typename F>
			using tuple_element_t = typename std::tuple_element<I, F>::type;

			template<size_t Index>
			static tuple_element_t<Index, T>&& declval_s();

			template<size_t... Ind>
			auto invoke_s(Index_tuple<Ind...>)
			noexcept(noexcept(std::invoke(declval_s<Ind>()...)))
			->decltype(std::invoke(declval_s<Ind>()...))
			{
				return std::invoke(std::get<Ind>(std::move(t))...);
			}

			using Indices = 
			typename Build_index_tuple<std::tuple_size<T>::value>::type;

			auto operator()()
			noexcept(noexcept(std::declval<Invoker&>().invoke_s(Indices())))
			->decltype(std::declval<Invoker&>().invoke_s(Indices()))
			{
				return invoke_s(Indices());
			}
		};

		// Alias for Invoker<std::tuple<DECAY_COPY(T)...>>
		template<typename... T>
		using invoker_type = 
			Invoker<decltype(std::make_tuple(std::declval<T>()...))>;
	public:
		// Returns a call wrapper that does
		// INVOKE(DECAY_COPY(callable), DECAY_COPY(args)).
		template<typename Callable, typename... Args>
		static invoker_type<Callable, Args...> make_invoker(
				Callable&& callable, Args&&... args)
		{
			return {std::make_tuple(std::forward<Callable>(callable),
					std::forward<Args>(args)...)};
		}
	private:
		//pthread* thread_;
		Id id_;
};

inline void swap(Thread& l, Thread& r) { l.swap(r); }

inline bool operator==(const Thread::Id& l, const Thread::Id& r) 
{
	// pthread_equal is undefined if either thread ID is not valid, so we
	// can't safely use __gthread_equal on default-constructed values (nor
	// the noe-zero value returned by this_thread::get_id() for single-
	// threaded programs using GUN libc). Assume EqualityComparable.
	return l.thread_ == r.thread_; 
}

inline bool operator!=(const Thread::Id& l, const Thread::Id& r) { return !(l == r); }

inline bool operator<(const Thread::Id& l, const Thread::Id& r) 
{
	// Pthreads doesn't define any way to do this, so we just have to assume
	// native_handle_type is LessThanComparable.
	return l.thread_ < r.thread_;
}

inline bool operator<=(const Thread::Id& l, const Thread::Id& r) 
{ return !(l < r); }

inline bool operator>(const Thread::Id& l, const Thread::Id& r) 
{ return l > r; }

inline bool operator>=(const Thread::Id& l, const Thread::Id& r) 
{ return !(l < r); }

/* wrong
template<>
struct std::hash<Thread::Id> : public std::hash<size_t, Thread::Id>
{
	size_t operator()(const Thread::Id& id) const
	{
		return std::hash(id.thread_);
	}
};
*/

template<typename C, typename T>
inline std::ostream& operator<<(std::ostream& out, Thread::Id id)
{
	if(id == Thread::Id())
		return out << "Thread::Id of a non-executing thread";
	else
		return out << id.thread_;
}

inline Thread::Id get_id() { return Thread::Id(pthread_self()); }

inline void yield() { pthread_yield(); }

void sleep_for(std::chrono::seconds, std::chrono::nanoseconds);

/* sleep_for */
template<typename Rep, typename Period>
inline void sleep_for(const std::chrono::duration<Rep, Period>& rtime)
{
	if(rtime <= rtime.zero())
		return;
	auto s = std::chrono::duration_cast<std::chrono::seconds>(rtime);
	auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(rtime, s);

    // USE_NANOSLEEP
	struct timespec ts =
	{
		static_cast<std::time_t>(s.count()),
		static_cast<long>(ns.count())
	};

	while(nanosleep(&ts, &ts) == -1 && errno == EINTR) {}
}

/* sleep_until */
template<typename Clock, typename Duration>
inline void sleep_until(const std::chrono::time_point<Clock, Duration>& 
		atime)
{
	auto now = Clock::now();
	if(Clock::is_steady)
	{
		if(now < atime)
			sleep_for(atime - now);
		return;
	}

	while(now < atime)
	{
		sleep_for(atime, now);
		now = Clock::now();
	}
}
