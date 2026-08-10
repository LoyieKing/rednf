// Minimal declarations restored from zen_os_adapt_dirent.cpp (gunnersvr).
// Original path: src/commlib/zenlib/zen_os_adapt_dirent.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_DIRENT_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_DIRENT_H_H_

#include <stddef.h>

namespace ZEN_OS {

int rmdir(const char *pathname);
int mkdir(const char *pathname, unsigned int mode);
int mkdir_recurse(const char *pathname, unsigned int mode);
int chdir(const char *dirname);
char *getcwd(char *buffer, int maxlen);
const char *dirname(const char *path_name, char *dir_name, size_t buf_len);

} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_DIRENT_H_H_
