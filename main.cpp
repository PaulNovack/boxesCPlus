#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <served/served.hpp>
#include <mysqlx/xdevapi.h>
using namespace mysqlx;
using namespace std;


std::mutex l_authkey_map;
std::mutex l_users_map;
std::mutex l_user_boxes_map;
std::map<std::string, int> authKeys;
std::string userBoxes[1000000];
std::string userItems[1000000];

std::string gen_random(const int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof (alphanum) - 1)];
    }
    return tmp_s;
}

int logUserIn(Client &cli, std::string username, std::string password, std::string& authKey) {
    std::stringstream ssUsername;
    std::stringstream ssPassword;
    std::stringstream ssAuthKey;
    std::string dbusername = "";
    std::string dbpassword = "";
    int user_id = 0;


    Session sess = cli.getSession();
    Schema myDb = sess.getSchema("boxes");
    SqlResult myRows = sess.sql("SELECT id,username,password,authKey FROM users where username = ?")
            .bind(username).execute();
    for (Row row : myRows.fetchAll()) {
        user_id = row[0];
        ssUsername << row[1];
        ssPassword << row[2];
        ssAuthKey << row[3];
        authKey = ssAuthKey.str();
    }
    if (ssUsername.str() == "") {
        authKey = gen_random(32);
        Table usersTable = myDb.getTable("users");
        Result res = usersTable.insert("username", "password", "authKey")
                .values(username, password, authKey).execute();
        user_id = res.getAutoIncrementValue();
    }
    return user_id;
}

// parse boxes json..
// 1 set defaults for all fields.
// 2 split by commas
// 3 set each value from split strings



// Should refactor this won't work for any query params just for this app.

std::string getQueryParam(std::string url, std::string param) {
    std::string ret = "";
    int plen = param.length();
    int idx = url.find(param);
    int delim = url.find("&");
    int end = url.length();
    std::string val = "";
    if (end > delim) {
        ret = url.substr(idx + plen + 1, delim - (idx + plen + 1));
    } else {
        ret = url.substr(idx + plen + 1, end - (idx + plen + 1));
    }
    return ret;
}

std::string getAuthKey(std::string cookie) {
    int pos = cookie.find("authToken=");
    int delpos = cookie.find(";");
    int end = cookie.length();
    std::string h = "";
    if (pos != -1) {
        if (delpos != -1) {
            h = cookie.substr(pos + 10, delpos - 10);
        } else {
            h = cookie.substr(pos + 10, end - 10);
        }
        return h;
    } else {
        return h;
    }
}
// Gets one time from database then puts in Map and gets from map
// when actual login is complete will get key at that time and then always get
// from map afterwords

