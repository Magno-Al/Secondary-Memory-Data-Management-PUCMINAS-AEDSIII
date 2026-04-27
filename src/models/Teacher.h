#ifndef TEACHER_H
#define TEACHER_H

#include <iostream>
#include <cstring>

struct Teacher
{
    bool removed;
    int _id;
    int user_id;
    char name[50];
    long birth_date;
    char expertise[50];

    Teacher()
    {
        removed = false;
        _id = -1;
        user_id = -1;
        birth_date = 0;
        memset(name, 0, sizeof(name));
        memset(expertise, 0, sizeof(expertise));
    }
};

#endif