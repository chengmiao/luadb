#pragma once

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

    ~LuaGDb()
    {
    }

    void RegisterGDbToLua()
    {
        m_lua_state->open_libraries();

        // push gdp::db::gdb functions to lua
        m_lua_state->set("get", sol::overload(
            [](int32_t index, std::string sql){
                return MysqlPool::Instance()->getDB(index)->get(sql.c_str());
            },
            [](int32_t index, gdp::db::DBQuery & query){
                return MysqlPool::Instance()->getDB(index)->get(query);
            }
        ));

        m_lua_state->set("execute", sol::overload(
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

        auto select_overload = sol::overload(static_cast<gdp::db::DBQuery& (gdp::db::DBQuery::*)(const std::string &)>(&gdp::db::DBQuery::select),
                   static_cast<gdp::db::DBQuery& (gdp::db::DBQuery::*)(sol::variadic_args)>(&gdp::db::DBQuery::to_lua_select));

        m_lua_state->new_usertype<gdp::db::DBQuery>( "DBQuery",
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
            "inner_join",       &gdp::db::DBQuery::inner_join,
            "left_join",        &gdp::db::DBQuery::left_join,
            "right_join",       &gdp::db::DBQuery::right_join,
            "from",             &gdp::db::DBQuery::from,
            "sql",              &gdp::db::DBQuery::sql,
            "limit",            limit_overload,
            "select",           select_overload,
            "where",            static_cast<gdp::db::DBQuery& (gdp::db::DBQuery::*)(const std::string&, const std::string&, sol::variadic_args)>(&gdp::db::DBQuery::to_lua_where)
        );

        // push gdp::db::resultset to lua
        m_lua_state->new_usertype<gdp::db::ResultSet>( "ResultSet",
            sol::constructors<gdp::db::ResultSet(MYSQL_RES *)>(),
            "get_int32_at",  &gdp::db::ResultSet::get_int32_at,
            "get_int64_at",  &gdp::db::ResultSet::get_int64_at,
            "get_int32",     &gdp::db::ResultSet::get_int32,
            "get_int64",     &gdp::db::ResultSet::get_int64,
            "get_string",    &gdp::db::ResultSet::get_string,
            "get_string_at", &gdp::db::ResultSet::get_string_at,
            "get_float",     &gdp::db::ResultSet::get_float,
            "get_float_at",  &gdp::db::ResultSet::get_float_at,
            "get_llong",     &gdp::db::ResultSet::get_llong,
            "get_llong_at",  &gdp::db::ResultSet::get_llong_at,
            "next",          &gdp::db::ResultSet::next
        );

        // push SqlResult to lua
        m_lua_state->new_usertype<SqlResult<bool>>( "SqlResultBool",
            "errorString", &SqlResult<bool>::errorString,
            "resultVal",   &SqlResult<bool>::resultVal
        );

        m_lua_state->new_usertype<SqlResult<std::shared_ptr<gdp::db::ResultSet>>>( "SqlResultSet",
            "errorString", &SqlResult<std::shared_ptr<gdp::db::ResultSet>>::errorString,
            "resultVal",   &SqlResult<std::shared_ptr<gdp::db::ResultSet>>::resultVal
        );
    }

    std::shared_ptr<sol::state>& GetLuaState()
    {
        return m_lua_state;
    }
};