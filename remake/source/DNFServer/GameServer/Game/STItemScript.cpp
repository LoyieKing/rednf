// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STItemScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r STItemScript 还原（2026-08-16）。
// 成员布局/赋值顺序以 ORIG Clear（0x898d718）逐条对照。

#include "STItemScript.h"

// ===================== SEXPERTJOB 嵌套结构构造（ORIG weak 0x849f34c / 0x850d0e8） =====================

#include "STItemScript/stExpertJobAdditionalExp.inc"
{
    m_field0 = 0.0f;
    m_field4 = 0;
}

#include "STItemScript/stExpertCompoundResultVariation.inc"
{
    m_field0 = 0.0f;
    m_field4 = 0.0f;
}

#include "STItemScript/STItemScript.inc"
{
    Clear();
}

#include "STItemScript/dtor_STItemScript.inc"
{
}

#include "STItemScript/Clear.inc"
{
    m_field0 = 0;
    m_field4 = 1;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = -1;
    m_pair28.first = -1;
    m_pair28.second = 0;
    m_field30 = 0;
    m_field31 = 0;
    m_field34 = 0;
    for (int i = 0; i < 0xb; ++i)
    {
        m_usable[i] = 0;
    }
    for (int i = 0; i < 0xb; ++i)
    {
        m_set44.insert(i);
    }
    m_field5c = 0;
    m_field60 = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_str6c = "";
    m_field70 = 0;
    m_str74 = "";
    m_field78 = 0;
    m_str84 = "";
    m_vec88.clear();
    m_vec94.clear();
    m_fielda0 = 0;
    m_fielda4 = 0;
    m_fielda8 = -1;
    m_strac = "";
    m_strb0 = "";
    m_strb4 = "";
    m_strb8 = "";
    m_strbc = "";
    m_strc0 = "";
    m_fieldc4 = -1;
    m_fieldc8 = 0;
    m_fieldcc = 0;
    m_fieldd0 = 0;
    m_fieldda = 0;
    m_fieldd9 = 0;
    m_fieldd1 = 0;
    m_setdc.clear();
    m_setdc.insert(0);
    m_fieldf4 = 0;
    m_fieldf8 = 0;
    m_fieldfc = 1;
    m_field100 = 0;
    m_field104 = 1;
    m_field10e = 0;
    m_field10f = 0;
    m_vec110.clear();
    m_set11c.clear();
    m_field134 = 0;
    m_expertJob.m_field18 = 0;
    m_expertJob.m_field1c = 0;
    m_expertJob.m_usable.clear();
    m_expertJob.m_variation.m_field4 = 0;
    m_expertJob.m_variation.m_field0 = 0;
    m_expertJob.m_additionalExp.m_field0 = 0;
    m_expertJob.m_additionalExp.m_field4 = 0;
    m_expertJob.m_field10 = 0;
    m_expertJob.m_field14 = 0;
    m_fieldd4 = 0;
    m_field80 = 0;
    m_dyeInfo.clear();
    m_str174.clear();
    m_fieldd8 = 0;
}

#include "STItemScript/hasCategory.inc"
{
    std::set<ENUM_ITEM_CATEGORY>::const_iterator it = m_set11c.find(category);
    return it != m_set11c.end();
}
