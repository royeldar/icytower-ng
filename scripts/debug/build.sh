#!/bin/sh

set -e

flatpak build-bundle --runtime repo icytower.Debug.flatpak io.github.royeldar.icytower.Debug
