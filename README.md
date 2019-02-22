# amazon-echo-server

## Introduction

Welcome to **amazon-echo-server**, a C++ implemented server.

## File Structure
1. Header files are stored in `/include`.
2. cpp files are stored in `/src`.
3. test files are stored in `/tests`.

## Setup Instructions

```bash
# Create a `build` directory for out-of-source builds.
mkdir build
cd build
cmake ..
make
```

```bash
# Create a `build_coverage` directory for coverage reports.
mkdir build_coverage
cd build_coverage
cmake -DCMAKE_BUILD_TYPE=Coverage ..
make coverage
```

## Docker

```bash
# Run in project root directory.
# Build docker base.
docker build -f docker/base.Dockerfile -t amazon-echo-server:base .
# Build docker image.
docker build -f docker/Dockerfile -t my_image .
# Run docker container. Example maps local port 8080 to container port 80.
docker run --rm -p 127.0.0.1:8080:80 --name my_run my_image:latest
# Stop docker container.
docker container stop my_run
```

## Server

Server can also be run via `docker run` command.
```bash
# Must be ran in project root for now. Need to fix this bug.
# Optionally, choose the number of threads allocated to the server.  Four threads are used by default.
./build/bin/server <config_file> [num_of_threads]
```

## Tests

```bash
# Run in build directory.
ctest -V #Add -V for more options
```

## Configuration File

Config files are written in a format similar to Nginx config files. Format consists of key-value pairs and blocks.

### Documented Keys:
```nginx
port <port number> # server's listening port. required.
root <path> # project root path. required.
handler <handler name> { # handler block.
    location <path> # url path handler responds to. required.
    root <path> # path of content.
    log <level> <file> # log level and file.
}
```

## Handlers

Steps to add a new handler.
1. Add handler configuration settings to config file.
2. Create new handler class and inherit from `Handler.`
3. Add handler to `handler_map_` object in `server.cc`.
4. Create new handler tests.

Refer to header files of handlers (ex. static_handler.h, status_handler.h, echo_handler.h) for examples

## Authors

* Kevin Church
* Simon Lam
* Thomas Pan
* Yongbum Yoo
