// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMCAPManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CMCAPManager.h"
#include <algorithm>

#include "GameTypes.h"   // cMyTrace

namespace WongWork
{
// ORIG W 0x8694fa0：+0x00=1、+0x04=1000、+0x08=+0x00（当前倍率拷贝）、
// +0x0c=1、+0x10=0、+0x14=0（逐 dword/byte 直写，无 memset）。
#include "CMCAPManager/reset.inc"
{
    m_point = 1;
    m_rate = 0x3e8;
    m_field08 = static_cast<int>(m_point);
    m_field0c = 1;
    m_field10 = 0;
    m_saveCleanPadPoint = false;
}

// ORIG T 0x0822eea4：max<unsigned>(min<unsigned>(10000, m_point), 0)
// ORIG W 0x08695038：m_field10(+0x10) += 1。
#include "CMCAPManager/incExposedCount.inc"
{
    m_field10 = m_field10 + 1;
}

// ORIG 仅非 const getPoint（W 0x0822eea4，读 this+0x00）；const 版为发明，已删。

// ORIG T 0x0822eeea：写 +0x14=0
#include "CMCAPManager/disableCleanPadPoint.inc"
{
    m_saveCleanPadPoint = false;
}

// ORIG T 0x080f8be0：trace 后把 0xf3c 个 int 拷贝到 pointPerHackType_。
#include "CMCAPManager/setPointPerHackType.inc"
{
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x1d2, 0);
    trace("Load cleanpad point per hack type");
    for (int i = 0; i <= 0xf3b; ++i)
        pointPerHackType_[i] = data[i];
}
}  // namespace WongWork
