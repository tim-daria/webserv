#!/usr/bin/env python3
"""
form_handler.py — CGI скрипт который ведёт себя по-разному
в зависимости от метода запроса.

GET  /cgi-bin/form_handler.py           → показать HTML форму
GET  /cgi-bin/form_handler.py?name=Alice → поприветствовать
POST /cgi-bin/form_handler.py           → обработать данные формы
"""

import os
import sys


def parse_query_string(qs):
    """Парсить query string в словарь."""
    params = {}
    if not qs:
        return params
    for pair in qs.split("&"):
        if "=" in pair:
            key, value = pair.split("=", 1)
            # Простое URL декодирование
            value = value.replace("+", " ").replace("%20", " ")
            params[key] = value
    return params


def parse_body(body):
    """Парсить тело POST запроса."""
    return parse_query_string(body)


def handle_get(params):
    """Обработать GET запрос."""
    print("Content-Type: text/html")
    print("")

    # Если пришло имя в query string — поприветствовать
    if "name" in params:
        name = params["name"]
        print("<!DOCTYPE html>")
        print("<html><head><title>Hello!</title></head><body>")
        print("<h1>Hello, " + name + "!</h1>")
        print("<p>Nice to meet you!</p>")
        print("<p>Request info:</p>")
        print("<ul>")
        print("  <li>Method: " + os.environ.get("REQUEST_METHOD", "") + "</li>")
        print("  <li>Query: "  + os.environ.get("QUERY_STRING", "") + "</li>")
        print("  <li>Server: " + os.environ.get("SERVER_NAME", "") + "</li>")
        print("</ul>")
        print("<a href='/cgi-bin/form_handler.py'>Back to form</a>")
        print("</body></html>")
        return

    # Иначе — показать форму
    print("<!DOCTYPE html>")
    print("<html>")
    print("<head><title>CGI Form</title></head>")
    print("<body>")
    print("<h1>CGI Form Handler</h1>")
    print("<hr>")

    # GET форма
    print("<h2>GET Form</h2>")
    print("<form method='GET' action='/cgi-bin/form_handler.py'>")
    print("  <label>Name: <input type='text' name='name'></label>")
    print("  <button type='submit'>Say Hello</button>")
    print("</form>")
    print("<hr>")

    # POST форма
    print("<h2>POST Form</h2>")
    print("<form method='POST' action='/cgi-bin/form_handler.py'>")
    print("  <label>Username: <input type='text' name='username'></label><br><br>")
    print("  <label>Message:  <input type='text' name='message'></label><br><br>")
    print("  <button type='submit'>Submit</button>")
    print("</form>")
    print("<hr>")

    # Информация об окружении
    print("<h2>Environment Variables</h2>")
    print("<table border='1' cellpadding='5'>")
    env_vars = [
        "REQUEST_METHOD",
        "QUERY_STRING",
        "CONTENT_LENGTH",
        "CONTENT_TYPE",
        "SERVER_NAME",
        "SERVER_PORT",
        "GATEWAY_INTERFACE"
    ]
    for var in env_vars:
        value = os.environ.get(var, "not set")
        print("  <tr><td><b>" + var + "</b></td><td>" + value + "</td></tr>")
    print("</table>")
    print("</body></html>")


def handle_post():
    """Обработать POST запрос."""
    # Прочитать тело запроса
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(content_length) if content_length > 0 else ""

    params = parse_body(body)

    print("Content-Type: text/html")
    print("")
    print("<!DOCTYPE html>")
    print("<html>")
    print("<head><title>Form Result</title></head>")
    print("<body>")
    print("<h1>Form Submitted!</h1>")
    print("<hr>")

    if params:
        print("<h2>Received Data:</h2>")
        print("<table border='1' cellpadding='5'>")
        print("  <tr><th>Field</th><th>Value</th></tr>")
        for key, value in params.items():
            print("  <tr><td><b>" + key + "</b></td><td>" + value + "</td></tr>")
        print("</table>")
    else:
        print("<p>No data received</p>")
        print("<p>Raw body: " + body + "</p>")

    print("<hr>")
    print("<a href='/cgi-bin/form_handler.py'>Back to form</a>")
    print("</body>")
    print("</html>")


def handle_unsupported(method):
    """Вернуть 405 для неподдерживаемых методов."""
    print("Status: 405 Method Not Allowed")
    print("Content-Type: text/html")
    print("")
    print("<h1>405 Method Not Allowed</h1>")
    print("<p>Method " + method + " is not supported by this script.</p>")


# ─────────────────────────────────────────────
# Main — определить метод и вызвать нужный handler
# ─────────────────────────────────────────────

method       = os.environ.get("REQUEST_METHOD", "GET")
query_string = os.environ.get("QUERY_STRING", "")
params       = parse_query_string(query_string)

if method == "GET":
    handle_get(params)
elif method == "POST":
    handle_post()
else:
    handle_unsupported(method)
