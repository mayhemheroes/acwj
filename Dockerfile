FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y make gcc

ADD . /acwj
WORKDIR /acwj/
RUN make -C ./02_Parser/ parser
RUN make -C ./01_Scanner/ scanner


RUN mkdir -p /deps
RUN ldd /acwj/02_Parser/parser | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'cp % /deps;'
RUN ldd /acwj/01_Scanner/scanner | tr -s '[:blank:]' '\n' | grep '^/' | xargs -I % sh -c 'cp % /deps;'

FROM ubuntu:20.04 as package

COPY --from=builder /deps /deps
COPY --from=builder /acwj/02_Parser/parser /acwj/02_Parser/parser
COPY --from=builder /acwj/01_Scanner/scanner /acwj/01_Scanner/scanner
ENV LD_LIBRARY_PATH=/deps
