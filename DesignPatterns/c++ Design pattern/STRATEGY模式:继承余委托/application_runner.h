/*
 * Copyright (C) 2017 by Shan daming
 */

#ifndef APPLICATION_RUNNER_H
#define APPLICATION_RUNNER_H

class Application_runner
{
	public:
		Application_runner(Application* app) : app_(app)
		{}

		void run()
		{
			app_.init();
			while(!app_.done())
				app_.idle();
			app_.cleanup();
		}
	private:
		Application* app_;
};

#endif
