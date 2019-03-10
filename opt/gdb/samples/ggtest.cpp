/**
 * @file googletest.cpp
 * @brief
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2018-10-11
 */


#include <iostream>
#include <sys/time.h>
#include "gdb.hpp"
#include "gtest/gtest.h"

using namespace gdp::db;

double cur_time()
{
     struct timeval  curtime;
     gettimeofday(&curtime,NULL);
     return curtime.tv_sec + curtime.tv_usec/1000000.0;
}

class GDbTest : public ::testing::Test {
protected:
    virtual void SetUp(){
        testdb.add("10.246.60.86",3306,"root","123456");//"127.0.0.1",3306,"root","123456");
        testdb.init("ai_check");//"tinydb");

        testdb.execute(
            "create table if not exists user_info ("
            "uid            int             unsigned            not null,"
            "name           varchar(32)     character set utf8  not null,"
            "status         tinyint         unsigned            not null,"
            "score          float                      not null,"
            "primary key (uid)"
            ") engine=innodb default charset=utf8;"
            );

    }
    virtual void TearDown(){

    }
    GDb testdb;
    DBQueue query;
};


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST_F(GDbTest, insert){
    for(int i = 0; i < 20; i ++) {
        fmt::MemoryWriter name;
        name << "test" << i ;
        fmt::MemoryWriter name1;
        name1 << "map" << i ;
        query.table("user_info").insert("uid", "name","status","score").values(i, name.c_str(), 0, 1.123);
        EXPECT_TRUE(testdb.execute(query))<<query.sql();

    //    query.insert_into("app_info", "AppID", "Name").values(name.c_str(), name.c_str());
    //    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    //    query.insert_into("confirm_list", "ID", "Text", "Label", "AppID", "ServerID").values(i+2, name1.c_str(), 2, name.c_str(),"s1212");
    //    EXPECT_TRUE(testdb.execute(query))<<query.sql();
    }
}

TEST_F(GDbTest, update){
    for(int i = 5; i < 10; i ++) {
        fmt::MemoryWriter name;
        name << "test" << i ;
        query.update("user_info").set("status", 1).where("name",name.c_str());
        EXPECT_TRUE(testdb.execute(query))<<query.sql();
    }

    query.table("confirm_list").update().set("Label", 6).where("AppID","like", "test1%");
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    query.table("confirm_list").update().set("ServerID", "s1215").where("AppID","like", "test1%").or_where("ID", "=", 0);
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    query.table("confirm_list").update().set("Label", 5).set("ServerID", "s1213").where("AppID","like", "test1%").or_where("ID", "=", 0);
    EXPECT_TRUE(testdb.execute(query))<<query.sql();
}

TEST_F(GDbTest, select_order_by_limit){

    query.table("rank_test_data").select("*").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id", "asc");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    // 此测试案例为错误query语句，执行结果为false,但使用了EXPECT_FALSE宏，因此测试结果为pass,后面有类似案例
    query.table("rank_test_data").select("*").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id", "des");
    EXPECT_FALSE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQueue& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,819036109);
    }).order_by("score");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
         //   std::cout << res->count() << std::endl;
         //   elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }))<<query.sql();

    query.table("confirm_list").select("*").where("Text","like","m%p%").where("Label", ">", 0).where([](DBQueue& query){
            query.where("ID" ,"<" ,100).or_where("ID" ,">" ,1);
    }).order_by("ServerID").limit(12);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
  //          std::cout << res->count() << std::endl;
  //          elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s",
  //              res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str());

        }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQueue& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,819036109);
    }).order_by("score").limit(10, 40);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
       //     std::cout << res->count() << std::endl;
       //     elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }))<<query.sql();

    query.table("user_info").select("*").where("score","=",1.123).where("score",1.1234).where([](DBQueue& query){
            query.where("score" ,"<" ,1.25).or_where("score" ,">=" ,1.111);
    }).order_by("score").limit(10, 40);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
       //     std::cout << res->count() << std::endl;
       //     elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }))<<query.sql();

}

TEST_F(GDbTest, select_group){

    query.table("rank_test_data").select("*").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).group_by("player_id");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where("player_id", "like", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).group_by("player_id").order_by("score");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQueue& query){
            query.where("score" ,"<" ,799040000).where("score" ,">=" ,719036109);
    }).group_by("player_id").order_by("score");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        //    std::cout << res->count() << std::endl;
        //    elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQueue& query){
            query.where("score" ,"<" ,799040000).where("score" ,">=" ,719036109);
    }).group_by("player_id").order_by("score").limit(5);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        //    std::cout << res->count() << std::endl;
        //    elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }))<<query.sql();

}

