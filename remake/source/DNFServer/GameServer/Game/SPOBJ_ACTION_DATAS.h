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
    // [b28d-Main 代落·S247 P317] 删用户空析构：ORIG 仅实例化平凡版
    // `_ZNSt12_Destroy_auxILb1EE9__destroyIP18SPOBJ_ACTION_DATASEEvT_S4_`（无 Lb0E）
    // ⇒ 类型 trivial；ImportMapScript 循环内 C1Ev 后无 D1 call。D1/D2 仍发射同名 W。
    int getActionType();        // ORIG 0x089e8708（直返 m_type）
};
