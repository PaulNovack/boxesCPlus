#include <stdlib.h>
#include <iostream>
#include <iostream>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>
using namespace mysqlx;
using namespace std;


std::mutex l_authkey_map;
std::map<std::string, int> authKeys;



std::string getAuthKey(std::string cookie){
    int pos = cookie.find("authToken=");
     std::string h = "";
    if(pos != -1){
        h = cookie.substr(pos+10,cookie.length()-10);
        return h;    
    } else {
        return h;
    }
}
int getUserIdFromAuthKey(Client &cli,std::string authKey){
    Session sess = cli.getSession();
     //std::lock_guard<std::mutex> guard(l_authkey_map);
     int user_id = 0;
     std::map<std::string, int>::iterator it;
     it = authKeys.find(authKey);
     if(it != authKeys.end()){
       user_id = it->second; 
       return user_id;
     } else {
        SqlResult myRows = sess.sql("SELECT id FROM users where authkey = ?")
                .bind(authKey).execute();
        for (Row row : myRows.fetchAll()) {
            user_id = row[0];
        }
        if(user_id != 0){
            authKeys.insert(std::make_pair(authKey, user_id));

        }

        return user_id;
     }
}




int main(void) {
    Client cli("boxes:boxes@127.0.0.1/boxes", ClientOption::POOL_MAX_SIZE, 60);
    // use Session sess as usual
    served::multiplexer mux;
    mux.handle("/boxes")
            .get([&](served::response &res, const served::request & req) {
                std::string name;
                std::string header = req.header("cookie");
                std::string authKey = getAuthKey(header);
 
     
                Session sess = cli.getSession();
                int user_id = getUserIdFromAuthKey(cli,authKey);
                
                
                SqlResult myRows = sess.sql("SELECT user_id, name,weight, picture,created_at FROM boxes where user_id = ?")
                        .bind(user_id).execute();
                std::stringstream buffer;
                bool hasRows = false;
                buffer << "[";
                for (Row row : myRows.fetchAll()) {
                    hasRows = true;
                    buffer << "{";
                    buffer << "\"user_id\":" << "\"" << row[0] << "\",\n";
                    buffer << "\"name\":" << "\"" << row[1] << "\",\n";
                    buffer << "\"weight\":" << "\"" << row[2] << "\",\n";
                    buffer << "\"picture\":" << "\"" << row[3] << "\"\n";
                    buffer << "},";
                }
                if(hasRows)buffer.seekp(-1, std::ios_base::end);
                buffer << "]";
                name = buffer.str();
                res.set_header("content-type", "application/json");
                res.set_header("Set-Cookie", "authToken=asdfghjkloiuytrewhkderwthysgedor;SameSite=Strict;Expires=Sat, Oct 15, 2022 00:00:00 GMT");
                res << name;
            });
            
    served::net::server server("0.0.0.0", "8123", mux);
    cout << "Server Up...." << endl;
    server.run(50);

    return (EXIT_SUCCESS);
}