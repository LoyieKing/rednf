// ============================================================================
// InitScriptProducers_2.cpp —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// 报告 docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数
// 生产 TU（第 2 批，InitScripts_2）。
//
// 每个函数均转写自 docs/class_func_reports/_lt_global_gt_/ 对应报告（objdump
// 汇编 + Ghidra C），统一模式：loadRDARScriptFile("",path) → ScanType 分派 →
// ScanInt/ScanStr/ScanFloat 字段回填。调用点（init.md 行号 / 目标成员）：
//   ImportReturnUserRewardScript 0x089180ea  init.md:1044  this+0xb548
//   ImportBingoScript            0x089187b9  init.md:1071  this+0xb568
//   ImportSecurityReward         0x089113ec  init.md:1123  this+0x50d8
//   ImportItemMakingSkill        0x0890f564  init.md:1146  this+0xb458
//   importGrowthPower            0x08911ee3  init.md:1796  this+0x8710
//   ImportWarAreaCounter         0x089026f3  init.md:1956  this+0x87b4
//   importEquipmentPartSetInfo   0x089073d9  init.md:4749  ("Etc/EquipmentPartSet.etc",3)
//   importPairPvPStatScript      0x08977d9c  init.md:4936  this+0x4df8
//
// 附带本批函数的被调依赖（全工程此前无定义，已广播认领）：
//   LoadRewardDataByGrowType 0x08917f44 / deleteTagBracketString 0x08917db3 /
//   getSameStringIndex 0x08990ed5 / characterEquipmentTypeTags D 0x093f4fc0 /
//   makeFairPVPEquipmentPackageKey 0x088945ad / g_equipmentPartSetMap_ B 0x094f7130 /
//   STEquipmentPartSetData C1 0x891b002·D1 0x08907348。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // 脚本类型权威定义（须先于 deps 头）
#include "sync_script.h"              // STEquipmentPartSetPiece 完整类型
#include "STEquipmentScript.h"        // STEquipmentScript/m_paramMap678/ImportEquipmentScript
#include "EquipmentParameterInfo.h"   // EquipmentParameterInfo copy/operator=
#include "DataManagerInitDepends.h"   // 本批声明唯一汇总点
#include "DNFLexWrapper.h"            // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"               // LogManager::logFormat

#include <cstring>

// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
const char* characterEquipmentTypeTags[26] = {
    "[hat avatar]",    "[hair avatar]",   "[face avatar]",    "[coat avatar]",
    "[pants avatar]",  "[shoes avatar]",  "[breast avatar]",  "[waist avatar]",
    "[skin avatar]",   "[aurora avatar]", "[weapon]",         "[title name]",
    "[coat]",          "[shoulder]",      "[pants]",          "[shoes]",
    "[waist]",         "[amulet]",        "[wrist]",          "[ring]",
    "[support]",       "[magic stone]",   "[creature]",
    "[artifact red]",  "[artifact blue]", "[artifact green]",
};

// ORIG B 0x094f7130（BSS，sizeof(std::map)=0x18）；init 尾段遍历回灌 DB。
std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;

// ============================================================================
// STEquipmentPartSetData —— ORIG C1 0x0891b002 / D1 0x08907348
// ============================================================================
STEquipmentPartSetData::STEquipmentPartSetData()
    : m_field0(-1), m_name4(), m_script(0), m_pieces()
{
}

STEquipmentPartSetData::~STEquipmentPartSetData()
{
    if (m_script != 0)
    {
        delete m_script;
        m_script = 0;
    }
}

// ============================================================================
// makeFairPVPEquipmentPackageKey —— ORIG T 0x088945ad
// (short)a 落低 16 位、(short)b 落高 16 位后按 int 返回。
// ============================================================================
int makeFairPVPEquipmentPackageKey(int job, int idx)
{
    unsigned short lo = (unsigned short)(job & 0xffff);
    unsigned short hi = (unsigned short)(idx & 0xffff);
    return (int)((unsigned int)lo | ((unsigned int)hi << 16));
}

