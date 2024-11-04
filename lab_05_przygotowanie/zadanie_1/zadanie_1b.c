#include <stdio.h>

int main() {
    printf("Hello, world.\n");
    return 0;
}


// 1) gcc -o zadanie_1b zadanie_1b.c


// 2) strace ./zadanie_1b

// execve("./zadanie_1b", ["./zadanie_1b"], 0x7fff2b558d00 /* 81 vars */) = 0
// brk(NULL)                               = 0x652057a88000
// arch_prctl(0x3001 /* ARCH_??? */, 0x7fffa8ad14e0) = -1 EINVAL (Zły argument)
// mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x79322ca54000
// access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (Nie ma takiego pliku ani katalogu)
// openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
// newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=77187, ...}, AT_EMPTY_PATH) = 0
// mmap(NULL, 77187, PROT_READ, MAP_PRIVATE, 3, 0) = 0x79322ca41000
// close(3)                                = 0
// openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
// read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\237\2\0\0\0\0\0"..., 832) = 832
// pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
// pread64(3, "\4\0\0\0 \0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 48, 848) = 48
// pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0I\17\357\204\3$\f\221\2039x\324\224\323\236S"..., 68, 896) = 68
// newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2220400, ...}, AT_EMPTY_PATH) = 0
// pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
// mmap(NULL, 2264656, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x79322c800000
// mprotect(0x79322c828000, 2023424, PROT_NONE) = 0
// mmap(0x79322c828000, 1658880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x79322c828000
// mmap(0x79322c9bd000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1bd000) = 0x79322c9bd000
// mmap(0x79322ca16000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x215000) = 0x79322ca16000
// mmap(0x79322ca1c000, 52816, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x79322ca1c000
// close(3)                                = 0
// mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x79322ca3e000
// arch_prctl(ARCH_SET_FS, 0x79322ca3e740) = 0
// set_tid_address(0x79322ca3ea10)         = 8883
// set_robust_list(0x79322ca3ea20, 24)     = 0
// rseq(0x79322ca3f0e0, 0x20, 0, 0x53053053) = 0
// mprotect(0x79322ca16000, 16384, PROT_READ) = 0
// mprotect(0x6520567f8000, 4096, PROT_READ) = 0
// mprotect(0x79322ca8e000, 8192, PROT_READ) = 0
// prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
// munmap(0x79322ca41000, 77187)           = 0
// newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x2), ...}, AT_EMPTY_PATH) = 0
// getrandom("\x92\x72\xb2\xd7\x97\x00\xf9\xca", 8, GRND_NONBLOCK) = 8
// brk(NULL)                               = 0x652057a88000
// brk(0x652057aa9000)                     = 0x652057aa9000
// write(1, "Hello, world.\n", 14Hello, world.              <=== tu wyświetla hello world
// )         = 14
// exit_group(0)                           = ?
// +++ exited with 0 +++
