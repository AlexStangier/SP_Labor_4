cmake_minimum_required(VERSION 3.19)
project(lab4 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(lab4
        Client.c
        lab4.h
        Server.c
        StatisticThread.c ResponseThread.c)
