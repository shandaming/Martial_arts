/*
 * Copyright (C) 
 */

#ifndef TV_H
#define TV_H

#include <iostream>

class Tv
{
	public:
		virtual void on() = 0;
		virtual void off() = 0;
		virtual void turn_channel() = 0;
};

class Rca_tv : public Tv
{
	public:
		void on() override { std::cout << "ON RCA TV\n"; }
		void off() override { std::cout << "OFF RCA TV\n"; }
		void turn_channel() override 
		{ 
			std::cout << "Turn channe of RCA TV\n"; 
		}
};

class Sony_tv : public Tv
{
	public:
		void on() override { std::cout << "ON SONY TV\n"; }
		void off() override { std::cout << "OFF SONY TV\n"; }
		void turn_channel() override 
		{
			std::cout << "Turn channel of the SONY TV\n"; 
		}
};

#endif
