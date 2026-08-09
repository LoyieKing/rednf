/* frog.cpp -- FROG 块密码（AC 源码集 FROG/frog.c 公版结构）
   与 AC 的差异（由二进制反汇编确认）：
   - 全局静态变量改为 CFrog 成员；Init 分配 0x100 字节 seed 堆副本并从静态
     常量表拷贝（表 = AC seed[256]，二进制约 .rodata 0x081ccd80）
   - SetKey 只用 sim_key（无 loc_key / 无二次 make_ikey / 无 key 反馈加密）；
     kb 字节数 = min(keylen>>3, 32)（keylen>0x107 时取 32）
   - MakePerm/MakeKey/Encrypt/Decrypt 与 AC make_perm/make_ikey/enc/decrypt 一致
   - Frog 包装层：SetKey(key, 0x100) + 16 字节块循环；余数字节交
     CreateTsLocal()->Seattos(key, benc, buf+processed, remaining)（共享库层）
   验证：uni_call 调二进制 SetKey/Encrypt/Decrypt 逐字节一致（随机多组）。 */

#include "inc/frog.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned char u1byte;
typedef unsigned int u4byte;

static const u1byte frog_seed[256] = {
#include "frog_seed.inc"
};

#define f_rnd(j)                                \
    k = pp[j];                                  \
    blk[j] = sp[blk[j] ^ xp[j]];                \
    blk[(j + 1) & 15] ^= blk[j];                \
    blk[k] ^= blk[j]

#define b_rnd(j)                                \
    ct = blk[j];                                \
    blk[pp[j]] ^= blk[j];                       \
    blk[(j + 1) & 15] ^= blk[j];                \
    blk[j] = sp[ct] ^ xp[j]

