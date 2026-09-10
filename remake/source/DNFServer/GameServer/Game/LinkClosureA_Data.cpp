// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureA_Data/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"

#include "LinkClosureA_Data/GetPvPMode.inc"
{
    return m_pvpChannel.m_field1c;      // [objdump 0x844d4b4 mov 0xb44c(%eax)]
}

#include "LinkClosureA_Data/GetPvPTeamCount.inc"
{
    return m_pvpChannel.m_field20;      // [objdump 0x8568ce6 mov 0xb450(%eax)]
}

// ==== QuestParameterScript 三 getter（ORIG T 0x08a6bd1c/0x08a6bd86/0x08a6bda6）====
// getRewardWeight：map<char,int> find，未命中返 100；exp/gold：vector::at 抛越界。
#include "LinkClosureA_Data/getRewardWeight.inc"
{
    // [b28c-L29] ORIG 0x08a6bd1c：find 结果直接落 it 槽（无 const_iterator
    // 转换构造调用），ne/ptEv 均为迭代器本型——用非 const iterator。
    std::map<char, int>::iterator it = m_map0.find(level);
    if (it != m_map0.end())
        return it->second;
    return 100;
}

#include "LinkClosureA_Data/getRewardExp.inc"
{
    return m_vec24.at(idx);
}

#include "LinkClosureA_Data/getRewardGold.inc"
{
    return m_vec30.at(idx);
}
