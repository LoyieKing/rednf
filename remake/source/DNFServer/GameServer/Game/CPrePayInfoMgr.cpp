// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPrePayInfoMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPrePayInfoMgr（LinkE1AB 批，E1-remaining B 组）。
// 符号：
//   CPrePayInfoMgr::CPrePayInfoMgr()        W 0x08231ede（map 构造 + clear）
//   CPrePayInfoMgr::InsertPrePayInfo        W 0x084ed0bc（m_map.insert(make_pair(...))）
//   G_CPrePayInfoMgr()                      W 0x08231f20（GlobalInstance<CPrePayInfoMgr>::inst_ptr）

#include "CPrePayInfoMgr.h"

#include "CEnvironment.h"   // GlobalInstance<T>（全局命名空间单例模板，CPrePayInfoMgr.h 同 TU 集）

#include "CPrePayInfoMgr/CPrePayInfoMgr.inc"
{
    m_map.clear();
}

#include "CPrePayInfoMgr/InsertPrePayInfo.inc"
{
    m_map.insert(std::make_pair(characNo, info));
}

#include "CPrePayInfoMgr/DeletePrePayInfo.inc"
{
    m_map.erase(idx);
}

#include "CPrePayInfoMgr/FindPrePayInfo.inc"
{
    // P28：ORIG 先默认构造迭代器（-0x10 槽）再 find 到临时（-0x1c）拷贝回来
    // （帧 0x38）；一行直构被 4.4 copy-elision 折叠（帧 0x28）。
    std::map<int, PrePayInfo>::iterator it;
    it = m_map.find(idx);
    if (it != m_map.end())
        return &it->second;
    return 0;
}

CPrePayInfoMgr* G_CPrePayInfoMgr()
{
    return GlobalInstance<CPrePayInfoMgr>::inst_ptr();
}
