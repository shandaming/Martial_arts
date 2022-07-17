#include <filesystem>
#include <thread>
#include "video.h"

namespace fs = std::filesystem;

#define CONFIG_FILE "jy.json"


int main(int argc, char* argv[])
{
    auto config_file = fs::absolute(CONFIG_FILE);

	VIDEO->init_window();

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::minutes(1));
	}

    return 0;
}
