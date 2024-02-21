#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define assert_zero(p) (_assert_zero((p), sizeof(*(p))))

static void
_assert_zero(void *p, s64 n)
{
	assert(n > 0);
	for (s64 i = 0; i < n; i++) {
		assert(((u8 *)p)[i] == 0);
	}
}

struct arch_state {
	u64 pc;
	u64 sp;
	u64 x[12];
	u64 d[8];
};

extern void save_arch_state(struct arch_state *state, void *restore_point);
extern void restore_arch_state(struct arch_state *state);
extern void swap_arch_state(
        struct arch_state *current, struct arch_state *target);

__attribute((noinline, unused)) static void
print_arch_state(struct arch_state *state)
{
	printf(" pc = 0x%016llx\n", state->pc);
	printf(" sp = 0x%016llx\n", state->sp);

	for (s64 reg = 19; reg <= 30; reg++) {
		s64 i = reg - 19;
		u64 value = state->x[i];
		if (reg < 10) {
			printf(" ");
		}
		printf("x%lld = 0x%016llx\n", reg, value);
	}

	for (s64 reg = 8; reg <= 15; reg++) {
		s64 i = reg - 8;
		u64 value = state->d[i];
		if (reg < 10) {
			printf(" ");
		}
		printf("d%lld = 0x%016llx (%f)\n", reg, value, *(f64 *)&value);
	}
}

struct arch_state example1_state;
struct arch_state example2_state;
struct arch_state main_state;

__attribute((noinline)) static void
example1(void)
{
	printf("example1: init\n");

	for (f64 i = 0; i < 4; i++) {
		printf("example1: %f\n", i);
		swap_arch_state(&example1_state, &example2_state);
	}

	printf("example1: end\n");
}

__attribute((noinline)) static void
example2(void)
{
	printf("example2: init\n");
	for (;;) {
		swap_arch_state(&example2_state, &example1_state);
		printf("example2: tick\n");
	}
	printf("example2: end\n");
}

static void
create_arch_state(struct arch_state *state, s64 stack_size, void *fn)
{
	s64 frame_size = 16;
	s64 red_zone_size = 128;

	assert(stack_size >= frame_size + red_zone_size);
	assert_zero(state);

	u64 sp = (u64)calloc(1, (u64)(stack_size + 32 * 1024));
	assert(sp % 16 == 0);
	state->sp = sp;
	state->pc = (u64)fn;
}

s32
main(void)
{
	create_arch_state(&example1_state, 1520, example1);
	create_arch_state(&example2_state, 1520, example2);
	example1_state.x[30 - 19] = (u64)(&&end);

	save_arch_state(&main_state, &&end2);
	restore_arch_state(&example1_state);
end:
	restore_arch_state(&main_state);
end2:
	printf("exiting!\n");
}
