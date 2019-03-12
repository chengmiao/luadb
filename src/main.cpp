#include <iostream>
#include "asio.hpp"
#include "server.hpp"
#include "mysql_pool.hpp"

using asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
            return 1; 
        }

        // init mysql pool
        MysqlPool::Instance()->init();

        MysqlPool::Instance()->getIOContext(1)->post([](){
                    std::cout << "Asio Post" << std::endl;
                });

        // init listen server
        asio::io_context io_context;
        Server s(io_context, std::atoi(argv[1]));
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
