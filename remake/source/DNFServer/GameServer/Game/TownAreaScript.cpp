// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TownAreaScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "TownAreaScript.h"

// [b28-L14] ORIG C1Ev 0x86d1d9a 只构造 m_mapName/m_rects/m_field18 后调 clear()
// （无标量清零）；C1ERKS_/aSERKS_ 为 W 隐式合成（P74），用户定义已删。
#include "TownAreaScript/TownAreaScript.inc"
{
    clear();
}

// [b28-L14] P74/P26：ORIG C1ERKS_ 0x86d296a / aSERKS_ 0x86d2a1a 是编译器隐式合成
// （逐成员、跳过尾部对齐空洞），`= default` 外联定义既保持 W 符号发射又不再搬 pad。
#include "TownAreaScript/TownAreaScript_1.inc"
TownAreaScript& TownAreaScript::operator=(const TownAreaScript& other) = default;

#include "TownAreaScript/dtor_TownAreaScript.inc"
{
}

#include "TownAreaScript/clear.inc"
{
    m_areaIndex = -1;
    m_mapName = "";
    m_areaType = 0;
    m_rects.clear();
    m_field18.clear();
    m_field24 = 1;
}
