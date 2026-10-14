/*
 * libtss_shmfix.so —— TSS SDK 共享内存名修正（LD_PRELOAD 给 df_game_r 用）
 *
 * TSS 反外挂 SDK（libtss_sdk.so）与 secsvr（gunnersvr/zergsvr/secagent）
 * 通过 POSIX 共享内存 sec_tss_sdk_bus_* 通讯。SDK 传给 shm_open 的名字里可能
 * 带路径分隔符，新版 glibc 的 shm_open 对这种名字直接返回 EINVAL，导致
 * AntibotInterface::Initialize 失败、选角界面无字/空槽。
 *
 * 本库拦截 shm_open/shm_unlink：保证名字以 '/' 开头，其余 '/' 换成 '_'，
 * 最长 255 字节，然后调用真正的 libc 实现。
 *
 * 构建：gcc -m32 -O2 -shared -fPIC -o libtss_shmfix.so libtss_shmfix.c -ldl
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/mman.h>

#define NAME_MAX_LEN 255

static void rewrite_shm_name(char *out, const char *name)
{
    size_t i = 0;
    out[i++] = '/';
    if (name) {
        const char *p = (*name == '/') ? name + 1 : name;
        for (; *p && i < NAME_MAX_LEN; ++p)
            out[i++] = (*p == '/') ? '_' : *p;
    }
    out[i] = '\0';
}

int shm_open(const char *name, int oflag, mode_t mode)
{
    static int (*real_shm_open)(const char *, int, mode_t);
    char buf[NAME_MAX_LEN + 1];
    if (!real_shm_open)
        real_shm_open = (int (*)(const char *, int, mode_t))dlsym(RTLD_NEXT, "shm_open");
    rewrite_shm_name(buf, name);
    return real_shm_open(buf, oflag, mode);
}

int shm_unlink(const char *name)
{
    static int (*real_shm_unlink)(const char *);
    char buf[NAME_MAX_LEN + 1];
    if (!real_shm_unlink)
        real_shm_unlink = (int (*)(const char *))dlsym(RTLD_NEXT, "shm_unlink");
    rewrite_shm_name(buf, name);
    return real_shm_unlink(buf);
}
