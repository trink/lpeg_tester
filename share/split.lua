require "lpeg"

local sep = lpeg.P" "
local elem = lpeg.C((1 - sep)^0)
grammar = elem * (sep * elem)^0
