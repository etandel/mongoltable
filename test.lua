#!/usr/bin/env lua5.2

----------- Helper funcs----------
local function is_type(v, xtype)
    if type(v) == xtype then
        return true
    else

        return false, string.format('element "%s" is not %s', tostring(v), xtype)
    end
end

local function is_table(v)
    return is_type(v, 'table')
end

local function is_function(v)
    return is_type(v, 'function')
end


-------- Test connections --------
local con = require 'connections'

assert(is_table(con))
assert(is_function(con.get))
assert(is_function(con.set))


-------- Test mongoltable ---------
local mongot = require 'mongoltable'
local mongoltable = mongot.mongoltable

assert(mongot.__VERSION == '0.0.1', 'wrong version!')
assert(mongot.__NAME == 'mongoltable', 'wrong name!')

assert(is_function(mongoltable))
assert(is_table(mongoltable()))


---------------------------------
print "All tests passed! =)"
