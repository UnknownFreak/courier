#include "menu.h"

#include <iostream>
namespace menu
{

    void showMenu(std::string_view title, const std::vector<std::string_view> &menuItems, std::string_view extra)
    {
        std::cout << title << std::endl;
        size_t i =0;
        for(;i < menuItems.size(); i++)
        {
            std::cout << "[" << i+1 << "] " << menuItems[i] << std::endl;
        }
        std::cout << extra << std::endl;
    }

}