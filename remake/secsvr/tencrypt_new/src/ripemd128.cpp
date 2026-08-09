/* ripemd128.cpp -- RIPEMD-128（Antoon Bosselaers 参考实现，ESAT-COSIC）
   按 DWARF 类结构封装：CRipeMD128。 */

#include "inc/ripemd128.h"

#include <string.h>

unsigned int CRipeMD128::ROL(unsigned int x, unsigned int n) {
    return (x << n) | (x >> (32 - n));
}

unsigned int CRipeMD128::F(unsigned int x, unsigned int y, unsigned int z) {
    return x ^ y ^ z;
}

unsigned int CRipeMD128::G(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) | (~x & z);
}

unsigned int CRipeMD128::H(unsigned int x, unsigned int y, unsigned int z) {
    return (x | ~y) ^ z;
}

unsigned int CRipeMD128::I(unsigned int x, unsigned int y, unsigned int z) {
    return (x & z) | (y & ~z);
}

void CRipeMD128::FF128(unsigned int &ra, unsigned int b, unsigned int c,
                       unsigned int d, unsigned int x, unsigned int s) {
    ra += F(b, c, d) + x;
    ra = ROL(ra, s);
}

void CRipeMD128::GG128(unsigned int &ra, unsigned int b, unsigned int c,
                       unsigned int d, unsigned int x, unsigned int s) {
    ra += G(b, c, d) + x + 0x5a827999u;
    ra = ROL(ra, s);
}

void CRipeMD128::HH128(unsigned int &ra, unsigned int b, unsigned int c,
                       unsigned int d, unsigned int x, unsigned int s) {
    ra += H(b, c, d) + x + 0x6ed9eba1u;
    ra = ROL(ra, s);
}

void CRipeMD128::II128(unsigned int &ra, unsigned int b, unsigned int c,
                       unsigned int d, unsigned int x, unsigned int s) {
    ra += I(b, c, d) + x + 0x8f1bbcdcu;
    ra = ROL(ra, s);
}

void CRipeMD128::FFF128(unsigned int &ra, unsigned int b, unsigned int c,
                        unsigned int d, unsigned int x, unsigned int s) {
    ra += F(b, c, d) + x;
    ra = ROL(ra, s);
}

void CRipeMD128::GGG128(unsigned int &ra, unsigned int b, unsigned int c,
                        unsigned int d, unsigned int x, unsigned int s) {
    ra += G(b, c, d) + x + 0x6d703ef3u;
    ra = ROL(ra, s);
}

void CRipeMD128::HHH128(unsigned int &ra, unsigned int b, unsigned int c,
                        unsigned int d, unsigned int x, unsigned int s) {
    ra += H(b, c, d) + x + 0x5c4dd124u;
    ra = ROL(ra, s);
}

void CRipeMD128::III128(unsigned int &ra, unsigned int b, unsigned int c,
                        unsigned int d, unsigned int x, unsigned int s) {
    ra += I(b, c, d) + x + 0x50a28be6u;
    ra = ROL(ra, s);
}

