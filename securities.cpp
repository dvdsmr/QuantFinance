#include "securities.h"
#include "sdes.h"
#include <string_view>
#include <type_traits>
#include <iostream>

// for later definitions of more complex member functions
namespace Securities
{
    void Stock::printInfo() const
    {
        std::cout << "Common stock with initial spot price of $" << getSpot() << ".";
    }
   
}
