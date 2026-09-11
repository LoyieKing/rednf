// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventLevelupA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CEventScriptMng.h"           // InGameEventScriptLoader/CLevelUpReward 声明
#include "InGameEventScripts.h"        // Action_SendMail/Condition_LevelUp/各脚本类
#include "LevelupSupportEventManger.h" // LevelupSupport(2nd)EventManger 及脚本解析器

// [T2-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit（ORIG 实有符号），
// 使本 TU 发射 _GLOBAL__I_（ORIG CU global_LevelupSupport.cpp 的键名
// `_ZN25LevelupSupportEventMangerC2Ev` = 本 TU 首个强外部符号）。零语义/布局改动。
#include <iostream>

// ============ LevelupSupportEventManger C1/D1（报告转写） ============
// 报告：Singleton 空 base → ScriptInterface base → 写 vptr →
// LevelupSupportSciprt(+0x04) → map<int,int>(+0x1c) → vector<EventInfo>(+0x34)。
// D1：写 vptr → 析构 vector → map → LevelupSupportSciprt。
// 注：ORIG C1 另有一个空基构造调用 _ZN4ARAD9SingletonI25…EC1Ev（W 0x08147b56，
// 体=push ebp/mov/pop ebp/ret）。本仓 Singleton<T> 主模板无用户构造（隐式平凡，
// g++ 不产生该调用），且 g++4.4 禁止特化隐式声明的特殊成员函数——为不污染
// 共享模板（Arad_DataManager 等 ORIG 无对应实例符号），此处接受该单条
// 基构造调用的指令形态差异，语义完全一致（空操作）。

#include "EventLevelupA2/LevelupSupportEventManger.inc"


#include "EventLevelupA2/dtor_LevelupSupportEventManger.inc"


#include "EventLevelupA2/LevelupSupport2ndEventManger.inc"


#include "EventLevelupA2/dtor_LevelupSupport2ndEventManger.inc"


#include "EventLevelupA2/InGameEventScript.inc"


#include "EventLevelupA2/dtor_InGameEventScript.inc"


#include "EventLevelupA2/reset.inc"
{
    m_jobMap.clear();
    m_field18 = 0;
    m_field1c = 0;
    m_addInfo = 0;
    m_itemIdx = 0;
    m_gold = 0;
    m_coin = 0;          // ORIG 仅清 +0x2c 低字节（movb），字段即按字节建模
    m_field30.clear();
    m_field34.clear();
}

#include "EventLevelupA2/set_item.inc"
{
    m_itemIdx = itemIndex;
    m_addInfo = addInfo;
}

namespace EventClassify
{

// 共同查找路径（两报告一致）：this->m_script1c(+0x1c) 为 LevelUpRewardScript*
// （事件记录类型 0x194 实体专用）；脚本 +0x10 为 map<int,Condition_LevelUp>，
// find 键=job；命中项 value 基址 +5 字节（即 m_filter1）为 growType 过滤。

#include "EventLevelupA2/check_recv_level.inc"


#include "EventLevelupA2/set_reward_item.inc"


}  // namespace EventClassify

#include "EventLevelupA2/importInGameEventScript.inc"

