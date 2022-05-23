#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>


/* 
 * File:   Item.h
 * Author: pnovack
 *
 * Created on May 19, 2022, 12:34 PM
 */

#ifndef ITEM_H
#define ITEM_H



struct mItem {
    int id;
    int user_id;
    int box_id;
    std::string name;
    int quantity;
    std::string picture;
};


class Item {
public:
    Item();
    Item(const Item& orig);
    std::string toJson(mItem item);
    virtual ~Item();
private:

};

#endif /* ITEM_H */

