#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>


#ifndef BOX_H
#define BOX_H
using namespace std;


struct mBox {
    int id;
    int user_id;
    std::string name;
    float weight;
    std::string picture;
};

class Box {
public:
    Box();
    Box(const Box& orig);
    std::string toJson(mBox box);
    virtual ~Box();
private:

};

#endif /* BOX_H */

