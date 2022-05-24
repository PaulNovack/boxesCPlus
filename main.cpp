#include "main.h"

using namespace mysqlx;
using namespace std;



// TODO: on PUT item or POST item set item[id] to empty string
//         then set boxItems[box_id] to empy string also next get sql select will be called and cache updated

// TODO: on PUT or POST a box change name etc set boxItems[box_id] to empty string
//          then set userBoxes[user_id] to empty string next get sql select will be called and cache updated

std::mutex l_authkey_map;
std::mutex l_users_map;
std::mutex l_user_boxes_array;
std::mutex l_box_items_array;

std::map<std::string, int> authKeys;
std::string userBoxes[1000000];
std::string boxItems[1000000];


std::string cDom;

Box *boxObj;
Item *itemObj;

mBox boxJsonToStruct(std::string s) {
    mBox box;
    box.id = 0;
    box.user_id = 0;
    box.name = "";
    box.weight = 0;
    box.picture = "";
    std::string::size_type key_pos = 0;
    std::string::size_type key_end;
    std::string::size_type val_pos;
    std::string::size_type val_end;
    int pos = 0;
    bool inQuote = false;
    std::string temp;
    for (int pos = 0; pos < s.length(); pos++) {
        if (s[pos] == '\"') {
            inQuote = !inQuote;
        }
        if (inQuote && s[pos] != '{' && s[pos] != '}' && s[pos] != '\n') {
            temp = temp + s[pos];
        } else if (!inQuote && s[pos] != ' ' && s[pos] != '{' && s[pos] != '}' && s[pos] != '\n') {
            temp = temp + s[pos];
        }
    }
    s = temp;
    bool inKeyValue = false;
    bool gotKey = false;
    bool gotValue = false;
    std::string key = "";
    std::string value = "";
    for (int pos = 0; pos < s.length(); pos++) {
        if (s[pos] == '\"') {
            if (inKeyValue) {
                gotKey = true;
            }
            inKeyValue = !inKeyValue;
            continue;
        }
        if (inKeyValue && value == "" && gotKey == false) {
            key = key + s[pos];
        }
        if (gotKey == true && s[pos] != ':' && s[pos] != ',') {
            value = value + s[pos];
        }
        if (s[pos] == ',' || s[pos] == '\n') {
            gotValue = true;
        }
        if (gotValue) {
            if (key == "id") {
                box.id = stoi(value);
                ;
            }
            if (key == "user_id") {
                box.user_id = stoi(value);
                ;
            }
            if (key == "name") {
                box.name = value;
            }
            if (key == "picture") {

                box.picture = value;
            }
            if (key == "weight") {

                box.weight = stof(value);
            }
            inKeyValue = false;
            gotKey = false;
            gotValue = false;
            key = "";
            value = "";
        }
    }
    return box;
}

mItem itemJsonToStruct(std::string s) {
    mItem item;
    item.name = "";
    item.quantity = 0;
    item.box_id = 0;
    item.user_id = 0;
    item.picture = "";
    std::string::size_type key_pos = 0;
    std::string::size_type key_end;
    std::string::size_type val_pos;
    std::string::size_type val_end;
    int pos = 0;
    bool inQuote = false;
    std::string temp;
    for (int pos = 0; pos < s.length(); pos++) {
        if (s[pos] == '\"') {
            inQuote = !inQuote;
        }
        if (inQuote && s[pos] != '{' && s[pos] != '}' && s[pos] != '\n') {
            temp = temp + s[pos];
        } else if (!inQuote && s[pos] != ' ' && s[pos] != '{' && s[pos] != '}' && s[pos] != '\n') {
            temp = temp + s[pos];
        }
    }
    s = temp;
    bool inKeyValue = false;
    bool gotKey = false;
    bool gotValue = false;
    std::string key = "";
    std::string value = "";
    for (int pos = 0; pos < s.length(); pos++) {
        if (s[pos] == '\"') {
            if (inKeyValue) {
                gotKey = true;
            }
            inKeyValue = !inKeyValue;
            continue;
        }
        if (inKeyValue && value == "" && gotKey == false) {
            key = key + s[pos];
        }
        if (gotKey == true && s[pos] != ':' && s[pos] != ',') {
            value = value + s[pos];
        }
        if (s[pos] == ',' || s[pos] == '\n') {
            gotValue = true;
        }
        if (gotValue) {
            if (key == "id") {
                item.id = stoi(value);
                ;
            }
            if (key == "name") {
                item.name = value;
            }
            if (key == "quantity") {
                item.quantity = stoi(value);
            }
            if (key == "box_id") {
                item.box_id = stoi(value);
            }
            if (key == "picture") {

                item.picture = value;
            }
            inKeyValue = false;
            gotKey = false;
            gotValue = false;
            key = "";
            value = "";
        }
    }
    return item;
}

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

