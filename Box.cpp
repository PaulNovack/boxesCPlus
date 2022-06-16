#include "Box.h"

Box::Box() {
}

Box::Box(const Box& orig) {
}

Box::~Box() {
}

std::string Box::toJson(mBox box){
    //id,user_id,name,weight, picture
    std::stringstream buffer;
    buffer << "{";
    if(box.id != 0){
        buffer << "\"id\":" << box.id << ",";
    }
    if(box.user_id != 0){
         buffer << "\"user_id\":" << box.user_id << ",";       
    }
    if(box.name != ""){
         buffer << "\"name\":" << "\"" << box.name << "\",";       
    }
    
    if(box.weight != 0){
         buffer << "\"weight\":" << box.weight << ",";       
    }
    if(box.picture != ""){
         buffer << "\"picture\":" << box.picture << ",";       
    }
    buffer.seekp(-1, std::ios_base::end);
    buffer << "}";
    return buffer.str();
}

