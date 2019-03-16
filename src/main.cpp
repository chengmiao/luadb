#include <iostream>
#include "asio.hpp"
#include "server.hpp"
#include "mysql_pool.hpp"
#include "server_mgr.hpp"

using asio::ip::tcp;

#define SOL_CHECK_ARGUMENTS 1

void handleSignal(int sig)
{
	printf("catch a signal:%d\n:", sig);
	
    MysqlPool::Instance()->Stop();
    ServerMgr::Instance()->stop();

    MysqlPool::Instance()->Release();
    ServerMgr::Instance()->Release();
}

void setDeamon()
{
    long pid = fork();
    if (pid != 0)
        exit(0);
    else if (pid < 0)
        exit(1);

    (void)setsid();
    pid = fork();
    if (pid != 0)
        exit(0);
    else if(pid < 0)
        exit(1);

    return;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
            return 1; 
        }

        if (argc == 3)
        {
            setDeamon();
        }

        // init mysql pool
        MysqlPool::Instance()->init();

        // init listen server
        //asio::io_context io_context;
        //Server s(io_context, std::atoi(argv[1]));
        //io_context.run();
        ServerMgr::Instance()->init(std::atoi(argv[1]));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
