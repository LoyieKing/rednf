/* shift.cpp -- Shift 自定义流式密码（由二进制反汇编还原）
   SetshiftKey：按 FUCKING_SHIFTIP[64]+FUCKING_SHIFTBBIT 对密钥做 64 位比特
   置换（BBIT 为特殊位掩码：{0x81,0x40,0x22,0x10,0x0a,0x04,0x02,0x01}）。
   shiftCrypt 内部两处查表均用 FUCKING_SHIFTBB（标准位掩码 1<<k）。
   shiftCrypt(k8, buf, benc) 逐 8 字节块：
   - local[8]：benc 时按 IP+SHIFTBB 对 buf 做比特收集；dec 时 local=buf[0..7]
   - buf ^= k8（随后被覆盖，无实际效果）
   - enc：buf = local 后直接返回（不执行展开循环）
   - dec：buf 清零后，对 i=0..63 若 local 第 i 位为 1，
     buf[IP[i]>>3] |= SHIFTBB[IP[i]&7]（按 IP 展开回写）
   Shift 包装：SetKey + 8 字节块 shiftCrypt + TsLocal 余数。
   验证：uni_call 调二进制 SetshiftKey/shiftCrypt/Shift 逐字节一致。 */

#include "inc/shift.h"
#include "include/TenCrypt.h"

#include <string.h>

const unsigned char CShift::FUCKING_SHIFTBBIT[8] = {
    0x81, 0x40, 0x22, 0x10, 0x0a, 0x04, 0x02, 0x01
};
const unsigned char CShift::FUCKING_SHIFTBB[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};
const unsigned char CShift::FUCKING_SHIFTIP[64] = {
    0x3c, 0x05, 0x36, 0x2e, 0x01, 0x1e, 0x04, 0x0e,
    0x07, 0x18, 0x10, 0x0b, 0x27, 0x39, 0x31, 0x29,
    0x3b, 0x13, 0x08, 0x3e, 0x0d, 0x1f, 0x00, 0x2f,
    0x37, 0x3f, 0x1c, 0x14, 0x0c, 0x16, 0x1b, 0x03,
    0x2c, 0x24, 0x38, 0x33, 0x2b, 0x23, 0x0f, 0x17,
    0x06, 0x3d, 0x35, 0x2d, 0x25, 0x1d, 0x15, 0x34,
    0x11, 0x09, 0x26, 0x3a, 0x32, 0x2a, 0x0a, 0x02,
    0x22, 0x1a, 0x30, 0x28, 0x20, 0x12, 0x21, 0x19
};

// mangled: _ZN6CShift11SetshiftKeyEPhS0_
void CShift::SetshiftKey(unsigned char *key, unsigned char *out) {
    unsigned int i;

    memset(out, 0, 8);
    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_SHIFTIP[i];
        unsigned char bit = FUCKING_SHIFTBBIT[ip & 7];
        unsigned char byte = key[ip >> 3];
        unsigned char v = (byte & bit) ? 1 : 0;

        out[i >> 3] |= (unsigned char)(v << (i & 7));
    }
}

// mangled: _ZN6CShift10shiftCryptEPhS0_b
void CShift::shiftCrypt(unsigned char *key, unsigned char *buf, bool benc) {
    unsigned char local[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int i;

    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_SHIFTIP[i];

        if (benc) {
        if (buf[ip >> 3] & FUCKING_SHIFTBB[ip & 7])
                local[i >> 3] |= (unsigned char)(1 << (i & 7));
        } else {
            if (i < 8)
                local[i] = buf[i];
        }
    }

    for (i = 0; i < 8; i++)
        buf[i] = (unsigned char)(buf[i] ^ key[i]);

    if (benc) {
        for (i = 0; i < 8; i++)
            buf[i] = local[i];
        return;
    }

    for (i = 0; i < 8; i++)
        buf[i] = 0;

    for (i = 0; i < 64; i++) {
        unsigned int ip = FUCKING_SHIFTIP[i];

        if ((local[i >> 3] >> (i & 7)) & 1)
            buf[ip >> 3] |= FUCKING_SHIFTBB[ip & 7];
    }
}

// mangled: _ZN6CShift4InitEPvS0_
void CShift::Init(void *arg0, void *arg1) {
    m_pSys = arg0;
    m_pLocal = arg1;
}

// mangled: _ZN6CShift6UnInitEv
void CShift::UnInit() {
}

// mangled: _ZN6CShiftC1Ev / _ZN6CShiftC2Ev
CShift::CShift() {
    Init(0, 0);
}

// mangled: _ZN6CShiftD1Ev / _ZN6CShiftD2Ev
CShift::~CShift() {
    UnInit();
}

// mangled: _ZN6CShift5ShiftEPhbS0_j
void CShift::Shift(unsigned char *key, bool benc, unsigned char *buf,
                   unsigned int buflen) {
    unsigned char k8[8];
    unsigned int blocks;
    unsigned int i;

    if (!buflen)
        return;

    SetshiftKey(key, k8);

    blocks = buflen >> 3;
    for (i = 0; i < blocks; i++)
        shiftCrypt(k8, buf + 8 * i, benc);

    CreateTsLocal()->Seattos(key, benc, buf + blocks * 8,
                             buflen - blocks * 8);
}

// mangled: _Z8TenShiftPhbS_j
void TenShift(unsigned char *buf, bool benc, unsigned char *key,
              unsigned int buflen) {
    CShift shift;

    shift.Shift(key, benc, buf, buflen);
}
