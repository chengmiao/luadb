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

        //m_luaState->set("execute", [](int32_t index, gdp::db::DBQuery query){
            //return MysqlPool::Instance()->getDB(index)->execute(query);
        //});

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
                    m_luaState->set("lua_index", index);
                    m_luaState->set("lua_recv_data", std::string(data_, length));
                    m_luaState->script_file("/script/db.lua");
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