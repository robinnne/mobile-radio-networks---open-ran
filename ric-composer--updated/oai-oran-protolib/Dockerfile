FROM ubuntu:22.04 as protobuf_builder

# Install protobuf compilers
RUN apt-get update && \
    apt-get install -y protobuf-compiler protobuf-c-compiler

# Set the volume and working directory
VOLUME ["/oai-oran-protolib"]
WORKDIR /oai-oran-protolib

# Run the script to compile protobuf definitions
ENTRYPOINT ["./compile_definitions.sh"]
