#include "sample.hpp"
#include "menu.h"
#include <sample_delete_timer/run.h>
#include <iostream>

namespace sample
{

	void run()
	{
		std::string input;
		{
			menu::showMenu("Sample application configuration",
			{
				"Run timer sample",
			},
		"[any other key] - Go back to main menu");

			std::getline(std::cin, input);

			if (input.length() > 0)
			{
				if (input[0] == '1')
				{
					std::cout << "Timer sample selected" << std::endl;
					timer::run();
				}
			}
		}
	}
}
