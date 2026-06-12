#!/usr/bin/env python3

import sys
import os


# POST body
if os.environ.get("REQUEST_METHOD") == "POST":
    body = sys.stdin.read()
    print("Content-Type: text/plain")
    print()
    print("Body:", body)
if os.environ.get("REQUEST_METHOD") == "GET":
	print("Content-Type: text/plain")
	print()
	print("Method:", os.environ.get("REQUEST_METHOD"))
	query = os.environ.get("QUERY_STRING", "")

	if not query:
		print("Hello from CGI!")
	else:
		print("Hello", query)
