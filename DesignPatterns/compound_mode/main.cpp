/*
 * Copyright (C) 2018
 */

#include "quack_counter.h"
#include "goose_adapter.h"
#include "duck_factory.h"

void simulate(Quackable* duck)
{
	duck->quack();
}

int main()
{
        Quackologist* quackologist = new Quackologist();

	Abstract_duck_factory* factory = new Counting_duck_factory();

	Quackable* mallard_duck = factory->create_mallard_duck();
        mallard_duck->quack();
        mallard_duck->register_observer(quackologist);

	Quackable* redhead_duck = factory->create_redhead_duck();
        mallard_duck->quack();
        redhead_duck->register_observer(quackologist);

	Quackable* duck_call = factory->create_duck_call();
        mallard_duck->quack();
        duck_call->register_observer(quackologist);

	Quackable* rubber_duck = factory->create_rubber_duck();
        mallard_duck->quack();
        rubber_duck->register_observer(quackologist);

	Quackable* goose_duck = new Goose_adapter(new Goose());

	Flock* flock = new Flock();
	flock->add(mallard_duck);
	flock->add(redhead_duck);
	flock->add(duck_call);
	flock->add(rubber_duck);
	flock->add(goose_duck);

	std::cout << "Duck Simulater: With Decorator\n";

	simulate(flock);

	std::cout << "The ducks quacked \"" <<
		Quack_counter::get_quacks() <<
		"\" times\n";
}
