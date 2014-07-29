require "lpeg"

local field = '"' * lpeg.Cs(((lpeg.P(1) - '"') + lpeg.P'""' / '"')^0) * '"'
+ lpeg.C((1 - lpeg.S',\n"')^0)

grammar = field * (',' * field)^0 * (lpeg.P'\n' + -1)