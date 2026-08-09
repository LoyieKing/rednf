// Auto-generated stub from DWARF info
// Original source: src/md5.cpp
// Compiler: GNU C++ 4.1.2 20070115 (prerelease) (SUSE Linux)
// 函数体暂为空；仅保留签名、参数名与局部变量名。

#include "inc/md5.h"

// line 118
void md5_process(md5_state_t *pms, const md5_byte_t *data) {
    // local: md5_word_t a;
    // local: md5_word_t b;
    // local: md5_word_t c;
    // local: md5_word_t d;
    // local: md5_word_t t;
    // local: md5_word_t X[];
    // local: const md5_byte_t *xp;
    // local: int i;
}

// line 276
void md5_init(md5_state_t *pms) {
}

// line 286
void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes) {
    // local: const md5_byte_t *p;
    // local: int left;
    // local: int offset;
    // local: md5_word_t nbits;
    // local: int copy;
}

// line 324
void md5_finish(md5_state_t *pms, md5_byte_t *digest) {
    // local: md5_byte_t data[];
    // local: int i;
    // local: const md5_byte_t pad[];
}

