/* tean.cpp -- TEA 变体（非标准：y 更新在 sum+=DELTA 之前，表达式为
   ((v<<4)^(v>>5)) + (v^sum) + k[idx]，加密先 y 后 z，解密镜像）
   反汇编确认：8 字节块 × ±32 轮；剩余块走 ITsLocal::ProcessLastBytes。 */

#include "inc/tean.h"
#include "include/TenCrypt.h"

static const unsigned int DELTA = 0x9e3779b9u;

// mangled: _ZN5CTean4teanEPjS0_i
void CTean::tean(unsigned int *k, unsigned int *v, int N) {
    unsigned int y;
    unsigned int z;
    unsigned int limit;
    unsigned int sum;

    y = v[0];
    z = v[1];
    if (N > 0) {
        limit = (unsigned int)(N * (int)DELTA);
        sum = 0;
        while (sum != limit) {
            y += ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum & 3];
            sum += DELTA;
            z += ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[(sum >> 11) & 3];
        }
    } else {
        sum = (unsigned int)(N * (int)0x61c88647u);
        while (sum != 0) {
            z -= ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[(sum >> 11) & 3];
            sum += DELTA;
            y -= ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[sum & 3];
        }
    }
    v[0] = y;
    v[1] = z;
}

// mangled: _ZN5CTean4TeanEPhbS0_j
void CTean::Tean(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        nLen = buflen / 8;
        for (i = 0; i < nLen; i++)
            tean((unsigned int *)key, (unsigned int *)(buf + i * 8), benc ? 32 : -32);
        pLocal = CreateTsLocal();
        pLocal->ProcessLastBytes(key, benc, buflen, 16, buf);
    }
}

// mangled: _ZN5CTean6UnInitEv
void CTean::UnInit() {
}

// mangled: _ZN5CTean4InitEPvS0_
void CTean::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
}

// mangled: _ZN5CTeanC1Ev / _ZN5CTeanC2Ev
CTean::CTean() {
    Init(0, 0);
}

// mangled: _ZN5CTeanD1Ev / _ZN5CTeanD2Ev
CTean::~CTean() {
    UnInit();
}
