local mongoconn = require'mongoconn'


local connections = setmetatable({}, {__mode='k'})


local function bind(t, connparams)
    connections[t] = {
        conn = mongoconn.open(connparams.host, connparams.port), 
        collpath = ('%s.%s'):format(connparams.database, connparams.collection),
    }
end


local function unbind(t)
    if connections[t] then
        mongoconn.close(connections[t].conn)
        connections[t] = nil
    end
end


local mt_mongo = {
    __index = function(self, k)
        return mongoconn.get(
            connections[self].conn,
            connections[self].collpath,
            k
        )
    end,

    __newindex = function(self, k, v)
        mongoconn.set(
            connections[self].conn,
            connections[self].collpath,
            k, v
        )
    end,

    __gc = function(self)
        unbind(self)
    end,

    __type = 'mongoltable', --for those who care...
}


local function mongoltable(connparams)
    assert(connparams.host, "host missing!")
    assert(connparams.port, "port missing!")
    assert(connparams.database, "database missing!")
    assert(connparams.collection, "collection missing!")

    local t = setmetatable({}, mt_mongo)
    bind(t, connparams)
    return t
end


return {
    __NAME = 'mongoltable',
    __VERSION = '0.0.1',
    mongoltable = mongoltable,
}

