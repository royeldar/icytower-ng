#!/bin/sh

set -e

rm -rf repo

flatpak-builder --force-clean --repo=repo builddir io.github.royeldar.icytower.yml
flatpak build-bundle repo icytower.flatpak io.github.royeldar.icytower
