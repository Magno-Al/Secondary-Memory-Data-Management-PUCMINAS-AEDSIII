#ifndef MODULE_H
#define MODULE_H

#include <cstdint>
#include <cstring>

struct Module
{
    bool removed;
    int _id;
    int course_id;
    int teacher_id;
    char name[30];

    Module()
    {
        removed = false;
        _id = -1;
        course_id = -1;
        teacher_id = -1;
        memset(name, 0, sizeof(name));
    }
};

#endif