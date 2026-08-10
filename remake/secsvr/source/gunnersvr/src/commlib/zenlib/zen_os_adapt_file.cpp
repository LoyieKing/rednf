// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_file.cpp
// Thin syscall wrappers over the 64-bit-offset file API (stat64/open64/
// mkstemp64/lseek64/ftruncate64/truncate64). Original quirks preserved:
// read_file_data does not close the fd on a read error, and 0660 is used as
// the default permission for O_RDONLY opens.

#define _GNU_SOURCE 1
#define _FILE_OFFSET_BITS 64

#include "src/commlib/zenlib/zen_os_adapt_predefine.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Real symbol is debug_errorex(char const*, ...) with a trailing variadic
// marker in the mangled name; bind directly so the emitted symbol matches.
extern void zen_debug_errorex(const char *str_format, ...)
    __asm__("_ZN16ZEN_Trace_LogMsg13debug_errorexEPKcz");

namespace ZEN_OS {

int access(const char *pathname, int mode) {
    return ::access(pathname, mode);
}

mode_t umask(mode_t cmask) {
    return ::umask(cmask);
}

int unlink(const char *filename) {
    return ::unlink(filename);
}

int filesize(ZEN_HANDLE file_handle, size_t *file_size) {
    struct stat sb;
    int ret = ::fstat(file_handle, &sb);
    if (ret == 0) {
        *file_size = sb.st_size;
    }
    return ret;
}

int stat(const char *path, zen_os_stat *file_stat) {
    return ::stat(path, file_stat);
}

ZEN_HANDLE mkstemp(char *template_name) {
    return ::mkstemp(template_name);
}

int close(ZEN_HANDLE handle) {
    return ::close(handle);
}

ZEN_HANDLE open(const char *filename, int open_mode, mode_t perms) {
    return ::open(filename, open_mode, perms);
}

int filesize(const char *filename, size_t *file_size) {
    ZEN_HANDLE file_handle = ZEN_OS::open(filename, 0, 0660);
    if (file_handle == -1) {
        return -1;
    }
    int ret = ZEN_OS::filesize(file_handle, file_size);
    ZEN_OS::close(file_handle);
    return ret;
}

ssize_t lseek(ZEN_HANDLE file_handle, ssize_t offset, int whence) {
    return ::lseek(file_handle, offset, whence);
}

int ftruncate(ZEN_HANDLE file_handle, size_t offset) {
    return ::ftruncate(file_handle, offset);
}

int truncate(const char *filename, size_t offset) {
    return ::truncate(filename, offset);
}

ssize_t write(ZEN_HANDLE file_handle, const void *buf, size_t count) {
    return ::write(file_handle, buf, count);
}

ssize_t read(ZEN_HANDLE file_handle, void *buf, size_t count) {
    return ::read(file_handle, buf, count);
}

int fstat(ZEN_HANDLE file_handle, zen_os_stat *file_stat) {
    return ::fstat(file_handle, file_stat);
}

int read_file_data(const char *filename, char *buffer, size_t buf_len,
                   size_t *read_len) {
    ZEN_HANDLE fd = ZEN_OS::open(filename, 0, 0660);
    if (fd == -1) {
        zen_debug_errorex("open file [%s]  fail ,error =%d", filename, errno);
        return -1;
    }
    int len = ZEN_OS::read(fd, buffer, buf_len - 1);
    if (len < 0) {
        // Original quirk: the fd is left open on the read-error path.
        zen_debug_errorex("read file [%s] fail ,error =%d", filename, errno);
        return -1;
    }
    ZEN_OS::close(fd);
    buffer[len] = 0;
    *read_len = len;
    return 0;
}

} // namespace ZEN_OS
