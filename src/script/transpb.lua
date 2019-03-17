package.path = package.path..';../opt/lua-protobuf/?.lua'
package.cpath = package.cpath..';../lib/?.so'

transpb = {}

transpb.pb = require "pb"
transpb.protoc = require "protoc"
transpb.ser = require "serpent"

transpb.protoc.paths[#transpb.protoc.paths + 1] = "../proto"
transpb.protoc.include_imports = true
transpb.pb.option("enum_as_value")

--[[
    transpb:load_file(file)        -- 加载proto
    transpb:load(str)              -- 加载文本消息
    transpb:find_message(message)  -- 是否有该消息定义
    transpb:encode(message, data)  -- 根据消息定义，序列化table
    transpb:decode(message, bytes) -- 根据消息定义，反序列话bytes
    head:pack({ length = 0 , type_flag = 0, control_flag = 0, magic_flag   = 0, reflect_flag = 0, reserve_flag = 0, extend_flag  = 0 }) --根据包头结构生成GMsgHead
    head:unpack(headStr)           --根据包头str,返回包头的table结构
--]]

function transpb:toHex(bytes)
    print(self.pb.tohex(bytes))
end

function transpb:showTable(t)
    print(self.ser.block(t))
end

function transpb:load_file(filename)
    local func1 = function() self.protoc:loadfile(filename) end
    if not pcall(func1)
    then
        return false
    end

    return true
end

function transpb:find_message(message)
    local dataType = nil
    local type_table = {}

    for name in self.pb.types() do
        type_table[name] = name
        if name == "."..message then
            dataType = name
        end
    end

    return dataType, type_table
end

function transpb:encode(message, data)
    if self:find_message(message) == nil
    then
        return ""
    end
    
    -- encode lua table data into binary format in lua string and return
    local bytes = assert(self.pb.encode(message, data))
    print(self.pb.tohex(bytes))

    -- and decode the binary data back into lua table
    local data2 = assert(self.pb.decode(message, bytes))
    print(self.ser.block(data2))

    return bytes
end

function transpb:decode(message, bytes)
    if self:find_message(message) == nil
    then
        return {}
    end

    -- and decode the binary data back into lua table
    local data2 = assert(self.pb.decode(message, bytes))
    print(require "serpent".block(data2))

    return data2
end

return transpb