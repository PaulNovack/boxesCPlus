
/* 
 * File:   Box.h
 * Author: pnovack
 *
 * Created on May 19, 2022, 12:34 PM
 */

#ifndef BOX_H

#define BOX_H
#include "main.h"
using namespace std;

class Box {
public:
    Box();
    Box(const Box& orig);
    std::string toJson(mBox box);
    virtual ~Box();
private:

};

#endif /* BOX_H */

