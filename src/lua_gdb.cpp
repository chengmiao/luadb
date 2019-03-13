#include "sol.hpp"
#include "gdb.hpp"

class LuaGDb 
{
private:
    std::shared_ptr<sol::state> m_lua_state;

public:
    LuaGDb()
    {
        m_lua_state = std::make_shared<sol::state>();
    }

    void RegisterGDbToLua()
    {
        m_lua_state->open_libraries();

        // push gdp::db::gdb functions to lua
        m_luaState->set("get", sol::overload(
            [](int32_t index, std::string sql){
                return MysqlPool::Instance()->getDB(index)->get(sql.c_str());
            },
            [](int32_t index, DBQuery & query){
                return MysqlPool::Instance()->getDB(index)->get(query);
            }
        ));

        m_luaState->set("execute", sol::overload(
            [](int32_t index, std::string sql){
                return MysqlPool::Instance()->getDB(index)->execute(sql.c_str());
            },
            [](int32_t index, gdp::db::DBQuery& query){
                return MysqlPool::Instance()->getDB(index)->execute(query);
            }
        ));

        // push gdp::db::DBQuery to lua
        auto limit_overload = sol::overload(sol::resolve<gdp::db::DBQuery&(unsigned int)>(&gdp::db::DBQuery::limit),
            sol::resolve<gdp::db::DBQuery&(unsigned int, unsigned int)>(&gdp::db::DBQuery::limit));

        m_luaState->new_usertype<gdp::db::DBQuery>( "DBQuery",
            sol::constructors<gdp::db::DBQuery(const std::string &)>(),
            "create",           &gdp::db::DBQuery::create,
            "def",              &gdp::db::DBQuery::def,
            "insert_into",      &gdp::db::DBQuery::to_lua_insert_into,
            "insert_or_update", &gdp::db::DBQuery::to_lua_insert_or_update,
            "update",           &gdp::db::DBQuery::to_lua_update,
            "del",              &gdp::db::DBQuery::del,
            "values",           &gdp::db::DBQuery::to_lua_values,
            "group_by",         &gdp::db::DBQuery::group_by,
            "order_by",         &gdp::db::DBQuery::order_by,
            "join",             &gdp::db::DBQuery::join,
            "inner_join",       &gdp::db::DBQuery:inner_join,
            "left_join",        &gdp::db::DBQuery::left_join,
            "right_join",       &gdp::db::DBQuery::right_join,
            "sql",              &gdp::db::DBQuery::sql,
            "limit",            limit_overload,
            
            //"execute", sol::overload(static_cast<SqlResult<bool> (gdp::db::GDb::*)(const gdp::db::DBQuery &)>(&gdp::db::GDb::execute),
                   //static_cast<SqlResult<bool> (gdp::db::GDb::*)(const std::string&)>(&gdp::db::GDb::execute))
        );

        // push gdp::db::resultset to lua
    }
};