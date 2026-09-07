// df_game_r Game/ DomainMiscDeps 专属 TU。
// 覆盖符号（全部 T/W，地址经 ORIG df_game_r nm/objdump 核实）：
//   IndependentDropParameterScript::{load_independent_drop_list,
//       ImportIndependentDrop, importIndependentDropParameterScript}
//       0x0897c0d8 / 0x0897c398 / 0x0897c1c6
//   PvPSkillTreeParameterScript::{load_pvp_skill_tree_list,
//       importPairPvPSkillPointScript, importPvPSkillTreeParameterScript}
//       0x08a5ce28 / 0x08a5e226 / 0x08a5cf16
//   RegenerationROI::_0_Load_RegenerationRandomOptionItem 0x085f963e
//   TimerNPCMoodChange::registNextTimer(long)            0x086381ec（纯 ret）
//   TimerUpdateItemGenerateStatistics::registNextTimer(long) 0x08637318
//   OS_API::Sleep(int)                                   0x0858c4e0
//   （DNFFLib::Make_Dir 0x08108ad7 归属 ServerCommon/DNFFunctionLib.cpp，不在此重复）
// 逐函数对照 docs/class_func_reports/<类>/<方法>.md 的 ## 汇编 与
// ## 反编译 C 转写；报告缺失处以 objdump 反汇编为准并标注。

#include <algorithm>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "CDataManager.h"       // IndependentDrop/PvPSkillTree/RegenerationROI 权威布局
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include "LogManager.h"
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "CSystemTime.h"        // CSystemTime::getCurSec
#include "TimerQueue.h"         // G_TimerQueue/TimerEntry::InsertTimer
#include "TimerDispatcher.h"    // TimerNPCMoodChange/TimerUpdateItemGenerateStatistics
#include "NpcScript.h"

// ---- ORIG B 全局脚本文件列表（定义 InitScriptProducers_5.cpp；本 TU 引用）----
// 本地 STScriptFileList 与 STScriptFileList.cpp / InitScriptProducers_5.cpp /
// NpcScript.h 同布局（+0x00 std::map<int,std::string>, sizeof 0x18），方法调用


extern char g_independentDropScriptBaseDirectory[0x100];   // B 0x094f7840
extern STScriptFileList g_independentDropScriptFileList;   // B 0x094f7940
extern char g_pvpSkillTreeScriptBaseDirectory[0x100];      // B 0x09500240
extern STScriptFileList g_pvpSkillTreeScriptFileList;      // B 0x09500340

// ORIG T 0x08a7e610（0x416 字节）：RandomOptionItem_Regenerator 脚本解析器，
// 属 RandomOptionScript 域——本批仅按 ABI 声明引用，定义待该域批次补。
int importRegenerationRandomOptionScript(RandomOptionItem_Regenerator& reg,
                                         const char* path);

// ===========================================================================
// OS_API::Sleep —— ORIG 0x0858c4e0：usleep(ms*1000)。
// W7_BuildHarmonize：唯一 owner 为 LinkFinalDeps.cpp（同 ORIG 符号），此处
// 原本并发批次撞车重复定义已移除，仅保留注释说明。
// ===========================================================================

// ===========================================================================
// DNFFLib::Make_Dir —— 唯一定义在 ServerCommon/DNFFunctionLib.cpp
// （ORIG 0x08108ad7：opendir 失败时 mkdir(dir,0777)；此处仅声明引用，不重复实现）。
// ===========================================================================
// 定时器注册
// ===========================================================================

// ORIG 0x086381ec：真实空操作定时注册函数（仅 ret）。
void TimerNPCMoodChange::registNextTimer(long /*sec*/)
{
}


// ===========================================================================
// RegenerationROI::_0_Load_RegenerationRandomOptionItem —— ORIG 0x085f963e
// （0x51 字节）：装载再生随机选项脚本；非 0 返回值即出错行并 printf。
// ===========================================================================
int RegenerationROI::_0_Load_RegenerationRandomOptionItem()
{
    int err = 0;                                            // ORIG movl $0,-0xc 预清
    err = importRegenerationRandomOptionScript(
        m_regenerator, "Etc/RandomOption/RegenerationRandomOption.etc");
    register bool fail asm("al") = (bool)(err != 0);        // ORIG cmp/setne/test 形
    if (fail)                                               // ORIG je +0x18
    {
        printf("Fail to Load Random Option Item Regenerator Line : %d\n", err);
        return err;
    }
    return 0;
}

