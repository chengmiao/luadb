#pragma once
#include "escape_string.h"
#include <stack>

namespace gdp
{
    namespace db
    {
        class DBQueue
        {
            public:
                static const uint32_t MAX_NEST_LEVEL = 16;
                typedef std::function<void(DBQueue & ) > SelfHandler;
                typedef std::function<void(sql::ResultSet &) > ResultFunc;
                DBQueue & into(const std::string & tbName)
                {
                    m_table = tbName;
                    m_sql.write(" {} ",tbName);
                    return *this;
                }
                DBQueue & create(const std::string &tbName,bool check = false)
                {
                    if (check)
                    {
                        m_sql.write(" {} IF NOT EXISTS " , tbName);
                    }
                    else
                    {
                        m_sql.write(" {} " , tbName);
                    }
                    return *this;
                }
                DBQueue & begin()
                {
                    m_sql << " ( " ;
                    return *this;
                }

                DBQueue & end()
                {
                    m_sql << " ) " ;
                    return *this;
                }

                DBQueue & def(const std::string  & key ,
                        const std::string & type,
                        int length =0 ,
                        const std::string & dft = "",bool inc  = false){

                    if (definitions.size() > 0)
                    {
                        m_sql << " , ";
                    }

                    fmt::MemoryWriter lenStr;
                    if (length >0)
                    {
                        lenStr << "("<< length<< ")";
                    }
                    //lenStr[1]  = '[';
                    fmt::MemoryWriter defStr;

                    defStr.write(" {} {}{} {} ", key,type,length>0?lenStr.c_str():"" ,inc ? " AUTO_INCREMENT " :"" );
                    m_sql << defStr.c_str();
                    return *this;
                }


                template <typename ... Args>
                    DBQueue & insert(const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);
                        m_sql << " insert into " << m_table ;

