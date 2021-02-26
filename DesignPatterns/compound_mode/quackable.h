/*
 * Copyright (C) 2018
 */

#ifndef QUACKABLE_H
#define QUACKABLE_H

#include "observable.h"

class Quackable : public Quack_observable
{
	public:
		virtual void quack() =0;
		virtual void register_observer(Observer* observer) =0;

		virtual void notify_observers() = 0;
};

class Mallard_duck : public Quackable
{
	public:
                Mallard_duck() : observable_(new Observable(this)) {}

		void quack() override
		{
			std::cout << "Quack\n";
                        notify_observers();
		}

                void register_observer(Observer* observer) override
                {
                        observable_->register_observer(observer);
                }

                void notify_observers() override
                {
                        observable_->notify_observers();
                }
        private:
                Observable* observable_;
};

class Redhead_duck : public Quackable
{
	public:
                Redhead_duck() : observable_(new Observable(this)) {}

		void quack() override
		{
			std::cout << "Quack\n";
                        notify_observers();
		}

                void register_observer(Observer* observer) override
                {
                        observable_->register_observer(observer);
                }

                void notify_observers() override
                {
                        observable_->notify_observers();
                }
        private:
                Observable* observable_;
};

class Duck_call : public Quackable
{
	public:
                Duck_call() : observable_(new Observable(this)) {}

		void quack() override
		{
			std::cout << "Quack\n";
                        notify_observers();
		}

                void register_observer(Observer* observer) override
                {
                        observable_->register_observer(observer);
                }

                void notify_observers() override
                {
                        observable_->notify_observers();
                }
        private:
                Observable* observable_;
};

class Rubber_duck : public Quackable
{
	public:
                Rubber_duck() : observable_(new Observable(this)) {}

		void quack() override
		{
			std::cout << "Squeak\n";
                        notify_observers();
		}

                void register_observer(Observer* observer) override
                {
                        observable_->register_observer(observer);
                }

                void notify_observers() override
                {
                        observable_->notify_observers();
                }
        private:
                Observable* observable_;
};

#endif
