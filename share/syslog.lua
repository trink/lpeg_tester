local syslog = require 'lpeg.syslog'
local template = "%pri% %TIMESTAMP% %TIMEGENERATED:::date-rfc3339% %HOSTNAME% %syslogtag%%msg:::sp-if-no-1st-sp%%msg:::drop-last-lf%\n"

grammar = syslog.build_rsyslog_grammar(template)
