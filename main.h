
#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>



struct mBox {
    int id;
    int user_id;
    std::string name;
    float weight;
    std::string picture;
};

struct mItem {
    int id;
    int user_id;
    int box_id;
    std::string name;
    int quantity;
    std::string picture;
};





#endif /* MAIN_H */

