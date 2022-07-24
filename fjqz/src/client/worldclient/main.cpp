#include <filesystem>
#include <thread>
#include "video.h"
#include "config.h"

namespace fs = std::filesystem;

#define CONFIG_FILE "jy.json"


int main(int argc, char* argv[])
{
    auto config_file = fs::absolute(CONFIG_FILE);
	CONFIG_MGR->load_init(config_file);

	VIDEO->init_window();

	while(1)
	{
		video::delay(1200);
	}

    return 0;
}
