#include "Item.h"


Item::Item() {
}

Item::Item(const Item& orig) {
}

std::string toJson(mItem item){
    //id,user_id,name,weight, picture
    std::stringstream buffer;
    buffer << "{";
    if(item.id != 0){
        buffer << "\"id\":" << item.id << ",";
    }
    if(item.user_id != 0){
         buffer << "\"user_id\":" << item.user_id << ",";       
    }
    if(item.box_id != 0){
         buffer << "\"box_id\":" << item.box_id << ",";       
    }
    if(item.name != ""){
         buffer << "\"name\":" << item.name << ",";       
    }
    
    if(item.quantity != 0){
         buffer << "\"quantity\":" << item.quantity << ",";       
    }
    if(item.picture != ""){
         buffer << "\"name\":" << item.picture << ",";       
    }
    buffer.seekp(-1, std::ios_base::end);
    buffer << "}";
    return buffer.str();
}

Item::~Item() {
}

