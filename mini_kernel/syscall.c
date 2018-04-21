#include "mini_kernel.h"

syscall_func syscall_functions[256] = { 0 };

void register_syscall(int n, syscall_func func) {
	syscall_functions[n] = func;
}

void syscall_handler(int a, int b) {
	if (a > 0 && a < sizeof(syscall_functions) / sizeof(*syscall_functions))
		if (syscall_functions[a] != NULL)
			syscall_functions[a]((void*)b);
}

void syscall(int a, void* b) {
	__asm mov eax, a;
	__asm mov ebx, b;
	__asm int 42;
}