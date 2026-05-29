# EBNF grammar rules

```
config          ::= ws (server_block ws)* ;

server_block    ::= "server" ws block ;

block           ::= "{" ws (statement ws)* "}" ;

statement       ::= directive ";" | location_block ;

location_block  ::= "location" ws path ws block ;

directive       ::= listen
                  | server_name
                  | root
                  | client_max_body_size
                  | error_page
                  | index
                  | limit_except
                  | autoindex
                  | auth_required
                  | return
                  | cgi_ext
                  | cgi_path ;

listen              ::= "listen" ws port ;
server_name         ::= "server_name" ws name ;
root                ::= "root" ws path ;
client_max_body_size::= "client_max_body_size" ws number ;
error_page          ::= "error_page" ws status ws+ path ;
index               ::= "index" ws filename ;
limit_except        ::= "limit_except" ws method (ws method)* ;
autoindex           ::= "autoindex" ws onoff ;
auth_required       ::= "auth_required" ws onoff ;
return              ::= "return" ws status ws+ uri ;
cgi_ext             ::= "cgi_ext" ws ext ;
cgi_path            ::= "cgi_path" ws path ;

method          ::= "GET" | "POST" | "DELETE" ;
onoff           ::= "on" | "off" ;

status          ::= digit digit digit ;
port            ::= digit+ ;
number          ::= digit+ ;

name            ::= token ;
path            ::= token ;
uri             ::= token ;
filename        ::= token ;
ext             ::= token ;

token           ::= (unquoted_char)+ ;
unquoted_char   ::= ? any non-whitespace, non-';', non-'{' , non-'}' ? ;

digit           ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

ws              ::= (space | tab | newline | comment)* ;
comment         ::= "#" ? any char except newline ?* newline ;
```
