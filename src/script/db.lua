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

local index = lua_index
local recv_data = lua_recv_data


function onRecv()
    print("On Recev Message")
    --func:toHex(data)

    local recv_table = transpb:decode("luadb.MsgHead", recv_data)
    diapatchHandler(recv_table["proto"], recv_table["data"])
end

function registerHandler(msgName, func)
    handler_table[msgName] = func
end

function diapatchHandler(msgName, real_data)
    if handler_table[msgName] ~= nil
    then
        handler_table[msgName](real_data)
    end
end

local function cbCreateRoleReq(real_data)
    local real_table = transpb:decode("luadb.CreateRoleReq", real_data)

    local result_set = get(index, "select count(*) from users where id = " .. tostring(123))

    print(result_set)
end

transpb:load_file("test.proto")
registerHandler("luadb.CreateRoleReq", cbCreateRoleReq)


-- 生成包头
--local head_str = head:pack({length = #bytes})
--local msg_head = {}
--msg_head["proto"] = messageName
--msg_head["data"]  = bytes
--local data = transpb:encode("chess.MsgHead", msg_head)

-- 返回数据库结果









