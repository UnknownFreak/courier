#include <iostream>

#include <courier/courier.hpp>

#include "bench.hpp"
#include "sample.hpp"

int main()
{
	std::cout << "Courier version: " << courier::getVersion() << std::endl;

	std::cout << "Select operation mode:" << std::endl;
	std::cout << "[1] - Run sample application" << std::endl;
	std::cout << "[2] - Run sample benchmark" << std::endl;
	std::cout << "[any other key] - Exit" << std::endl;
	std::string input;
	std::getline(std::cin, input);

	if (input.length() > 0)
	{
		if (input[0] == '1')
		{
			sample::run();
		}
		else if (input[0] == '2')
		{
			bench::run();
		}
	}
}