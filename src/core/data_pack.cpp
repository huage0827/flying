#include "data_pack.h"

namespace flyingspore{
    const data_value& data_pack::operator[](std::string _path)
    {
        //mapping path to index and find it
        return data_value();
    }

    const data_value& data_pack::operator[](char* _path)
    {
        if(!_path)
            return data_value();
        return (*this)[std::string(_path)];
    }

}

