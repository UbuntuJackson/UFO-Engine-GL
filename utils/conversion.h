#pragma once
#include <sstream>
#include <string>
#include <iostream>

namespace ufo{

    std::string MemoryAddressToString(const void* _address){
        std::ostringstream get_address;
        get_address << _address;
        std::string address_as_string = get_address.str();

        return address_as_string;

    }

}
