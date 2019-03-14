package.path = package.path..';../src/script/?.lua'

require "transpb"

--[[
    transpb:load_file(file)        -- 加载proto
    transpb:load(str)              -- 加载文本消息
    transpb:find_message(message)  -- 是否有该消息定义
    transpb:encode(message, data)  -- 根据消息定义，序列化table
    transpb:decode(message, bytes) -- 根据消息定义，反序列话bytes
    head:pack({ length = 0 , type_flag = 0, control_flag = 0, magic_flag   = 0, reflect_flag = 0, reserve_flag = 0, extend_flag  = 0 }) --根据包头结构生成GMsgHead
    head:unpack(headStr)           --根据包头str,返回包头的table结构
--]]

print("Lua Start")

local handler_table = {}
local send_length

--local index = lua_index
--local recv_data = lua_recv_data

function init()
    transpb:load_file("test.proto")
    registerHandler("luadb.CreateRoleReq", cbCreateRoleReq)
end

function onRecv(index, recv_data, length)
    print("On Recev Message")
    --func:toHex(data)

    send_length = length

    local recv_table = transpb:decode("luadb.MsgHead", recv_data)
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

    local real_table = transpb:decode("luadb.CreateRoleReq", real_data)

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

    send(send_length)
end



init()


-- 生成包头
--local head_str = head:pack({length = #bytes})
--local msg_head = {}
--msg_head["proto"] = messageName
--msg_head["data"]  = bytes
--local data = transpb:encode("chess.MsgHead", msg_head)

-- 返回数据库结果









