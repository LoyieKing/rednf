/* ============================================================================
 * /dev/shm 命名空间兼容（dnf-compat-layer 的编译期内联版）
 *
 * 上游：https://github.com/llnut/dnf-compat-layer（运行时用 LD_PRELOAD 修二进制）
 * 语义（逐条对齐上游 glibc-compat/src/lib.rs）：
 *   ① shm 名：除首字符外，所有 '/' → '_'
 *      （老代码把 POSIX shm 名当路径用，如 "/dev/shm/xxx/yyy"；现代内核查名
 *        里的 '/' ⇒ 直接 EINVAL）
 *   ② /dev/shm/<a>/<b> 路径 → /dev/shm/<a>_<b>
 *      （现代 /dev/shm 是 tmpfs，多级子目录不再存在）
 *   ③ mkdir/mkdirat 作用在 /dev/shm 下时按"命名空间已扁平化"假成功。
 *
 * 本头只提供 inline 助手；宏与符号定义见 decompile_verify_off.h 与
 * dnf_compat_stat.cpp（同名符号会遮蔽 libc，勿在 ON 路线包含本头）。
 * ==========================================================================*/
#ifndef DNF_SHM_COMPAT_H
#define DNF_SHM_COMPAT_H

#include <string.h>

#define DNF_SHM_NAME_MAX 255
#define DNF_SHM_PATH_MAX 512
#define DNF_DEV_SHM_PREFIX "/dev/shm/"

namespace dnf_compat {

/* ① shm 名扁平化：返回原名（无嵌套 '/' 或超长）或写入 buf 后的名字。 */
inline const char* flatten_shm_name(const char* name, char* buf, unsigned long cap)
{
    if (name == 0)
        return name;
    unsigned long len = (unsigned long)strlen(name);
    bool nested = false;
    for (unsigned long i = 1; i < len; i++)
    {
        if (name[i] == '/')
        {
            nested = true;
            break;
        }
    }
    if (!nested || len > DNF_SHM_NAME_MAX || len + 1 > cap)
        return name;
    for (unsigned long i = 0; i < len; i++)
        buf[i] = (name[i] == '/' && i > 0) ? '_' : name[i];
    buf[len] = '\0';
    return buf;
}

/* ② /dev/shm 多级路径扁平化：返回原路径（非 /dev/shm 或无嵌套 '/'）或 buf。 */
inline const char* flatten_dev_shm_path(const char* path, char* buf, unsigned long cap)
{
    const unsigned long plen = sizeof(DNF_DEV_SHM_PREFIX) - 1;
    if (path == 0 || strncmp(path, DNF_DEV_SHM_PREFIX, plen) != 0)
        return path;
    const char* suffix = path + plen;
    bool nested = false;
    for (const char* p = suffix; *p; p++)
    {
        if (*p == '/')
        {
            nested = true;
            break;
        }
    }
    if (!nested)
        return path;
    unsigned long slen = (unsigned long)strlen(suffix);
    if (plen + slen + 1 > cap)
        return path;
    memcpy(buf, DNF_DEV_SHM_PREFIX, plen);
    for (unsigned long i = 0; i < slen; i++)
        buf[plen + i] = (suffix[i] == '/') ? '_' : suffix[i];
    buf[plen + slen] = '\0';
    return buf;
}

/* ③ 任意非空 /dev/shm/... 路径（mkdir 假成功用；比 ② 更宽）。 */
inline bool is_dev_shm_path(const char* path)
{
    const unsigned long plen = sizeof(DNF_DEV_SHM_PREFIX) - 1;
    return path != 0 && strncmp(path, DNF_DEV_SHM_PREFIX, plen) == 0
           && path[plen] != '\0';
}

}  /* namespace dnf_compat */

#endif  /* DNF_SHM_COMPAT_H */