TEST_F(GDbTest, select_or_where){

    query.table("rank_test_data").select("*").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQueue& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    });
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","r22011").where("player_id", "8826906").where([](DBQueue& query){
            query.where("player_id","234j342").or_where("score" ,"<=" ,719036109);
    }).order_by("player_id", "asc");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","like","1_8114%").where("player_id", "like", "%张三").where([](DBQueue& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where([](DBQueue& query){
            query.where("player_id", "<", "8826906").or_where("player_id" ,">=" ,"8h826906");
    });
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

// select  *  from confirm_list  where  (   ID < 10  and  Label >= 0   and   (  Text like "m%p%"  or  Text like "%p%"    )   )
    query.table("confirm_list").select("*").where([](DBQueue& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0)
            .where([](DBQueue& query){
                    query.where("Text","like","m%p%").or_where("Text", "like", "%p%");
                });
            });
//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
//
//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str(), res->get_string("Name").c_str());
                }))<<query.sql();

// select  *  from confirm_list  where  (   ID < 10  and  Label >= 0    )  and   (  Text like "m%p%"  or  Text like "%p%"    )
    query.table("confirm_list").select("*").where([](DBQueue& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0);
            }).where([](DBQueue& query){
                    query.where("Text","like","m%p%").or_where("Text", "like", "%p%");
                });

//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {

//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str(), res->get_string("Name").c_str());
                }))<<query.sql();

//select  *  from confirm_list  where  (   ID < 10  and  Label >= 0    ) and Text like "m%p%"
    query.table("confirm_list").select("*").where([](DBQueue& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0);
    }).where("Text","like","m%p%");
//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {

//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str(), res->get_string("Name").c_str());
                }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").or_where("player_id", "<", "a8826906").where([](DBQueue& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

    query.table("rank_test_data").select("*").where("rank_type","=","108114").where([](DBQueue& query){
            query.where("player_id" ,"<" ,8826906).or_where("score" ,">=" ,8826950);

        }).where([](DBQueue& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) { }))<<query.sql();

}

TEST_F(GDbTest, select_inner_join){

    query.table("app_info").select("*").join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

    // 下面两个案例是错误输入 EXPECT_FALSE
    query.table("app_info").select("*").join("confirm_list", "app_info.App", "=", "confirm_list.AppID");
    EXPECT_FALSE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

    query.table("app_info").select("*").join("confirm_list", "AppID", "=", "AppID");
    EXPECT_FALSE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

    query.table("app_info").select("*").inner_join("confirm_list", "app_info.AppID", ">", "confirm_list.AppID");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

    query.table("app_info").select("*").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

//  等数据结构改好后再测试
    query.table("confirm_list").update().inner_join("app_info", "app_info.AppID", "=", "confirm_list.AppID")
        .set("app_info.PrivateKey","127").where("confirm_list.Label", 4);
    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    query.table("confirm_list a").update().inner_join("app_info b", "a.AppID", "=", "b.AppID")
        .set("b.PrivateKey","129").where("a.Label", 3);
    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    query.table("app_info").select("*").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID")
        .where("ServerID", ">", "s12121").or_where("app_info.AppID", ">", "1212")
        .group_by("confirm_list.ID").limit(10);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str(), res->get_string("Name").c_str());

        }))<<query.sql();

    query.table("app_info").select("*").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID")
        .where("ID", ">", 1)
        .where([](DBQueue& query){
            query.where("ServerID", ">", "s12121").or_where("ServerID", "=", "12121");
                })
        .where("Label", "=", 2);
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
    //      std::cout << res->count() << std::endl;
    //      elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
    //          res->get_int32("ID"), res->get_string("Text").c_str(), res->get_int32("Label"), res->get_string("AppID").c_str(), res->get_string("ServerID").c_str(), res->get_string("Name").c_str());

        }))<<query.sql();
}

TEST_F(GDbTest, select_left_join){

    query.table("app_info").select("*").left_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();
    std::cout << query.sql() << std::endl;
}

TEST_F(GDbTest, select_right_join){

    query.table("app_info").select("*").right_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.test_get(query, [&](sql::ResultSet& res) {
        }))<<query.sql();

}

TEST_F(GDbTest, del){

    query.table("user_info").del().where("uid",1).or_where("score","=",1.123);
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    query.table("user_info").del().where("uid",1).where("name","=","test1");
    EXPECT_TRUE(testdb.execute(query))<<query.sql();

    for(int i = 0; i < 20; i ++) {
        fmt::MemoryWriter name;
        name << "test" << i ;
        query.table("user_info").del().where("name", name.c_str());
        EXPECT_TRUE(testdb.execute(query))<<query.sql();

//        query.del("app_info").where("AppID", name.c_str());
//        EXPECT_TRUE(testdb.execute(query))<<query.sql();

//        query.del("confirm_list").where("AppID", name.c_str());
//        EXPECT_TRUE(testdb.execute(query))<<query.sql();
    }
}
