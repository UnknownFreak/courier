#include <iostream>

#include <courier/courier.hpp>

#include "menu.h"
#include "sample.hpp"

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;

	std::string input;
	bool running = true;
	while(running)
	{
		menu::showMenu("Select operation mode:", 
			{
				"Run sample application (new menu)",
			}, "[any other key] - Exit");

		std::getline(std::cin, input);

		if (input.length() > 0)
		{
			if (input[0] == '1')
			{
				std::cout << "Sample selected" << std::endl;
				sample::run();
			}
			else 
			{
				running = false;
			}
		}
		else
		{
			running = false;
		}
	}
}