// ===========================================================================
// IndependentDropParameterScript —— 独立掉落域
// ===========================================================================

// ORIG 0x0897c1c6（0x1d1 字节）逐分支转写：
//   以 g_independentDropScriptBaseDirectory 为根装载；记录索引=
//   FindIndexByFullScan(path)；[list] 段 (itemId,cumRate) 对累积进
//   info.m_list04；结束后 m_map24[索引]=(m_vec0.size(), 累积和) 并入 m_vec0。
bool IndependentDropParameterScript::importIndependentDropParameterScript(
    const char* path)
{
    if (!loadRDARScriptFile(g_independentDropScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    IndependentDropListInfo info;
    info.m_field0 =
        g_independentDropScriptFileList.FindIndexByFullScan(path);
    int acc = 0;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        if (token != "[list]")
        {
            continue;
        }
        while (true)
        {
            int itemId = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            int rate = ScanInt(&ok);
            if (!ok)
            {
                return false;                      // 报告 0x0897c296 路径
            }
            acc += rate;
            info.m_list04.push_back(std::pair<int, int>(itemId, acc));
        }
    }
    int count = static_cast<int>(m_vec0.size());
    m_map24[info.m_field0] = std::make_pair((unsigned)count, acc);
    m_vec0.push_back(info);
    return true;
}

// ORIG 0x0897c0d8（0xec 字节）：遍历 g_independentDropScriptFileList，
// 逐文件 importIndependentDropParameterScript；失败按行号 0x30 记日志返回 0。
bool IndependentDropParameterScript::load_independent_drop_list()
{
    std::map<int, std::string>::iterator it;
    it = g_independentDropScriptFileList.m_fileMap.begin();
    for (; it != g_independentDropScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (!name)
        {
            return true;                       // ORIG 0897c1bd：空名直接成功返回
        }
        if (!importIndependentDropParameterScript(name))
        {
            LogManager::logFormat(
                1, "../RDARScriptIndependentDrop.cpp",
                __FUNCTION__, 0x30,
                "IndependentDropParameterScript File Script Error: %s\n",
                name);
            return false;
        }
    }
    return true;
}

// ORIG 0x0897c398（0x78f 字节）逐分支转写。返回值=出错行，0=成功：
//   0x66 装载失败；0x7d..0xcf 各 ScanInt/校验失败；0xe1 [list] 段第二列失败；
//   扫描自然结束返回 1。[independent drop] 段每条记录 16 个整数 +
//   flag(0=直接入表,1=[list] 明细,2=[list] 合并既有列表)，累积主掉率写
//   m_drop.m_rate[10]，(itemId,累积率) 入 m_drop.m_pairList 后整条入 m_vecC。
//   [dungeon drop rate balance] 段每条 3 整数入 m_vec18。
int IndependentDropParameterScript::ImportIndependentDrop(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x66;
    }
    std::string token;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 1;                              // ORIG 0897caee：mov $0x0,%esi
        }
        if (token == "[independent drop]")
        {
            for (;;)
            {
                stIndependentDropInfo info;
                info.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // 段结束（不压栈当前半条）
                }
                info.m_monsterIndex = ScanInt(&ok);
                if (!ok) { return 0x7d; }
                info.m_drop.m_itemIdx = ScanInt(&ok);
                if (!ok) { return 0x81; }
                info.m_drop.m_rate[0] = ScanInt(&ok);
                if (!ok) { return 0x89; }
                info.m_drop.m_rate[1] = ScanInt(&ok);
                if (!ok) { return 0x91; }
                info.m_drop.m_rate[2] = ScanInt(&ok);
                if (!ok) { return 0x99; }
                info.m_drop.m_rate[3] = ScanInt(&ok);
                if (!ok) { return 0xa1; }
                for (int i = 0; i < 4; ++i)
                {
                    info.m_drop.m_rate[4 + i] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xab;
                    }
                }
                info.m_drop.m_rate[8] = ScanInt(&ok);
                if (!ok)
                {
                    return 0xb0;
                }
                info.m_drop.m_rate[9] = 0;
                info.m_drop.m_pairList.clear();
                info.m_drop.m_rangeMin = ScanInt(&ok);
                if (!ok) { return 0xb7; }
                info.m_drop.m_rangeMax = ScanInt(&ok);
                if (!ok) { return 0xbb; }
                if (info.m_drop.m_rangeMin > info.m_drop.m_rangeMax)
                {
                    return 0xbe;
                }
                info.m_drop.m_levelFilter = ScanInt(&ok);
                if (!ok) { return 0xc3; }
                int flag = ScanInt(&ok);
                if (flag != 0 && flag != 1 && flag != 2)
                {
                    LogManager::logFormat(
                        1, "../RDARScriptIndependentDrop.cpp",
                        __FUNCTION__, 0xca,
                        "flag wrong number(%d)", flag);
                    return 0xcb;
                }
                if (!ok)
                {
                    return 0xcf;
                }

                std::string listToken;
                if (flag == 1)
                {
                    if (ScanType(listToken, true) && listToken == "[list]")
                    {
                        while (true)
                        {
                            int itemId = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            int rate = ScanInt(&ok);
                            if (!ok)
                            {
                                return 0xe1;       // ORIG esi=0xe1/ebx=0 → 错误返回
                            }
                            info.m_drop.m_rate[9] += rate;
                            info.m_drop.m_pairList.push_back(
                                std::pair<int, int>(
                                    itemId, info.m_drop.m_rate[9]));
                        }
                    }
                }
                else if (flag == 2)
                {
                    if (ScanType(listToken, true) && listToken == "[list]")
                    {
                        while (true)
                        {
                            int idx = ScanInt(&ok);
                            if (!ok)
                            {
                                break;             // 结束本段明细（ORIG 0897c83a）
                            }
                            bool found = false;
                            for (int e = 0; e < (int)m_vec0.size(); ++e)
                            {
                                if (m_vec0[e].m_field0 != idx)
                                {
                                    continue;
                                }
                                for (int p = 0;
                                     p < (int)m_vec0[e].m_list04.size();
                                     ++p)
                                {
                                    int itemId =
                                        m_vec0[e].m_list04[p].first;
                                    int rate =
                                        m_vec0[e].m_list04[p].second;
                                    info.m_drop.m_rate[9] += rate;
                                    info.m_drop.m_pairList.push_back(
                                        std::pair<int, int>(
                                            itemId,
                                            info.m_drop.m_rate[9]));
                                }
                                found = true;
                                break;             // ORIG 命中后跳出线性搜索
                            }
                            (void)found;
                        }
                    }
                }
                m_vecC.push_back(info);
            }
        }
        else if (token == "[dungeon drop rate balance]")
        {
            for (;;)
            {
                stIndependentDropRateControl rc;
                rc.m_itemId = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field04 = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field08 = ScanInt(&ok);
                if (!ok) { break; }
                m_vec18.push_back(rc);
            }
        }
    }
}

