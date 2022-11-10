#!/bin/sh

rm -rf opencv.tar.gz opencv-* opencv

apk add zlib libjpeg-turbo libwebp libpng libtiff openjpeg gstreamer ffmpeg v4l-utils

apk add zlib-devel libjpeg-turbo-devel libwebp-devel libpng-devel libtiff-devel openjpeg-devel gstreamer-devel ffmpeg-devel gst-plugins-base-devel v4l-utils-devel vulkan-tools
apk add curl clang lld cmake ninja python

#curl -L https://github.com/opencv/opencv/archive/refs/tags/4.7.0.tar.gz -o opencv.tar.gz
#tar -xf opencv.tar.gz
#mv opencv-* opencv
#cd opencv

git clone https://github.com/opencv/opencv.git
cd opencv

# See https://www.simonwenkel.com/notes/software_libraries/opencv/compiling-opencv.html
cmake -G Ninja -B build \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_INSTALL_LIBDIR=lib \
	-DOPENCV_ENABLE_NONFREE=NO \
	-DCPU_DISPATCH=SSE4_2,AVX512,NEON \
	-DWITH_CUDA=NO \
	-DWITH_NVCUVID=NO \
	-DWITH_CUDA_FAST_MATH=NO \
	-DWITH_CUBLAS=NO \
	-DWITH_CUFFT=NO \
	-DOPENCV_DNN_CUDA=NO \
	-DWITH_DC1394=NO \
	-DWITH_FFMPEG=YES \
	-DWITH_GSTREAMER=YES \
	-DWITH_V4L=YES \
	-DWITH_VULKAN=YES \
	-DWITH_OPENGL=NO \
	-DWITH_OPENCL=NO \
	-DWITH_IPP=NO \
	-DWITH_TBB=NO \
	-DWITH_EIGEN=NO \
	-DWITH_OPENMP=NO \
	-DWITH_GTK=NO \
	-DWITH_QT=NO \
	-DWITH_VTK=NO \
	-DBUILD_TIFF=NO \
	-DBUILD_JPEG=NO \
	-DBUILD_PNG=NO \
	-DBUILD_WEBP=NO	\
	-DBUILD_OPENJPEG=NO \
	-DBUILD_EXAMPLES=NO \
	-DBUILD_TESTS=NO \
	-DBUILD_PERF_TESTS=NO \
	-DINSTALL_TESTS=NO \
	-DBUILD_opencv_apps=NO \
	-DBUILD_opencv_java=NO \
	-DBUILD_opencv_python=NO \
	-DENABLE_THIN_LTO=YES \
	-DBUILD_SHARED_LIBS=YES \
	.

cd build
ninja
ninja install

rm -rf opencv.tar.gz opencv-* opencv
