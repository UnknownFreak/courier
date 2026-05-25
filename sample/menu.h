#pragma once

#include <string_view>
#include <vector>

namespace menu
{
    void showMenu(std::string_view title, const std::vector<std::string_view>& menuItems, std::string_view extra);    
}