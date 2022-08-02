FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y make gcc

ADD . /acwj
WORKDIR /acwj/02_Parser
RUN make parser

RUN mkdir -p /deps
RUN ldd /acwj/02_Parser/parser | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'cp % /deps;'

FROM ubuntu:20.04 as package

COPY --from=builder /deps /deps
COPY --from=builder /acwj/02_Parser/parser /acwj/02_Parser/parser
ENV LD_LIBRARY_PATH=/deps
