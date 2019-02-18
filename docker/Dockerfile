### Build/test container ###
# Define builder stage
FROM amazon-echo-server:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure -VV


### Deploy container ###
# Define deploy stage
FROM ubuntu:latest as deploy

# TODO(!): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server ./build/bin/
COPY --from=builder /usr/src/project/new_example_config .
COPY --from=builder /usr/src/project/new_dev_config .
COPY --from=builder /usr/src/project/static ./static
COPY --from=builder /usr/src/project/www/data ./www/data
# TODO(!): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
ENTRYPOINT ["./build/bin/server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
CMD ["./new_example_config"]
