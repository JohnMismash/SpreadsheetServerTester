FROM ubuntu

SHELL ["/bin/bash", "-c"]

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ="America/Salt Lake City"

RUN apt-get -y update && apt-get -y install \
  build-essential \
  libboost-all-dev \
  && mkdir /tester

COPY . .

RUN  make