// ============================================================================
// getSameStringIndex —— ORIG T 0x08990ed5
// strcmp 线性查表，命中返回下标，未命中返回 -1。
// ============================================================================
int getSameStringIndex(const char* s, const char** table, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (std::strcmp(s, table[i]) == 0)
            return i;
    }
    return -1;
}

// deleteTagBracketString（ORIG T 0x08917db3）定义归 InitScriptProducers_1.cpp
// （InitScripts_1 认领），本 TU 经 DataManagerInitDepends.h 声明引用。

// ============================================================================
// LoadRewardDataByGrowType —— ORIG T 0x08917f44
// 扫描到 endTag 结束；每条：deleteTagBracketString→getCharacterJobToEnumValue
// （-1 → 0x3693），grow=ScanInt 必需；循环 level=ScanInt（失败→回到外层标签循
// 环）、data 两 int（失败→0x36a3/0x36a7）插入 out.m_map0。返回 0=正常结束。
// ============================================================================
int LoadRewardDataByGrowType(stLevelUpRewardItemScript& out, std::string endTag)
{
    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == endTag)
            return 0;

        deleteTagBracketString(token);
        const int job = getCharacterJobToEnumValue(token);
        if (job == -1)
            return 0x3693;

        LevelUpRewardItemTableKey key(0, job, 0);
        key.m_job = (unsigned char)job;                  // +0x02
        key.m_grow = (unsigned char)ScanInt(&ok);        // +0x03
        if (!ok)
            return 0;

        while (true)
        {
            key.m_lv = (unsigned short)ScanInt(&ok);     // +0x00
            if (!ok)
                break;                                   // 回到外层标签扫描

            LevelUpRewardItemData data;
            data.m_field0 = ScanInt(&ok);
            if (!ok)
                return 0x36a3;
            data.m_field4 = ScanInt(&ok);
            if (!ok)
                return 0x36a7;

            out.m_map0.insert(
                std::make_pair(key, data));
        }
    }
    return 0;
}

// ============================================================================
// ImportReturnUserRewardScript —— ORIG T 0x089180ea（init.md:1044，
// "Event/ReturnUserReward.evt" → this+0xb548）。返回 0=成功，非 0=错误行。
// [pacakege item] 为 ORIG 原文拼写。
// ============================================================================
int ImportReturnUserRewardScript(const char* path, stReturnUserRewardScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x36b4;

    std::string token;
    out.m_levelUp.clear();

    while (ScanType(token, true))
    {
        if (token == "[pacakege item]")
        {
            ScanInt(&out.m_field0);
            ScanInt(&out.m_field4);
            continue;
        }
        if (token != "[reward equip item]")
            continue;

        const int err = LoadRewardDataByGrowType(out.m_levelUp,
                                                 "[/reward equip item]");
        if (err != 0)
            return err;
    }
    return 0;
}

// ============================================================================
// ImportBingoScript —— ORIG T 0x089187b9（init.md:1071，"Event/BingoEvent.evt"
// → this+0xb568）。ORIG 所有路径恒返回 0（含加载失败），忠实保留。
// 段：[board]→m_map18、[quiz value]→m_map0、[reward]→m_map30、[use item]→m_vec48。
// ============================================================================
bool ImportBingoScript(const char* path, stBingoScript& out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[board]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count && (ok = false, true); )
            {
                const int key = ScanInt(&ok);
                if (!ok)
                    break;                               // 直接回主循环，不插入

                std::vector<int> values;                 // 每行新建（ORIG ctor+clear）
                for (int j = 0; j < 25; )                // ORIG 上限 0x19
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        break;
                    values.push_back(v);
                    ++j;
                }
                out.m_map18.insert(std::make_pair(key, values));
                ++i;
            }
            continue;
        }

        if (token == "[quiz value]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count; )
            {
                const int k = ScanInt(&ok);
                if (!ok)
                    return false;
                const int v = ScanInt(&ok);
                if (!ok)
                    return false;
                out.m_map0.insert(std::make_pair(k, v));
                ++i;
            }
            continue;
        }

        if (token == "[reward]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count; ++i)
            {
                std::pair<int, int> reward;              // 默认构造（ORIG pair()）
                const int a = ScanInt(&ok);
                if (!ok)
                    break;
                reward.first = ScanInt(&ok);
                if (!ok)
                    break;
                reward.second = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_map30.insert(std::make_pair(a, reward));
            }
            continue;
        }

        if (token == "[use item]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_vec48.push_back(v);
            }
            continue;
        }
    }
    return false;
}

