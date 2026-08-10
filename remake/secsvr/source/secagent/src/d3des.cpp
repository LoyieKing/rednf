/* d3des.cpp -- 三重 DES（Richard Outerbridge 公版 D3DES.C，类封装）
   SP1-8/TOTROT/BYTEBIT/BIGBYTE/PC1/PC2/DF_KEY 表从二进制 .rodata 提取。
   des2key/des3key 的密钥顺序与 edf 参数按二进制反汇编还原：
   - des2key：deskey(key+8, mode==0)；cpkey(KnR)；deskey(key, mode)；cpkey(Kn3)
   - des3key：enc 时 deskey(key+8,1)/cpkey(KnR)/deskey(key+16,0)/cpkey(Kn3)/
     deskey(key,0)；dec 时密钥顺序相反（key+16 先）
   - Ddes = desfunc(KnL/KnR/Kn3) 三次（16 字节密钥的 3DES）
   - D3des161=des2key+Ddes(8字节块)、D3des162=des2key+D2des(16字节块)、
     D3des24=des3key+D3des(24字节块)；余数交 TsLocal->Seattos
   验证：uni_call 调二进制 deskey/Ddes/D2des/D3des/包装层逐字节一致。 */

#include "inc/d3des.h"
#include "include/TenCrypt.h"

#include <string.h>

#define EN0 0
#define DE1 1

const unsigned int CD3des::FUCKING_SP1[64] = {
#include "d3des_sp1.inc"
};
const unsigned int CD3des::FUCKING_SP2[64] = {
#include "d3des_sp2.inc"
};
const unsigned int CD3des::FUCKING_SP3[64] = {
#include "d3des_sp3.inc"
};
const unsigned int CD3des::FUCKING_SP4[64] = {
#include "d3des_sp4.inc"
};
const unsigned int CD3des::FUCKING_SP5[64] = {
#include "d3des_sp5.inc"
};
const unsigned int CD3des::FUCKING_SP6[64] = {
#include "d3des_sp6.inc"
};
const unsigned int CD3des::FUCKING_SP7[64] = {
#include "d3des_sp7.inc"
};
const unsigned int CD3des::FUCKING_SP8[64] = {
#include "d3des_sp8.inc"
};
const unsigned char CD3des::FUCKING_TOTROT[16] = {
#include "d3des_totrot.inc"
};
const unsigned short CD3des::FUCKING_BYTEBIT[8] = {
#include "d3des_bytebit.inc"
};
const unsigned int CD3des::FUCKING_BIGBYTE[24] = {
#include "d3des_bigbyte.inc"
};
const unsigned char CD3des::FUCKING_PC1[56] = {
#include "d3des_pc1.inc"
};
const unsigned char CD3des::FUCKING_PC2[48] = {
#include "d3des_pc2.inc"
};
const unsigned char CD3des::FUCKING_DF_KEY[24] = {
#include "d3des_dfkey.inc"
};

static void cookey(unsigned int *raw1);

// mangled: _ZN6CD3des6deskeyEPKhs
void CD3des::deskey(const unsigned char *key, short edf) {
    int i;
    int j;
    int l;
    int m;
    unsigned int pc1m[56 + 1];
    unsigned int pcr[56 + 1];
    unsigned int kn[32];

    for (j = 0; j < 56; j++) {
        l = FUCKING_PC1[j];
        m = l & 7;
        pc1m[j] = ((key[l >> 3] & FUCKING_BYTEBIT[m]) != 0);
    }
    for (i = 0; i < 16; i++) {
        m = 0;
        for (j = 0; j < 28; j++) {
            l = j + FUCKING_TOTROT[i];
            if (l < 28)
                pcr[j] = pc1m[l];
            else
                pcr[j] = pc1m[l - 28];
        }
        for (j = 28; j < 56; j++) {
            l = j + FUCKING_TOTROT[i];
            if (l < 56)
                pcr[j] = pc1m[l];
            else
                pcr[j] = pc1m[l - 28];
        }
        for (j = 0; j < 24; j++) {
            if (pcr[FUCKING_PC2[j]])
                m |= FUCKING_BIGBYTE[j];
        }
        kn[i * 2] = m;

        m = 0;
        for (j = 0; j < 24; j++) {
            if (pcr[FUCKING_PC2[j + 24]])
                m |= FUCKING_BIGBYTE[j];
        }
        kn[i * 2 + 1] = m;
    }

    cookey(kn);
    usekey(kn);
    if (edf == DE1) {
        for (i = 0; i < 16; i += 2) {
            unsigned int t = KnL[i];
            KnL[i] = KnL[30 - i];
            KnL[30 - i] = t;
            t = KnL[i + 1];
            KnL[i + 1] = KnL[31 - i];
            KnL[31 - i] = t;
        }
    }
}

