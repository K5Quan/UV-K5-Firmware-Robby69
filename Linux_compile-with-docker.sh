#!/bin/sh
#export DOCKER_DEFAULT_PLATFORM=linux/amd64
#export DOCKER_NETWORK="--network=host"

# first clean images older than 24h, you will run out of disk space one day
docker image prune -a --force --filter "until=24h"


IMAGE_NAME="uvk5"
rm "${PWD}/compiled-firmware/*"
echo "Building docker image $IMAGE_NAME"
if ! docker build -t $DOCKER_NETWORK $IMAGE_NAME .
then
    echo "Failed to build docker image"
    exit 1
fi

FR() {
    echo "FR compilation..."
    docker run --rm -v "${PWD}/compiled-firmware/:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "rm ./compiled-firmware/*; cd /app && make -s \
        ENABLE_FR_BAND=1 \
        ENABLE_PL_BAND=0 \
        TARGET=Robzyl.fr \
        && cp f4hwn.custom* compiled-firmware/"
}

PL() {
    echo "Standard compilation..."
    docker run --rm -v "${PWD}/compiled-firmware:/app/compiled-firmware" $IMAGE_NAME /bin/bash -c "rm ./compiled-firmware/*; cd /app && make -s \
        ENABLE_FR_BAND=0 \
        ENABLE_PL_BAND=1 \
        TARGET=Robzyl.pl \
        && cp f4hwn.standard* compiled-firmware/"
}



case "$1" in
    FR)
        fr
        ;;
    PL)
        pl
        ;;
    all)
        fr
        pl
        ;;
    *)
        echo "Usage: $0 {fr|pl|all}"
        exit 1
        ;;
esac