#!/bin/bash

git submodule update --init
git submodule sync --recursive
git submodule update --remote --merge
cd oai-oran-protolib
git checkout mrn-base
git pull
