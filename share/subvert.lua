require "lpeg"
require "string"

local function match(p, s)
    return string.match(s, "(%d%d%d%d)(%d%d)(%d%d)T(%d%d)(%d%d)(%d%d)")
end

lpeg.match =  match
grammar = lpeg.P""
