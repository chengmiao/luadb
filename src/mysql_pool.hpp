#pragma once
#include "asio.hpp"
#include "gdb.hpp"
#include "singleton.h"

class MysqlPool : public Singleton <MysqlPool>
{
public:
    void init()
    {
        m_threadNum = 3;

        m_ioContextArrary.resize(m_threadNum, nullptr);
        m_threadArrary.resize(m_threadNum, nullptr);
        m_pDBArray.resize(m_threadNum, nullptr);

        for (auto i = 0; i < m_threadNum; ++i)
        {
            m_pDBArray.at(i) = std::make_shared<gdp::db::GDb>("host", "port", "username", "password");
            auto result = m_pDBArray.at(i)->init("database_name");

            if (result.resultVal != 0)
            {
                // log error!
                //abort();
            }
        }

        for (auto i = 0; i < m_threadNum; ++i)
        {
            auto ios = std::make_shared<asio::io_context>();
            m_ioContextArrary.at(i) = ios;
        }

        Start();

        return;
    }

    void Start()
    {
        for (auto i = 0; i < m_threadNum; ++i)
        {
            auto thr = std::make_shared<std::thread>([this, i](){
                m_ioContextArrary.at(i)->run();
            });

            m_threadArrary.at(i) = thr;
        }
    }

    
    void Post(int32_t index, std::function<void()> f)
    {
        if (index >= m_ioContextArrary.size())
        {
            return;
        }

        m_ioContextArrary[index]->post(f);

        return;
    }

    void DBGet(int32_t index, gdp::db::DBQueue queue)
    {
        if (index >= m_ioContextArrary.size())
        {
            return;
        }

        m_pDBArray[index]->get(queue);

        return;
    }


private:
    std::vector<std::shared_ptr<asio::io_context>> m_ioContextArrary;
    std::vector<std::shared_ptr<std::thread>> m_threadArrary;
    std::vector<std::shared_ptr<gdp::db::GDb>> m_pDBArray;

    int32_t m_threadNum;
};