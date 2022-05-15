#include <stdlib.h>
#include <iostream>
#include <iostream>
#include <served/served.hpp>


#include <mysqlx/xdevapi.h>

using namespace mysqlx;




using namespace std;


int main(void) {

    Client cli("boxes:boxes@127.0.0.1/boxes", ClientOption::POOL_MAX_SIZE, 60);
    // use Session sess as usual
    served::multiplexer mux;
    mux.handle("/boxes")
            .get([&](served::response &res, const served::request & req) {
                std::string name;
               
                Session sess = cli.getSession();
                SqlResult myRows = sess.sql("SELECT user_id, name,weight, picture,created_at FROM boxes where user_id = 1").execute();
                std::stringstream buffer;
                buffer << "[";
                for (Row row : myRows.fetchAll()) {
                    buffer << "{";
                    buffer << "\"user_id\":" << "\"" << row[0] << "\",\n";
                    buffer << "\"name\":" << "\"" << row[1] << "\",\n";
                    buffer << "\"weight\":" << "\"" << row[2] << "\",\n";
                    buffer << "\"picture\":" << "\"" << row[3] << "\"\n";
                    buffer << "},";
                }
                buffer.seekp(-1, std::ios_base::end);
                buffer << "]";
                name = buffer.str();
                res.set_header("content-type", "application/json");
                res << name;
            });
    served::net::server server("127.0.0.1", "8123", mux);
    server.run(50);

    return (EXIT_SUCCESS);
}
