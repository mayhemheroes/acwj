FROM --platform=linux/amd64 ubuntu:20.04

## Install build dependencies.
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y make gcc

ADD . /acwj
WORKDIR /acwj/02_Parser
RUN make parser
