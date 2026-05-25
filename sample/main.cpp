#include <iostream>

#include <courier/courier.hpp>

#include "bench.hpp"
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
				"Run sample benchmark (new menu)"
			}, "[any other key] - Exit");

		std::getline(std::cin, input);

		if (input.length() > 0)
		{
			if (input[0] == '1')
			{
				std::cout << "Sample selected" << std::endl;
				sample::run();
			}
			else if (input[0] == '2')
			{
				std::cout << "Benchmark selected" << std::endl;
				bench::run();
			}
			else 
			{
				running = false;
			}
		}
	}
}