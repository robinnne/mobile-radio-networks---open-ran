# gNB-e2sm-emu
This repo contains a simple gNB emulator to test custom E2SM definitions. Definitions are plugged in as the 'oai-oran-protolib' submodule.

## Docker container deployment

This is the recommended deployment option, as it allows the emulator to run on any host system. First, [install docker](https://docs.docker.com/get-docker/). You'll also need `git`. 

### Instructions

Clone this repo and enter the repo's folder:

```
git clone https://github.com/ANTLab-polimi/gNB-e2sm-emu.git
cd gNB-e2sm-emu
```

Then build the image:

```
docker build -f Dockerfile -t gnb_e2sm_emu:mrn_base .
```

If the build is successfull, you will see the built image with `docker images`

Now start the container:
```
docker run -dit --name gnb --net=host gnb_e2sm_emu:mrn_base
```
The gNB emulator does not start automatically. First connect a terminal to the running container:

```
docker exec -it gnb bash
```

and then in this terminal you can run the gNB: 
```
./build/gnb_e2server_emu
```

## Baremetal 
This depends on your system, but you basically need a C compiler, cmake, and protobuf-c. Ubuntu instructions can be extracted from the Dockerfile.
