GCC = gcc
CC_FLAGS = -Wall -Wextra -std=c17

CC_OP_FLAG = -O1

#CC_OP_FLAG = -g -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow

#CC_OP_FLAG = -g 

CC_FLAG_DEPENDENCIES = -MD

MAKEFLAGS += -j3

SRC := $(shell find . -name "*.c" -type f)

OBJS = $(patsubst %.c,%.o,$(wildcard $(SRC)))

.PHONY: all clean 

.default: all

all: test 

clean:
	@rm -rf a;
	@find ./ -type f \( -iname \*.o -o -iname \*.d -o -iname \*.S \) -delete;

test: $(SRC:%.c=%.o)
	$(GCC) $(CC_OP_FLAG) $(CC_FLAGS) $^ -o a

%.o: %.c
	$(GCC) $(CC_FLAGS) $(CC_OP_FLAG) $(CC_FLAG_DEPENDENCIES) -c $*.c -o $*.o
	$(GCC) $(CC_FLAGS) $(CC_OP_FLAG) $(CC_FLAG_DEPENDENCIES) -S $*.c -o $*.S

-include $(SRC:%.c=%.d)