// ===========================================================================
// PvPSkillTreeParameterScript —— PvP 技能树域
// ===========================================================================

// ORIG 0x08a5cf16（0xc43 字节）逐分支转写：
//   [level]/[job index]/[grow type index]/[awakening type] 维护当前键与等级；
//   [skill] 段：明细对入新 PvPSkillTreeValue.m_vec00 且同步写 m_map30[key]
//   的加/减技能表（缺项建 FAIR 默认值）；段尾把 value 插 m_map0[key][level]
//   （重复 grade/键 → 失败）。[static basic skill] 段要求键已存在，明细对追加
//   到既有 value.m_vec0c 并同样更新 m_map30。
bool PvPSkillTreeParameterScript::importPvPSkillTreeParameterScript(
    const char* path)
{
    if (!loadRDARScriptFile(g_pvpSkillTreeScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    PvPSkillTreeKey key;
    int curLevel = 0;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;                           // ORIG 08a5db1a：mov $0x1,%ebx
        }
        if (token == "[level]")
        {
            curLevel = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[job index]")
        {
            key.m_a = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[grow type index]")
        {
            key.m_b = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[awakening type]")
        {
            key.m_c = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[static basic skill]")
        {
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            if (treeIt == m_map0.end())
            {
                return false;                      // ORIG 08a5d76c 路径
            }
            std::map<int, PvPSkillTreeValue>::iterator gradeIt =
                treeIt->second.find(curLevel);
            if (gradeIt == treeIt->second.end())
            {
                PvPSkillTreeValue fresh;
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = treeIt->second.insert(
                    std::make_pair(curLevel, fresh));
                gradeIt = ins.first;
            }
            while (true)
            {
                int skillId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // ORIG 08a5da22 双失败出口
                }
                gradeIt->second.m_vec0c.push_back(
                    std::make_pair(skillId, level));
                std::map<PvPSkillTreeKey,
                         FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                    perIt = m_map30.find(key);
                if (perIt == m_map30.end())
                {
                    return false;                  // ORIG 08a5d9c8 路径
                }
                std::map<int, int>& table = perIt->second.m_addSkills;
                std::map<int, int>::iterator slot = table.find(skillId);
                if (slot == table.end())
                {
                    table.insert(
                        std::make_pair(skillId, level));
                }
                else
                {
                    slot->second = level;
                }
            }
        }
        else if (token == "[skill]")
        {
            PvPSkillTreeValue value;
            while (true)
            {
                int skillId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                value.m_vec00.push_back(
                    std::make_pair(skillId, level));
                std::map<PvPSkillTreeKey,
                         FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                    perIt = m_map30.find(key);
                if (perIt == m_map30.end())
                {
                    FAIR_PVP_SKILL_TREE_SKILL_PER_POINT fresh;
                    fresh.m_addSkills.insert(
                        std::make_pair(skillId, level));
                    m_map30.insert(
                        std::make_pair(key, fresh));
                }
                else
                {
                    std::map<int, int>& table =
                        perIt->second.m_addSkills;
                    std::map<int, int>::iterator slot =
                        table.find(skillId);
                    if (slot == table.end())
                    {
                        table.insert(
                            std::make_pair(skillId, level));
                    }
                    else
                    {
                        slot->second = level;
                    }
                }
            }
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            if (treeIt == m_map0.end())
            {
                std::map<int, PvPSkillTreeValue> inner;
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = inner.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // ORIG local_f0!=1 路径
                }
                m_map0.insert(
                    std::make_pair(key, inner));
            }
            else
            {
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = treeIt->second.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // 重复 grade
                }
            }
        }
    }
}

// ORIG 0x08a5e226（0x42b 字节）逐分支转写：
//   [skill point]：ScanFloat 四舍五入为 int（fstcw/fldcw 就近偶数舍入），
//   按 (jobIdx, 0..0x36) 分行插入 m_map18；残行（非 0 非 0x37）→ 失败。
//   [version]：单字节版本写 +0x50。[init skill tree]：(key,val) 入 m_map54，
//   重复键 → 失败。
bool PvPSkillTreeParameterScript::importPairPvPSkillPointScript(
    const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    int slotIdx = 0;
    int jobIdx = 0;
    const int slotsPerJob = 0x37;
    std::map<int, int> row;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;                           // ORIG 08a5e5eb
        }
        if (token == "[skill point]")
        {
            for (;;)
            {
                // ORIG：先 (int) 转换（x87 fistpl 截断）后检查 ok；lrintf 是
                // round-to-nearest，对 .5 值语义不同（真差异）。
                int rounded = static_cast<int>(ScanFloat(&ok));
                if (!ok)
                {
                    break;
                }
                row.insert(std::make_pair(slotIdx++, rounded));
                if (static_cast<int>(row.size()) == slotsPerJob)
                {
                    m_map18.insert(
                        std::make_pair(jobIdx++, row));
                    row.clear();
                    slotIdx = 0;
                }
            }
            int left = static_cast<int>(row.size());
            if (left != slotsPerJob && left != 0)
            {
                return false;                      // 残行 → 失败（ORIG 08a5e49a）
            }
        }
        else if (token == "[version]")
        {
            int ver = ScanInt(&ok);
            m_field50 = (char)ver;
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[init skill tree]")
        {
            int key = ScanInt(&ok);
            while (true)
            {
                if (!ok)
                {
                    break;                         // 段结束回主扫描
                }
                int val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!m_map54.insert(std::make_pair(key, val)).second)
                {
                    return false;                  // ORIG local_2c!=1 路径
                }
                key = ScanInt(&ok);
            }
        }
    }
}

// ORIG 0x08a5ce28（0xec 字节）：遍历 g_pvpSkillTreeScriptFileList，逐文件
// importPvPSkillTreeParameterScript；失败按行号 0x45 记日志返回 0。
bool PvPSkillTreeParameterScript::load_pvp_skill_tree_list()
{
    std::map<int, std::string>::const_iterator it =
        g_pvpSkillTreeScriptFileList.m_fileMap.begin();
    for (; it != g_pvpSkillTreeScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (!importPvPSkillTreeParameterScript(name))
        {
            LogManager::logFormat(
                1, "../RDARScriptPvPSkillTree.cpp",
                __FUNCTION__, 0x45,
                "PvPSkillTreeParameterScript File Script Error: %s\n",
                name);
            return false;
        }
    }
    return true;
}
