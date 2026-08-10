// Reconstructed from secagent binary DWARF + disassembly (zen_os_adapt_file.cpp).
// Original path: src/commlib/zenlib/zen_os_adapt_file.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_FILE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_FILE_H_H_

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

#include <stddef.h>
#include <sys/types.h>

namespace ZEN_OS {

int access(const char *pathname, int mode);
mode_t umask(mode_t cmask);
int unlink(const char *filename);
int filesize(ZEN_HANDLE file_handle, size_t *file_size);
int filesize(const char *filename, size_t *file_size);
int stat(const char *path, zen_os_stat *file_stat);
int fstat(ZEN_HANDLE file_handle, zen_os_stat *file_stat);
ZEN_HANDLE mkstemp(char *template_name);
int close(ZEN_HANDLE handle);
ZEN_HANDLE open(const char *filename, int open_mode, mode_t perms);
ssize_t lseek(ZEN_HANDLE file_handle, ssize_t offset, int whence);
int ftruncate(ZEN_HANDLE file_handle, size_t offset);
int truncate(const char *filename, size_t offset);
ssize_t write(ZEN_HANDLE file_handle, const void *buf, size_t count);
ssize_t read(ZEN_HANDLE file_handle, void *buf, size_t count);
int read_file_data(const char *filename, char *buffer, size_t buf_len,
                   size_t *read_len);

} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_FILE_H_H_
