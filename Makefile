CC := clang

CFLAGS := \
	-ftrivial-auto-var-init=zero \
	-fno-strict-aliasing \
	-fwrapv \
	-O1 \
	-g3 \
	-W \
	-Wall \
	-Wextra \
	-Wconversion \
	-Wimplicit-fallthrough \
	-Wmissing-prototypes \
	-Wshadow \
	-Wstrict-prototypes

all: out/coro tidy

out/coro: main.c out/arch_state.o
	@ mkdir -p out
	@ $(CC) $(CFLAGS) -o $@ $^

out/arch_state.o: arch_state.s
	@ mkdir -p out
	@ as -o $@ $^

tidy: main.c
	@ clang-format -i $^

clean:
	@ rm -rf out

.PHONY: all tidy clean
