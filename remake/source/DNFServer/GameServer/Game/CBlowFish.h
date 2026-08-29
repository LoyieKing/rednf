// CBlowFish：df_game_r ORIG BlowFish 分组加密类还原。
// 依据 docs/class_func_reports/CBlowFish/（7 函数 + F 盒 objdump 实证）：
//   C1EPhjRK6SBlock 0x080a956c / EncryptER6SBlock 0x080a97a4 /
//   DecryptER6SBlock 0x080a99b0 / EncryptEPhji 0x080a9bbc /
//   DecryptEPhji 0x080a9d5e / EncryptEPKhPhji 0x080a9f4e /
//   DecryptEPKhPhji 0x080aa0fc / FEj(W) 0x080aa34a。
// 布局（ctor objdump 实证）：m_left SBlock @+0x00、m_right SBlock @+0x08
//   （均以 iv 拷贝初始化；缓冲版 mode 1/2 用 m_right 做 CBC 链接寄存器）、
//   m_pbox[18] @+0x10（0x48）、m_sbox[4][256] @+0x58（0x1000），sizeof=0x1058。
// scm_auiInitP @0x08b16540 / scm_auiInitS @0x08b165a0：标准 BlowFish 初始
//   P 数组与 S 盒（.rodata 实测，首值 0x243f6a88 / 0xd1310ba6）。
#ifndef DNF_GAME_CBLOWFISH_H_
#define DNF_GAME_CBLOWFISH_H_

#include "SBlock.h"

class CBlowFish
{
public:
    CBlowFish(unsigned char* key, unsigned int keyLen, SBlock const& iv);

    unsigned int F(unsigned int x);          // ORIG W 0x080aa34a

    void Encrypt(SBlock& block);             // ORIG T 0x080a97a4
    void Decrypt(SBlock& block);             // ORIG T 0x080a99b0

    int Encrypt(unsigned char* buf, unsigned int len, int mode);   // 0x080a9bbc
    int Decrypt(unsigned char* buf, unsigned int len, int mode);   // 0x080a9d5e
    int Encrypt(unsigned char const* in, unsigned char* out,
                unsigned int len, int mode);                       // 0x080a9f4e
    int Decrypt(unsigned char const* in, unsigned char* out,
                unsigned int len, int mode);                       // 0x080aa0fc

private:
    SBlock m_left;                  // +0x00
    SBlock m_right;                 // +0x08（CBC 链接寄存器）
    unsigned int m_pbox[18];        // +0x10
    unsigned int m_sbox[4][256];    // +0x58

    static unsigned int const scm_auiInitP[18];    // R 0x08b16540
    static unsigned int const scm_auiInitS[1024];  // R 0x08b165a0
};

#endif  // DNF_GAME_CBLOWFISH_H_