// ============================================================================
// ImportItemMakingSkill —— ORIG T 0x0890f564（init.md:1146，
// "Etc/ItemMakingSkill.etc" → this+0xb458）。返回 0=成功，非 0=错误行。
// ============================================================================
int ImportItemMakingSkill(const char* path, stItemMakingSkill& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x279e;

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[itemmaking exp limit]")
        {
            while (true)
            {
                const int lv = ScanInt(&ok);
                if (!ok)
                    break;
                const int exp = ScanInt(&ok);
                if (!ok)
                    return 0x27b2;
                out.m_vec18.push_back(std::make_pair(lv, exp));
            }
            continue;
        }

        if (token == "[itemmaking exp]")
        {
            out.m_exp = ScanInt(&ok);
            if (!ok)
                return 0x27bb;
            out.m_bonusExp = ScanInt(&ok);
            if (!ok)
                return 0x27bf;
            continue;
        }

        if (token == "[itemmaking level limit]")
        {
            out.m_levelLimit = ScanInt(&ok);
            if (!ok)
                return 0x27c5;
            continue;
        }

        if (token == "[upgrade with compound]")
        {
            while (true)
            {
                stUpgradeWithCompound data;              // ctor 清零（ORIG clear）
                const int key = ScanInt(&ok);
                if (!ok)
                    break;

                data.m_field00 = ScanInt(&ok);
                if (!ok)
                    return 0x27d3;
                data.m_field04 = ScanInt(&ok);
                if (!ok)
                    return 0x27d7;
                data.m_field08 = ScanInt(&ok);
                if (!ok)
                    return 0x27db;
                data.m_rangeExp0C = (data.m_field04 - data.m_field00) * 1000 + 999;

                data.m_field10 = ScanInt(&ok);
                if (!ok)
                    return 0x27e1;
                data.m_field14 = ScanInt(&ok);
                if (!ok)
                    return 0x27e5;
                data.m_field18 = ScanInt(&ok);
                if (!ok)
                    return 0x27e9;
                data.m_rangeExp1C = (data.m_field14 - data.m_field10) * 1000 + 999;

                out.m_map0.insert(std::make_pair(key, data));
            }
            continue;
        }
    }
    return 0;
}

// ============================================================================
// importGrowthPower —— ORIG T 0x08911ee3（init.md:1796，
// "Etc/GrowthPowerRewardBuff.etc" → this+0x8710）。返回 bool（真=成功）。
// 段：[breakaway section]→m_vec0、[exp reward]→m_vecC、
//     [package reward]{[group]/[package selection]}→m_map18。
// ============================================================================
bool importGrowthPower(char* path, STGrowthPowerData* out)
{
    if (out == 0)
        return false;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string group;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[breakaway section]")
        {
            while (true)
            {
                STBreakAwaySection section;
                section.m_field0 = (char)ScanInt(&ok);
                if (!ok)
                    break;
                section.m_field1 = (char)ScanInt((bool*)0);
                section.m_field2 = (char)ScanInt((bool*)0);
                out->m_vec0.push_back(section);
            }
            continue;
        }

        if (token == "[exp reward]")
        {
            while (true)
            {
                STExpReward reward;                      // ctor 仅清前 4 字节（同 ORIG）
                // ORIG 逐字节写入 +0x00..+0x03（mov %al）：
                //   +0x00=m_reward 低字节、+0x01=m_field4、+0x02=m_field5、
                //   +0x03=m_powerUp。
                reward.m_reward = (unsigned char)ScanInt(&ok);
                if (!ok)
                    break;
                reward.m_field4 = (char)ScanInt((bool*)0);
                reward.m_field5 = (char)ScanInt((bool*)0);
                reward.m_powerUp = (char)ScanInt((bool*)0);
                out->m_vecC.push_back(reward);
            }
            continue;
        }

        if (token == "[package reward]")
        {
            do
            {
                if (!ScanType(token, true))
                    break;                               // EOF → 回主循环

                if (token == "[group]")
                {
                    if (!ScanStr(&group))
                        return false;

                    const int job = getCharacterJob(group.c_str());
                    const int n = ScanInt((bool*)0);
                    const int key = makeFairPVPEquipmentPackageKey(job, n);

                    std::vector<std::vector<STStackablePackageData> > groups;
                    ScanType(token, true);
                    while (token == "[package selection]")
                    {
                        std::vector<STStackablePackageData> items;
                        while (true)
                        {
                            const int idx = ScanInt(&ok);
                            if (!ok)
                                break;
                            STStackablePackageData d;
                            d.m_field0 = idx;
                            d.m_field4 = ScanInt((bool*)0);
                            items.push_back(d);
                        }
                        groups.push_back(items);
                        ScanType(token, true);
                    }
                    out->m_map18[key] = groups;
                }
            } while (token != "[/package reward]");
            continue;
        }
    }
    return true;
}

