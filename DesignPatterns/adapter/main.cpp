/*
 * Copyright (C) 2018
 */

#include "turkey_adapter.h"

int main()
{
	Duck* duck = new Mallard_duck();

	Turkey* turkey = new Wild_turkey();
        Duck* turkey_adapter = new Turkey_adapter(turkey);

	std::cout << "The Turkey says...\n";
	turkey->gobble();
	turkey->fly();

	std::cout << "The Duck says...\n";
	duck->quack();
	duck->fly();

	std::cout << "The Turkey_adapter says...\n";
	turkey_adapter->quack();
	turkey_adapter->fly();
}
