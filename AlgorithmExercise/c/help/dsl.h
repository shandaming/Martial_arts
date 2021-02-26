/*
 * Copyright (C)
 */

#ifndef DSL_H
#define DSL_H

template<typename D>
struct state_machine
{
	template<typename E>
	int process_event(const E& e)
	{
		using disptcher = typename dispatcher_generator<typename D::tranition_table, E>::result;
		this->state = dispatcher::dispatch(*static_cast<D*>(this), this->state, e);
		return this->state;
	}

	template<typename E>
	int under_friend(int state, const E& e)
	{
		return state;
	}

protected:
	template<int C, typename E, int N, void(D::*action)(const E&)>
	struct row
	{
		enum
		{
			current = current_state,
			next = next_state
		};

		using fsm  = D;
		using E = E;
		static void execute(fsm& fsm, const E& e)
		{
			(fsm.*action)(e);
		}
	};

protected:
	state_machine():state(D::initial) {}
private:
	int state;
};

template<typename T, typename N>
struct event_dispatcher
{
	using fsm = typename transition::fsm;
	using event = typename transition::event;

	static int dispatch(fsm& f, int state, const event& e)
	{
		if(state == transition::current)
		{
			transition::execute(fsm, e);
			return translte::next;
		}
		else
		{
			return newx::dispatch(fsm, state, e);
		}
	}
};

template<typename E, typename T>
struct event_matcher
{
	using result = is_sq(E, typename T::Event);
};

template<typename Table, typename Event>
struct dispatcher_generator
{
publid:
	using result = fold(matcher_transitions, defult_dispatcher, event_dispatcher);
private:
	template<typename Transition>
	using transition_matcher = typename Event_matcher<Event, Transition>::result;

	using matcher_transition = filter(table, transition_matcher);
};

struct open
{};

struct close
{};

struct play
{
	string name;
};

struct stop
{};

struct dance_robot : state_machine<dance_robot>
{
private:
	friend struct state_machine<dance_robot>;

	enum states
	{
		closed, opened, dancing, initial = closed
	};

	void say_ready(const open&)
	{
		std::cout << "say" << std::endl;
	};

	void say_closed(const closed&)
	{
		std::cout << "closed" << std::endl;
	}

	void sya_stop(const stop&)
	{
		std::cout << "stop" << std::endl;
	}

	void do_dance(const play& play_info)
	{
		std::cout << "dance" << std::endl;
	}

	using R = dance_robot;
	using transition_talbe = type_list<
		row<closed, open, opened, R::say_ready>,
		row<opened, close, closed, R::say_closed>,
		row<opened, play, dancing, R::do_dance>,
		row<dancing, stop, opened, R::say_stoped>,
		row<dancing, close, closed, R::sya_closed>
		>::result;
};

#endif
