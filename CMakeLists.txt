cmake_minimum_required(VERSION 3.19)
project(lab4 C)
set(CMAKE_C_STANDARD 11)
include_directories(.)
add_executable(Server Server.c StatisticThread.c lab4.h)
add_executable(Client Client.c ResponseThread.c lab4.h)


