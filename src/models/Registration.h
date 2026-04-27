#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <cstdint>

struct Registration
{
    bool removed;
    int _id;
    int student_id;
    int module_id;
    int64_t date;
    bool approved;
    bool revised;

    Registration()
    {
        removed = false;
        _id = -1;
        student_id = -1;
        module_id = -1;
        date = 0;
        revised = false;
        approved = false;
    };
};

#endif