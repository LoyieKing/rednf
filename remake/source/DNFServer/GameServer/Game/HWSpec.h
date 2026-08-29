#ifndef GAME_HWSPEC_H_
#define GAME_HWSPEC_H_

// LinkHelper 核销批次：ORIG _ZN6HWSpecC1Ev W @0x0822b890（ctor: +0=0xff,
// +4=0xffffffff, +8=0xffffffff）；Packet_Client_Spec_Statistic 内嵌 0xfe× stride 0xc。
// sizeof=0xc（成员名沿用 HWSpecResearcher.cpp 现有定义）。
class HWSpec
{
public:
    HWSpec();

    char          m_category1;   // +0x00（ctor 置 0xff）
    char          m_pad1[3];     // +0x01..+0x03
    unsigned int  m_category2;   // +0x04（ctor 置 0xffffffff）
    unsigned int  m_category3;   // +0x08（ctor 置 0xffffffff）
};

#endif  // GAME_HWSPEC_H_
