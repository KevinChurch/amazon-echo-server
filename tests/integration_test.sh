#!/bin/bash

# Simple integration test.
# This test can only be run on the host machine at the moment. 
# TODO: Figure out how to run this test inside the docker container,
# and make this part of the deployment process.
# TODO: The strings are the same, but it looks like there might be
# some hidden characters differences that I'm not able to detect atm.

STRING1=`echo -e "GET / HTTP/1.1\r\n\r\n" | nc localhost 80`
STRING2=$'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET / HTTP/1.1\r\n'

# echo "$STRING1"
# echo "$STRING2"

if [ "$STRING1" == "$STRING2" ]; then
    echo "pass"
else
    echo "fail"
fi

# diff <(echo "$STRING1") <(echo "$STRING2")