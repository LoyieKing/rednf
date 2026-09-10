// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBurningFatigueEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CBurningFatigueEvent 实现（批3：从 GameStubs.cpp 迁移）。
// 依据 docs/class_func_reports/CBurningFatigueEvent/*.md + ORIG elf 反汇编：
//   ctor 0x8109fa4：CEventBase() + m_params = G_CDataManager()+0x4f4
//   D1 0x8109ff4：恢复 vptr 后调 CEventBase::~CEventBase()
//   StartEvent 0x810a042：空
//   StartEvent(Word_Param) 0x810a048 / EndEvent 0x810a09a：SetEventFlag + cMyTrace
//   getBonusExpRate 0x810a0ec / getRefillFatigue 0x810a166：遍历 m_params
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN20CBurningFatigueEventC2Ev`（ORIG 0x810a265）；ctor 已是首强符号。
#include <iostream>

#include "CBurningFatigueEvent.h"

#include "CGuildServerProxy.h"   // G_CDataManager / CDataManager
#include "cMyTraceNoop.h"

#include "CBurningFatigueEvent/CBurningFatigueEvent.inc"
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；
    // m_params 指向全局 CDataManager 内 +0x4f4 的 burning-fatigue 参数 vector
    // （= CDataManager::m_serverParameter(+0x68) 内 ServerParameterScript+0x48c，
    //   与 importServerParameterScript 的 push_back 目标互证）。
    m_params = reinterpret_cast<std::vector<stBurningFatigueParam_t>*>(
        reinterpret_cast<char*>(G_CDataManager()) + 0x4f4);
}

#include "CBurningFatigueEvent/dtor_CBurningFatigueEvent.inc"
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
}

#include "CBurningFatigueEvent/StartEvent.inc"
{
    // ORIG 0x810a042：空实现。
}

#include "CBurningFatigueEvent/StartEvent_1.inc"
{
    // ORIG 0x810a048：SetEventFlag(true) + 日志。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("Burning Fatigue Event Start");
}

#include "CBurningFatigueEvent/EndEvent.inc"
{
    // ORIG 0x810a09a：SetEventFlag(false) + 日志。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x21, 0);
    tr("Burning Fatigue Event End");
}

#include "CBurningFatigueEvent/getBonusExpRate.inc"
{
    // ORIG 0x810a0ec：从 i=1 开始找第一个 m_level > level 的项，返回其前一项
    // m_bonusExpRate(+0x04)；越界返回 0。每次访问均重解引用 this+0xc（ORIG 形态）。
    for (size_t i = 1; i < m_params->size(); ++i)
    {
        if (level < (*m_params)[i].m_level)
            return (*m_params)[i - 1].m_bonusExpRate;
    }
    return 0;
}

#include "CBurningFatigueEvent/getRefillFatigue.inc"
                                                      unsigned short cur) const
{
    // ORIG 0x810a166：找第一个 (level >= m_level) 且 m_level > (level-cur)
    // （16 位值提升为 int 相减）的项，返回其 m_refillFatigue(+0x08)；越界返回 0。
    // ORIG 汇编为 || 短路求值后 test/je 判定：ja/jle 落在置 0 分支。
    for (size_t i = 1; i < m_params->size(); ++i)
    {
        if (!(level < (*m_params)[i].m_level ||
              (int)(*m_params)[i].m_level <= (int)(level - cur)))
            return (*m_params)[i].m_refillFatigue;
    }
    return 0;
}
