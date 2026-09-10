// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarbleUserInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarbleUserInfo（G2-4 大富翁子对象）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleUserInfo.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// BuffInfo/TileIndexInfo 为跨类依赖；本 TU 直接按 ORIG C1/reset 语义清零字段，
// 不调用 BuffInfo.h（0x10 与 ORIG 0xc 布局分歧）/TileIndexInfo 的 reset 符号
// （避免跨 TU 重复定义与尺寸错配），§9 落地。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN18BlueMarbleUserInfoC2Ev`（ORIG 0x80d9e63）；ctor 已是首强符号定义。
#include <iostream>

#include "BlueMarbleUserInfo.h"

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// BlueMarbleScriptManager 最大 buff 值位于 +0x64（ORIG setExp/setGold/setDrop 直读）。
namespace
{
struct BM_ScriptMaxBuff
{
    char m_pad[0x64];
    int m_maxBuffValue;  // +0x64
};
}

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// ORIG 0x080d6a90：标量按声明序 init-list 直写 → 成员 C1 call(+0x10/+0x1c)
// → +0x24 直写 → 显式 reset() 两 call（C1 后二次 reset 语义照抄）。
#include "BlueMarbleUserInfo/BlueMarbleUserInfo.inc"
    : m_user(0),
      m_userState((BlueMarbleUserState::T)0),
      m_boardZone(1),
      m_grade((BlueMarbleUserGrade::T)4),
      m_buff(),
      m_tileIndex(),
      m_pScript(0)
{
    m_buff.reset();
    m_tileIndex.reset();
}

#include "BlueMarbleUserInfo/dtor_BlueMarbleUserInfo.inc"
{
}

#include "BlueMarbleUserInfo/resetBlueMarbleUserInfo.inc"
{
    m_user = 0;
    m_userState = (BlueMarbleUserState::T)0;
    m_boardZone = 1;
    m_grade = (BlueMarbleUserGrade::T)4;
    resetBuff();   // ORIG BuffInfo::reset（m_exp=m_gold=m_drop=1）
    resetIndex();
}
#include "BlueMarbleUserInfo/resetBuff.inc"
{
    m_buff.reset();   // ORIG BuffInfo::reset（定义在 BuffInfo.cpp）
}

#include "BlueMarbleUserInfo/resetIndex.inc"
{
    m_tileIndex.reset();   // ORIG TileIndexInfo::reset
}

#include "BlueMarbleUserInfo/setUser.inc"
{
    m_user = user;
}

#include "BlueMarbleUserInfo/getUser.inc"
{
    return m_user;
}

#include "BlueMarbleUserInfo/setScript.inc"
{
    m_pScript = script;
}

#include "BlueMarbleUserInfo/setUserState.inc"
{
    m_userState = state;
}

#include "BlueMarbleUserInfo/getUserState.inc"
{
    return m_userState;
}

#include "BlueMarbleUserInfo/setBoardZone.inc"
{
    if (m_boardZone + zone >= pos)
    {
        m_boardZone = pos;
    }
    else if (m_boardZone + zone <= 1)
    {
        m_boardZone = 1;
    }
    else
    {
        m_boardZone = m_boardZone + zone;
    }
}

#include "BlueMarbleUserInfo/getBoardZone.inc"
{
    return m_boardZone;
}

#include "BlueMarbleUserInfo/setGrade.inc"
{
    m_grade = grade;
}

#include "BlueMarbleUserInfo/getGrade.inc"
{
    return m_grade;
}

#include "BlueMarbleUserInfo/setDungeon.inc"
{
    m_tileIndex.m_dungeon = dungeon;
}

#include "BlueMarbleUserInfo/getDungeon.inc"
{
    return m_tileIndex.m_dungeon;
}

#include "BlueMarbleUserInfo/setItem.inc"
{
    m_tileIndex.m_item = itemIdx;
}

#include "BlueMarbleUserInfo/getItem.inc"
{
    return m_tileIndex.m_item;
}

#include "BlueMarbleUserInfo/setExp.inc"
{
    if (m_buff.m_exp * exp >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_exp = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_exp = m_buff.m_exp * exp;
    }
}

#include "BlueMarbleUserInfo/getExp.inc"
{
    return m_buff.m_exp;
}

#include "BlueMarbleUserInfo/setGold.inc"
{
    if (m_buff.m_gold * gold >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_gold = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_gold = m_buff.m_gold * gold;
    }
}

#include "BlueMarbleUserInfo/getGold.inc"
{
    return m_buff.m_gold;
}

#include "BlueMarbleUserInfo/setDrop.inc"
{
    if (m_buff.m_drop * drop >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_drop = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_drop = m_buff.m_drop * drop;
    }
}

#include "BlueMarbleUserInfo/getDrop.inc"
{
    return m_buff.m_drop;
}
