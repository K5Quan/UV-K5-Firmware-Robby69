FROM ubuntu:22.04

# Préinstallations de base
RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y \
  curl \
  git \
  make \
  python3 \
  python3-pip \
  gcc \
  g++ \
  libncurses-dev \
  ca-certificates \
  unzip \
  xz-utils \
  build-essential \
  wget

# Toolchain ARM (remplace ou adapte selon version)
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 && \
    tar -xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /opt && \
    rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

ENV PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

WORKDIR /workspaces/ROBZYL
RUN pip3 install crcmod
