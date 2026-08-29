// df_game_r SPOBJ_ACTION_DATAS 还原（由 CMap.h 移出，独立 TU）。
// 布局依据：ORIG ctor(089e86e4) = memset(this,0xff,0x18)（6 个 int 全 -1）、
// getActionType(089e8708) = mov (%eax),%eax 直返 m_type（非 const，返回 int）、
// D1(089f0b92) = 空体。sizeof = 0x18。
#pragma once

struct SPOBJ_ACTION_DATAS
{
    int m_type;    // +0x00
    int m_4;       // +0x04
    int m_8;       // +0x08
    int m_c;       // +0x0c
    int m_10;      // +0x10
    int m_14;      // +0x14

    SPOBJ_ACTION_DATAS();       // ORIG C1Ev W 0x089e86e4（memset 0xff 全字段 -1）
    ~SPOBJ_ACTION_DATAS();      // ORIG D1Ev W 0x089f0b92（空体）
    int getActionType();        // ORIG 0x089e8708（直返 m_type）
};
