// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 randomMessageBalloonInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "randomMessageBalloonInfo.h"

// ORIG 0x08a00c08：string 默认构造 → vector 默认构造 → clear()（带 unwind 清理路径）
#include "randomMessageBalloonInfo/randomMessageBalloonInfo.inc"
    : m_randomMessageAnimation(),
      m_randomMessageList()
{
    clear();
}

// ORIG 0x08a00bd2：+0x04 置 0 + vector::clear + vector::reserve(0xa)
#include "randomMessageBalloonInfo/clear.inc"
{
    m_randomMessageProbability = 0;
    m_randomMessageList.clear();
    m_randomMessageList.reserve(10);
}

// [AnonLane2 2026-09-11 试形-回滚] 曾删本垫片：全局逐符号 verdict 全等
// （D1 AE→NPCScript.cpp:AE），但本 TU 四数 13/30/0/0/1 → 13/29/0/0/0（ae −1）
// ⇒ 按 TU 口径回滚，判词保留。
// 强制实例化隐式析构（D1/D0），使 ORIG 弱符号 _ZN24randomMessageBalloonInfoD1Ev
// 在链接期可得；函数本身为内部链接，不产生对外符号。
namespace
{
#include "randomMessageBalloonInfo/force_randomMessageBalloonInfo_dtor.inc"
{
    delete p;
}
}
