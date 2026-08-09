/* ripemd160.cpp -- RIPEMD-160（Antoon Bosselaers 参考实现，ESAT-COSIC）
   按 DWARF 类结构封装：CRipeMD160::AddData/FinalDigest/Reset/Transform +
   F..J 与 FF160..JJJ160 私有方法。 */

#include "inc/ripemd160.h"

#include <string.h>

unsigned int CRipeMD160::ROL(unsigned int x, unsigned int n) {
    return (x << n) | (x >> (32 - n));
}

unsigned int CRipeMD160::F(unsigned int x, unsigned int y, unsigned int z) {
    return x ^ y ^ z;
}

unsigned int CRipeMD160::G(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) | (~x & z);
}

unsigned int CRipeMD160::H(unsigned int x, unsigned int y, unsigned int z) {
    return (x | ~y) ^ z;
}

unsigned int CRipeMD160::I(unsigned int x, unsigned int y, unsigned int z) {
    return (x & z) | (y & ~z);
}

unsigned int CRipeMD160::J(unsigned int x, unsigned int y, unsigned int z) {
    return x ^ (y | ~z);
}

void CRipeMD160::FF160(unsigned int &ra, unsigned int b, unsigned int &rc,
                       unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += F(b, rc, d) + x;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::GG160(unsigned int &ra, unsigned int b, unsigned int &rc,
                       unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += G(b, rc, d) + x + 0x5a827999u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::HH160(unsigned int &ra, unsigned int b, unsigned int &rc,
                       unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += H(b, rc, d) + x + 0x6ed9eba1u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::II160(unsigned int &ra, unsigned int b, unsigned int &rc,
                       unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += I(b, rc, d) + x + 0x8f1bbcdcu;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::JJ160(unsigned int &ra, unsigned int b, unsigned int &rc,
                       unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += J(b, rc, d) + x + 0xa953fd4eu;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::FFF160(unsigned int &ra, unsigned int b, unsigned int &rc,
                        unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += F(b, rc, d) + x;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::GGG160(unsigned int &ra, unsigned int b, unsigned int &rc,
                        unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += G(b, rc, d) + x + 0x7a6d76e9u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::HHH160(unsigned int &ra, unsigned int b, unsigned int &rc,
                        unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += H(b, rc, d) + x + 0x6d703ef3u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::III160(unsigned int &ra, unsigned int b, unsigned int &rc,
                        unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += I(b, rc, d) + x + 0x5c4dd124u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::JJJ160(unsigned int &ra, unsigned int b, unsigned int &rc,
                        unsigned int d, unsigned int e, unsigned int x, unsigned int s) {
    ra += J(b, rc, d) + x + 0x50a28be6u;
    ra = ROL(ra, s) + e;
    rc = ROL(rc, 10);
}

void CRipeMD160::Transform() {
    unsigned int aa = m_auiBuf[0], bb = m_auiBuf[1], cc = m_auiBuf[2],
                 dd = m_auiBuf[3], ee = m_auiBuf[4];
    unsigned int aaa = m_auiBuf[0], bbb = m_auiBuf[1], ccc = m_auiBuf[2],
                 ddd = m_auiBuf[3], eee = m_auiBuf[4];
    unsigned int X[16];
    int i;

    for (i = 0; i < 16; i++)
        X[i] = (unsigned int)m_aucIn[i * 4] |
               ((unsigned int)m_aucIn[i * 4 + 1] << 8) |
               ((unsigned int)m_aucIn[i * 4 + 2] << 16) |
               ((unsigned int)m_aucIn[i * 4 + 3] << 24);

    FF160(aa, bb, cc, dd, ee, X[0], 11);  FF160(ee, aa, bb, cc, dd, X[1], 14);
    FF160(dd, ee, aa, bb, cc, X[2], 15);  FF160(cc, dd, ee, aa, bb, X[3], 12);
    FF160(bb, cc, dd, ee, aa, X[4], 5);   FF160(aa, bb, cc, dd, ee, X[5], 8);
    FF160(ee, aa, bb, cc, dd, X[6], 7);   FF160(dd, ee, aa, bb, cc, X[7], 9);
    FF160(cc, dd, ee, aa, bb, X[8], 11);  FF160(bb, cc, dd, ee, aa, X[9], 13);
    FF160(aa, bb, cc, dd, ee, X[10], 14); FF160(ee, aa, bb, cc, dd, X[11], 15);
    FF160(dd, ee, aa, bb, cc, X[12], 6);  FF160(cc, dd, ee, aa, bb, X[13], 7);
    FF160(bb, cc, dd, ee, aa, X[14], 9);  FF160(aa, bb, cc, dd, ee, X[15], 8);

    GG160(ee, aa, bb, cc, dd, X[7], 7);   GG160(dd, ee, aa, bb, cc, X[4], 6);
    GG160(cc, dd, ee, aa, bb, X[13], 8);  GG160(bb, cc, dd, ee, aa, X[1], 13);
    GG160(aa, bb, cc, dd, ee, X[10], 11); GG160(ee, aa, bb, cc, dd, X[6], 9);
    GG160(dd, ee, aa, bb, cc, X[15], 7);  GG160(cc, dd, ee, aa, bb, X[3], 15);
    GG160(bb, cc, dd, ee, aa, X[12], 7);  GG160(aa, bb, cc, dd, ee, X[0], 12);
    GG160(ee, aa, bb, cc, dd, X[9], 15);  GG160(dd, ee, aa, bb, cc, X[5], 9);
    GG160(cc, dd, ee, aa, bb, X[2], 11);  GG160(bb, cc, dd, ee, aa, X[14], 7);
    GG160(aa, bb, cc, dd, ee, X[11], 13); GG160(ee, aa, bb, cc, dd, X[8], 12);

    HH160(dd, ee, aa, bb, cc, X[3], 11);  HH160(cc, dd, ee, aa, bb, X[10], 13);
    HH160(bb, cc, dd, ee, aa, X[14], 6);  HH160(aa, bb, cc, dd, ee, X[4], 7);
    HH160(ee, aa, bb, cc, dd, X[9], 14);  HH160(dd, ee, aa, bb, cc, X[15], 9);
    HH160(cc, dd, ee, aa, bb, X[8], 13);  HH160(bb, cc, dd, ee, aa, X[1], 15);
    HH160(aa, bb, cc, dd, ee, X[2], 14);  HH160(ee, aa, bb, cc, dd, X[7], 8);
    HH160(dd, ee, aa, bb, cc, X[0], 13);  HH160(cc, dd, ee, aa, bb, X[6], 6);
    HH160(bb, cc, dd, ee, aa, X[13], 5);  HH160(aa, bb, cc, dd, ee, X[11], 12);
    HH160(ee, aa, bb, cc, dd, X[5], 7);   HH160(dd, ee, aa, bb, cc, X[12], 5);

    II160(cc, dd, ee, aa, bb, X[1], 11);  II160(bb, cc, dd, ee, aa, X[9], 12);
    II160(aa, bb, cc, dd, ee, X[11], 14); II160(ee, aa, bb, cc, dd, X[10], 15);
    II160(dd, ee, aa, bb, cc, X[0], 14);  II160(cc, dd, ee, aa, bb, X[8], 15);
    II160(bb, cc, dd, ee, aa, X[12], 9);  II160(aa, bb, cc, dd, ee, X[4], 8);
    II160(ee, aa, bb, cc, dd, X[13], 9);  II160(dd, ee, aa, bb, cc, X[3], 14);
    II160(cc, dd, ee, aa, bb, X[7], 5);   II160(bb, cc, dd, ee, aa, X[15], 6);
    II160(aa, bb, cc, dd, ee, X[14], 8);  II160(ee, aa, bb, cc, dd, X[5], 6);
    II160(dd, ee, aa, bb, cc, X[6], 5);   II160(cc, dd, ee, aa, bb, X[2], 12);

    JJ160(bb, cc, dd, ee, aa, X[4], 9);   JJ160(aa, bb, cc, dd, ee, X[0], 15);
    JJ160(ee, aa, bb, cc, dd, X[5], 5);   JJ160(dd, ee, aa, bb, cc, X[9], 11);
    JJ160(cc, dd, ee, aa, bb, X[7], 6);   JJ160(bb, cc, dd, ee, aa, X[12], 8);
    JJ160(aa, bb, cc, dd, ee, X[2], 13);  JJ160(ee, aa, bb, cc, dd, X[10], 12);
    JJ160(dd, ee, aa, bb, cc, X[14], 5);  JJ160(cc, dd, ee, aa, bb, X[1], 12);
    JJ160(bb, cc, dd, ee, aa, X[3], 13);  JJ160(aa, bb, cc, dd, ee, X[8], 14);
    JJ160(ee, aa, bb, cc, dd, X[11], 11); JJ160(dd, ee, aa, bb, cc, X[6], 8);
    JJ160(cc, dd, ee, aa, bb, X[15], 5);  JJ160(bb, cc, dd, ee, aa, X[13], 6);

    JJJ160(aaa, bbb, ccc, ddd, eee, X[5], 8);   JJJ160(eee, aaa, bbb, ccc, ddd, X[14], 9);
    JJJ160(ddd, eee, aaa, bbb, ccc, X[7], 9);   JJJ160(ccc, ddd, eee, aaa, bbb, X[0], 11);
    JJJ160(bbb, ccc, ddd, eee, aaa, X[9], 13);  JJJ160(aaa, bbb, ccc, ddd, eee, X[2], 15);
    JJJ160(eee, aaa, bbb, ccc, ddd, X[11], 15); JJJ160(ddd, eee, aaa, bbb, ccc, X[4], 5);
    JJJ160(ccc, ddd, eee, aaa, bbb, X[13], 7);  JJJ160(bbb, ccc, ddd, eee, aaa, X[6], 7);
    JJJ160(aaa, bbb, ccc, ddd, eee, X[15], 8);  JJJ160(eee, aaa, bbb, ccc, ddd, X[8], 11);
    JJJ160(ddd, eee, aaa, bbb, ccc, X[1], 14);  JJJ160(ccc, ddd, eee, aaa, bbb, X[10], 14);
    JJJ160(bbb, ccc, ddd, eee, aaa, X[3], 12);  JJJ160(aaa, bbb, ccc, ddd, eee, X[12], 6);

    III160(eee, aaa, bbb, ccc, ddd, X[6], 9);   III160(ddd, eee, aaa, bbb, ccc, X[11], 13);
    III160(ccc, ddd, eee, aaa, bbb, X[3], 15);  III160(bbb, ccc, ddd, eee, aaa, X[7], 7);
    III160(aaa, bbb, ccc, ddd, eee, X[0], 12);  III160(eee, aaa, bbb, ccc, ddd, X[13], 8);
    III160(ddd, eee, aaa, bbb, ccc, X[5], 9);   III160(ccc, ddd, eee, aaa, bbb, X[10], 11);
    III160(bbb, ccc, ddd, eee, aaa, X[14], 7);  III160(aaa, bbb, ccc, ddd, eee, X[15], 7);
    III160(eee, aaa, bbb, ccc, ddd, X[8], 12);  III160(ddd, eee, aaa, bbb, ccc, X[12], 7);
    III160(ccc, ddd, eee, aaa, bbb, X[4], 6);   III160(bbb, ccc, ddd, eee, aaa, X[9], 15);
    III160(aaa, bbb, ccc, ddd, eee, X[1], 13);  III160(eee, aaa, bbb, ccc, ddd, X[2], 11);

    HHH160(ddd, eee, aaa, bbb, ccc, X[15], 9);  HHH160(ccc, ddd, eee, aaa, bbb, X[5], 7);
    HHH160(bbb, ccc, ddd, eee, aaa, X[1], 15);  HHH160(aaa, bbb, ccc, ddd, eee, X[3], 11);
    HHH160(eee, aaa, bbb, ccc, ddd, X[7], 8);   HHH160(ddd, eee, aaa, bbb, ccc, X[14], 6);
    HHH160(ccc, ddd, eee, aaa, bbb, X[6], 6);   HHH160(bbb, ccc, ddd, eee, aaa, X[9], 14);
    HHH160(aaa, bbb, ccc, ddd, eee, X[11], 12); HHH160(eee, aaa, bbb, ccc, ddd, X[8], 13);
    HHH160(ddd, eee, aaa, bbb, ccc, X[12], 5);  HHH160(ccc, ddd, eee, aaa, bbb, X[2], 14);
    HHH160(bbb, ccc, ddd, eee, aaa, X[10], 13); HHH160(aaa, bbb, ccc, ddd, eee, X[0], 13);
    HHH160(eee, aaa, bbb, ccc, ddd, X[4], 7);   HHH160(ddd, eee, aaa, bbb, ccc, X[13], 5);

    GGG160(ccc, ddd, eee, aaa, bbb, X[8], 15);  GGG160(bbb, ccc, ddd, eee, aaa, X[6], 5);
    GGG160(aaa, bbb, ccc, ddd, eee, X[4], 8);   GGG160(eee, aaa, bbb, ccc, ddd, X[1], 11);
    GGG160(ddd, eee, aaa, bbb, ccc, X[3], 14);  GGG160(ccc, ddd, eee, aaa, bbb, X[11], 14);
    GGG160(bbb, ccc, ddd, eee, aaa, X[15], 6);  GGG160(aaa, bbb, ccc, ddd, eee, X[0], 14);
    GGG160(eee, aaa, bbb, ccc, ddd, X[5], 6);   GGG160(ddd, eee, aaa, bbb, ccc, X[12], 9);
    GGG160(ccc, ddd, eee, aaa, bbb, X[2], 12);  GGG160(bbb, ccc, ddd, eee, aaa, X[13], 9);
    GGG160(aaa, bbb, ccc, ddd, eee, X[9], 12);  GGG160(eee, aaa, bbb, ccc, ddd, X[7], 5);
    GGG160(ddd, eee, aaa, bbb, ccc, X[10], 15); GGG160(ccc, ddd, eee, aaa, bbb, X[14], 8);

    FFF160(bbb, ccc, ddd, eee, aaa, X[12], 8);  FFF160(aaa, bbb, ccc, ddd, eee, X[15], 5);
    FFF160(eee, aaa, bbb, ccc, ddd, X[10], 12); FFF160(ddd, eee, aaa, bbb, ccc, X[4], 9);
    FFF160(ccc, ddd, eee, aaa, bbb, X[1], 12);  FFF160(bbb, ccc, ddd, eee, aaa, X[5], 5);
    FFF160(aaa, bbb, ccc, ddd, eee, X[8], 14);  FFF160(eee, aaa, bbb, ccc, ddd, X[7], 6);
    FFF160(ddd, eee, aaa, bbb, ccc, X[6], 8);   FFF160(ccc, ddd, eee, aaa, bbb, X[2], 13);
    FFF160(bbb, ccc, ddd, eee, aaa, X[13], 6);  FFF160(aaa, bbb, ccc, ddd, eee, X[14], 5);
    FFF160(eee, aaa, bbb, ccc, ddd, X[0], 15);  FFF160(ddd, eee, aaa, bbb, ccc, X[3], 13);
    FFF160(ccc, ddd, eee, aaa, bbb, X[9], 11);  FFF160(bbb, ccc, ddd, eee, aaa, X[11], 11);

    ddd += cc + m_auiBuf[1];
    m_auiBuf[1] = m_auiBuf[2] + dd + eee;
    m_auiBuf[2] = m_auiBuf[3] + ee + aaa;
    m_auiBuf[3] = m_auiBuf[4] + aa + bbb;
    m_auiBuf[4] = m_auiBuf[0] + bb + ccc;
    m_auiBuf[0] = ddd;
}

void CRipeMD160::Reset() {
    m_auiBuf[0] = 0x67452301u;
    m_auiBuf[1] = 0xefcdab89u;
    m_auiBuf[2] = 0x98badcfeu;
    m_auiBuf[3] = 0x10325476u;
    m_auiBuf[4] = 0xc3d2e1f0u;
    m_auiBits[0] = m_auiBits[1] = 0;
    m_bAddData = false;
}

void CRipeMD160::AddData(unsigned char *pcData, unsigned int iDataLength) {
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

void CRipeMD160::FinalDigest(unsigned char *pcDigest) {
    unsigned char bits[8];
    unsigned char padding[64] = {0x80};
    unsigned int i, index, padLen;

    for (i = 0; i < 8; i++)
        bits[i] = (unsigned char)((m_auiBits[i >> 2] >> (8 * (i & 3))) & 255);
    index = (unsigned int)((m_auiBits[0] >> 3) & 0x3F);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    AddData(padding, padLen);
    AddData(bits, 8);
    for (i = 0; i < 5; i++) {
        pcDigest[i * 4]     = (unsigned char)(m_auiBuf[i] & 255);
        pcDigest[i * 4 + 1] = (unsigned char)((m_auiBuf[i] >> 8) & 255);
        pcDigest[i * 4 + 2] = (unsigned char)((m_auiBuf[i] >> 16) & 255);
        pcDigest[i * 4 + 3] = (unsigned char)((m_auiBuf[i] >> 24) & 255);
    }
}

CRipeMD160::CRipeMD160() {
    Reset();
}

CRipeMD160::~CRipeMD160() {
}
