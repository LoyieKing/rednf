/* Link shim: Debian i386 OpenSSL 3 libcrypto.a vs CentOS6-era ld/glibc.
 * Provides symbols the archive references so df_game_r can link.
 * Not used at runtime by the game paths that matter. */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

char *secure_getenv(const char *name) { return getenv(name); }

long __isoc23_strtol(const char *nptr, char **endptr, int base) {
  return strtol(nptr, endptr, base);
}

/* glibc fortified FD_SET helper; old libc lacks it */
long __fdelt_chk(long d) {
  if (d < 0 || d >= 1024) abort();
  return d / (8 * (long)sizeof(long));
}

struct mmsghdr; /* opaque */
int sendmmsg(int sockfd, struct mmsghdr *msgvec, unsigned int vlen, int flags) {
  (void)sockfd; (void)msgvec; (void)vlen; (void)flags;
  errno = ENOSYS;
  return -1;
}

/* ZSTD stubs - OpenSSL zstd COMP filter unused by DNF game paths */
typedef struct ZSTD_CCtx_s ZSTD_CCtx;
typedef struct ZSTD_DCtx_s ZSTD_DCtx;
typedef struct ZSTD_CStream_s ZSTD_CStream;
typedef struct ZSTD_DStream_s ZSTD_DStream;
typedef struct ZSTD_inBuffer_s { const void* src; size_t size; size_t pos; } ZSTD_inBuffer;
typedef struct ZSTD_outBuffer_s { void* dst; size_t size; size_t pos; } ZSTD_outBuffer;
struct ZSTD_customMem { void*(*customAlloc)(void*, size_t); void(*customFree)(void*, void*); void* opaque; };

size_t ZSTD_CStreamInSize(void) { return 0; }
size_t ZSTD_DStreamInSize(void) { return 0; }
size_t ZSTD_compress(void* dst, size_t dstCap, const void* src, size_t srcSize, int lvl) {
  (void)dst;(void)dstCap;(void)src;(void)srcSize;(void)lvl; return (size_t)-1;
}
size_t ZSTD_decompress(void* dst, size_t dstCap, const void* src, size_t srcSize) {
  (void)dst;(void)dstCap;(void)src;(void)srcSize; return (size_t)-1;
}
size_t ZSTD_compressStream2(ZSTD_CCtx* c, ZSTD_outBuffer* o, ZSTD_inBuffer* i, int endOp) {
  (void)c;(void)o;(void)i;(void)endOp; return (size_t)-1;
}
size_t ZSTD_decompressStream(ZSTD_DStream* d, ZSTD_outBuffer* o, ZSTD_inBuffer* i) {
  (void)d;(void)o;(void)i; return (size_t)-1;
}
size_t ZSTD_flushStream(ZSTD_CStream* c, ZSTD_outBuffer* o) { (void)c;(void)o; return (size_t)-1; }
size_t ZSTD_endStream(ZSTD_CStream* c, ZSTD_outBuffer* o) { (void)c;(void)o; return (size_t)-1; }
size_t ZSTD_initCStream(ZSTD_CStream* c, int lvl) { (void)c;(void)lvl; return (size_t)-1; }
size_t ZSTD_initDStream(ZSTD_DStream* d) { (void)d; return (size_t)-1; }
unsigned ZSTD_isError(size_t code) { return code == (size_t)-1; }
const char* ZSTD_getErrorName(size_t code) { (void)code; return "shim"; }
ZSTD_CStream* ZSTD_createCStream_advanced(struct ZSTD_customMem m) { (void)m; return 0; }
ZSTD_DStream* ZSTD_createDStream_advanced(struct ZSTD_customMem m) { (void)m; return 0; }
size_t ZSTD_freeCStream(ZSTD_CStream* c) { (void)c; return 0; }
size_t ZSTD_freeDStream(ZSTD_DStream* d) { (void)d; return 0; }