// ============================================================================
// ImportWarAreaCounter —— ORIG T 0x089026f3（init.md:1956，this+0x87b4，
// 固定路径 "Etc/WarArea.etc"）。返回 1=成功 / 0=失败。
// 注：ORIG 无下标越界检查（裸指针写入靠相邻区布局兜底）；字段化访问后对四个
// 数组加上限（24/24/10/10，与相邻成员边界一致），语义不变。
// ============================================================================
int ImportWarAreaCounter(WarAreaCounter* out)
{
    if (!loadRDARScriptFile("", "Etc/WarArea.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              "ImportWarAreaCounter", 0xdeb,
                              "CRITICAL ERROR : \xec\x9f\x81\xec\x97\xad "
                              "\xec\xb9\xb4\xec\x9a\xb4\xed\x84\xb0 "
                              "\xec\x8a\xa4\xed\x81\xac\xeb\xa6\xbd\xed\x8a\xb8 "
                              "\xeb\xa1\x9c\xeb\x93\x9c \xec\x8b\xa4\xed\x8c\xa8!");
        return 0;
    }

    std::string token;
    bool ok = false;
    int timeCount = 0;
    int maxValue = 0;
    int maxIndex = -1;
    int doorCount = 0;
    int dungeonIdx = 2;                                  // ORIG 从下标 2 起

    while (ScanType(token, true))
    {
        if (token == "[time table]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                if (timeCount < 24)
                    out->m_timeHourList[timeCount] = v;  // ORIG 写 this+(i+0x18)*4
                ++timeCount;
            }
            continue;
        }

        if (token == "[door fee]")
        {
            doorCount = 0;
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                if (doorCount < 10)
                    out->m_doorFee[doorCount] = v;       // ORIG 写 this+(i+0x30)*4
                ++doorCount;
            }
            continue;
        }

        if (token == "[ban interval]")
        {
            out->m_banInterval = ScanInt(&ok);
            if (!ok)
                return 0;
            continue;
        }

        if (token == "[kick point]")
        {
            out->m_kickPoint = ScanInt(&ok);
            if (!ok)
                return 0;
            continue;
        }

        if (token == "[ready count]")
        {
            out->m_readyCount = ScanInt(&ok);
            if (!ok)
                return 0;
            continue;
        }

        if (token == "[wp penalty]")
        {
            out->m_wpPenalty = ScanInt(&ok);
            if (!ok)
                return 0;
            continue;
        }

        if (token == "[monster exp const]")
        {
            out->m_monsterExpConst = ScanFloat(&ok);
            if (!ok)
                return 0;
            continue;
        }

        if (token == "[wararea dungeon]")
        {
            dungeonIdx = 2;
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                if (dungeonIdx >= 2 && dungeonIdx < 12)
                    out->m_warAreaDungeon[dungeonIdx] = v;
                ++dungeonIdx;
            }
            continue;
        }

        if (token == "[wp per monster hp]")
        {
            while (true)
            {
                stWarPoint wp;                           // ctor memset 0x10
                const int key = ScanInt(&ok);
                if (!ok)
                    break;                               // 回主循环

                for (int j = 0; j < 4; ++j)
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        return 0;
                    if (j == 0)
                        wp.m_0 = v;
                    else
                        wp.m_pts[j - 1] = v;
                }
                out->m_wpPerMonsterHp.insert(
                    std::make_pair(key, wp));            // greater<int> 键序
            }
            continue;
        }

        if (token == "[counter]")
        {
            out->m_num = timeCount;
            int counter = 0;
            do
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                if (v > maxValue)
                {
                    maxIndex = counter;
                    maxValue = v;
                }
                if (counter < 24)
                    out->m_counterTable[counter] = v;
                ++counter;
            } while (out->m_num != counter);
            out->m_curHourIdx = maxIndex;                // ORIG 写 +0xe8
            continue;
        }
    }
    return 1;
}

