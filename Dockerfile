FROM ubuntu:latest


WORKDIR /app


RUN apt-get update && \
    apt-get install -y build-essential wget curl git python3

RUN apt-get -y install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib build-essential ninja-build