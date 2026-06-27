FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential protobuf-c-compiler libprotobuf-c-dev git cmake && \
    rm -rf /var/lib/apt/lists/*

# clone and build e2ap agent
WORKDIR /
RUN git clone -b oai-support https://github.com/EugenioMoro/ocp-e2sim.git
WORKDIR /ocp-e2sim
RUN chmod +x build_e2sim.sh && ./build_e2sim.sh -i

# prepare volume and workdir
VOLUME ["/gNB-e2sm-emu"]
WORKDIR /gNB-e2sm-emu

ENTRYPOINT ["tail", "-f", "/dev/null"]
