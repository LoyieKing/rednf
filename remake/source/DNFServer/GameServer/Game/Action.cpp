// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Action/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Action.cpp — StackableAction::Action 独占实现（run/check/~Action）。
// ORIG: check 0x0827fa26 / run 0x0827fa9a / D1 0x0827f760。
// 类定义在 Action.h（共享头，BingoEventLink4.cpp include）；GlobalData.cpp 持有
// 该 TU 内私有同布局视图（含 C1/proc_*/init_function_type 定义），本文件不触碰。
// check 查 m_map1（+0x00）、run 查 m_map2（+0x18）：find(type) → 未命中返 0，
// 命中则调 boost::function operator()(p)。
#include "Action.h"

namespace StackableAction
{

#include "Action/check.inc"
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map1.find(type);
    if (it != m_map1.end())
    {
        return it->second(p);
    }
    return 0;
}

#include "Action/run.inc"
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map2.find(type);
    if (it != m_map2.end())
    {
        return it->second(p);
    }
    return 0;
}

#include "Action/dtor_Action.inc"
{
}

}  // namespace StackableAction
