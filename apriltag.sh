#!/bin/sh

rm -rf apriltag.tar.gz apriltag-* apriltag

apk add curl clang lld cmake ninja

curl -L https://github.com/AprilRobotics/apriltag/archive/refs/tags/v3.2.0.tar.gz -o apriltag.tar.gz
tar -xf apriltag.tar.gz
mv apriltag-* apriltag
cd apriltag

cmake -G Ninja -B build \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_INSTALL_LIBDIR=lib \
	-DCMAKE_C_FLAGS="-flto=thin" \
	-DBUILD_PYTHON_WRAPPER=NO \
	-DBUILD_SHARED_LIBS=YES \
	.

cd build
ninja
ninja install

rm -rf apriltag.tar.gz apriltag-* apriltag
