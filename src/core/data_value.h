#ifndef flying_spore_data_value_h
#define flying_spore_data_value_h
#include <string>

namespace flyingspore{
    class data_value{
    public:
        bool as_bool();
        char as_char();
        unsigned char as_byte();
        short as_short();
        long as_long();
        long long as_longlong();
        std::string as_string();
    };
}



#endif/*flying_spore_data_value_h*/