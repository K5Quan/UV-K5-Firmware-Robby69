FROM archlinux:latest

RUN pacman -Syyu --noconfirm && \
    pacman -S --noconfirm base-devel arm-none-eabi-gcc arm-none-eabi-newlib git python-pip python-crcmod && \
    pacman -Scc --noconfirm

WORKDIR /app
COPY . .

#RUN git submodule update --init --recursive
# RUN make && cp firmware* compiled-firmware/
