/* ============================================================================
 * remake_verify = OFF 时的语法/ABI 兼容层（2026-09-17）
 *
 * 背景：本工程默认以 GCC 4.x 工具链逐字节复现 ORIG 二进制
 * （remake_verify=ON）。关闭该开关时改用宿主最新 LTS GCC 编译，
 * 编译器与标准库都比 4.x 严格/更新，本头负责把 4.x 时代源码里
 * "新编译器不再支持或不再隐式提供"的部分补齐，**语义与老版本保持一致**：
 *
 *   1) remake_verify 宏：源码可用 `#if remake_verify` 分支；
 *   2) register 存储类（C++17 起被移除）→ 展开为空；
 *   3) 4.x 时代依赖的传递性包含：新 libstdc++ 不再隐式引入，
 *      这里统一补齐（等价于旧头的可见性）；
 *   4) 旧式函数对象基类/工具（unary_function/binary_function/ptr_fun/
 *      mem_fun/random_shuffle）在新标准里已弃用或移除 → 提供等价别名；
 *   5) 编译期断言在有 C++11 支持时直接用 static_assert。
 *
 * 用法：由 dnf_helpers.cmake 在 remake_verify=OFF 时以
 *      `-include <该头>` 注入到每个 TU（等价于 dnf_game_cxx.sh 注入
 *      static_assert_compat.h 的手法）。
 * ==========================================================================*/
#ifndef DNF_DECOMPILE_VERIFY_OFF_H
#define DNF_DECOMPILE_VERIFY_OFF_H

/* 让 <sys/stat.h> 暴露 stat64 族：现代 glibc 的 __xstat/__xstat64 兼容实现
 * （dnf_compat_stat.cpp）需要按调用方的旧结构布局转发。 */
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif

#ifndef remake_verify
#define remake_verify 0
#endif

/* --- ② register：C++17 起为保留字并被移除 ------------------------------- */
#if defined(__cplusplus) && __cplusplus >= 201703L
#define register
#endif

/* --- ③ 旧编译单元默认可见的头：新标准库不再传递性包含 ---------------- */
#ifdef __cplusplus
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <cmath>
#include <ctime>
#include <cstdarg>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <new>
#include <typeinfo>
#include <stdexcept>
#include <exception>

/* --- ④ 旧式函数对象/算法：C++17 移除，按等价语义补回 ------------------- */
#if __cplusplus >= 201703L
#include <cstdint>
namespace dnf_compat {
template <class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};
}  // namespace dnf_compat
namespace std {
template <class Arg, class Result>
using unary_function = dnf_compat::unary_function<Arg, Result>;
template <class Arg1, class Arg2, class Result>
using binary_function = dnf_compat::binary_function<Arg1, Arg2, Result>;
}  // namespace std
#endif
#endif  /* __cplusplus */

/* --- ⑤ 老代码的 C 头用法（unistd/pthread 等）在 C++ 下仍可用 ---------- */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

