#!/bin/bash

# Simple integration test.
# This test can only be run on the host machine at the moment. 
# TODO: Figure out how to run this test inside the docker container,
# and make this part of the deployment process.

# GLOBAL VARIABLE
CWD="$(pwd)"
TEST_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD="build"
BUILD_COVERAGE="build_coverage"
PASS=1

# FUNCTIONS

# goto directory
goto() {
    cd "$1"
}

# starts server
run_server() {
    if [ -d "$BUILD" ]; then
        echo "build/bin/server dev_config &"
        build/bin/server dev_config &
    elif [ -d "$BUILD_COVERAGE" ]; then
        echo "build_coverage/bin/server dev_config &"
        build_coverage/bin/server dev_config &
    else
        exit 1
    fi
    SERVER_PID=$!
    # TODO: Figure out how to make the rest of the script wait 
    # for the server to stat up instead of usign sleep. 
    sleep 3
}

# kills server
kill_server() {
    echo "kill -9 $SERVER_PID"
    kill -9 "$SERVER_PID"
}

# tests string equality
# $1 string 1
# $2 string 2
test_equal() {
    if [ "$1" == "$2" ]; then
        echo "pass"
    else
        echo "fail"
        PASS=0
        echo "$1" | od -c
        echo "$2" | od -c
    fi
}

# tests echo server
# must include hidden characters such as CRLF "\r\n"
# $1 HTTP Request Message
# $2 HTTP Response Message
echo_test() {
    EXPECTED=`echo -e "$1" | netcat localhost 8080`
    ACTUAL=$2
    ACTUAL=${ACTUAL%$'\n'} # Removes automatic newline added to end of variable.
    test_equal "$EXPECTED" "$ACTUAL"
}

# tests files returned by static content server
# $1 URL
# $2 FILE PATH
file_test() {
    FILE=`basename $1`
    EXT="${FILE##*.}"
    if [ "$FILE" == "$EXT" ]; then
        EXT="html"
    fi
    TEMP_FILE="temp.$EXT"
    curl -s $1 > $TEMP_FILE
    if cmp -s $TEMP_FILE $2 ; then
        echo "pass"
    else
        echo "fail"
        PASS=0
    fi
    rm $TEMP_FILE
}

# tests that server is multithreaded
multithreaded_test() {
    echo "curling localhost:8080/sleep"
    curl -s localhost:8080/sleep &
    SLEEP_PID=$!
    echo "curling localhost:8080/echo"
    curl -s localhost:8080/echo &
    ECHO_PID=$!
    wait $SLEEP_PID
    if ! kill -0 $ECHO_PID; then
        echo "pass"
    else
        echo "fail"
        PASS=0
    fi
}


# TESTS

goto "$TEST_DIR"
goto ".."

run_server

# Echo Handler
INPUT=$"GET /echo HTTP/1.1\r\n\r\n"
OUTPUT=$'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\n\r\n'
echo_test "$INPUT" "$OUTPUT"

# INPUT=$"GET / HTTP\r\n\r\n"
# OUTPUT=$'HTTP/1.0 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n'
# echo_test "$INPUT" "$OUTPUT"

# INPUT=$"GEET / HTTP/1.1\r\n\r\n"
# OUTPUT=$'HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\n'
# test_equal "$INPUT" "$OUTPUT"

INPUT=`curl -s localhost:8080/echo`
OUTPUT=$'GET /echo HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\n'
OUTPUT=${OUTPUT%$'\n'} # Removes automatic newline added to end of variable.
test_equal "$INPUT" "$OUTPUT"

# Sleep Handler
INPUT=`curl -s localhost:8080/sleep`
OUTPUT=$'Sleping 5 seconds...\r\n'
OUTPUT=${OUTPUT%$'\n'} # Removes automatic newline added to end of variable.
test_equal "$INPUT" "$OUTPUT"

# HTML Handler
file_test "localhost:8080/static/index.html" "static/index.html"

# Image Handler
file_test "localhost:8080/static/amazon.jpg" "static/amazon.jpg"

# TXT Handler
file_test "localhost:8080/static/amazon.txt" "static/amazon.txt"

multithreaded_test

kill_server

if [ "$PASS" == 1 ]; then
    exit 0
else
    exit 1
fi