// ============================================================================
// importPairPvPStatScript —— ORIG T 0x08977d9c（init.md:4936，
// this+0x4df8 ← "Etc/FairPvPStat.etc"）。恒返回 0（ORIG 全路径 ebx=0）。
// [grow type index] 以 (job,grow) 为键在 m_map0 插入清零 CharacterStatus，
// 后续段直接写该值字段；[attack speed]/[jump speed] 无 cur 判空（ORIG 缺失，
// 引用恒非空故等价）。[jump speed] 读后丢弃（ORIG 同）。
// ============================================================================
bool importPairPvPStatScript(FairPvPStatScript& script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script.clear();

    std::string token;
    bool ok = false;
    char jobIndex = 0;
    char growTypeIndex = 0;
    CharacterStatus* current = 0;

    while (ScanType(token, true))
    {
        if (token == "[job index]")
        {
            jobIndex = (char)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }

        if (token == "[grow type index]")
        {
            growTypeIndex = (char)ScanInt(&ok);
            if (!ok)
                return false;

            CharacterStatus status;
            std::memset(&status, 0, sizeof(status));     // ORIG memset 0x54
            const std::pair<std::map<CharacterKind, CharacterStatus>::iterator,
                            bool> inserted =
                script.m_map0.insert(
                    std::make_pair(CharacterKind(jobIndex, growTypeIndex),
                                   status));
            if (!inserted.second)
                return false;
            current = &inserted.first->second;
            continue;
        }

        if (current == 0)
            return false;                                // 各数值段的 ORIG 前置判空

        if (token == "[HP MAX]")
        {
            current->m_field00 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP MAX]")
        {
            current->m_field04 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical attack]")
        {
            current->m_field08[0] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical defense]")
        {
            current->m_field08[1] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical attack]")
        {
            current->m_field08[2] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical defense]")
        {
            current->m_field08[3] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[fire resistance]")
        {
            current->m_field08[4] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[water resistance]")
        {
            current->m_field08[5] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[dark resistance]")
        {
            current->m_field08[6] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[light resistance]")
        {
            current->m_field08[7] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[inventory limit]")
        {
            current->m_field3c = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP regen speed]")
        {
            current->m_field42 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[move speed]")
        {
            current->m_field44 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[attack speed]")                   // ORIG 此段无判空
        {
            current->m_field48 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[cast speed]")
        {
            current->m_field4a = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[hit recovery]")
        {
            current->m_field4c = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump power]")
        {
            current->m_field4e = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump speed]")                     // ORIG 读后丢弃
        {
            ScanInt(&ok);
            continue;
        }
        if (token == "[weight]")
        {
            current->m_field50 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
    }
    return false;
}

// ============================================================================
// ImportSecurityReward —— ORIG T 0x089113ec（init.md:1123，this+0x50d8）。
// 三段各 4 个 int；返回 bool（真=成功）。
// ============================================================================
bool ImportSecurityReward(SecurityReward* out)
{
    if (!loadRDARScriptFile("", "Etc/SecurityReward.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              "ImportSecurityReward", 0x2c36,
                              "CRITICAL ERROR : SecurityReward faild!");
        return false;
    }

    std::string token;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[GoblinPad Reward]")
        {
            for (int i = 0; i < 4; ++i)
                out->m_goblinPad[i] = ScanInt(&ok);
        }
        else if (token == "[SecurityCard Reward]")
        {
            for (int i = 0; i < 4; ++i)
                out->m_securityCard[i] = ScanInt(&ok);
        }
        else if (token == "[OTP Reward]")
        {
            for (int i = 0; i < 4; ++i)
                out->m_otp[i] = ScanInt(&ok);
        }
    }
    return true;
}

// ============================================================================
// importEquipmentPartSetInfo —— ORIG T 0x089073d9（init.md:4749，
// "Etc/EquipmentPartSet.etc", ENUM_MODULE_TYPE 3）。返回 bool（真=成功）。
// 解析 [equipment part set] → g_equipmentPartSetMap_；随后逐集合
// new STEquipmentScript 并 ImportEquipmentScript(name,false,module,key)；
// 成功后把该脚本 m_paramMap678[-1]（存在时）迁移为键=pieces.size() 的条目。
// ============================================================================
bool importEquipmentPartSetInfo(const char* path, ENUM_MODULE_TYPE moduleType)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              "importEquipmentPartSetInfo", 0x168b,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 "
                              "\xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 "
                              "\xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.",
                              path);
        return false;
    }

    std::string token;
    std::string name;
    std::string pieceName;
    std::string tag;
    bool ok = false;

    g_equipmentPartSetMap_.clear();

    while (ScanType(token, true))
    {
        if (token != "[equipment part set]")
            continue;

        STEquipmentPartSetData data;
        const int key = ScanInt((bool*)0);
        data.m_field0 = key;
        ScanStr(&name);
        data.m_name4 = name;

        do
        {
            STEquipmentPartSetPiece piece;               // ctor 三 int=-1
            if (ScanStr(&pieceName))
            {
                piece.m_name = pieceName;
                ScanStr(&tag);
                piece.m_field4 = getSameStringIndex(tag.c_str(),
                                                    characterEquipmentTypeTags,
                                                    0x1a);
                piece.m_field8 = ScanInt((bool*)0);
                piece.m_fieldc = ScanInt((bool*)0);
                data.m_pieces.push_back(piece);

                g_equipmentPartSetMap_[key] = data;      // operator[] + operator=
            }
        } while (ok = false, ScanStr(&pieceName));
    }

    // ---- 逐集合装载装备脚本并合并 [-1] 参数条目 ----
    for (std::map<int, STEquipmentPartSetData>::iterator it =
             g_equipmentPartSetMap_.begin();
         it != g_equipmentPartSetMap_.end(); ++it)
    {
        STEquipmentScript* script = new STEquipmentScript;   // ORIG 0x894 字节
        it->second.m_script = script;

        if (!ImportEquipmentScript(script, it->second.m_name4.c_str(), false,
                                   moduleType, it->first))
        {
            LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                  "importEquipmentPartSetInfo", 0x16c2,
                                  "ERROR : \xec\x9e\xa5\xeb\xb9\x84\xeb\xb6\x80"
                                  "\xec\x9c\x84\xec\x85\x8b\xed\x8a\xb8 \xea\xb5"
                                  "\xac\xec\x84\xb1\xed\x92\x88 %s \xed\x8c\x8c"
                                  "\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d"
                                  "\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb"
                                  "\x8b\x88\xeb\x8b\xa4.",
                                  it->second.m_name4.c_str());
            delete script;
            return false;
        }

        std::map<int, EquipmentParameterInfo>& paramMap = script->m_paramMap678;
        std::map<int, EquipmentParameterInfo>::iterator def =
            paramMap.find(-1);
        if (def != paramMap.end())
        {
            const EquipmentParameterInfo merged(def->second);
            paramMap.clear();
            paramMap[it->second.m_pieces.size()] = merged;
        }
    }
    return true;
}
