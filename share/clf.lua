local clf = require 'common_log_format'
local log_format = '$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent"'

grammar = clf.build_nginx_grammar(log_format)
