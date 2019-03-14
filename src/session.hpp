#pragma once

#include "asio.hpp"
#include "sol.hpp"
#include "mysql_pool.hpp"
#include "lua_gdb.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket) : socket_(std::move(socket))
    {}

    void Start()
    {
        m_luaGDb = std::make_shared<LuaGDb>();
        m_luaGDb->RegisterGDbToLua();
        m_luaGDb->GetLuaState()->set("send", [this](std::size_t length){
            do_write(length);
        });
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
                    
                    m_luaGDb->GetLuaState()->script_file("../src/script/db.lua");
                    sol::function lua_on_recv = (*(m_luaGDb->GetLuaState()))["onRecv"];
                    lua_on_recv(index, std::string(data_, length), length);
                });
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
    std::shared_ptr<LuaGDb> m_luaGDb;
};