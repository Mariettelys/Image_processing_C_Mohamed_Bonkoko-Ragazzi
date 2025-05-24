cmake_minimum_required(VERSION 3.30)
project(Image_processing_C_Mohamed_Bonkoko_Ragazzi C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Image_processing_C_Mohamed_Bonkoko_Ragazzi
    bmp8.c
    main.c
        bmp24.c
        bmp24.h)
