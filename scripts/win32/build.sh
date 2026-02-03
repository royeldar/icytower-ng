#!/bin/sh

set -e

rm -f icytower.zip

IMAGE_ID=
CONTAINER_ID=

cleanup() {
    [ -n "$CONTAINER_ID" ] && docker rm -f "$CONTAINER_ID" >/dev/null 2>&1
    [ -n "$IMAGE_ID" ] && docker rmi -f "$IMAGE_ID" >/dev/null 2>&1
    rm -f .cidfile
    rm -f .iidfile
}

trap cleanup EXIT

docker image inspect icytower-base-win32 >/dev/null
docker build -f Dockerfile.win32 --iidfile .iidfile .
IMAGE_ID=$(cat .iidfile)
docker create --cidfile .cidfile $IMAGE_ID >/dev/null
CONTAINER_ID=$(cat .cidfile)
docker cp $CONTAINER_ID:/app/icytower.zip .
