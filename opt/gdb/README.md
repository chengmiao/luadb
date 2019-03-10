# GDB

* 介绍:
   1. 简单的数据库封装库，使用c++11 简化sql语句的组装； 在数据库编程中，比较麻烦的是sql语句的组装，常见的方式是使用iostream 或printf方式进行组装，需要处理较多的类型转换或临时变量进行拼装。
     此数据库封装库采用了C++11 语法，并将sql语句中的关键字通过函数的方式实现，简化编写sql的难度，减少声明的临时变量等。 
   2. 此数据库组件包含两个核心库，一个组装Sql语句，另一个管理sql连接，包括数据库连接池等。
   3. 此库依赖mysql官方提供的mysql-connector c++ 版本  [mysql Connector/C++](https://dev.mysql.com/downloads/connector/cpp/)


* 例子: 


```cpp
GDb db("127.0.0.1",3306,"root","passwd","tinydb"); //声明一个数据库实例
DBQueue queue; 
queue.table("users").insert("name", "age","sex","phone","address").values( name.c_str() ,
    20+i,1,"18930878762","shanghai"); //组装sql语句

db.execute(queue); // 执行sql语句
```

* 获取数据结果

    [mysql connector/C++](https://dev.mysql.com/downloads/connector/cpp/)   对数据库结果集做了很好的封装。本库主要在怎么获取结果集增加了简单的接口，能快速获取结果集。 

    1. 直接获取数据集 
        框架对底层c接口进行了封装，使用智能指针包装了内存的申请和释放，应用层无需担心内存泄漏问题。 

        ```cpp
        MysqlSetPtr mSet = db.execute(queue).get(); 
        ```

    2. 直接通过lambda函数获取结果集
        ```cpp
        db.execute(queue).get([](sql::ResultSet & rstSet){ 
        //process result set 
        }); 
        ```


# 编译

```bash
git submodule init
git submodule update
cmake .
make
```

# 其他
    此库还在开发和完善中，接口存在变动风险，另外，对sql语法的支持目前还比较初级，请谨慎使用，也欢迎大家多提意见和想法。共同完善此库。 
