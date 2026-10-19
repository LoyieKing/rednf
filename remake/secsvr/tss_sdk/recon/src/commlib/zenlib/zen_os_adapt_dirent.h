// ZEN_OS dirent wrappers, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/zenlib/zen_os_adapt_dirent.cpp
//
// The ORIG was compiled with _FILE_OFFSET_BITS=64: its DWARF `struct dirent`
// is the 276-byte 64-bit layout (d_ino:__ino64_t, d_off:__off64_t) and every
// call lands in the libc *64 entry point (readdir64, scandir64, alphasort64,
// readdir64_r).  Modern glibc headers declare scandir/alphasort through
// struct dirent64, which is a different C++ type, so this header models the
// 2009 ABI explicitly and the .cpp binds directly to the libc symbols.
#ifndef TSS_RECON_ZEN_OS_ADAPT_DIRENT_H_
#define TSS_RECON_ZEN_OS_ADAPT_DIRENT_H_

#ifndef TSS_RECON_DIRENT_LAYOUT_DEFINED
#define TSS_RECON_DIRENT_LAYOUT_DEFINED

#include <stddef.h>

// glibc 2.9 i386, _FILE_OFFSET_BITS=64 (matches ORIG DWARF sizeof 276).
struct dirent {
    unsigned long long d_ino;   // +0x00 __ino64_t
    long long d_off;            // +0x08 __off64_t
    unsigned short d_reclen;    // +0x10
    unsigned char d_type;       // +0x12
    char d_name[256];           // +0x13
};

struct __dirstream;
typedef struct __dirstream DIR;

#endif // TSS_RECON_DIRENT_LAYOUT_DEFINED

namespace ZEN_OS {

int rmdir(const char *pathname);
int mkdir(const char *pathname, unsigned int mode);
int mkdir_recurse(const char *pathname, unsigned int mode);
int chdir(const char *pathname);
char *getcwd(char *buffer, int maxlen);

const char *dirname(const char *path_name, char *dir_name, size_t buf_len);
const char *basename(const char *path_name, char *file_name, size_t buf_len);

int alphasort(const dirent **left, const dirent **right);
int scandir(const char *dirname, dirent ***namelist,
            int (*selector)(const dirent *entry),
            int (*comparator)(const dirent **left, const dirent **right));

int readdir_r(DIR *dir_handle, dirent *entry, dirent **result);
dirent *readdir(DIR *dir_handle);
int closedir(DIR *dir_handle);
DIR *opendir(const char *dir_name);

} // namespace ZEN_OS

#endif // TSS_RECON_ZEN_OS_ADAPT_DIRENT_H_
