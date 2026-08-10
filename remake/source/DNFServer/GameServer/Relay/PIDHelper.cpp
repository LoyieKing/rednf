// df_relay_r — save_pid/delete_pid（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "PIDHelper.h"
#include "LinuxService.h"

// ---- pid 文件 ----

bool delete_pid()
{
    char path[30] = {0};
    snprintf(path, 0x1e, "./pid/%s.pid",
             LinuxService::getInstance()->getPIDFileName());
    printf("delete_pid() : file[%s]\n", path);
    int r = unlink(path);
    if (r == 0)
    {
        printf("delete %s\n", path);
    }
    return r == 0;
}

int save_pid()
{
    char path[512];
    char tmp[30] = {0};
    snprintf(tmp, 0x1e, "./pid/%s.pid",
             LinuxService::getInstance()->getPIDFileName());
    int fd = open(tmp, 0x42, 0x1a4);
    if (fd < 0)
    {
        return 0;
    }
    memset(path, 0, 0x200);
    snprintf(path, 0x200, "%ld\n", (long)getpid());
    ssize_t n = write(fd, path, strlen(path));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
}
