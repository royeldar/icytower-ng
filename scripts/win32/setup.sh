#!/bin/sh

set -e

docker build -f Dockerfile.base -t icytower-base-win32 .