// mangled: _ZN5CFrog4InitEPvS0_
void CFrog::Init(void *arg0, void *arg1) {
    seed = new u1byte[0x100];
    memset(seed, 0, 0x100);
    memmove(seed, frog_seed, 0x100);
    ik_len = 0x900;
    lkp = 0;
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN5CFrog6UnInitEv
void CFrog::UnInit() {
    if (seed) {
        delete[] seed;
        seed = 0;
    }
}

// mangled: _ZN5CFrogC1Ev / _ZN5CFrogC2Ev
CFrog::CFrog() {
    Init(0, 0);
}

// mangled: _ZN5CFrogD1Ev / _ZN5CFrogD2Ev
CFrog::~CFrog() {
    UnInit();
}

// mangled: _ZN5CFrog8MakePermEPhj
void CFrog::MakePerm(u1byte *ip, u4byte top) {
    u1byte ua[260];
    u4byte i;
    u4byte j;
    u4byte ie;
    u4byte ne;

    /* top==0 在二进制中会触发除零/越界（死代码，实际不调用），此处直接返回 */
    if (!top)
        return;

    for (i = 0; i < top; ++i)
        ua[i] = (u1byte)i;

    ie = 0;
    ne = top;

    for (i = 0; i < top - 1; ++i) {
        ie = (ie + ip[i]) % ne;
        ip[i] = ua[ie];
        ne--;
        for (j = ie; j < ne; ++j)
            ua[j] = ua[j + 1];
    }

    ip[top - 1] = ua[0];
}

// mangled: _ZN5CFrog7MakeKeyEPNS_7key_strE
void CFrog::MakeKey(key_str *kp) {
    u4byte i;
    u4byte j;
    u4byte k;
    u4byte ix;
    u4byte ll;
    u1byte ua[16];

    for (i = 0; i < 8; ++i) {
        MakePerm(kp->f_key[i].k_spu, 256);

        for (j = 0; j < 256; ++j)
            kp->i_key[i][kp->f_key[i].k_spu[j]] = (u1byte)j;

        MakePerm(kp->f_key[i].k_bpu, 16);

        for (j = 0; j < 16; ++j)
            ua[j] = 0;

        for (j = ix = 0; j < 15; ++j) {
            if (!kp->f_key[i].k_bpu[ix]) {
                k = ix;
                do {
                    k = (k + 1) & 15;
                } while (ua[k]);

                kp->f_key[i].k_bpu[ix] = (u1byte)k;
                ll = k;

                while (kp->f_key[i].k_bpu[ll] != k)
                    ll = kp->f_key[i].k_bpu[ll];

                kp->f_key[i].k_bpu[ll] = 0;
            }

            ua[ix] = 1;
            ix = kp->f_key[i].k_bpu[ix];
        }

        for (j = 0; j < 16; ++j)
            if (kp->f_key[i].k_bpu[j] == ((j + 1) & 15))
                kp->f_key[i].k_bpu[j] = (u1byte)((j + 2) & 15);
    }
}

// mangled: _ZN5CFrog6SetKeyEPjj
void CFrog::SetKey(u4byte *arg0, u4byte arg1) {
    u1byte kb[32];
    u4byte i;
    u4byte j;
    u4byte k;
    u4byte n;

    n = (arg1 > 0x107) ? 0x20 : (arg1 >> 3);
    memmove(kb, arg0, n);

    if (ik_len) {
        for (i = j = k = 0; i < ik_len; ++i) {
            ((u1byte *)(&sim_key))[i] = seed[j] ^ kb[k];
            j = (j < 0xfa ? j + 1 : 0);
            k = (k < n - 1 ? k + 1 : 0);
        }
    }

    MakeKey(&sim_key);
    lkp = &sim_key;
}

// mangled: _ZN5CFrog7EncryptEPjS0_
void CFrog::Encrypt(u4byte *inData, u4byte *outData) {
    u4byte i;
    u4byte k;
    u1byte blk[16];
    u1byte *xp;
    u1byte *sp;
    u1byte *pp;

    memmove(blk, inData, 16);

    for (i = 0; i < 8; ++i) {
        xp = lkp->f_key[i].k_xbu;
        sp = lkp->f_key[i].k_spu;
        pp = lkp->f_key[i].k_bpu;

        f_rnd(0); f_rnd(1); f_rnd(2); f_rnd(3);
        f_rnd(4); f_rnd(5); f_rnd(6); f_rnd(7);
        f_rnd(8); f_rnd(9); f_rnd(10); f_rnd(11);
        f_rnd(12); f_rnd(13); f_rnd(14); f_rnd(15);
    }

    memmove(outData, blk, 16);
}

// mangled: _ZN5CFrog7DecryptEPjS0_
void CFrog::Decrypt(u4byte *inData, u4byte *outData) {
    int i;
    u1byte blk[16];
    u1byte ct;
    u1byte *xp;
    u1byte *sp;
    u1byte *pp;

    memmove(blk, inData, 16);

    for (i = 7; i >= 0; --i) {
        xp = lkp->f_key[i].k_xbu;
        sp = lkp->i_key[i];
        pp = lkp->f_key[i].k_bpu;

        b_rnd(15); b_rnd(14); b_rnd(13); b_rnd(12);
        b_rnd(11); b_rnd(10); b_rnd(9); b_rnd(8);
        b_rnd(7); b_rnd(6); b_rnd(5); b_rnd(4);
        b_rnd(3); b_rnd(2); b_rnd(1); b_rnd(0);
    }

    memmove(outData, blk, 16);
}

// mangled: _ZN5CFrog4FrogEPhbS0_j
void CFrog::Frog(u1byte *key, bool benc, u1byte *buf, u4byte buflen) {
    u4byte blocks;
    u4byte i;

    if (!buflen)
        return;

    SetKey((u4byte *)key, 0x100);

    blocks = buflen >> 4;
    for (i = 0; i < blocks; i++) {
        if (benc)
            Encrypt((u4byte *)(buf + 16 * i), (u4byte *)(buf + 16 * i));
        else
            Decrypt((u4byte *)(buf + 16 * i), (u4byte *)(buf + 16 * i));
    }

    CreateTsLocal()->Seattos(key, benc, buf + blocks * 16,
                             buflen - blocks * 16);
}

// mangled: _Z10TenFrogPhbS_j
void TenFrog(unsigned char *buf, bool benc, unsigned char *key,
             unsigned int buflen) {
    CFrog frog;

    frog.Frog(key, benc, buf, buflen);
}
