# Webserv

A lightweight HTTP/1.0 web server written in C++98, focusing on low-level networking, CGI, and configuration parsing

## Authors

Nikolai Efimov
Tanya Semenova
Daria Timofeeva

## Requirements

- `c++` compiler with C++98 support (clang or gcc)
- `make`
- `Python 3.12+`
- `uv` — Python package manager

## Build

```bash
make all
```

## Run

```bash
./webserv default.conf
```

## Testing

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

## Project Structure

```
.
├── src/                  # Source files
│   ├── config/           # Server and route config parsing
│   ├── handler/          # HTTP request handler and response
│   └── server/           # Server (socket, accept, send)
├── include/              # Header files
├── integration_tests/    # Python integration tests (pytest)
├── unit_tests/           # C++ unit tests (Catch2)
├── default.conf          # Default server configuration
└── Makefile
```

## Configuration

Server is configured via a `.conf` file. Example:

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

## Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110.html)
- [MDN Guides](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Overview)