std::map<std::string, std::string> parseCookies(std::string const& s) {
    std::map<std::string, std::string> m;

    std::string::size_type key_pos = 0;
    std::string::size_type key_end;
    std::string::size_type val_pos;
    std::string::size_type val_end;

    while ((key_end = s.find('=', key_pos)) != std::string::npos) {
        if ((val_pos = s.find_first_not_of(";", key_end)) == std::string::npos)
            break;

        val_end = s.find(';', val_pos);
        m.emplace(s.substr(key_pos, key_end - key_pos), s.substr(val_pos + 1, val_end - val_pos - 1));

        key_pos = val_end;

        if (key_pos != std::string::npos)
            ++key_pos;
    }

    return m;
}
// replace this with a proper cookie parser !

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
    Session sess = cli.getSession();
    // use Session sess as usual
    cDom = "http://192.168.86.45:8123/";
    served::multiplexer mux;
    mux.handle("/item/{id}")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                std::string name;
                std::string item = "";
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string item_id_str = req.params["id"];
                int item_id = stoi(item_id_str);

                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";

                } else {
                    {
                        // this is copy?  Safe ?  no lock if have to fetch with sql....

                        //comment out the lock guards and see what happens to cout
                        //cout is not thread safe you will get messages have written all mixed up
                        // Enter and exit and the endl all out of order must mutex cout
                        std::lock_guard<std::mutex> guard(l_box_items_array);
                                item = boxItems[item_id];
                                //  cout << "Enter" << endl;
                    }
                    // {
                    //     std::lock_guard<std::mutex> guard(l_box_items_array);
                    //     cout << "Exit" << endl;
                    //}

                    if (item != "") {
                        std::string search = "\"id\":";
                                search = search + item_id_str;
                                search = search + ",";
                                search = search + "\"user_id\":";
                                search = search + std::to_string(user_id);
                                int pos = item.find(search);
                        if (pos == -1) {
                            // if we can not match user_id and box_id in the json we do not own box here trying to 
                            // access someone elses box and items
                            user_id = 0;
                                    res << "[]";
                        }
                    } else {
                        Session sess = cli.getSession();
                                cout << "running sql " << endl;
                                SqlResult myRows =
                                sess.sql("SELECT id,user_id,box_id, name,quantity, picture,created_at FROM items where user_id = ? and id = ?")
                                .bind(user_id, item_id).execute();
                                std::stringstream buffer;
                                bool hasRows = false;
                                buffer << "[";
                        for (Row row : myRows.fetchAll()) {
                            hasRows = true;
                                    buffer << "{";
                                    buffer << "\"id\":" << row[0] << ",";
                                    buffer << "\"user_id\":" << row[1] << ",";
                                    buffer << "\"box_id\":" << row[2] << ",";
                                    buffer << "\"name\":" << "\"" << row[3] << "\",";
                                    buffer << "\"quantity\":" << row[4] << ",";
                                    buffer << "\"picture\":" << "\"" << row[5] << "\"";
                                    buffer << "},";
                        }
                        if (hasRows) {
                            buffer.seekp(-1, std::ios_base::end);
                        }
                        buffer << "]";
                                item = buffer.str();
                        {

                        }
                    }
                    if (user_id != 0) {
                        res << item;
                    }
                }
            }).put([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                mItem item;
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string item_id_str = req.params["id"];
                int item_id = stoi(item_id_str);

                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                } else {
                    Session sess = cli.getSession();
                            item = itemJsonToStruct(req.body());
                            //we want to allow update of any combination of 
                            // quantity != 0, name != "" and picture != ""
                            // if 0 or in other fields empty string we leave
                            //existing value in db using case like this is one way to do
                            // without having to create multiple sql statements
                            // depending on what fields were passed to be updated
                            auto result = sess.sql("UPDATE items "
                            "    SET quantity = CASE "
                            "	WHEN ? = 0 "
                            "	THEN quantity "
                            "	ELSE  ? "
                            "    END "
                            "    ,name = CASE "
                            "	WHEN ? = '' "
                            "	THEN name "
                            "	ELSE  ? "
                            "    END"
                            "    ,picture = "
                            "    CASE "
                            "	when ? = ''"
                            "	THEN picture"
                            "	ELSE ?"
                            "    END "
                            " where user_id = ? "
                            " AND id = ?")
                            .bind(item.quantity)
                            .bind(item.quantity)
                            .bind(item.name)
                            .bind(item.name)
                            .bind(item.picture)
                            .bind(item.picture)
                            .bind(user_id)
                            .bind(item.id)
                            .execute();

                }
                std::lock_guard<std::mutex> guard(l_box_items_array);
                boxItems[item.box_id] = "";
                res << req.body();

            }).del([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                mItem item;
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string item_id_str = req.params["id"];
                int item_id = stoi(item_id_str);
                int box_id = 0;
                std::string box_id_str = "";
                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                } else {
                    item = itemJsonToStruct(req.body());
                    Session sess = cli.getSession();
                    // Get box to kill cache
                    SqlResult myRows =
                    sess.sql("SELECT box_id from items where id = ? and user_id = ?")
                    .bind(item_id, user_id).execute();
                    std::stringstream buffer;
                    bool hasRows = false;
                    for (Row row : myRows.fetchAll()) {
                        buffer << row[0];
                    }
                    box_id_str = buffer.str();
                    box_id = stoi(box_id_str);
                    auto result = sess.sql("delete from items "
                            " where user_id = ? "
                            " AND id = ?")
                            .bind(user_id)
                            .bind(item_id)
                            .execute();
                }
                std::lock_guard<std::mutex> guard(l_box_items_array);
                boxItems[box_id] = "";
                res << "{\"deleted_item_id\":" << item_id_str << "}";

            }).post([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                std::string header = req.header("cookie");
                std::string box_id_str = req.params["id"];
                int box_id = stoi(box_id_str);
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }

                mItem item = itemJsonToStruct(req.body());
                item.quantity = 1;
                item.box_id = box_id;
                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                } else {
                    Session sess = cli.getSession();

                            //we want to allow update of any combination of 
                            // weight != 0, name != "" and picture != ""
                            // if 0 or in other fields empty string we leave
                            //existing value in db using case like this is one way to do
                            // without having to create multiple sql statements
                            // depending on what fields were passed to be updated
                            auto result = sess.sql("insert into items (name,quantity,picture,box_id,user_id) values(?,?,?,?,?)")
                            .bind(item.name)
                            .bind(item.quantity)
                            .bind(item.picture)
                            .bind(box_id)
                            .bind(user_id)
                            .execute();
                            item.id = result.getAutoIncrementValue();
                            item.user_id = user_id;
                    {

                    }
                    std::lock_guard<std::mutex> guard(l_box_items_array);
                            boxItems[box_id] = "";
                            res << itemObj->toJson(item);
                }


            });
    mux.handle("/box/{id}")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string name = "";
                std::string items = "";
                std::string authKey = "";

                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string box_id_str = req.params["id"];
                int box_id = stoi(box_id_str);

                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";

                } else {
                    {
                        // this is copy?  Safe ?  no lock if have to fetch with sql....
                        std::lock_guard<std::mutex> guard(l_box_items_array);
                                items = boxItems[box_id];
                    }
                    if (items != "") {
                        std::string search = "\"user_id\":";
                                search = search + std::to_string(user_id);;
                                search = search + ",";
                                search = search + "\"box_id\":";
                                search = search + box_id_str;
                                int pos = items.find(search);
                        if (pos == -1) {
                            // if we can not match user_id and box_id in the json we do not own box here trying to 
                            // access someone elses box and items
                            user_id = 0;
                                    res << "[]";
                        }
                    } else {
                        Session sess = cli.getSession();
                                SqlResult myRows =
                                sess.sql("SELECT id,user_id,box_id, name,quantity, picture,created_at FROM items where user_id = ? and box_id = ?")
                                .bind(user_id, box_id).execute();
                                std::stringstream buffer;
                                bool hasRows = false;
                                buffer << "[";
                        for (Row row : myRows.fetchAll()) {
                            hasRows = true;
                                    buffer << "{";
                                    buffer << "\"id\":" << row[0] << ",";
                                    buffer << "\"user_id\":" << row[1] << ",";
                                    buffer << "\"box_id\":" << row[2] << ",";
                                    buffer << "\"name\":" << "\"" << row[3] << "\",";
                                    buffer << "\"quantity\":" << row[4] << ",";
                                    buffer << "\"picture\":" << "\"" << row[5] << "\"";
                                    buffer << "},";
                        }
                        if (hasRows) {
                            buffer.seekp(-1, std::ios_base::end);
                        }
                        buffer << "]";
                                items = buffer.str();
                        {
                            std::lock_guard<std::mutex> guard(l_box_items_array);
                                    boxItems[box_id] = items;
                        }
                    }
                    if (user_id != 0) {
                        res << items;
                    }
                }
            }).put([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string box_id_str = req.params["id"];
                int box_id = stoi(box_id_str);

                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                } else {
                    Session sess = cli.getSession();
                            mBox box = boxJsonToStruct(req.body());
                            //we want to allow update of any combination of 
                            // weight != 0, name != "" and picture != ""
                            // if 0 or in other fields empty string we leave
                            //existing value in db using case like this is one way to do
                            // without having to create multiple sql statements
                            // depending on what fields were passed to be updated
                            auto result = sess.sql("UPDATE boxes "
                            "    SET weight = CASE "
                            "	WHEN ? = 0 "
                            "	THEN weight "
                            "	ELSE  ? "
                            "    END "
                            "    ,name = CASE "
                            "	WHEN ? = '' "
                            "	THEN name "
                            "	ELSE  ? "
                            "    END"
                            "    ,picture = "
                            "    CASE "
                            "	when ? = ''"
                            "	THEN picture"
                            "	ELSE ?"
                            "    END "
                            " where user_id = ? "
                            " AND id = ?")
                            .bind(box.weight)
                            .bind(box.weight)
                            .bind(box.name)
                            .bind(box.name)
                            .bind(box.picture)
                            .bind(box.picture)
                            .bind(user_id)
                            .bind(box_id)
                            .execute();
                }
                std::lock_guard<std::mutex> guard(l_user_boxes_array);
                userBoxes[user_id] = "";
                res << req.body();

            }) .del([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string authKey = "";
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                std::string box_id_str = req.params["id"];
                int box_id = stoi(box_id_str);

                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                } else {
                    Session sess = cli.getSession();
                            auto result = sess.sql("delete from boxes "
                            " where user_id = ? "
                            " AND id = ?")
                            .bind(user_id)
                            .bind(box_id)
                            .execute();
                }
                std::lock_guard<std::mutex> guard(l_user_boxes_array);
                userBoxes[user_id] = "";
                res << req.body();

            });
    mux.handle("/box")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                std::string name;
                std::string boxes;
                std::string authKey = "";
                std::string header = req.header("cookie");
                auto cookies = parseCookies(header);
                std::map<std::string, std::string>::iterator it;
                it = cookies.find("authToken");
                if (it != cookies.end()) {
                    authKey = it->second;
                }
                int user_id = getUserIdFromAuthKey(cli, authKey);
                if (user_id == 0) {
                    res << "{\"logout\": true}";
                    return;
                }
                {
                    // this is copy?  Safe ?  no lock if have to fetch with sql....
                    std::lock_guard<std::mutex> guard(l_user_boxes_array);
                            boxes = userBoxes[user_id];
                }
                if (boxes != "") {
                    // TODO:  needs to check if not "" if the user has access to the box in cache
                    // simalar to /boxes/{id} route
                } else {
                    Session sess = cli.getSession();
                            SqlResult myRows = sess.sql("SELECT id,user_id, name,weight, picture,created_at FROM boxes where user_id = ?")
                            .bind(user_id).execute();
                            std::stringstream buffer;
                            bool hasRows = false;
                            buffer << "[";
                    for (Row row : myRows.fetchAll()) {
                        hasRows = true;
                                buffer << "{";
                                buffer << "\"id\":" << row[0] << ",";
                                buffer << "\"user_id\":" << row[1] << ",";
                                buffer << "\"name\":" << "\"" << row[2] << "\",";
                                buffer << "\"weight\":" << row[3] << ",";
                                buffer << "\"picture\":" << "\"" << row[4] << "\"";
                                buffer << "},";
                    }
                    if (hasRows) {
                        buffer.seekp(-1, std::ios_base::end);
                    }
                    buffer << "]";
                            boxes = buffer.str();
                    {
                        std::lock_guard<std::mutex> guard(l_user_boxes_array);
                                userBoxes[user_id] = boxes;
                    }
                }
                res << boxes;
            })

    .post([&](served::response &res, const served::request & req) {
        res.set_header("Access-Control-Allow-Origin", cDom);
        res.set_header("Access-Control-Allow-Credentials", "true");
        std::string authKey = "";
        std::string header = req.header("cookie");
        auto cookies = parseCookies(header);
        std::map<std::string, std::string>::iterator it;
        it = cookies.find("authToken");
        if (it != cookies.end()) {
            authKey = it->second;
        }


        int user_id = getUserIdFromAuthKey(cli, authKey);
        if (user_id == 0) {
            res << "{\"logout\": true}";
        } else {
            Session sess = cli.getSession();
                    mBox box = boxJsonToStruct(req.body());
                    //we want to allow update of any combination of 
                    // weight != 0, name != "" and picture != ""
                    // if 0 or in other fields empty string we leave
                    //existing value in db using case like this is one way to do
                    // without having to create multiple sql statements
                    // depending on what fields were passed to be updated
                    auto result = sess.sql("insert into boxes (name,weight,picture,user_id) values(?,?,?,?)")
                    .bind(box.name)
                    .bind(box.weight)
                    .bind(box.picture)
                    .bind(user_id)
                    .execute();
                    box.id = result.getAutoIncrementValue();
                    box.user_id = user_id;
            {
                std::lock_guard<std::mutex> guard(l_user_boxes_array);
                        userBoxes[user_id] = "";
            }
            res << boxObj->toJson(box);
        }


    });
    mux.handle("/front/box.webp")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "image/webp");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/public/";
                std::string image_name = baseDir + "box.webp";
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
            });
    mux.handle("/front")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "application/json");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/build/";
                std::string image_name = baseDir + "index.html";
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
                res.set_header("content-type", "text/html");
            });
    mux.handle("/manifest.json")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "application/json");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/build/";
                std::string image_name = baseDir + "manifest.json";
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
                res.set_header("content-type", "text/html");
            });
    mux.handle("/static/js/")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "application/json");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/build/";
                std::string url = req.url().path();
                std::string filename = url.substr(url.rfind('/') + 1);;
                std::string image_name = baseDir + "static/js/" + filename;
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
            });
    mux.handle("/static/css/")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "text/css");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/build/";

                std::string url = req.url().path();
                std::string filename = url.substr(url.rfind('/') + 1);;
                std::string image_name = baseDir + "static/css/" + filename;
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
            });
    mux.handle("/logo192.png")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "image/png");
                std::string baseDir = "/home/pnovack/code/Boxes/BoxesReact/boxes/build/";
                std::string image_name = baseDir + "logo192.png";
                std::ifstream ifs(image_name);
                res.set_body(std::string(
                        (std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>())
                        ));
            });

    mux.handle("/login")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "application/json");
                std::stringstream buffer;
                int user_id = 0;
                //std::string url = req.url().query();
                std::string username = req.query["username"]; //getQueryParam(url, "username");
                std::string password = req.query["password"]; //getQueryParam(url, "password");
                std::string authKey = "";
                if (password != "" && username != "") {
                    user_id = logUserIn(cli, username, password, authKey);
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
                if (user_id != 0) {
                    buffer << "{";
                            buffer << "\"user_id\":" << "\"" << user_id << "\",";
                            buffer << "\"authToken\":" << "\"" << authKey << "\"";
                            buffer << "}";
                            res << buffer.str();
                }
            });
    mux.handle("/logout")
            .get([&](served::response &res, const served::request & req) {
                res.set_header("Access-Control-Allow-Origin", cDom);
                res.set_header("Access-Control-Allow-Credentials", "true");
                res.set_header("content-type", "application/json");
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
                res << "{\"logout\": true}";
            });
    served::net::server server("0.0.0.0", "8123", mux);
    cout << "Server Up...." << endl;
    server.run(50);
    return (EXIT_SUCCESS);
}