#pragma once

#include "asio.hpp"
#include "sol.hpp"
#include "mysql_pool.hpp"
#include "lua_gdb.hpp"
#include "msg_head.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket)
    : socket_(std::move(socket)),
    produce_pos_(0),
    consume_pos_(0),
    read_buf_(new char[64 * 1024])
    {}

    ~Session() { delete[] read_buf_; }

    void Start()
    {
        //m_lua_state = std::make_shared<sol::state>();
        m_luaGDb = std::make_shared<LuaGDb>();
        m_luaGDb->RegisterGDbToLua();
        m_luaGDb->GetLuaState()->script_file("../src/script/db.lua");
        m_luaGDb->GetLuaState()->set("send", [this](std::string context){
            do_write(context);
        });

        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(produce_pos(), producible()),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "Read Buffer" << std::to_string(length) << std::endl;

                produce_pos_ += length;
                consume();
                if (produce_end())
                {
                    rearrange_read_buf();
                }

                do_read();
            }
            else
            {
                close();
            }
        });
    }

    void do_write(std::string content)
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(content.data(), content.size()),
        [this, self](std::error_code ec, std::size_t)
        {
            if (!ec)
            {
                //do_read();
            }
            else
            {
                close();
            }
        });
    }

    void consume()
    {
        while (true)
	    {
		    NetHead Head;
		    if (consumable() < sizeof(Head))
		    {
                std::cout << "consumable() < sizeof(Head)" << std::endl;
			    break;
		    }

		    Head.len = *((uint32_t*)consume_pos());
            std::cout << "Client Msg Length :" << std::to_string(Head.len) << std::endl;
		    //Head.len = htonl(Head.len);

		    if (Head.len > kMaxSize)
		    {
			    //LOG_ERROR("read_size_ overflow");
			    close();
                std::cout << "read_size_ overflow :" << std::endl;
			    break;
		    }

		    if (Head.len < 0)
		    {
			    //LOG_ERROR("incorrect body size");
			    close();
                std::cout << "incorrect body size :" << std::endl;
			    break;
		    }

		    if (consumable() - sizeof(Head) >= Head.len)
		    {
                std::size_t length = Head.len + sizeof(Head);
                std::string lua_data = std::string(consume_pos(), length);

                int32_t index = 2;
                MysqlPool::Instance()->getIOContext(index)->post([this, lua_data, index](){
                    std::cout << "Asio Post" << std::endl;
                
                    sol::function lua_on_recv = (*(m_luaGDb->GetLuaState()))["onRecv"];
                    lua_on_recv(index, lua_data);
                });

                consume_pos_ += length;
		    }
		    else
		    {
			    break;
		    }
	    }
    }

private:
    tcp::socket socket_;
    std::shared_ptr<LuaGDb> m_luaGDb;
    char *read_buf_;
	uint32_t kMaxSize = 64 * 1024;
    //char read_buf_[64 * 1024];

	std::size_t produce_pos_;
	std::size_t consume_pos_;

	char *consume_pos(){ std::cout << "consume_pos :" << std::to_string(consume_pos_) << std::endl; return read_buf_ + consume_pos_; }
	char *produce_pos(){ std::cout << "produce_pos :" << std::to_string(produce_pos_) << std::endl; return read_buf_ + produce_pos_; }
	std::size_t consumable(){ std::cout << "consumable :" << std::to_string(produce_pos_ - consume_pos_) << std::endl; return produce_pos_ - consume_pos_; }
	std::size_t producible(){ std::cout << "producible :" << std::to_string(kMaxSize - produce_pos_) << std::endl; return kMaxSize - produce_pos_; }
	bool produce_end(){ return produce_pos_ == kMaxSize; }
	void rearrange_read_buf()
    {
        std::cout << "rearrange_read_buf :" << std::endl;

        std::size_t cur_size = consumable();
	    memcpy(read_buf_, consume_pos(), cur_size);
	    consume_pos_ = 0;
	    produce_pos_ = cur_size;
    }

    void close()
    {
        socket_.close();
    }

    std::shared_ptr<sol::state> m_lua_state;
};