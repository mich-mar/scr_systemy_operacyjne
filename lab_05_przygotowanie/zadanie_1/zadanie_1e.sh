#!/bin/bash
echo "Hello from the shell!"

 # 1) chmod +x zadanie_1e.sh

 # 2) strace -e trace=open sh zadanie_1e.sh
 # Hello from the shell!
 # +++ exited with 0 +++

 # 3) strace -e trace=all sh zadanie_1e.sh
 # execve("/usr/bin/sh", ["sh", "zadanie_1e.sh"], 0x7fffc4740998 /* 81 vars */) = 0
 # brk(NULL)                               = 0x6326fd79b000
 # arch_prctl(0x3001 /* ARCH_??? */, 0x7fffe58c6090) = -1 EINVAL (Zły argument)
 # mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff4ac76e000
 # access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (Nie ma takiego pliku ani katalogu)
 # openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
 # newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=77187, ...}, AT_EMPTY_PATH) = 0
 # mmap(NULL, 77187, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7ff4ac75b000
 # close(3)                                = 0
 # openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
 # read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\237\2\0\0\0\0\0"..., 832) = 832
 # pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
 # pread64(3, "\4\0\0\0 \0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 48, 848) = 48
 # pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0I\17\357\204\3$\f\221\2039x\324\224\323\236S"..., 68, 896) = 68
 # newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2220400, ...}, AT_EMPTY_PATH) = 0
 # pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
 # mmap(NULL, 2264656, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7ff4ac400000
 # mprotect(0x7ff4ac428000, 2023424, PROT_NONE) = 0
 # mmap(0x7ff4ac428000, 1658880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7ff4ac428000
 # mmap(0x7ff4ac5bd000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1bd000) = 0x7ff4ac5bd000
 # mmap(0x7ff4ac616000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x215000) = 0x7ff4ac616000
 # mmap(0x7ff4ac61c000, 52816, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7ff4ac61c000
 # close(3)                                = 0
 # mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7ff4ac758000
 # arch_prctl(ARCH_SET_FS, 0x7ff4ac758740) = 0
 # set_tid_address(0x7ff4ac758a10)         = 9834
 # set_robust_list(0x7ff4ac758a20, 24)     = 0
 # rseq(0x7ff4ac7590e0, 0x20, 0, 0x53053053) = 0
 # mprotect(0x7ff4ac616000, 16384, PROT_READ) = 0
 # mprotect(0x6326fc013000, 8192, PROT_READ) = 0
 # mprotect(0x7ff4ac7a8000, 8192, PROT_READ) = 0
 # prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
 # munmap(0x7ff4ac75b000, 77187)           = 0
 # getuid()                                = 1000
 # getgid()                                = 1000
 # getpid()                                = 9834
 # rt_sigaction(SIGCHLD, {sa_handler=0x6326fc009aa0, sa_mask=~[RTMIN RT_1], sa_flags=SA_RESTORER, sa_restorer=0x7ff4ac442520}, NULL, 8) = 0
 # geteuid()                               = 1000
 # getrandom("\x0c\x3c\xf2\xd2\xef\x61\x8c\x11", 8, GRND_NONBLOCK) = 8
 # brk(NULL)                               = 0x6326fd79b000
 # brk(0x6326fd7bc000)                     = 0x6326fd7bc000
 # getppid()                               = 9831
 # newfstatat(AT_FDCWD, "/home/michal/Documents/sem_5/SCR - Systemy Operacyjne/lab_05_przygotowanie", {st_mode=S_IFDIR|0775, st_size=4096, ...}, 0) = 0
 # newfstatat(AT_FDCWD, ".", {st_mode=S_IFDIR|0775, st_size=4096, ...}, 0) = 0
 # openat(AT_FDCWD, "zadanie_1e.sh", O_RDONLY) = 3
 # fcntl(3, F_DUPFD, 10)                   = 10
 # close(3)                                = 0
 # fcntl(10, F_SETFD, FD_CLOEXEC)          = 0
 # geteuid()                               = 1000
 # getegid()                               = 1000
 # rt_sigaction(SIGINT, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
 # rt_sigaction(SIGINT, {sa_handler=0x6326fc009aa0, sa_mask=~[RTMIN RT_1], sa_flags=SA_RESTORER, sa_restorer=0x7ff4ac442520}, NULL, 8) = 0
 # rt_sigaction(SIGQUIT, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
 # rt_sigaction(SIGQUIT, {sa_handler=SIG_DFL, sa_mask=~[RTMIN RT_1], sa_flags=SA_RESTORER, sa_restorer=0x7ff4ac442520}, NULL, 8) = 0
 # rt_sigaction(SIGTERM, NULL, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
 # rt_sigaction(SIGTERM, {sa_handler=SIG_DFL, sa_mask=~[RTMIN RT_1], sa_flags=SA_RESTORER, sa_restorer=0x7ff4ac442520}, NULL, 8) = 0
 # read(10, " # !/bin/bash\necho \"Hello from the"..., 8192) = 202
 # write(1, "Hello from the shell!\n", 22Hello from the shell!
 # ) = 22
 # read(10, "", 8192)                      = 0
 # exit_group(0)                           = ?
 # +++ exited with 0 +++
