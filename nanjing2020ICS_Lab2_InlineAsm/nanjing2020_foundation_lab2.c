#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define ARRLEN(A) (sizeof (A) / sizeof(A[0]))
#define STRLEN(B) (ARRLEN(B) - 1)




int64_t asm_add(int64_t a, int64_t b)
{
    __asm__
    (
        "addq	%[a],	%[b]	\n\t"    // Add a to b.
        : [b] "+r" (b) 
        : [a] "r" (a)
    );

    return b;
}

int asm_popcnt(uint64_t x)
{
    __asm__
    (
        "popcnt    %[re], %[re]\n\t" // Cheating, directly using a instruction :).
        : [re] "+r" (x)
    );

    return x;
}

void* asm_memcpy(void *dest, const void *src, size_t n)
{
    __asm__
    (
		"xorq	%%rcx, %%rcx	\n\t"	// Use rcx as a counter. Set to 0.
		"cmpq	%%rcx, %[n]		\n\t"	// Compare with n.
		"je		END%=			\n\t"	// If equal to n (which is 0), jump to END.

	"LOOP%=: \n\t"
		
		"movb	(%[src], %%rcx), %%al	\n\t" // Move 1 byte from src[rcx] to al.
		"movb	%%al, (%[dest], %%rcx)	\n\t" // Move from al to dest[rcx]
		"incq	%%rcx					\n\t" // ++rcx.
		"cmpq	%%rcx, %[n]				\n\t" // if rcx == n:
		"jne	LOOP%=					\n\t"

	"END%=:	\n\t"

		:	
		:	[dest] "r" (dest),
			[src] "r" (src),
        	[n] "r" (n)
		: "rcx", "al"
    );

    return dest;
}

typedef struct
{
    uint64_t rbx;
	uint64_t rbp;

	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint64_t rsp;
	uint64_t rip; // The program counter.
} __attribute__ ((aligned (8))) JmpBufferStruct;

typedef JmpBufferStruct asm_jmp_buf[1];

static asm_jmp_buf jmpBuffer;

int asm_setjmp(asm_jmp_buf env)
{
	__asm__ volatile
	(
		"movq	%%rbx,	(%[buffer])			\n\t"
		"movq	%%rbp,	8(%[buffer])		\n\t"

		"movq	%%r12,	16(%[buffer])		\n\t"
		"movq	%%r13,	24(%[buffer])		\n\t"
		"movq	%%r14,	32(%[buffer])		\n\t"
		"movq	%%r15,	40(%[buffer])		\n\t"

		"lea	8(%%rsp),	%%rdx			\n\t"
		"movq	%%rdx,		48(%[buffer])	\n\t"

		"movq	(%%rsp),	%%rax			\n\t"
		"movq	%%rax,		56(%[buffer]) 	\n\t"

		:	[buffer] "+r" (env)
		:
	);

    return 0;
}

void __attribute__((noreturn)) asm_longjmp(asm_jmp_buf env, int val)
{
    __asm__ volatile
	(
		"movq	(%[buffer]), %%rbx		\n\t"
		"movq	8(%[buffer]), %%rbp		\n\t"
	
		"movq	16(%[buffer]), %%r12	\n\t"
		"movq	24(%[buffer]), %%r13	\n\t"
		"movq	32(%[buffer]), %%r14	\n\t"
		"movq	40(%[buffer]), %%r15	\n\t"

		"movq	48(%[buffer]), %%rsp	\n\t"
		"movq	56(%[buffer]), %%rdx	\n\t"

		"movq	$1, %%rax				\n\t"

		"cmpl		$0, %[val]			\n\t"
		"cmovnel	%[val], %%eax		\n\t"

		"jmp		*%%rdx				\n\t"
		:	
		:	[buffer] "r" (env),
			[val] "rm" (val)
		:
	);

	assert(0);
}

void bb()
{
	asm_longjmp(jmpBuffer, 99);
	assert(0);
}

void aa()
{
	bb();
}

void test()
{
	__asm__
	(	
		"movq (%rsp), %rax \n\t"
		"addq $8, %rsp\n\t"
		"jmp *%rax\n\t"
	);
}

int main()
{
    int64_t a = 100;
    int64_t b = 7456465456;
	
	// Test add and popcnt.
    assert(asm_add(a, b) == a + b);
    assert(asm_popcnt(a + b) == __builtin_popcountll(a + b));

    char str1[] = "H";
    char str2[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

	// Test memcpy	
	asm_memcpy(str2, str1, ARRLEN(str1));
	printf("%s %s %d\n", str2, str2 + ARRLEN(str1), strcmp(str1, str2));
	for (size_t i = ARRLEN(str1) + 1; i < STRLEN(str2); ++i)
	{
		assert(str2[i] == 'A');
	}

	// Test set jmp.
	const int val = asm_setjmp(jmpBuffer);
	if (val == 0)
	{
		aa();
	}
	else if (val == 99) 
	{
		printf("Success!\n");
	}
	else
	{
		printf("Failed\n");
		assert(0);
	}

    return 0;
}