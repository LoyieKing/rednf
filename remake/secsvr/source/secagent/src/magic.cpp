/* magic.cpp -- CMagic8：Magic8/16/32 位级密码（tencrypt 复刻）
   与 secagent 二进制逐字节对照还原（2026-08-09）。

   本文件由反汇编机械转写（toolchain 外脚本 transcribe_magic.py）：
   MagicEc8/MagicDc8 为密钥位驱动的 64 位状态决策树（8 个密钥位，每位
   SET/CLEAR 两条置换路径），纯位操作无查表。函数体以栈槽（M[]）忠实镜像
   二进制帧布局，逐指令转写，语义与二进制一致（含 0/1 与掩码值转换、
   sbb/not 的 CF 语义、帧槽指针 P14C/P6C）。
   Magic8_8/16/32 包装：按 8/16/32 字节块循环 MagicEc8/MagicDc8（in-place），
   余数交 CreateTsLocal()->ProcessLastBytes(key,benc,buflen,块大小,buf)。
   TenMagic8/16/32 为直通包装。 */

#include "inc/magic.h"
#include "include/TenCrypt.h"

#include <string.h>
#include <stdio.h>
#include <stdio.h>

#define DW(M, d) (*(unsigned int *)&(M)[(d)])

// mangled: _ZN7CMagic810MagicEc8EPhjS0_S0_
void CMagic8::MagicEc8(unsigned char *key, unsigned int keylen,
                       unsigned char *buf, unsigned char *out) {
    unsigned char M[0x200];
    unsigned char *P14C = 0;
    unsigned char *P6C = 0;
    unsigned int ZF = 0, SF = 0, CF = 0;
    struct {
        unsigned int u;
        unsigned char *p;
    } EAX, ECX, EDX, ESI, EDI;
    EAX.u = ECX.u = EDX.u = ESI.u = EDI.u = 0;
    EAX.p = ECX.p = EDX.p = ESI.p = EDI.p = 0;
    memset(M, 0, sizeof(M));
    (void)P14C;
    (void)P6C;
#include "magic_ec_body.inc"
}

// mangled: _ZN7CMagic810MagicDc8EPhjS0_S0_
void CMagic8::MagicDc8(unsigned char *key, unsigned int keylen,
                       unsigned char *buf, unsigned char *out) {
    unsigned char M[0x200];
    unsigned char *P14C = 0;
    unsigned char *P6C = 0;
    unsigned int ZF = 0, SF = 0, CF = 0;
    struct {
        unsigned int u;
        unsigned char *p;
    } EAX, ECX, EDX, ESI, EDI;
    EAX.u = ECX.u = EDX.u = ESI.u = EDI.u = 0;
    EAX.p = ECX.p = EDX.p = ESI.p = EDI.p = 0;
    memset(M, 0, sizeof(M));
    (void)P14C;
    (void)P6C;
#include "magic_dc_body.inc"
}

// mangled: _ZN7CMagic8C1Ev / _ZN7CMagic8C2Ev
CMagic8::CMagic8() {
    Init(0, 0);
}

// mangled: _ZN7CMagic8D1Ev / _ZN7CMagic8D2Ev
CMagic8::~CMagic8() {
    UnInit();
}

// mangled: _ZN7CMagic84InitEPvS0_
void CMagic8::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN7CMagic86UnInitEv
void CMagic8::UnInit() {
}

// mangled: _ZN7CMagic88Magic8_8EPhbS0_j
void CMagic8::Magic8_8(unsigned char *key, bool benc, unsigned char *buf,
                       unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++) {
        if (benc)
            MagicEc8(key, 8, buf + i * 8, buf + i * 8);
        else
            MagicDc8(key, 8, buf + i * 8, buf + i * 8);
    }
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 8, buf);
}

// mangled: _ZN7CMagic88Magic8_16EPhbS0_j
void CMagic8::Magic8_16(unsigned char *key, bool benc, unsigned char *buf,
                        unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++) {
        if (benc)
            MagicEc8(key, 16, buf + i * 8, buf + i * 8);
        else
            MagicDc8(key, 16, buf + i * 8, buf + i * 8);
    }
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 16, buf);
}

// mangled: _ZN7CMagic88Magic8_32EPhbS0_j
void CMagic8::Magic8_32(unsigned char *key, bool benc, unsigned char *buf,
                        unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (!buflen)
        return;
    nLen = buflen / 8;
    for (i = 0; i < nLen; i++) {
        if (benc)
            MagicEc8(key, 32, buf + i * 8, buf + i * 8);
        else
            MagicDc8(key, 32, buf + i * 8, buf + i * 8);
    }
    pLocal = CreateTsLocal();
    pLocal->ProcessLastBytes(key, benc, buflen, 32, buf);
}

// mangled: _Z9TenMagic8PhbS_j
void TenMagic8(unsigned char *key, bool benc, unsigned char *buf,
               unsigned int buflen) {
    CMagic8 magic;

    magic.Magic8_8(key, benc, buf, buflen);
}

// mangled: _Z10TenMagic16PhbS_j
void TenMagic16(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CMagic8 magic;

    magic.Magic8_16(key, benc, buf, buflen);
}

// mangled: _Z10TenMagic32PhbS_j
void TenMagic32(unsigned char *key, bool benc, unsigned char *buf,
                unsigned int buflen) {
    CMagic8 magic;

    magic.Magic8_32(key, benc, buf, buflen);
}
