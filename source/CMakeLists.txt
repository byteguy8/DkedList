cmake_minimum_required(VERSION 3.2)

project(dkedlist)

add_library(dkedlist STATIC dkedlist.c)

add_compile_options(-Wall -Werror -pedantic)