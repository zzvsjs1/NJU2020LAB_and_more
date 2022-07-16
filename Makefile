GCC = gcc
CC_FLAGS += -Wall -Wextra -std=c17

MAKEFLAGS += -j3

SRC := $(shell find . -name "*.c" -type f)

OBJS = $(patsubst %.c,%.o,$(wildcard $(SRC)))

.PHONY: all clean 

.default: all

all: bin 

clean:
	@rm -f $(PROGRAM_NAME);
	@find ./ -type f \( -iname \*.o -o -iname \*.d -o -iname \*.S \) -delete;

bin: $(SRC:%.c=%.o)
	$(GCC) $(CC_OP_FLAG) $(CC_FLAGS) $(LIB_FLAGS) $(PROGRAM_SANITIZE_FLAG) $^ -o $(PROGRAM_NAME)

%.o: %.c
	$(GCC) $(CC_FLAGS) $(PROGRAM_DEBUG_FLAG) $(PROGRAM_OP_FLAG) $(PROGRAM_SANITIZE_FLAG) -MD -c $*.c -o $*.o
	$(GCC) $(CC_FLAGS) $(ASM_DEBUG_FLAG) $(ASM_OP_FLAG) -MD -S $*.c -o $*.S

-include $(SRC:%.c=%.d)
