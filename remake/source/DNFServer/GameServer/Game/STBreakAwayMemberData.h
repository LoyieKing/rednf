// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STBreakAwayMemberData：ORIG 仅导出 clear() W @0x0844d584；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STBREAKAWAYMEMBERDATA_H_
#define DNF_GAME_STBREAKAWAYMEMBERDATA_H_

// ORIG clear 0x0844d584 逐偏移：+0=0、memset(+4,0,0x1f)、+0x24..+0x3c 七个 int=0、
// +0x40 byte=0；sizeof 0x41（DB_Login.cpp OFF_BREAKAWAY_REC=0x3f18 消费）。
struct STBreakAwayMemberData
{
    int m_field00;           // +0x00
    char m_name[0x1f];       // +0x04（memset 0x1f）
    int m_field24;           // +0x24
    int m_field28;           // +0x28
    int m_field2c;           // +0x2c
    int m_field30;           // +0x30
    int m_field34;           // +0x34
    int m_field38;           // +0x38
    int m_field3c;           // +0x3c
    char m_field40;          // +0x40

    void clear();  // ORIG W @0x0844d584
};

#endif  // DNF_GAME_STBREAKAWAYMEMBERDATA_H_
