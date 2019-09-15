cmake_minimum_required(VERSION 3.12)
project(ft_nm)
include_directories(inc )
add_executable(ft_nm src/swap.c src/validator_macho.c src/mem_validate_null_string.c src/nm.c src/nm_type_char.c src/main.c src/mem_access.c)