static void cookey(unsigned int *raw1) {
    int i;

    for (i = 0; i < 16; i++) {
        unsigned int c = raw1[0];
        unsigned int r = raw1[1];

        raw1[0] = ((c & 0x00fc0000) << 6) | ((c & 0x00000fc0) << 10) |
                  ((r & 0x00fc0000) >> 10) | ((r & 0x00000fc0) >> 6);
        raw1[1] = ((c & 0x0003f000) << 12) | ((c & 0x3f) << 16) |
                  (r & 0x3f) | ((r & 0x0003f000) >> 4);
        raw1 += 2;
    }
}

// mangled: _ZN6CD3des5cpkeyEPj
void CD3des::cpkey(unsigned int *into) {
    int i;

    for (i = 0; i < 32; i++)
        into[i] = KnL[i];
}

// mangled: _ZN6CD3des6usekeyEPj
void CD3des::usekey(unsigned int *from) {
    int i;

    for (i = 0; i < 32; i++)
        KnL[i] = from[i];
}

// mangled: _ZN6CD3des7scrunchEPhPj
void CD3des::scrunch(unsigned char *outof, unsigned int *into) {
    int i;

    *into = 0;
    for (i = 0; i < 4; i++)
        *into = (*into << 8) | outof[i];
    into++;
    *into = 0;
    for (i = 4; i < 8; i++)
        *into = (*into << 8) | outof[i];
}

// mangled: _ZN6CD3des7unscrunEPjPh
void CD3des::unscrun(unsigned int *outof, unsigned char *into) {
    int i;

    for (i = 0; i < 4; i++)
        into[i] = (unsigned char)(outof[0] >> (24 - 8 * i));
    for (i = 0; i < 4; i++)
    into[i + 4] = (unsigned char)(outof[1] >> (24 - 8 * i));
}

