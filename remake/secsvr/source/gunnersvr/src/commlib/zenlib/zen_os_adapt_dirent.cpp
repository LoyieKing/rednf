// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_dirent.cpp
// Thin wrappers over the glibc dirent64 family (the binary links against
// readdir64 / scandir64 / alphasort64 / readdir64_r; _FILE_OFFSET_BITS=64
// makes `dirent` the 64-bit layout while keeping the original type name).

#define _FILE_OFFSET_BITS 64

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

namespace ZEN_OS {

int rmdir(const char *pathname) {
    return ::rmdir(pathname);
}

int mkdir(const char *pathname, mode_t mode) {
    return ::mkdir(pathname, mode);
}

int mkdir_recurse(const char *pathname, mode_t mode) {
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

int chdir(const char *dirname) {
    return ::chdir(dirname);
}

char *getcwd(char *buffer, int maxlen) {
    return ::getcwd(buffer, maxlen);
}

const char *dirname(const char *path_name, char *dir_name, size_t buf_len) {
    const char *temp = strrchr(path_name, '/');
    if (temp != NULL) {
        // Result keeps the trailing '/' (e.g. "/a/b" -> "/a/").
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
    return ::alphasort(left, right);
}

int scandir(const char *dirname, dirent ***namelist,
            int (*selector)(const dirent *arg0),
            int (*comparator)(const dirent **arg0, const dirent **arg1)) {
    return ::scandir(dirname, namelist, selector,
                     (int (*)(const void *, const void *))comparator);
}

int readdir_r(DIR *dir_handle, dirent *entry, dirent **result) {
    return ::readdir_r(dir_handle, entry, result);
}

dirent *readdir(DIR *dir_handle) {
    return ::readdir(dir_handle);
}

int closedir(DIR *dir_handle) {
    return ::closedir(dir_handle);
}

DIR *opendir(const char *dir_name) {
    return ::opendir(dir_name);
}

} // namespace ZEN_OS
