// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventMailTypesC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "InGameEventScripts.h"   // ::Action_SendMail / Condition_LevelUp 权威布局

// ============ Action_SendMail C1/D1【报告缺失·objdump 转写】 ============
// C1 0x088e40ce：_ZNSt3mapIiSt4pairIiiESt4lessIiESaIS0_IKiS1_EEEC1Ev(this) →
//   _ZNSsC1Ev(this+0x30) → _ZNSsC1Ev(this+0x34) → _ZNSt6vectorIiSaIiEEC1Ev(this+0x38)。
//   忠实保留：+0x18..+0x2f 标量区（m_field18/m_field1c/m_itemIdx/m_addInfo/
//   m_gold/m_coin/pad）不初始化——由 reset()（0x080ec026）或各 import* 解析器写入。
// D1 0x088e4160：逆序析构 vector(+0x38) → string(+0x34) → string(+0x30) → map(+0x00)。
#include "EventMailTypesC2/Action_SendMail.inc"
    : m_jobMap(),
      m_field30(),
      m_field34(),
      m_vec38()
{
}

#include "EventMailTypesC2/dtor_Action_SendMail.inc"
{
}

// ============ Action_SendMail C1ERKS_/aSERKS_（ORIG W 0x08987b48/0x0898a922） ============
// [b28d-Lane SymSrvParam] 原 forceInstantiateActionSendMailCtors 已删：nm 实证
// 同名 W（C1ERKS/C2ERKS/aSERKS）由 EventScriptsCompleteB2.cpp.o 的
// std::pair<XxxRule, Action_SendMail> 实例化点自然发射，链接期 COMDAT 合并，
// 本 TU 无独立发射职责 ⇒ 删除补发函数不产生 only_orig。

// ============ Condition_LevelUp C1/D1【报告缺失·objdump 转写】 ============
// C1 0x0898605a：仅 _ZNSt6vectorIN17Condition_LevelUp11Reward_ItemsaIS1_EEC1Ev(this+4)；
//   忠实保留：m_filter0/m_filter1（+0x00/+0x01）不初始化（解析器写入；+1 字节为 growType 过滤）。
// D1 0x08986070：仅析构该 vector。
#include "EventMailTypesC2/Condition_LevelUp.inc"
    : m_items()
{
}

#include "EventMailTypesC2/dtor_Condition_LevelUp.inc"
{
}

// ============ Condition_LevelUp::Reward_Item C1/D1【报告缺失·objdump 转写】 ======
// C1 0x08986086：_ZNSsC1Ev(this+0xc) → _ZNSsC1Ev(this+0x10)；忠实保留：
//   m_level/m_itemIndex/m_itemCount（+0x00..+0x0b 三个 int）不初始化。
// D1 0x089860d2：逆序析构 string(+0x10) → string(+0xc)。
#include "EventMailTypesC2/Reward_Item.inc"
    : m_title(),
      m_content()
{
}

#include "EventMailTypesC2/dtor_Reward_Item.inc"
{
}
