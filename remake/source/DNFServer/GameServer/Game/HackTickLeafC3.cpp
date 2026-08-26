// HackTickLeafC3.cpp —— df_game_r 还原：hack 计数回调叶子 + 地图游玩 tick。
// 独立叶子 TU（Main 登记 CMake）；逐函数对照 ORIG df_game_r 反汇编实现。
//
// 恢复函数与证据源：
//   1. addUserHackCount149(CUser*)        ORIG T 0x830dc9d
//      报告：docs/class_func_reports/_lt_global_gt_/addUserHackCount149.md
//      （## 汇编 + ## 反编译 C 已完整读取）
//   2. addUserHackCount150(CUser*)        ORIG T 0x830dce4
//      报告：docs/class_func_reports/_lt_global_gt_/addUserHackCount150.md
//      （## 汇编 + ## 反编译 C 已完整读取）
//   3. stMapPlayInfo_t::getPlayTick()       ORIG W 0x830e7ea（ORIG 非 const）
//      【无独立报告——objdump 直接转写并标记】：
//        mov 0x4(%eax),%edx   ; this+0x04 = m_clearTick
//        mov (%eax),%eax      ; this+0x00 = m_startTick
//        sub %eax,%edx → 返回 edx-eax（无符号）
//      布局佐证：WarField.h stMapPlayInfo_t{m_startTick +0x00, m_clearTick +0x04}；
//      同类佐证：FieldSmallStructs.cpp init() 清零同四字段；调用方语义
//      CBattle_Field/onClearMap（m_clearTick=now 后取游玩时长）一致。
//
// ODR 说明：Game/CBattle_Field_helpers.cpp（未入 CMake 孤儿 TU）含本三符号重复
// 定义；本 TU 登记后不得同时登记该文件（或须先删其重复段）。

#include "WarField.h"
#include "CUser.h"

// ============================================================================
// addUserHackCount149(CUser*) —— ORIG T 0x830dc9d [_Z19addUserHackCount149P5CUser]
// 报告反编译：
//   pCVar1 = CUser::getHackAnalyzer(param_1);
//   addServerHackCnt(pCVar1, param_1, 0x198, 1, 0, 0);
//   return 1;
// 实参序（汇编压栈）：type=0x198(ENUM_HACKTYPE), count=1, a=0, b=0。
// ============================================================================
bool addUserHackCount149(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)0x198, 1u, 0u, 0u);
    return true;
}

// ============================================================================
// addUserHackCount150(CUser*) —— ORIG T 0x830dce4 [_Z19addUserHackCount150P5CUser]
// 同 149，仅 hack type=0x199。
// ============================================================================
bool addUserHackCount150(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)0x199, 1u, 0u, 0u);
    return true;
}

// ============================================================================
// stMapPlayInfo_t::getPlayTick() —— ORIG W 0x830e7ea
// [_ZN15stMapPlayInfo_t11getPlayTickEv]（objdump 转写，见文件头标记；ORIG 非 const）
//   return m_clearTick - m_startTick;（unsigned 减法回绕）
// ============================================================================
unsigned int stMapPlayInfo_t::getPlayTick()
{
    return m_clearTick - m_startTick;
}
