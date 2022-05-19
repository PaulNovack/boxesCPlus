#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>
/* 
 * File:   Box.h
 * Author: pnovack
 *
 * Created on May 19, 2022, 12:34 PM
 */

#ifndef BOX_H
#define BOX_H
using namespace std;

class Box {
public:
    Box();
    Box(const Box& orig);
    virtual ~Box();
private:
    std::mutex l_authkey_map;
    std::mutex l_users_map;
    std::mutex l_user_boxes_array;
    std::mutex l_box_items_array;
    std::map<std::string, int> authKeys;
    std::string userBoxes[1000000];
    std::string boxItems[1000000];
};

#endif /* BOX_H */