// mangled: _ZN6CD3des7desfuncEPjS0_
void CD3des::desfunc(unsigned int *block, unsigned int *keys) {
    unsigned int fval;
    unsigned int work;
    unsigned int right;
    unsigned int leftt;
    unsigned int t;
    int round;

    leftt = block[0];
    right = block[1];

    /* IP：64 位置换（从二进制经验提取，out 位 b ← in 位 ip_perm[b]） */
    {
        static const unsigned char ip_perm[64] = {
            39, 25, 17, 9, 1, 57, 49, 41, 33, 27, 19, 11, 3, 59, 51, 43,
            35, 29, 21, 13, 5, 61, 53, 45, 37, 31, 23, 15, 7, 63, 55, 47,
            38, 24, 16, 8, 0, 56, 48, 40, 32, 26, 18, 10, 2, 58, 50, 42,
            34, 28, 20, 12, 4, 60, 52, 44, 36, 30, 22, 14, 6, 62, 54, 46
        };
        unsigned int in[2] = {leftt, right};
        unsigned int out[2] = {0, 0};
        int b;

        for (b = 0; b < 64; b++) {
            int s = ip_perm[b];
            int bit = (in[s >> 5] >> (s & 31)) & 1;
            out[b >> 5] |= bit << (b & 31);
        }
        leftt = out[0];
        right = out[1];
    }

    for (round = 0; round < 8; round++) {
        work = ((leftt >> 4) | (leftt << 28)) ^ keys[0];
        fval = FUCKING_SP7[work & 0x3f];
        fval |= FUCKING_SP5[(work >> 8) & 0x3f];
        fval |= FUCKING_SP3[(work >> 16) & 0x3f];
        fval |= FUCKING_SP1[(work >> 24) & 0x3f];
        work = leftt ^ keys[1];
        fval |= FUCKING_SP8[work & 0x3f];
        fval |= FUCKING_SP6[(work >> 8) & 0x3f];
        fval |= FUCKING_SP4[(work >> 16) & 0x3f];
        fval |= FUCKING_SP2[(work >> 24) & 0x3f];
        right ^= fval;

        work = ((right >> 4) | (right << 28)) ^ keys[2];
        fval = FUCKING_SP7[work & 0x3f];
        fval |= FUCKING_SP5[(work >> 8) & 0x3f];
        fval |= FUCKING_SP3[(work >> 16) & 0x3f];
        fval |= FUCKING_SP1[(work >> 24) & 0x3f];
        work = right ^ keys[3];
        fval |= FUCKING_SP8[work & 0x3f];
        fval |= FUCKING_SP6[(work >> 8) & 0x3f];
        fval |= FUCKING_SP4[(work >> 16) & 0x3f];
        fval |= FUCKING_SP2[(work >> 24) & 0x3f];
        leftt ^= fval;

        keys += 4;
    }

    /* FP：64 位置换（从二进制经验提取，out 位 b ← in 位 fp_perm[b]） */
    {
        static const unsigned char fp_perm[64] = {
            4, 36, 12, 44, 20, 52, 28, 60, 3, 35, 11, 43, 19, 51, 27, 59,
            2, 34, 10, 42, 18, 50, 26, 58, 1, 33, 9, 41, 17, 49, 25, 57,
            8, 40, 16, 48, 24, 56, 0, 32, 7, 39, 15, 47, 23, 55, 31, 63,
            6, 38, 14, 46, 22, 54, 30, 62, 5, 37, 13, 45, 21, 53, 29, 61
        };
        unsigned int in[2] = {leftt, right};
        unsigned int out[2] = {0, 0};
        int b;

        for (b = 0; b < 64; b++) {
            int s = fp_perm[b];
            int bit = (in[s >> 5] >> (s & 31)) & 1;
            out[b >> 5] |= bit << (b & 31);
        }
        leftt = out[0];
        right = out[1];
    }

    block[0] = leftt;
    block[1] = right;
}

// mangled: _ZN6CD3des3desEPhS0_
void CD3des::des(unsigned char *inblock, unsigned char *outblock) {
    unsigned int work[2];

    scrunch(inblock, work);
    desfunc(work, KnL);
    unscrun(work, outblock);
}

// mangled: _ZN6CD3des7des2keyEPKhs
void CD3des::des2key(const unsigned char *hexkey, short mode) {
    deskey(hexkey + 8, mode == EN0);
    cpkey(KnR);
    deskey(hexkey, mode);
    cpkey(Kn3);
}

// mangled: _ZN6CD3des4DdesEPhS0_
void CD3des::Ddes(unsigned char *from, unsigned char *into) {
    unsigned int work[2];

    scrunch(from, work);
    desfunc(work, KnL);
    desfunc(work, KnR);
    desfunc(work, Kn3);
    unscrun(work, into);
}

// mangled: _ZN6CD3des5D2desEPhS0_
void CD3des::D2des(unsigned char *from, unsigned char *into) {
    unsigned int *right;
    unsigned int *l1;
    unsigned int swap;
    unsigned int leftt[2];
    unsigned int bufR[2];

    right = bufR;
    l1 = &leftt[1];
    scrunch(from, leftt);
    scrunch(&from[8], right);
    desfunc(leftt, KnL);
    desfunc(right, KnL);
    swap = *l1;
    *l1 = *right;
    *right = swap;
    desfunc(leftt, KnR);
    desfunc(right, KnR);
    swap = *l1;
    *l1 = *right;
    *right = swap;
    desfunc(leftt, Kn3);
    desfunc(right, Kn3);
    unscrun(leftt, into);
    unscrun(right, &into[8]);
}

