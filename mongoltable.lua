local connection = require'connections'

local mt_mongo = {
    __index = function(self, k)
        return connection.get(self, k)
    end,

    __newindex = function(self, k, v)
        connection.set(self, k, v)
    end,

    __gc = function(self)
        connection.unbind(self)
    end,
}

function mongoltable(conn_prefs)
    local t = setmetatable({}, mt_mongo)
    connection.bind(t, conn_prefs)
    return t
end

return {
    __NAME = 'mongoltable',
    __VERSION = '0.0.1',
    mongoltable = mongoltable,
}
