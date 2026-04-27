#ifndef PHONE_H
#define PHONE_H

#include <cstring>

struct Phone
{
    bool removed;
    int _id;
    int user_id;
    short country_code;
    short area_code;
    char number[15];

    Phone()
    {
        removed = false;
        _id = -1;
        user_id = -1;
        country_code = -1;
        area_code = -1;
        memset(number, 0, sizeof(number));
    }
};

#endif