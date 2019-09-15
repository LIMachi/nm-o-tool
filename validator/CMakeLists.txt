cmake_minimum_required(VERSION 3.12)
project(validator)
include_directories(inc )
add_executable(validator src/validator.c src/load_file.c src/archive.c src/fat.c src/main.c src/load_commands.c src/elf/header.c src/macho.c)
