#pragma once

#include "asio.hpp"
#include "sol.hpp"
#include "mysql_pool.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket) : socket_(std::move(socket))
    {}

    void Start()
    {
        m_luaState = std::make_shared<sol::state>();
        m_luaState->open_libraries();

        //(*m_luaState)["pool"] = *(MysqlPool::Instance());
        //m_luaState->new_usertype<MysqlPool>( "pool",
            //sol::constructors<MysqlPool()>(),
            // typical member function
            //"post", &MysqlPool::Post,
            //"getDB", &MysqlPool::DBGet
        //);

        m_luaState->set("get", [](int32_t index, std::string sql){
            return MysqlPool::Instance()->getDB(index)->get(sql.c_str());
        });

        m_luaState->set_function("my_print", [](sol::variadic_args args) {
            lua["print"](args);
        });

        m_luaState->script("my_print(1, 2, 3)");
        //m_luaState->set("execute", [](int32_t index, gdp::db::DBQuery query){
            //return MysqlPool::Instance()->getDB(index)->execute(query);
        //});

        //auto overload = sol::overload(sol::resolve<gdp::db::DBQuery&(const std::string&)>(&gdp::db::DBQuery::set),
            //sol::resolve<gdp::db::DBQuery&(const std::string&, int)>(&gdp::db::DBQuery::set<int>));

        //m_luaState->new_usertype<gdp::db::DBQuery>( "DBQuery",
            //sol::constructors<gdp::db::DBQuery(const std::string &)>(),
            //"insert_into", &gdp::db::DBQuery::insert_into,
            //"insert_or_update", &gdp::db::DBQuery::insert_or_update,
            //"update", &gdp::db::DBQuery::update,
            //"set", static_cast<gdp::db::DBQuery& (gdp::db::DBQuery::*)(const std::string&)>(&gdp::db::DBQuery::set)
        //);


        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "Read Buffer" << std::endl;

                int32_t index = 2;
                MysqlPool::Instance()->getIOContext(index)->post([this, self, length, index](){
                    std::cout << "Asio Post" << std::endl;
                    
                    m_luaState->script_file("../src/script/db.lua");
                    sol::function lua_on_recv = (*m_luaState)["onRecv"];
                    lua_on_recv(index, std::string(data_, length));
                });

                //do_write(length);
            }
        });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
        [this, self](std::error_code ec, std::size_t)
        {
            if (!ec)
            {
                do_read();
            }
        });
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::shared_ptr<sol::state> m_luaState;
};