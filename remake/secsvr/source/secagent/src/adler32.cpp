/* adler32.cpp -- Adler-32（RFC 1950，zlib 算法）
   由 DWARF 桩重建：CAdler32::adler32（局部 s1/s2/k 与 zlib 一致）。 */

#include "inc/adler32.h"

#define BASE 65521L
#define NMAX 5552

unsigned int CAdler32::adler32(unsigned int adler, const unsigned char *buf, unsigned int len) {
    unsigned int s1 = adler & 0xffff;
    unsigned int s2 = (adler >> 16) & 0xffff;
    int k;

    while (len > 0) {
        k = (int)(len < NMAX ? len : NMAX);
        len -= (unsigned int)k;
        while (k >= 16) {
            s1 += buf[0];  s2 += s1;
            s1 += buf[1];  s2 += s1;
            s1 += buf[2];  s2 += s1;
            s1 += buf[3];  s2 += s1;
            s1 += buf[4];  s2 += s1;
            s1 += buf[5];  s2 += s1;
            s1 += buf[6];  s2 += s1;
            s1 += buf[7];  s2 += s1;
            s1 += buf[8];  s2 += s1;
            s1 += buf[9];  s2 += s1;
            s1 += buf[10]; s2 += s1;
            s1 += buf[11]; s2 += s1;
            s1 += buf[12]; s2 += s1;
            s1 += buf[13]; s2 += s1;
            s1 += buf[14]; s2 += s1;
            s1 += buf[15]; s2 += s1;
            buf += 16;
            k -= 16;
        }
        while (k-- > 0) {
            s1 += *buf++;
            s2 += s1;
        }
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

CAdler32::CAdler32() {
}

CAdler32::~CAdler32() {
}
