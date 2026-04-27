#ifndef USER_H
#define USER_H

#include <iostream>
#include <cstring>

struct User {
    bool removed;
    int _id;
    char username[50];
    char password[50];
    char type[10];

    User() {
        removed = false;
        _id = -1;
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        memset(type, 0, sizeof(type));    
    }
};

#endif