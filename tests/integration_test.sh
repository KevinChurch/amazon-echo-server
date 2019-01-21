#!/bin/bash

# Simple integration test.
# This test can only be run on the host machine at the moment. 
# TODO: Figure out how to run this test inside the docker container,
# and make this part of the deployment process.

STRING1=`echo -e "hello" | nc localhost 80`
STRING2=$'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nhello'

if [ "$STRING1" == "$STRING2" ]; then
    echo "pass"
else
    echo "fail"
fi