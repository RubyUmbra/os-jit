#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>

typedef unsigned char (* func)(unsigned char);

/** unsigned char shift(unsigned char x) { return x + 0; } */
unsigned char shift_func[] = {
        0x55,               // push   %rbp
        0x48, 0x89, 0xe5,   // mov    %rsp,%rbp
        0x89, 0xf8, 0x88,   // mov    %edi,%eax
        0x45, 0xfc, 0x0f,   // mov    %al,-0x4(%rbp)
        0xb6, 0x45, 0xfc,   // movzbl -0x4(%rbp),%eax
        0x83, 0xc0, 0x00,   // sub    $0x1,%eax         // change shift here
        0x5d,               // pop    %rbp
        0xc3,               // retq
};

void change_shift(unsigned char c) { memcpy(shift_func + 15, &c, 1); }

func shift;

void map_func_mem() {
    shift = mmap(NULL, sizeof(shift_func), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (shift == MAP_FAILED) {
        perror("memory mapping error");
        exit(EXIT_FAILURE);
    }
}

void place_func() {
    memcpy(shift, shift_func, sizeof(shift_func));
}

void make_func_executable() {
    if (mprotect(shift, sizeof(shift_func), PROT_READ | PROT_EXEC) == -1) {
        perror("memory privileges changing error");
        exit(EXIT_FAILURE);
    }
}

void free_func_mem() {
    if (munmap(shift, 4096) == -1) {
        perror("memory unmapping error");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        perror("bad args");
        exit(EXIT_FAILURE);
    }

    change_shift((unsigned char) strtol(argv[1], argv + 1, 10));

    map_func_mem();
    place_func();
    make_func_executable();

    printf("%s\n", argv[2]);
    for (int i = 0; argv[2][i]; ++i) {
        printf("%c", shift(argv[2][i]));
    }
    printf("\n");

    free_func_mem();
    return EXIT_SUCCESS;
}