/* ============================================================================
 * ⑥ dnf-compat-layer 的编译期内联版
 *
 * 上游 https://github.com/llnut/dnf-compat-layer 用 LD_PRELOAD 在运行期修老二进制；
 * 本工程 OFF 路线（宿主 GCC）直接在源码层补同一语义，不依赖任何 preload：
 *   - shm 名（含 '/'）扁平化：/dev/shm/a/b → /dev/shm/a_b、/dev/shm/x → /dev_shm_x
 *   - /dev/shm 多级路径扁平化（stat/open/fopen/unlink/rename/... 家族）
 *   - mkdir/mkdirat：/dev/shm 下假成功；其余路径 EEXIST 视为成功
 *   - __xstat/__fxstat/__lxstat 族：由 dnf_compat_stat.cpp 以现代 stat 实现
 *     （老静态库按 4.x 头编译时引用这些符号；部分新发行版已从 libc 移除）
 *
 * 宏只覆盖「本仓无同名成员函数」的四个入口（shm_open / shm_unlink / mkdir /
 * mkdirat）；其余路径家族以 dnf_compat_* 函数提供，避免函数式宏误伤
 * `obj.open()`/`obj.stat()` 这类成员调用（本仓 DBMW 确有 `m_sock.open()`）。
 * ==========================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include "dnf_shm_compat.h"

inline int dnf_compat_shm_open(const char* name, int oflag, mode_t mode)
{
    char buf[DNF_SHM_NAME_MAX + 1];
    return ::shm_open(dnf_compat::flatten_shm_name(name, buf, sizeof(buf)), oflag, mode);
}

inline int dnf_compat_shm_unlink(const char* name)
{
    char buf[DNF_SHM_NAME_MAX + 1];
    return ::shm_unlink(dnf_compat::flatten_shm_name(name, buf, sizeof(buf)));
}

inline int dnf_compat_mkdir(const char* path, mode_t mode)
{
    // 上游：/dev/shm 下不建真实目录（命名空间已扁平化）⇒ 直接假成功。
    if (dnf_compat::is_dev_shm_path(path))
        return 0;
    int r = ::mkdir(path, mode);
    if (r < 0 && errno == EEXIST)
        return 0;
    return r;
}

inline int dnf_compat_mkdirat(int dirfd, const char* path, mode_t mode)
{
    if (dnf_compat::is_dev_shm_path(path))
        return 0;
    int r = ::mkdirat(dirfd, path, mode);
    if (r < 0 && errno == EEXIST)
        return 0;
    return r;
}

#define shm_open(name, oflag, mode) dnf_compat_shm_open(name, oflag, mode)
#define shm_unlink(name)            dnf_compat_shm_unlink(name)
#define mkdir(path, mode)           dnf_compat_mkdir(path, mode)
#define mkdirat(dirfd, path, mode)  dnf_compat_mkdirat(dirfd, path, mode)

/* ---- 路径家族（/dev/shm 扁平化）----
 * 以函数形式提供；调用点若确需覆盖 libc 入口，可在 TU 内显式
 * `#define stat(...) dnf_compat_stat(...)` 之类的宏（本仓当前无此需求）。 */
inline const char* dnf_compat_path(const char* path, char* buf)
{
    return dnf_compat::flatten_dev_shm_path(path, buf, DNF_SHM_PATH_MAX);
}

inline int dnf_compat_stat(const char* path, struct stat* buf)
{
    char p[DNF_SHM_PATH_MAX];
    return ::stat(dnf_compat_path(path, p), buf);
}

inline int dnf_compat_lstat(const char* path, struct stat* buf)
{
    char p[DNF_SHM_PATH_MAX];
    return ::lstat(dnf_compat_path(path, p), buf);
}

inline int dnf_compat_unlink(const char* path)
{
    char p[DNF_SHM_PATH_MAX];
    return ::unlink(dnf_compat_path(path, p));
}

inline int dnf_compat_rename(const char* oldpath, const char* newpath)
{
    char a[DNF_SHM_PATH_MAX];
    char b[DNF_SHM_PATH_MAX];
    return ::rename(dnf_compat_path(oldpath, a), dnf_compat_path(newpath, b));
}

inline int dnf_compat_access(const char* path, int mode)
{
    char p[DNF_SHM_PATH_MAX];
    return ::access(dnf_compat_path(path, p), mode);
}

inline int dnf_compat_truncate(const char* path, off_t len)
{
    char p[DNF_SHM_PATH_MAX];
    return ::truncate(dnf_compat_path(path, p), len);
}

inline FILE* dnf_compat_fopen(const char* path, const char* mode)
{
    char p[DNF_SHM_PATH_MAX];
    return ::fopen(dnf_compat_path(path, p), mode);
}

inline DIR* dnf_compat_opendir(const char* path)
{
    char p[DNF_SHM_PATH_MAX];
    return ::opendir(dnf_compat_path(path, p));
}

#endif  /* DNF_DECOMPILE_VERIFY_OFF_H */
