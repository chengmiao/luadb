#pragma once
#include "asio.hpp"
#include "session.hpp"

using asio::ip::tcp;

class Server
{
public:
    Server(asio::io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        MysqlPool::Instance()->getIOContext(1)->post([](){
                    std::cout << "Asio Post" << std::endl;
                });

        acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(std::move(socket))->Start();
            }

            do_accept();
        });
    }

private:
    tcp::acceptor acceptor_;
};