package.path = package.path..';../src/script/?.lua'

require "transpb"

local handler_table = {}

function init()
    transpb:load_file("test.proto")
    registerHandler("luadb.CreateRoleReq", cbCreateRoleReq)
end

function onRecv(index, recv_data)
    print("On Recev Message")
    transpb:toHex(recv_data)

    local length = #recv_data
    if length <= 4
    then
        return
    end

    local real_data = string.sub(recv_data, 5, length) 

    local recv_table = transpb:decode("luadb.MsgHead", real_data)
    diapatchHandler(recv_table["proto"], recv_table["data"])
end

function registerHandler(msgName, func)
    handler_table[msgName] = func
end

function diapatchHandler(msgName, real_data)
    print("DispatchHandler")
    if handler_table[msgName] ~= nil
    then
        handler_table[msgName](real_data)
    end
end

function cbCreateRoleReq(real_data)
    print("Callback Start")

    --local real_table = transpb:decode("luadb.CreateRoleReq", real_data)

    --local result_set = get(1, "select count(*) from users where id = " .. tostring(123))

    --local query = DBQuery.new("")
    --query:insert_into("users", "id", "name", "age", "sex", "phone", "address"):values(2, "linda", 123, 2, "166", "shanghai")
    --local result_set = execute(1, query)

    --if result_set.resultVal == true
    --then
        --print("Lua Execute Success")
        --print(result_set.errorString)
    --end

    --print(result_set)

    send("Send MSG to Client")
end


-- 生成包头
--local head_str = head:pack({length = #bytes})
--local msg_head = {}
--msg_head["proto"] = messageName
--msg_head["data"]  = bytes
--local data = transpb:encode("chess.MsgHead", msg_head)

-- 返回数据库结果









