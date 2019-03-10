/**
 * @file test.cpp
 * @brief 
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2017-11-23
 */


#include <iostream>
#include "gdb.hpp"

using namespace gdp::db; 
int main() {

    GDb tinydb("127.0.0.1",3306,"root","123456"); 
    tinydb.init("test"); 

    DBQueue queue; 

    tinydb.execute(
        "create table if not exists user_info ("
            "uid            int             unsigned            not null,"
            "name           varchar(32)     character set utf8  not null,"
            "status         tinyint         unsigned            not null,"
            "primary key (uid)"
        ") engine=innodb default charset=utf8;"
    );

    for(int i = 0; i < 10; i ++) {
        fmt::MemoryWriter name; 
        name << "test" << i ; 
        queue.table("user_info").insert("uid", "name","status").values(i, name.c_str(), 0); 
        tinydb.execute(queue); 
    }

    queue.table("user_info").select("uid", "name");
    tinydb.get(queue, [](sql::ResultSet& res) {
                int row = 0;
                while(res.next()) {
                    //std::cout << row << ", " << res.getRow() << std::endl;
                    std::cout << "uid=" << res.getInt("uid");
                    std::cout << ", name=" << res.getString("name") << std::endl;
                    row++;
                }
            });

    //tinydb.execute(queue); 
    bool has_user = false;
    queue.table("user_info").select("uid", "name");
    tinydb.get(queue, [&has_user](sql::ResultSet& res) {
                std::cout << res.rowsCount() << std::endl;
                has_user = res.rowsCount() > 0;
    });
    std::cout << "has_user: " << has_user << std::endl;

    for (int i = 0; i < 10; i++) {
        fmt::MemoryWriter name;
        name << "test" << i;
        queue.table("user_info").update().set("status", 1);
        tinydb.execute(queue);
    }

    for (int i = 0; i < 10; i++) {
        fmt::MemoryWriter name;
        name << "test" << i;
        queue.table("user_info").del().where("name", name.c_str());
        tinydb.execute(queue);
    }
    
    queue.table("user_info").select("uid", "name");
    tinydb.get(queue, [&has_user](sql::ResultSet& res) {
                std::cout << res.rowsCount() << std::endl;
                has_user = res.rowsCount() > 0;
    });
    std::cout << "has_user: " << has_user << std::endl;
    
    return 0; 
}
