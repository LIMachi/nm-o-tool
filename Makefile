NAME := mem_test
mem_test_DEBUG := 1
mem_test_INC_DIR := memory/inc
override mem_test_SRCS := memory/test.c
BLACK_LIST_SRCS := elf/ nm_bin/ test.c validator_bin/
include Makefiles/MK3/body.mk
include Makefiles/MK3/footer.mk
