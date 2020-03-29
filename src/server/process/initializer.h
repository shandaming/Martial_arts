/*
 * Copyright (C) 2019
 */

#ifndef PRO_INITIALZER_H
#define PRO_INITIALZER_H

struct initializer_base
{
	template<typename T>
	void on_fork_setup(T&) const {}

	template<typename T>
	void on_fork_error(T&) const {}

	template<typename T>
	void on_fork_success(T&) const {}

	template<typename T>
	void on_exec_setup(T&) const {}

	template<typename T>
	void on_exec_error(T&) const {}
};

#endif