void CRipeMD128::Transform() {
    unsigned int aa = m_auiBuf[0], bb = m_auiBuf[1], cc = m_auiBuf[2], dd = m_auiBuf[3];
    unsigned int aaa = m_auiBuf[0], bbb = m_auiBuf[1], ccc = m_auiBuf[2], ddd = m_auiBuf[3];
    unsigned int X[16];
    int i;

    for (i = 0; i < 16; i++)
        X[i] = (unsigned int)m_aucIn[i * 4] |
               ((unsigned int)m_aucIn[i * 4 + 1] << 8) |
               ((unsigned int)m_aucIn[i * 4 + 2] << 16) |
               ((unsigned int)m_aucIn[i * 4 + 3] << 24);

    FF128(aa, bb, cc, dd, X[0], 11);  FF128(dd, aa, bb, cc, X[1], 14);
    FF128(cc, dd, aa, bb, X[2], 15);  FF128(bb, cc, dd, aa, X[3], 12);
    FF128(aa, bb, cc, dd, X[4], 5);   FF128(dd, aa, bb, cc, X[5], 8);
    FF128(cc, dd, aa, bb, X[6], 7);   FF128(bb, cc, dd, aa, X[7], 9);
    FF128(aa, bb, cc, dd, X[8], 11);  FF128(dd, aa, bb, cc, X[9], 13);
    FF128(cc, dd, aa, bb, X[10], 14); FF128(bb, cc, dd, aa, X[11], 15);
    FF128(aa, bb, cc, dd, X[12], 6);  FF128(dd, aa, bb, cc, X[13], 7);
    FF128(cc, dd, aa, bb, X[14], 9);  FF128(bb, cc, dd, aa, X[15], 8);

    GG128(aa, bb, cc, dd, X[7], 7);   GG128(dd, aa, bb, cc, X[4], 6);
    GG128(cc, dd, aa, bb, X[13], 8);  GG128(bb, cc, dd, aa, X[1], 13);
    GG128(aa, bb, cc, dd, X[10], 11); GG128(dd, aa, bb, cc, X[6], 9);
    GG128(cc, dd, aa, bb, X[15], 7);  GG128(bb, cc, dd, aa, X[3], 15);
    GG128(aa, bb, cc, dd, X[12], 7);  GG128(dd, aa, bb, cc, X[0], 12);
    GG128(cc, dd, aa, bb, X[9], 15);  GG128(bb, cc, dd, aa, X[5], 9);
    GG128(aa, bb, cc, dd, X[2], 11);  GG128(dd, aa, bb, cc, X[14], 7);
    GG128(cc, dd, aa, bb, X[11], 13); GG128(bb, cc, dd, aa, X[8], 12);

    HH128(aa, bb, cc, dd, X[3], 11);  HH128(dd, aa, bb, cc, X[10], 13);
    HH128(cc, dd, aa, bb, X[14], 6);  HH128(bb, cc, dd, aa, X[4], 7);
    HH128(aa, bb, cc, dd, X[9], 14);  HH128(dd, aa, bb, cc, X[15], 9);
    HH128(cc, dd, aa, bb, X[8], 13);  HH128(bb, cc, dd, aa, X[1], 15);
    HH128(aa, bb, cc, dd, X[2], 14);  HH128(dd, aa, bb, cc, X[7], 8);
    HH128(cc, dd, aa, bb, X[0], 13);  HH128(bb, cc, dd, aa, X[6], 6);
    HH128(aa, bb, cc, dd, X[13], 5);  HH128(dd, aa, bb, cc, X[11], 12);
    HH128(cc, dd, aa, bb, X[5], 7);   HH128(bb, cc, dd, aa, X[12], 5);

    II128(aa, bb, cc, dd, X[1], 11);  II128(dd, aa, bb, cc, X[9], 12);
    II128(cc, dd, aa, bb, X[11], 14); II128(bb, cc, dd, aa, X[10], 15);
    II128(aa, bb, cc, dd, X[0], 14);  II128(dd, aa, bb, cc, X[8], 15);
    II128(cc, dd, aa, bb, X[12], 9);  II128(bb, cc, dd, aa, X[4], 8);
    II128(aa, bb, cc, dd, X[13], 9);  II128(dd, aa, bb, cc, X[3], 14);
    II128(cc, dd, aa, bb, X[7], 5);   II128(bb, cc, dd, aa, X[15], 6);
    II128(aa, bb, cc, dd, X[14], 8);  II128(dd, aa, bb, cc, X[5], 6);
    II128(cc, dd, aa, bb, X[6], 5);   II128(bb, cc, dd, aa, X[2], 12);

    III128(aaa, bbb, ccc, ddd, X[5], 8);   III128(ddd, aaa, bbb, ccc, X[14], 9);
    III128(ccc, ddd, aaa, bbb, X[7], 9);   III128(bbb, ccc, ddd, aaa, X[0], 11);
    III128(aaa, bbb, ccc, ddd, X[9], 13);  III128(ddd, aaa, bbb, ccc, X[2], 15);
    III128(ccc, ddd, aaa, bbb, X[11], 15); III128(bbb, ccc, ddd, aaa, X[4], 5);
    III128(aaa, bbb, ccc, ddd, X[13], 7);  III128(ddd, aaa, bbb, ccc, X[6], 7);
    III128(ccc, ddd, aaa, bbb, X[15], 8);  III128(bbb, ccc, ddd, aaa, X[8], 11);
    III128(aaa, bbb, ccc, ddd, X[1], 14);  III128(ddd, aaa, bbb, ccc, X[10], 14);
    III128(ccc, ddd, aaa, bbb, X[3], 12);  III128(bbb, ccc, ddd, aaa, X[12], 6);

    HHH128(aaa, bbb, ccc, ddd, X[6], 9);   HHH128(ddd, aaa, bbb, ccc, X[11], 13);
    HHH128(ccc, ddd, aaa, bbb, X[3], 15);  HHH128(bbb, ccc, ddd, aaa, X[7], 7);
    HHH128(aaa, bbb, ccc, ddd, X[0], 12);  HHH128(ddd, aaa, bbb, ccc, X[13], 8);
    HHH128(ccc, ddd, aaa, bbb, X[5], 9);   HHH128(bbb, ccc, ddd, aaa, X[10], 11);
    HHH128(aaa, bbb, ccc, ddd, X[14], 7);  HHH128(ddd, aaa, bbb, ccc, X[15], 7);
    HHH128(ccc, ddd, aaa, bbb, X[8], 12);  HHH128(bbb, ccc, ddd, aaa, X[12], 7);
    HHH128(aaa, bbb, ccc, ddd, X[4], 6);   HHH128(ddd, aaa, bbb, ccc, X[9], 15);
    HHH128(ccc, ddd, aaa, bbb, X[1], 13);  HHH128(bbb, ccc, ddd, aaa, X[2], 11);

    GGG128(aaa, bbb, ccc, ddd, X[15], 9);  GGG128(ddd, aaa, bbb, ccc, X[5], 7);
    GGG128(ccc, ddd, aaa, bbb, X[1], 15);  GGG128(bbb, ccc, ddd, aaa, X[3], 11);
    GGG128(aaa, bbb, ccc, ddd, X[7], 8);   GGG128(ddd, aaa, bbb, ccc, X[14], 6);
    GGG128(ccc, ddd, aaa, bbb, X[6], 6);   GGG128(bbb, ccc, ddd, aaa, X[9], 14);
    GGG128(aaa, bbb, ccc, ddd, X[11], 12); GGG128(ddd, aaa, bbb, ccc, X[8], 13);
    GGG128(ccc, ddd, aaa, bbb, X[12], 5);  GGG128(bbb, ccc, ddd, aaa, X[2], 14);
    GGG128(aaa, bbb, ccc, ddd, X[10], 13); GGG128(ddd, aaa, bbb, ccc, X[0], 13);
    GGG128(ccc, ddd, aaa, bbb, X[4], 7);   GGG128(bbb, ccc, ddd, aaa, X[13], 5);

    FFF128(aaa, bbb, ccc, ddd, X[8], 15);  FFF128(ddd, aaa, bbb, ccc, X[6], 5);
    FFF128(ccc, ddd, aaa, bbb, X[4], 8);   FFF128(bbb, ccc, ddd, aaa, X[1], 11);
    FFF128(aaa, bbb, ccc, ddd, X[3], 14);  FFF128(ddd, aaa, bbb, ccc, X[11], 14);
    FFF128(ccc, ddd, aaa, bbb, X[15], 6);  FFF128(bbb, ccc, ddd, aaa, X[0], 14);
    FFF128(aaa, bbb, ccc, ddd, X[5], 6);   FFF128(ddd, aaa, bbb, ccc, X[12], 9);
    FFF128(ccc, ddd, aaa, bbb, X[2], 12);  FFF128(bbb, ccc, ddd, aaa, X[13], 9);
    FFF128(aaa, bbb, ccc, ddd, X[9], 12);  FFF128(ddd, aaa, bbb, ccc, X[7], 5);
    FFF128(ccc, ddd, aaa, bbb, X[10], 15); FFF128(bbb, ccc, ddd, aaa, X[14], 8);

    ddd += cc + m_auiBuf[1];
    m_auiBuf[1] = m_auiBuf[2] + dd + aaa;
    m_auiBuf[2] = m_auiBuf[3] + aa + bbb;
    m_auiBuf[3] = m_auiBuf[0] + bb + ccc;
    m_auiBuf[0] = ddd;
}