// mangled: _ZN6CD3des7des3keyEPKhs
void CD3des::des3key(const unsigned char *hexkey, short mode) {
    if (mode == EN0) {
        deskey(hexkey + 8, DE1);
        cpkey(KnR);
        deskey(hexkey + 16, EN0);
        cpkey(Kn3);
        deskey(hexkey, EN0);
    } else {
        deskey(hexkey + 8, EN0);
        cpkey(KnR);
        deskey(hexkey, DE1);
        cpkey(Kn3);
        deskey(hexkey + 16, DE1);
    }
}

// mangled: _ZN6CD3des5D3desEPhS0_
void CD3des::D3des(unsigned char *from, unsigned char *into) {
    unsigned int work[2];
    unsigned int right[2];
    unsigned int leftt[2];

    scrunch(from, leftt);
    scrunch(&from[8], right);
    scrunch(&from[16], work);
    desfunc(leftt, KnL);
    desfunc(right, KnL);
    desfunc(work, KnL);
    {
        unsigned int t = leftt[1];
        leftt[1] = right[0];
        right[0] = t;
        t = right[1];
        right[1] = work[0];
        work[0] = t;
    }
    desfunc(leftt, KnR);
    desfunc(right, KnR);
    desfunc(work, KnR);
    {
        unsigned int t = leftt[1];
        leftt[1] = right[0];
        right[0] = t;
        t = right[1];
        right[1] = work[0];
        work[0] = t;
    }
    desfunc(leftt, Kn3);
    desfunc(right, Kn3);
    desfunc(work, Kn3);
    unscrun(leftt, into);
    unscrun(right, &into[8]);
    unscrun(work, &into[16]);
}

// mangled: _ZN6CD3des6cp3keyEPj
void CD3des::cp3key(unsigned int *into) {
    int i;

    for (i = 0; i < 32; i++)
        into[i] = KnL[i];
    for (i = 0; i < 32; i++)
        into[32 + i] = KnR[i];
    for (i = 0; i < 32; i++)
        into[64 + i] = Kn3[i];
}

// mangled: _ZN6CD3des7use3keyEPj
void CD3des::use3key(unsigned int *from) {
    int i;

    for (i = 0; i < 32; i++)
        KnL[i] = from[i];
    for (i = 0; i < 32; i++)
        KnR[i] = from[32 + i];
    for (i = 0; i < 32; i++)
        Kn3[i] = from[64 + i];
}

// mangled: _ZN6CD3des7makekeyEPcPh
void CD3des::makekey(char *aptr, unsigned char *kptr) {
    int first;
    int i;
    unsigned int savek[96];

    cp3key(savek);
    des2key(FUCKING_DF_KEY, EN0);
    for (i = 0; i < 8; i++)
        kptr[i] = FUCKING_DF_KEY[i];
    first = 1;
    while ((*aptr != '\0') || first) {
        unsigned char *store = kptr;
        for (i = 0; i < 8 && (*aptr != '\0'); i++) {
            *store++ ^= (unsigned char)(*aptr & 0x7f);
            *aptr++ = '\0';
        }
        Ddes(kptr, kptr);
        first = 0;
    }
    use3key(savek);
}

// mangled: _ZN6CD3des8make2keyEPcPh
void CD3des::make2key(char *aptr, unsigned char *kptr) {
    int first;
    int i;
    unsigned int savek[96];

    cp3key(savek);
    des2key(FUCKING_DF_KEY, EN0);
    for (i = 0; i < 16; i++)
        kptr[i] = FUCKING_DF_KEY[i];
    first = 1;
    while ((*aptr != '\0') || first) {
        unsigned char *store = kptr;
        for (i = 0; i < 16 && (*aptr != '\0'); i++) {
            *store++ ^= (unsigned char)(*aptr & 0x7f);
            *aptr++ = '\0';
        }
        D2des(kptr, kptr);
        first = 0;
    }
    use3key(savek);
}

