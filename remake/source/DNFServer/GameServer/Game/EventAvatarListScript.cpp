// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventAvatarListScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventAvatarListScript 实现。
// ORIG：C1 W 0x8372e10（set 构造 + clear()）；clear W 0x8372e52（set::clear）。
#include "EventAvatarListScript.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；isfindEventAvatar() 前移为本 TU 首个
// 强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZN21EventAvatarListScript17isfindEventAvatarEi`
// （ORIG 0x896239d）。零语义/布局改动。
#include <iostream>

#include "EventAvatarListScript/isfindEventAvatar.inc"
{
    if (m_avatars.empty())
        return false;
    std::set<int>::const_iterator it = m_avatars.find(avatar_id);
    if (it != m_avatars.end())
        return true;
    return false;
}

#include "EventAvatarListScript/EventAvatarListScript.inc"
{
    clear();
}

#include "EventAvatarListScript/dtor_EventAvatarListScript.inc"

#include "EventAvatarListScript/clear.inc"
{
    m_avatars.clear();
}

