/* ============================================================================
 * __xstat/__fxstat/__lxstat 族兼容实现（decompile_verify=OFF 专用）
 *
 * 上游：https://github.com/llnut/dnf-compat-layer 的 glibc-compat stat 钩子。
 * 背景：本工程链接的 4.x 时代静态库（MySQL Connector/C、yaSSL 等）按老 glibc
 * 头编译，引用 `__xstat@GLIBC_2.0` / `__fxstat64@GLIBC_2.2` 等旧入口；新发行版
 * 的 libc 可能不再导出这些符号 ⇒ 运行期 `symbol lookup error`。上游用 LD_PRELOAD
 * 在运行期补；本实现把同族符号**静态定义进可执行文件**，不依赖任何 preload。
 *
 * 实现要点（与上游逐条对齐）：
 *   - 参数里的 `ver`（旧 _STAT_VER）忽略：现代实现无版本分支；
 *   - 路径入口套用 /dev/shm 扁平化（dnf_shm_compat.h ②），fd 入口不处理路径；
 *   - 直接转发现代 glibc 的同布局入口（i386 的 stat64 布局自 glibc 2.0 起冻结），
 *     不做 int 0x80 裸调用：本工程同时存在 32 位（13 服务）与 64 位（stun）目标。
 *
 * 该 TU 只被 OFF 路线收进目标（见 dnf_helpers.cmake），ON 路线产物不受影响。
 * ==========================================================================*/
#define _LARGEFILE64_SOURCE 1

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dnf_shm_compat.h"

namespace {

inline const char* dnf_path(const char* path, char* buf)
{
    return dnf_compat::flatten_dev_shm_path(path, buf, DNF_SHM_PATH_MAX);
}

}  /* namespace */

extern "C" int __xstat(int ver, const char* path, struct stat* buf)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::stat(dnf_path(path, p), buf);
}

extern "C" int __xstat64(int ver, const char* path, struct stat64* buf)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::stat64(dnf_path(path, p), buf);
}

extern "C" int __fxstat(int ver, int fd, struct stat* buf)
{
    (void)ver;
    return ::fstat(fd, buf);
}

extern "C" int __fxstat64(int ver, int fd, struct stat64* buf)
{
    (void)ver;
    return ::fstat64(fd, buf);
}

extern "C" int __lxstat(int ver, const char* path, struct stat* buf)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::lstat(dnf_path(path, p), buf);
}

extern "C" int __lxstat64(int ver, const char* path, struct stat64* buf)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::lstat64(dnf_path(path, p), buf);
}

extern "C" int __fxstatat(int ver, int dirfd, const char* path, struct stat* buf,
                          int flags)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::fstatat(dirfd, dnf_path(path, p), buf, flags);
}

extern "C" int __fxstatat64(int ver, int dirfd, const char* path,
                            struct stat64* buf, int flags)
{
    (void)ver;
    char p[DNF_SHM_PATH_MAX];
    return ::fstatat64(dirfd, dnf_path(path, p), buf, flags);
}