// mangled: _ZN6CD3des8make3keyEPcPh
void CD3des::make3key(char *aptr, unsigned char *kptr) {
    int first;
    int i;
    unsigned int savek[96];

    cp3key(savek);
    des3key(FUCKING_DF_KEY, EN0);
    for (i = 0; i < 24; i++)
        kptr[i] = FUCKING_DF_KEY[i];
    first = 1;
    while ((*aptr != '\0') || first) {
        unsigned char *store = kptr;
        for (i = 0; i < 24 && (*aptr != '\0'); i++) {
            *store++ ^= (unsigned char)(*aptr & 0x7f);
            *aptr++ = '\0';
        }
        D3des(kptr, kptr);
        first = 0;
    }
    use3key(savek);
}

// mangled: _ZN6CD3des4InitEPvS0_
void CD3des::Init(void *arg0, void *arg1) {
    memset(KnL, 0, sizeof(KnL));
    memset(KnR, 0, sizeof(KnR));
    memset(Kn3, 0, sizeof(Kn3));
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN6CD3des6UnInitEv
void CD3des::UnInit() {
}

// mangled: _ZN6CD3desC1Ev / _ZN6CD3desC2Ev
CD3des::CD3des() {
    Init(0, 0);
}

// mangled: _ZN6CD3desD1Ev / _ZN6CD3desD2Ev
CD3des::~CD3des() {
    UnInit();
}

// mangled: _ZN6CD3des8D3des161EPhbS0_j
void CD3des::D3des161(unsigned char *key, bool benc, unsigned char *buf,
                      unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;
    des2key(key, (short)(!benc));
    blocks = buflen >> 3;
    for (i = 0; i < blocks; i++)
        Ddes(buf + 8 * i, buf + 8 * i);
    CreateTsLocal()->Seattos(key, benc, buf + blocks * 8,
                             buflen - blocks * 8);
}

// mangled: _ZN6CD3des8D3des162EPhbS0_j
void CD3des::D3des162(unsigned char *key, bool benc, unsigned char *buf,
                      unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;
    des2key(key, (short)(!benc));
    blocks = buflen >> 4;
    for (i = 0; i < blocks; i++)
        D2des(buf + 16 * i, buf + 16 * i);
    CreateTsLocal()->Seattos(key, benc, buf + blocks * 16,
                             buflen - blocks * 16);
}

// mangled: _ZN6CD3des7D3des24EPhbS0_j
void CD3des::D3des24(unsigned char *key, bool benc, unsigned char *buf,
                     unsigned int buflen) {
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;
    des3key(key, (short)(!benc));
    blocks = buflen / 24;
    for (i = 0; i < blocks; i++)
        D3des(buf + 24 * i, buf + 24 * i);
    CreateTsLocal()->Seattos(key, benc, buf + blocks * 24,
                             buflen - blocks * 24);
}

static void TenD3desN(unsigned char *key, bool benc, unsigned char *buf,
                      unsigned int buflen, int which) {
    CD3des d3;

    if (which == 0)
        d3.D3des161(key, benc, buf, buflen);
    else if (which == 1)
        d3.D3des162(key, benc, buf, buflen);
    else
        d3.D3des24(key, benc, buf, buflen);
}

// mangled: _Z11TenD3des161PhbS_j
void TenD3des161(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    TenD3desN(key, benc, buf, buflen, 0);
}

// mangled: _Z11TenD3des162PhbS_j
void TenD3des162(unsigned char *key, bool benc, unsigned char *buf,
                 unsigned int buflen) {
    TenD3desN(key, benc, buf, buflen, 1);
}

// mangled: _Z10TenD3des24PhbS_j
void TenD3des24(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    TenD3desN(key, benc, buf, buflen, 2);
}
