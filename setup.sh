#!/bin/sh

# Check if vcpkg directory exists
if [ ! -d "vcpkg" ]; then
  echo "vcpkg directory not found. Cloning vcpkg..."
  git clone https://github.com/microsoft/vcpkg
else
  echo "vcpkg directory already exists. Skipping clone."
fi

# Set path for vcpkg
echo "Run this command manually (once) to set the VCPKG_ROOT environment variable:"
echo 'export VCPKG_ROOT=$(pwd)/vcpkg'

