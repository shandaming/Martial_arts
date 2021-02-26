/*
 * Copyright (C) 2018
 */

#ifndef GOOSE_ADAPTER_H
#define GOOSE_ADAPTER_H

#include "quackable.h"
#include "goose.h"

class Goose_adapter : public Quackable
{
	public:
		Goose_adapter(Goose* g) : goose_(g) {}

		void quack()
		{
			goose_->honk();
		}

		void register_observer(Observer* observer) override {}

		void notify_observers() override {}
	private:
		Goose* goose_;
};

#endif
