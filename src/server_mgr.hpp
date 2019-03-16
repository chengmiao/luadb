#include "singleton.h"
#include "server.hpp"

class ServerMgr : public Singleton<ServerMgr>
{
public:
    void init(int32_t port)
    {
        m_context = std::make_shared<asio::io_context>();
        m_server = std::make_shared<Server>(m_context, port);
        m_context->run(); 
    }   

    void stop()
    {
        m_server->stop();
        m_context->stop();
    }

private:
    std::shared_ptr<Server> m_server;
    std::shared_ptr<asio::io_context> m_context;
};