int getUserIdFromAuthKey(Client &cli, std::string authKey) {

    int user_id = 0;
    std::lock_guard<std::mutex> guard(l_authkey_map);
    std::map<std::string, int>::iterator it;
    it = authKeys.find(authKey);
    if (it != authKeys.end()) {
        user_id = it->second;
        return user_id;
    } else {
        // only get mysql session if needed slows code otherwise
        // using find in map is much faster!!
        Session sess = cli.getSession();
        SqlResult myRows = sess.sql("SELECT id FROM users where authkey = ?")
                .bind(authKey).execute();
        for (Row row : myRows.fetchAll()) {
            user_id = row[0];
        }
        if (user_id != 0) {
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
                int user_id = getUserIdFromAuthKey(cli, authKey);

                if (userBoxes[user_id] == "") {


                    SqlResult myRows = sess.sql("SELECT id,user_id, name,weight, picture,created_at FROM boxes where user_id = ?")
                            .bind(user_id).execute();
                            std::stringstream buffer;
                            bool hasRows = false;
                            buffer << "[";
                    for (Row row : myRows.fetchAll()) {
                        hasRows = true;
                                buffer << "{";
                                buffer << "\"id\":" << "\"" << row[0] << "\",";
                                buffer << "\"user_id\":" << row[1] << ",";
                                buffer << "\"name\":" << "\"" << row[2] << "\",";
                                buffer << "\"weight\":" << "\"" << row[3] << "\",";
                                buffer << "\"picture\":" << "\"" << row[4] << "\"";
                                buffer << "},";
                    }
                    if (hasRows) {
                        buffer.seekp(-1, std::ios_base::end);
                    }
                    buffer << "]";
                            userBoxes[user_id] = buffer.str();
                }
                res << userBoxes[user_id];
            });
    mux.handle("/boxes{id}")

            .get([&](served::response &res, const served::request & req) {
                std::string name;
                std::string header = req.header("cookie");
                std::string authKey = getAuthKey(header);
                std::string box_id_str = req.params["id"];
                int box_id = stoi(box_id_str);
                Session sess = cli.getSession();
                int user_id = getUserIdFromAuthKey(cli, authKey);

                if(userItems[box_id] != "") {
                        // check user owns
                } else {
                    SqlResult myRows =
                            sess.sql("SELECT id,user_id,box_id, name,quantity, picture,created_at FROM items where user_id = ? and box_id = ?")
                            .bind(user_id, box_id).execute();
                            std::stringstream buffer;
                            bool hasRows = false;
                            buffer << "[";
                    for (Row row : myRows.fetchAll()) {
                        hasRows = true;
                                buffer << "{";
                                buffer << "\"id\":" << "\"" << row[0] << "\",";
                                buffer << "\"user_id\":" << row[0] << ",";
                                buffer << "\"box_id\":" << row[1] << ",";
                                buffer << "\"name\":" << "\"" << row[2] << "\",";
                                buffer << "\"quantity\":" << row[3] << ",";
                                buffer << "\"picture\":" << "\"" << row[4] << "\"";
                                buffer << "},";
                    }
                    if (hasRows) {
                        buffer.seekp(-1, std::ios_base::end);
                    }
                    buffer << "]";
                            userItems[user_id] = buffer.str();
                }
                res << userItems[user_id];
            });
    mux.handle("/login")
            .get([&](served::response &res, const served::request & req) {
                std::stringstream buffer;
                int user_id = 0;
                std::string url = req.url().query();
                std::string username = getQueryParam(url, "username");
                std::string password = getQueryParam(url, "password");
                std::string authKey = "";
                if (password != "" && username != "") {
                    user_id = logUserIn(cli, username, password, authKey);
                            res.set_header("content-type", "application/json");
                            time_t now = time(0);
                            // cookies are in GMT time.
                            tm *ltm = gmtime(&now);
                            ltm->tm_min = ltm->tm_min + 10;
                            std::string timeReadable = "";
                            char buffer[80];
                            strftime(buffer, 80, "%a, %e %h %G %T GMT", ltm);
                            timeReadable = buffer;
                            std::string cookie = "authToken=" + authKey + ";SameSite=Strict;Expires=" + timeReadable;
                            res.set_header("Set-Cookie", cookie);
                    if (authKey == "") {
                        // bad password
                    }
                } else {
                    // missing username and password message
                }
                buffer << "{";
                buffer << "\"user_id\":" << "\"" << user_id << "\",";
                buffer << "\"authToken\":" << "\"" << authKey << "\"";
                buffer << "}";
                res << buffer.str();
            });
    mux.handle("/logout")
            .get([&](served::response &res, const served::request & req) {
                std::stringstream buffer;
                std::string timeReadable = "";
                time_t now = time(0);
                tm *ltm = localtime(&now);
                char buf[80];
                strftime(buf, 80, "%a, %e %h %G %T GMT", ltm);
                buffer << buf;
                // not for certain but set cookie to expire now and value deleted just
                // in case users browser does not allow you to serverside delete a 
                // cookie by an expiration some browsers supposedly do not?
                std::string cookie = "authToken=deleted;SameSite=Strict;Expires=" + buffer.str();
                res.set_header("Set-Cookie", cookie);
                res << "{\"logout\": \"true\"}";
            });
    served::net::server server("0.0.0.0", "8123", mux);
    cout << "Server Up...." << endl;
    server.run(50);

    return (EXIT_SUCCESS);
}