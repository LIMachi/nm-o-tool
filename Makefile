NAME := mem_test.a
mem_test.a_DEBUG := 1
mem_test.a_INC_DIR := memory/inc
override mem_test.a_TEST_SRCS := memory/test.c
BLACK_LIST_SRCS := elf/ nm_bin/ test.c validator_bin/
include Makefiles/MK3/body.mk
