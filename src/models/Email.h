#ifndef EMAIL_H
#define EMAIL_H

#include <cstring>

struct Email {
    bool removed;
    int _id;
    int user_id;
    char email_address[50];

    Email() {
        removed = false;
        _id = -1;
        user_id = -1;
        memset(email_address, 0, sizeof(email_address));
    }
};

#endif