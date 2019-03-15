#include <iostream>
#include "asio.hpp"
#include "server.hpp"
#include "mysql_pool.hpp"

using asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        //if (argc != 2)
        //{
            //std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
            //return 1; 
        //}

        // init mysql pool
        MysqlPool::Instance()->init();

        // init listen server
        asio::io_context io_context;
        //Server s(io_context, std::atoi(argv[1]));
        Server s(io_context, 3389);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
