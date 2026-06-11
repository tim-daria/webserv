#!/usr/bin/env python3

import sys
import os


# POST body
if os.environ.get("REQUEST_METHOD") == "POST":
    body = sys.stdin.read()
    print("Content-Type: text/plain")
    print()
    print("=== CGI TEST (POST) ===")
    print("Body:", body)
if os.environ.get("REQUEST_METHOD") == "GET":
	print("Content-Type: text/plain")
	print()

	print("=== CGI TEST ===")
	print("Method:", os.environ.get("REQUEST_METHOD"))
	print("Path:", os.environ.get("PATH_INFO"))
	print("Query:", os.environ.get("QUERY_STRING"))
    # Get current working directory
	current_directory = os.getcwd()

	# Print it
	print("Current working directory:", current_directory)
