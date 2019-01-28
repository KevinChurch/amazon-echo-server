#!/bin/bash

# Simple integration test.
# This test can only be run on the host machine at the moment. 
# TODO: Figure out how to run this test inside the docker container,
# and make this part of the deployment process.

CWD="$(pwd)" 
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD="../build"
BUILD_COVERAGE="../build_coverage"

goto() {
    cd "$1"
}

run_server() {
    goto "$DIR"
    if [ -d "$BUILD" ]; then
        echo "../build/bin/server ../dev_config &"
        ../build/bin/server ../dev_config &
    elif [ -d "$BUILD_COVERAGE" ]; then
        echo "../build_coverage/bin/server ../dev_config &"
        ../build_coverage/bin/server ../dev_config &
    else
        exit 1
    fi
    SERVER_PID=$!
    # TODO: Figure out how to make the rest of the script wait 
    # for the server to stat up instead of usign sleep. 
    sleep 5
    goto "$CWD"
}

kill_server() {
    echo "kill -9 $SERVER_PID"
    kill -9 "$SERVER_PID"
}

test() {
    if [ "$1" == "$2" ]; then
        echo "pass"
    else
        echo "fail"
        echo "$1" | od -c
        echo "$2" | od -c
        kill_server
        exit 1
    fi
}

run_server

# GOOD REQUEST
STRING_INPUT_1=`echo -e "GET / HTTP/1.1\r\n\r\n" | netcat localhost 8080`
STRING_OUTPUT_1=$'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET / HTTP/1.1\r\n\r\n'
STRING_OUTPUT_1=${STRING_OUTPUT_1%$'\n'} # Removes automatic newline added to end of variable.
test "$STRING_INPUT_1" "$STRING_OUTPUT_1"

STRING_INPUT_2=`curl -s localhost:8080`
STRING_OUTPUT_2=$'GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\n'
STRING_OUTPUT_2=${STRING_OUTPUT_2%$'\n'} # Removes automatic newline added to end of variable.
test "$STRING_INPUT_2" "$STRING_OUTPUT_2"

# BAD REQUESTS
# STRING_INPUT_3=`echo -e "GET / HTTP\r\n\r\n" | netcat localhost 8080`
# STRING_OUTPUT_3=$'HTTP/1.0 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n'
# STRING_OUTPUT_3=${STRING_OUTPUT_3%$'\n'} # Removes automatic newline added to end of variable.
# test "$STRING_INPUT_3" "$STRING_OUTPUT_3"

# STRING_INPUT_4=`echo -e "GEET / HTTP/1.1\r\n\r\n" | netcat localhost 8080`
# STRING_OUTPUT_4=$'HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\n'
# STRING_OUTPUT_4=${STRING_OUTPUT_4%$'\n'} # Removes automatic newline added to end of variable.
# test "$STRING_INPUT_4" "$STRING_OUTPUT_4"

kill_server

exit 0