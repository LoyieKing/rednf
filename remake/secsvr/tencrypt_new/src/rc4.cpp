/* rc4.cpp -- RC4 流密码（标准 RC4，Ron Rivest）
   由 DWARF 桩重建：签名/局部变量名与二进制一致。
   反汇编确认：Rc4() 固定 16 字节密钥调用 set_key()，buflen==0 时直接返回。 */

#include "inc/rc4.h"

#include <string.h>

// mangled: _ZN4CRc47set_keyEPhj
void CRc4::set_key(unsigned char *key_data_ptr, unsigned int key_data_len) {
    unsigned char t;
    unsigned char index1;
    unsigned char index2;
    unsigned char *state;
    unsigned short counter;

    state = key->state;
    for (counter = 0; counter < 256; counter++)
        state[counter] = (unsigned char)counter;
    key->x = 0;
    key->y = 0;
    index1 = 0;
    index2 = 0;
    for (counter = 0; counter < 256; counter++) {
        index2 = (unsigned char)(key_data_ptr[index1] + state[counter] + index2);
        t = state[counter];
        state[counter] = state[index2];
        state[index2] = t;
        index1 = (unsigned char)((index1 + 1) % key_data_len);
    }
}

// mangled: _ZN4CRc45cryptEPhj
void CRc4::crypt(unsigned char *buffer_ptr, unsigned int buffer_len) {
    unsigned char t;
    unsigned char x;
    unsigned char y;
    unsigned char *state;
    unsigned int counter;

    state = key->state;
    x = key->x;
    y = key->y;
    for (counter = 0; counter < buffer_len; counter++) {
        x = (unsigned char)(x + 1);
        t = state[x];
        y = (unsigned char)(y + t);
        state[x] = state[y];
        state[y] = t;
        buffer_ptr[counter] ^= state[(unsigned char)(t + state[x])];
    }
    key->x = x;
    key->y = y;
}

// mangled: _ZN4CRc43Rc4EPhbS0_j
void CRc4::Rc4(unsigned char *key_orign, bool benc, unsigned char *buf, unsigned int buflen) {
    if (buflen != 0) {
        set_key(key_orign, 16);
        crypt(buf, buflen);
    }
}

// mangled: _ZN4CRc46UnInitEv
void CRc4::UnInit() {
    if (key != 0) {
        delete[] key;
        key = 0;
    }
}

// mangled: _ZN4CRc44InitEPvS0_
void CRc4::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    key = 0;
    m_pLocal = pLocal;
    key = new rc4_key;
    memset(key, 0, sizeof(rc4_key));
}

// mangled: _ZN4CRc4C1Ev / _ZN4CRc4C2Ev（-fabi-version=2 自动生成双符号）
CRc4::CRc4() {
    Init(0, 0);
}

// mangled: _ZN4CRc4D1Ev / _ZN4CRc4D2Ev
CRc4::~CRc4() {
    UnInit();
}

// mangled: _Z6TenRc4PhbS_j
void TenRc4(unsigned char *key, bool benc, unsigned char *buf,
            unsigned int buflen) {
    CRc4 rc4;

    rc4.Rc4(key, benc, buf, buflen);
}