void CRipeMD128::Reset() {
    m_auiBuf[0] = 0x67452301u;
    m_auiBuf[1] = 0xefcdab89u;
    m_auiBuf[2] = 0x98badcfeu;
    m_auiBuf[3] = 0x10325476u;
    m_auiBits[0] = m_auiBits[1] = 0;
    m_bAddData = false;
}

void CRipeMD128::AddData(unsigned char *pcData, unsigned int iDataLength) {
    unsigned int i, index, partLen;

    if (iDataLength == 0)
        return;
    m_bAddData = true;
    index = (unsigned int)((m_auiBits[0] >> 3) & 0x3F);
    if ((m_auiBits[0] += (iDataLength << 3)) < (iDataLength << 3))
        m_auiBits[1]++;
    m_auiBits[1] += (iDataLength >> 29);

    partLen = 64 - index;
    if (iDataLength >= partLen) {
        memcpy(m_aucIn + index, pcData, partLen);
        Transform();
        for (i = partLen; i + 63 < iDataLength; i += 64) {
            memcpy(m_aucIn, pcData + i, 64);
            Transform();
        }
        index = 0;
    } else {
        i = 0;
    }
    memcpy(m_aucIn + index, pcData + i, iDataLength - i);
}

void CRipeMD128::FinalDigest(unsigned char *pcDigest) {
    unsigned char bits[8];
    unsigned char padding[64] = {0x80};
    unsigned int i, index, padLen;

    for (i = 0; i < 8; i++)
        bits[i] = (unsigned char)((m_auiBits[i >> 2] >> (8 * (i & 3))) & 255);
    index = (unsigned int)((m_auiBits[0] >> 3) & 0x3F);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    AddData(padding, padLen);
    AddData(bits, 8);
    for (i = 0; i < 4; i++) {
        pcDigest[i * 4]     = (unsigned char)(m_auiBuf[i] & 255);
        pcDigest[i * 4 + 1] = (unsigned char)((m_auiBuf[i] >> 8) & 255);
        pcDigest[i * 4 + 2] = (unsigned char)((m_auiBuf[i] >> 16) & 255);
        pcDigest[i * 4 + 3] = (unsigned char)((m_auiBuf[i] >> 24) & 255);
    }
}

CRipeMD128::CRipeMD128() {
    Reset();
}

CRipeMD128::~CRipeMD128() {
}
