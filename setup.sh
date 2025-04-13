#!/bin/sh

# Check if vcpkg directory exists
if [ ! -d "vcpkg" ]; then
  echo "vcpkg directory not found. Cloning vcpkg..."
  git clone https://github.com/microsoft/vcpkg
else
  echo "vcpkg directory already exists. Skipping clone."
fi

# Set path for vcpkg
export VCPKG_ROOT=$(pwd)/vcpkg