                        if (argLen > 0 )
                        {
                            m_sql << "( ";
                        }
                        fmt::MemoryWriter format;
                        for (int i  = 0; i   < argLen  ; i++)
                        {
                            if (i < argLen -1 )
                            {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        //std::cout << "format is " << format.c_str() << std::endl;
                        m_sql.write(format.c_str(),args...);
                        if (argLen > 0)
                        {
                            m_sql.write(" )  " );
                        }

                        return *this;
                    }

                template <typename ... Args>
                    DBQueue & update (const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);
                        m_sql << " update " << m_table ;

                        fmt::MemoryWriter format;
                        for (int i  = 0; i   < argLen  ; i++)
                        {
                            if (i < argLen -1 )
                            {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        //std::cout << "format is " << format.c_str() << std::endl;
                        m_sql.write(format.c_str(),args...);

                        return *this;
                    }

                DBQueue& set(const std::string& term) {
                    fmt::MemoryWriter termStr;
                    termStr.write(" {} ", term);
                    set_item_count ++;
                    if (set_item_count <= 1)
                    {
                        m_sql << " set " << termStr.c_str();
                    }
                    else {
                        m_sql << " ," << termStr.c_str();
                    }

                    return *this;
                }

                template<class T>
                    DBQueue& set(const std::string & key, T val )
                    {
                        fmt::MemoryWriter termStr;
                        termStr.write(" {} = {} ", key ,  val);
                        set_item_count ++;
                        if (set_item_count <= 1)
                        {
                            m_sql << " set " << termStr.c_str();
                        }
                        else {
                            m_sql << " ," << termStr.c_str();
                        }

                        return *this;
                    }

                DBQueue & del()
                {
                    clear();
                    m_sql << "delete  from " << m_table  ;
                    return *this;
                }

                DBQueue& set(const std::string & key, const char *  val)
                {
                    return this->set(key,std::string(val));
                }
                DBQueue& set(const std::string & key, const std::string & val)
                {
                    fmt::MemoryWriter termStr;
                    auto str = gdp::db::EscapeString(val);
                    termStr.write(" {} = \"{}\" ", key, str);
                    set_item_count ++;
                    if (set_item_count <= 1)
                    {
                        m_sql << " set " << termStr.c_str();
                    }
                    else {
                        m_sql << " ," << termStr.c_str();
                    }

                    return *this;
                }

                template <class T>
                    std::string printarg(T t)
                    {
                        fmt::MemoryWriter val ;
                        val << t ;
                        return val.c_str() ;
                    }



                std::string printarg(const char *  t)
                {
                    auto str = gdp::db::EscapeString(t);
                    return std::string("\"") + str +"\"";
                }

                std::string printarg(const std::string& t )
                {
                    auto str = gdp::db::EscapeString(t);
                    return std::string("\"") + str +"\"";
                }



                template <typename ... Args>
                    DBQueue & values(const Args & ... args)
                    {
                        int argLen = sizeof ...(Args);
                        m_sql << " values " << " ( "  ;

                        fmt::MemoryWriter format;
                        for (int i  = 0; i   < argLen  ; i++)
                        {
                            if (i < argLen -1 )
                            {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        //std::cout << "format is " << format.c_str() << std::endl;
                        m_sql.write(format.c_str(),printarg(args)...);
                        m_sql.write(" )  " );
                        return *this;
                    }



                template <typename ... Args>
                    DBQueue & select(const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);
                        m_sql << "select " ;

                        fmt::MemoryWriter format;
                        for (int i  = 0; i   < argLen  ; i++)
                        {
                            if (i < argLen -1 )
                            {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        //std::cout << "format is " << format.c_str() << std::endl;
                        m_sql.write(format.c_str(),args...);
                        m_sql.write(" from {} " , m_table);
                        return *this;
                    }
                DBQueue & select(const std::string & selData)
                {
                    clear();
                    m_sql.write("select {} from {} ",selData,m_table );
                    return *this;
                }

                DBQueue & table(const std::string & tbName)
                {
                    m_table = tbName;
                    return *this;
                }
                DBQueue & where(SelfHandler self)
                {
                    where_levels ++;
                    if (where_levels >= MAX_NEST_LEVEL)
                    {
                        std::cout << "exceed the max nest level" << std::endl;
                        return *this;
                    }
                    where_level_count[where_levels] = 0;
                    if (where_level_count[where_levels - 1] > 0)
                    {
                        m_sql << " and ";
                    }
                    if (where_levels == 1 && where_level_count[0] == 0)
                    {
                        m_sql << " where ";
                        where_level_count[0] ++;
                    }
                    m_sql << " ( ";
                    self(*this);
                    m_sql << " ) ";
                    where_levels --;

                    return *this;
                }

                DBQueue & where(const std::string & key ,  const char * term )
                {
                    return this->where(key,"=",term);
                }
                DBQueue & where(const std::string & key , const std::string & op, const char * term )
                {
                    return where(key,op,std::string(term));
                }
                DBQueue & where(const std::string & key , const std::string & op, const std::string & term)
                {
                    fmt::MemoryWriter termStr;
                    auto str = gdp::db::EscapeString(term);
                    termStr.write("{} {} \"{}\"", key , op, str);
                    this->_where(termStr.c_str());
                    return *this;
                }
                template <typename T>
                    DBQueue & where_raw(T term)
                    {
                        fmt::MemoryWriter termStr;
                        termStr.write(" {} ",  term);
                        this->_where(termStr.c_str());
                        return *this;
                    }

                void _where(const std::string & term)
                {
                    where_level_count[where_levels] ++;
                    if (where_levels > 0)
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            m_sql.write("and {} ", term);
                        }
                        else
                        {
                            m_sql.write(" {} ", term);
                        }
                    }
                    else
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            m_sql.write("and {} ", term);
                        }
                        else
                        {
                            m_sql.write(" where {} ", term);
                        }
                    }
                }

                template <typename T>
                    DBQueue & where(const std::string & key ,  T   term)
                    {
                        return where<T>(key,"=",term);
                    }

                template <typename T>
                    DBQueue & where(const std::string & key , const std::string & op, T   term)
                    {
                        fmt::MemoryWriter termStr;
                        termStr.write(" {} {} {} ", key , op, term);
                        this->_where(termStr.c_str());
                        return *this;
                    }

                template<typename T>
                    DBQueue & or_where(const std::string & key ,const std::string & op , T  term)
                    {
                        fmt::MemoryWriter termStr;
                        termStr.write(" {} {} {} ", key , op, printarg(term));
                        where_level_count[where_levels] ++;
                        m_sql.write( " or {} ", termStr.c_str());
                        return *this;
                    }

                DBQueue & limit( unsigned int count)
                {
                    m_sql.write(" limit {} " ,count);

                    return *this;
                }
                DBQueue & limit(unsigned int offset , unsigned int count)
                {
                    m_sql.write(" limit {}, {}  " ,offset , count);
                    return *this;
                }
                DBQueue & group_by (const std::string & col)
                {
                    m_sql.write(" group by {} " ,col);
                    return *this;
                }


                DBQueue & order_by(const std::string & col, const std::string & type = "asc" )
                {
                    m_sql.write(" order by {} {} " ,col,type );
                    return *this;
                }

                DBQueue & join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    return inner_join(tb,lId,op,rId);
                }

                DBQueue & inner_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" inner join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                DBQueue & left_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" left join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                DBQueue & right_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" right join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                const char *  sql() const
                {
                    return m_sql.c_str();
                }


                void clear()
                {
                    m_sql.clear();
                    for(int i = 0; i < MAX_NEST_LEVEL; i++)
                    {
                        where_level_count[i] = 0;
                    }
                    set_item_count = 0;
                    where_levels = 0;
                }


            private:

                std::vector<std::string > definitions;
                std::string  m_table;
                fmt::MemoryWriter m_sql ;
                uint32_t where_level_count[MAX_NEST_LEVEL] = {0};//std::vector<std::string > wheres;
                uint32_t where_levels = 0;
                uint32_t set_item_count = 0; //std::vector<std::string > sets ;
        };

    }
}
