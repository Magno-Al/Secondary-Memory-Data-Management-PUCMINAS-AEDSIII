#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <cstring>

struct Student {
    bool removed;
    int _id;
    int user_id;
    char name[50];
    long birth_date;

    Student() {
        removed = false;
        _id = -1;
        user_id = -1;
        birth_date = 0;
        memset(name, 0, sizeof(name));    
    }
};

#endif