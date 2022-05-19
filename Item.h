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

class Item {
public:
    Item();
    Item(const Item& orig);
    virtual ~Item();
private:
    std::string items[1000000];
    std::mutex l_items_array;
};

#endif /* ITEM_H */

