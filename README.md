_This project has been created as part of the 42 curriculum by dtimofee, nefimov, tsemenov_

# Webserv

## Description

The goal of this project is to gain a deep understanding of how HTTP servers work by building one from scratch in C++98 — handling raw sockets, I/O multiplexing, and the HTTP protocol without relying on external libraries.

The result is a lightweight HTTP/1.0 web server that handles concurrent connections using a non-blocking I/O model with `poll()`, supports CGI script execution, serves static files, and is fully configurable via a Nginx-inspired `.conf` file. Features include custom error pages, file uploads, redirects, directory listing, and per-route method restrictions.

### Project Structure

```
.
├── src/                  # Source files
│   ├── config/           # Server and route configuration
│   ├── filesystem/       # File I/O and path utilities
│   ├── handler/          # HTTP request handling, autoindex, error pages
│   ├── http/             # HTTP request and response parsing
│   ├── parser/           # Config file lexer and parser
│   └── server/           # Server, client, and ServerHub (poll loop)
├── include/              # Header files (mirrors src/ structure)
├── conf/                 # Additional config files for testing
├── integration_tests/    # Python integration tests (pytest)
├── unit_tests/           # C++ unit tests (Catch2)
├── www/                  # Static files served by the server
├── webserv.conf          # Default server configuration
└── Makefile
```

## Instructions

<details>
<summary>Requirements</summary>

- `c++` compiler with C++98 support (clang or gcc)
- `make`
- `Python 3.12+`
- `uv` — Python package manager

</details>

<details>
<summary>Build & Run</summary>

Build:

```bash
make all
```

Run with default config:

```bash
./webserv
```

Run with a custom config:

```bash
./webserv [filename].conf
```

The server is configured via a `.conf` file. Example:

```
server {
    listen 127.0.0.1:8080;
    server_name localhost;

    location / {
        root ./www;
        index index.html;
        methods GET POST;
        autoindex off;
    }
}
```

</details>

<details>
<summary>Testing</summary>

### Unit tests (C++)

Uses [Catch2 v2](https://github.com/catchorg/Catch2/tree/v2.x).

```bash
make test
```

### Integration tests (Python)

Uses `pytest`, `requests`, and `psutil`.

#### Setup (first time only)

Install `uv` if you don't have it:

```bash
pip install uv
```

Then install Python dependencies:

```bash
cd integration_tests
uv sync
```

#### Run

Make sure the project is built first (`make all`), then:

```bash
cd integration_tests
uv run pytest -v
```

> The test suite automatically starts and stops the server — no need to run it manually.

</details>

<details>
<summary>Manual testing</summary>

### curl

```bash
# GET a file
curl http://localhost:8080/index.html

# Upload a file (POST)
curl -X POST http://localhost:8080/upload/file.txt --data-binary @file.txt

# Upload plain text without a file
echo "hello world" | curl -X POST http://localhost:8080/upload/file.txt --data-binary @-

# Multipart POST upload
curl -X POST http://localhost:8080/upload/ -F "file=@localfile.txt"

# DELETE a file
curl -X DELETE http://localhost:8080/upload/file.txt

# Follow redirects
curl -L http://localhost:8080/old-path
```

### telnet

```bash
telnet localhost 8080
```

Then type a raw HTTP request:

```
GET / HTTP/1.0
Host: localhost

```

_(Press Enter twice to send)_

### siege (load testing)

```bash
# Install siege (macOS)
brew install siege

# Install siege (Linux)
sudo apt install siege

# Run 10 concurrent users for 30 seconds
siege -c10 -t30s http://localhost:8080/

# Run with a list of URLs
siege -c10 -t30s -f urls.txt
```

</details>

## Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [RFC 1945](https://greenbytes.de/tech/webdav/rfc1945.html#POST)
- [MDN Guides](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Overview)
- [HTTP Responce Status Codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status#server_error_responses)

## AI usage

- Helped design the project architecture and plan the work
- Summarized documentation and resources
- Provided explanation of key concepts
- Used for refactoring and code optimization
- Assisted with debugging
- Helped draft and refactor HTML, CSS, and JavaScript for served pages and error pages
- Helped write and structure unit and integration tests
- Composed commit messages (GitHub Copilot)
