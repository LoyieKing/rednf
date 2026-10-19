// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_os_adapt_dirent.cpp
// Address range 0x001a2030-0x001a23e2.
//
// Behaviour identical to the ORIG:
//   dirname("/a/b") -> "/a/"   (trailing slash kept, no allocation)
//   dirname("abc")  -> "./"
//   basename("/a/b")-> "b"     basename("abc") -> "abc"
//   mkdir_recurse   -> creates every prefix, ignores EEXIST, returns the first
//                      other errno value (0 when nothing failed).
//
// The libc imports are declared with the ORIG 64-bit dirent ABI and bound to
// the glibc symbol names via asm labels; this avoids depending on the host
// <dirent.h> prototype for scandir/alphasort.
#include "src/commlib/zenlib/zen_os_adapt_dirent.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

extern "C" {

int zen_libc_rmdir(const char *pathname) __asm__("rmdir");
int zen_libc_mkdir(const char *pathname, unsigned int mode) __asm__("mkdir");
int zen_libc_chdir(const char *pathname) __asm__("chdir");
char *zen_libc_getcwd(char *buffer, int maxlen) __asm__("getcwd");

int zen_libc_alphasort64(const dirent **left, const dirent **right)
    __asm__("alphasort64");
int zen_libc_scandir64(const char *dirname, dirent ***namelist,
                       int (*selector)(const dirent *entry),
                       int (*comparator)(const dirent **left,
                                         const dirent **right))
    __asm__("scandir64");
int zen_libc_readdir64_r(DIR *dir_handle, dirent *entry, dirent **result)
    __asm__("readdir64_r");
dirent *zen_libc_readdir64(DIR *dir_handle) __asm__("readdir64");
int zen_libc_closedir(DIR *dir_handle) __asm__("closedir");
DIR *zen_libc_opendir(const char *dir_name) __asm__("opendir");

} // extern "C"

namespace ZEN_OS {

int rmdir(const char *pathname) {
    return zen_libc_rmdir(pathname);
}

int mkdir(const char *pathname, unsigned int mode) {
    return zen_libc_mkdir(pathname, mode);
}

int mkdir_recurse(const char *pathname, unsigned int mode) {
    char process_dir[4097];
    memset(process_dir, 0, sizeof(process_dir));
    size_t path_len = strlen(pathname);
    size_t i = 0;
    int ret;
    if (path_len == 0) {
        return 0;
    }
    for (;;) {
        if (pathname[i] != '/') {
            ++i;
            if (i >= path_len) {
                break;
            }
            continue;
        }
        ++i;
        strncpy(process_dir, pathname, i);
        ret = ZEN_OS::mkdir(process_dir, mode);
        if (ret != 0 && errno != EEXIST) {
            return ret;
        }
        ++i;
        if (i >= path_len) {
            break;
        }
    }
    return 0;
}

int chdir(const char *pathname) {
    return zen_libc_chdir(pathname);
}

char *getcwd(char *buffer, int maxlen) {
    return zen_libc_getcwd(buffer, maxlen);
}

const char *dirname(const char *path_name, char *dir_name, size_t buf_len) {
    const char *temp = strrchr(path_name, '/');
    if (temp != NULL) {
        dir_name[(temp - path_name) + 1] = '\0';
        strncpy(dir_name, path_name, (temp - path_name) + 1);
    } else {
        strncpy(dir_name, "./", buf_len);
    }
    return dir_name;
}

const char *basename(const char *path_name, char *file_name, size_t buf_len) {
    const char *temp = strrchr(path_name, '/');
    if (temp != NULL) {
        strncpy(file_name, temp + 1, buf_len);
    } else {
        strncpy(file_name, path_name, buf_len);
    }
    return file_name;
}

int alphasort(const dirent **left, const dirent **right) {
    return zen_libc_alphasort64(left, right);
}

int scandir(const char *dirname_arg, dirent ***namelist,
            int (*selector)(const dirent *entry),
            int (*comparator)(const dirent **left, const dirent **right)) {
    return zen_libc_scandir64(dirname_arg, namelist, selector, comparator);
}

int readdir_r(DIR *dir_handle, dirent *entry, dirent **result) {
    return zen_libc_readdir64_r(dir_handle, entry, result);
}

dirent *readdir(DIR *dir_handle) {
    return zen_libc_readdir64(dir_handle);
}

int closedir(DIR *dir_handle) {
    return zen_libc_closedir(dir_handle);
}

DIR *opendir(const char *dir_name) {
    return zen_libc_opendir(dir_name);
}

} // namespace ZEN_OS
