/* Copyright 2008, 2010, Oracle and/or its affiliates.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Standard C++ includes */
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
    cout << "Server Up...." << endl;
    server.run(50);

    return (EXIT_SUCCESS);
}