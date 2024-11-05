#include <stdio.h>
#include <stdlib.h>

int main() {
    char *username = getenv("LOGNAME");

    printf("Hello, %s!\n", username);

    return 0;
}

// 0) gcc -o zadanie_1d zadanie_1d.c

// 1) strace ./zadanie_1d
// michal@legion-michal:~/Documents/sem_5/SCR - Systemy Operacyjne/lab_05_przygotowanie$ strace ./zadanie_1d
// execve("./zadanie_1d", ["./zadanie_1d"], 0x7fffc5150720 /* 81 vars */) = 0
// brk(NULL)                               = 0x576ee4ad4000
// arch_prctl(0x3001 /* ARCH_??? */, 0x7ffd1a30b1e0) = -1 EINVAL (Zły argument)
// mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f6a1d404000
// access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (Nie ma takiego pliku ani katalogu)
// openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
// newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=77187, ...}, AT_EMPTY_PATH) = 0
// mmap(NULL, 77187, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f6a1d3f1000
// close(3)                                = 0
// openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
// read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\237\2\0\0\0\0\0"..., 832) = 832
// pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
// pread64(3, "\4\0\0\0 \0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 48, 848) = 48
// pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0I\17\357\204\3$\f\221\2039x\324\224\323\236S"..., 68, 896) = 68
// newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2220400, ...}, AT_EMPTY_PATH) = 0
// pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
// mmap(NULL, 2264656, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f6a1d000000
// mprotect(0x7f6a1d028000, 2023424, PROT_NONE) = 0
// mmap(0x7f6a1d028000, 1658880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7f6a1d028000
// mmap(0x7f6a1d1bd000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1bd000) = 0x7f6a1d1bd000
// mmap(0x7f6a1d216000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x215000) = 0x7f6a1d216000
// mmap(0x7f6a1d21c000, 52816, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f6a1d21c000
// close(3)                                = 0
// mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f6a1d3ee000
// arch_prctl(ARCH_SET_FS, 0x7f6a1d3ee740) = 0
// set_tid_address(0x7f6a1d3eea10)         = 9218
// set_robust_list(0x7f6a1d3eea20, 24)     = 0
// rseq(0x7f6a1d3ef0e0, 0x20, 0, 0x53053053) = 0
// mprotect(0x7f6a1d216000, 16384, PROT_READ) = 0
// mprotect(0x576ee381c000, 4096, PROT_READ) = 0
// mprotect(0x7f6a1d43e000, 8192, PROT_READ) = 0
// prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
// munmap(0x7f6a1d3f1000, 77187)           = 0
// newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x2), ...}, AT_EMPTY_PATH) = 0
// getrandom("\xd3\xa5\xa6\x5e\xed\x5c\xe9\x82", 8, GRND_NONBLOCK) = 8
// brk(NULL)                               = 0x576ee4ad4000
// brk(0x576ee4af5000)                     = 0x576ee4af5000
// write(1, "Hello, michal!\n", 15Hello, michal!
// )        = 15
// exit_group(0)                           = ?
// +++ exited with 0 +++

// 2) ltrace ./zadanie_1d
// getenv("LOGNAME")                                                                                          = "michal"
// printf("Hello, %s!\n", "michal"Hello, michal!
// )                                                                           = 15
// +++ exited (status 0) +++

//3) wnioski:
//strace śledzi wywołania systemowe, co oznacza, że pokazuje wszystkie operacje komunikacji z jądrem systemu operacyjnego
//(np. otwieranie plików, alokacje pamięci, zmiany uprawnień dostępu itp.). Przykłady wywołań widocznych w strace to execve,
//mmap, access, openat, write, exit_group i inne, czyli funkcje, które bezpośrednio angażują jądro systemu operacyjnego.
//ltrace śledzi natomiast wywołania funkcji bibliotecznych (standardowych funkcji z bibliotek C, takich jak getenv czy printf).
//ltrace pozwala zobaczyć, jak program korzysta z bibliotek, ale nie wnika w komunikację z jądrem, co skutkuje bardziej
//"wysokopoziomowym" wglądem w działanie programu.
//
//W podanym przykładzie:
//strace pokazuje dużo szczegółów dotyczących zarządzania pamięcią i zasobami, np. użycia mmap, mprotect, czy brk,
//a także wywołanie write, które bezpośrednio angażuje jądro w celu wyświetlenia tekstu na ekranie.
//ltrace ogranicza się do pokazania wywołania getenv("LOGNAME"), które uzyskuje zmienną środowiskową LOGNAME,
//oraz printf, który generuje komunikat powitalny „Hello, michal!”.