#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>
#include "main.h"

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
    std::string toJson(mItem item);
    virtual ~Item();
private:

};

#endif /* ITEM_H */

