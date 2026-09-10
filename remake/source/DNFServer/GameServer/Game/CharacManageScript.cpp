// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CharacManageScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CharacManageScript.h"

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus —— ORIG T 0x088e0f58
// m_expBonus(+0x18)[level] 命中 → fildl 值后 flds _DAT_08e0c554(=0.1f，
// rodata 8e0c554: 3d cc cc cd) fdivrp → 值/0.1f；未命中 fldz。
// （GetGoldBonus 同构，map 为 m_goldBonus +0x30。）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetExpBonus.inc"
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus —— ORIG T 0x088e0fce（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetGoldBonus.inc"
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevel —— ORIG T 0x088e0ed4
// point 为 0 返 0；自高向低找首个不高于 point 的门槛，等级取其下标加一，
// 最后 clamp 到 m_manageLevelMax。
// b28c-28：cursor 声明 int（非 unsigned）⇒ 4.4 复现 ORIG 实参求值序
// `mov slot,%eax; mov this,%edx; add $0xc,%edx`（探针 /tmp/l28_probe2.cpp fD/fE）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetLevel.inc"
{
    if (point == 0)
        return 0;

    int level = 0;
    int cursor = (int)m_manageLevelPoint.size() - 1;
    while (cursor >= 0)
    {
        if (m_manageLevelPoint.operator[]((unsigned int)cursor) <= point)
        {
            level = (int)(cursor + 1);
            break;
        }
        --cursor;
    }

    if (m_manageLevelMax < level)
        return m_manageLevelMax;
    return level;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetPoint —— ORIG T 0x088e0e0a
// Σ 每行区间 [start..end] 内 lv 不超过 level 的 bonus。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetPoint.inc"
{
    int total = 0;
    for (std::vector<ManagePointBonus>::iterator it = m_pointBonus.begin();
         it != m_pointBonus.end(); it++)
    {
        ManagePointBonus& bonus = *it;
        for (int lv = it->m_field00; lv <= it->m_field04; ++lv)
        {
            if (level >= lv)
                total += it->m_field08;
        }
    }
    return total;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevelUpRequirePoint —— ORIG T 0x088e1114
// max 为 0 或越界返 0；否则取第 max 个门槛值。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetLevelUpRequirePoint.inc"
{
    if ((maxLevel == 0) || (maxLevel > m_manageLevelPoint.size()))
        return 0;
    return m_manageLevelPoint[maxLevel - 1];
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus(int, int*) —— ORIG T 0x088e1044
// 命中 m_expBonus[level] 写 *out，未命中不动 *out（void 返回）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetExpBonus_1.inc"
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        *out = it->second;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus(int, int*) —— ORIG T 0x088e10ac（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetGoldBonus_1.inc"
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        *out = it->second;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetMinimumPointCharacterLevel —— ORIG T 0x088e1266
// m_pointBonus 空 → 0x28；否则取首行 m_field00。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetMinimumPointCharacterLevel.inc"
{
    if (m_pointBonus.empty())
        return 0x28;
    return m_pointBonus[0].m_field00;
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetName —— ORIG T 0x088e1166
// m_basicExplain[key] 命中 → 拷贝 string，未命中 → 空串（std::string 返回值）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetName.inc"
{
    std::map<int, std::string>::iterator it = m_basicExplain.find((int)key);
    if (it != m_basicExplain.end())
        return it->second;
    return std::string();
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetOptionValue —— ORIG T 0x088e11fc
// m_manageOption[type] 命中 → second，未命中 → 0。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetOptionValue.inc"
{
    std::map<int, int>::iterator it = m_manageOption.find(type);
    if (it != m_manageOption.end())
        return it->second;
    return 0;
}
