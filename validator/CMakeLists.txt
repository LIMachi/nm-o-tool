cmake_minimum_required(VERSION 3.12)
project(validator)
include_directories(inc )
add_executable(validator src/elf/header.c inc/mach_o_validator.h src/mach_o/macho_validator.c)
