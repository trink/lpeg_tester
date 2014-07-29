require "lpeg"

lpeg.locale(lpeg)   -- adds locale entries into 'lpeg' table
local space = lpeg.space^0
local name = lpeg.C(lpeg.alpha^1) * space
local sep = lpeg.S(",;") * space
local pair = lpeg.Cg(name * "=" * space * name) * sep^-1
grammar = lpeg.Cf(lpeg.Ct("") * pair^0, rawset)