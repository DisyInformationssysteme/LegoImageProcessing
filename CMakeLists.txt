cmake_minimum_required(VERSION 3.10)
project(LegoImageProcessing)

set(CMAKE_CXX_STANDARD 11)

add_executable(LegoImageProcessing main.cpp lib/stb_image.h Color.h lib/stb_image_write.h)