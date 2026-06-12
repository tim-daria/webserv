#!/usr/bin/env python3
# cgi-bin/hello.py — greets the user by name (GET query or POST body)
import os
import sys
from urllib.parse import parse_qs, unquote_plus

method = os.environ.get("REQUEST_METHOD", "")

if method == "POST":
    data = sys.stdin.read()
else:
    data = os.environ.get("QUERY_STRING", "")

params = parse_qs(data)
name = params.get("name", ["stranger"])[0]

print("Content-Type: text/html")
print()
print("<!DOCTYPE html>")
print("<html><head><title>Hello</title></head><body>")
print("<h1>Hello, {}!</h1>".format(name))
print('<a href="/cgi-bin/test.html">Back to CGI tester</a>')
print("</body></html>")
