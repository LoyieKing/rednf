// ============================================================================
// ScriptLeafA2.cpp — 脚本叶子依赖恢复（独占 TU，任务 ScriptLeafA2）
//
// 覆盖符号（全部经 ORIG df_game_r nm 核实；有报告者全文阅读后转写，
// 报告缺失三处以 objdump 逐指令转写并标注）：
//   convertTabTypeStringToEnum(char const*)                        T 0x08aa7d98
//   loadNPCCommonData(stNPCCommonData_t&, char const*)             T 0x089fafc5
//   stNPCCommonData_t::clear()                                     W 0x089fae9e
//   importRegenerationRandomOptionScript(RandomOptionItem_Regenerator&, char const*)
//                                                                  T 0x08a7e610
//   StrikerScript::importScript(char const*)                       T 0x08a9ddb2
//   StageMapList::loadList()                                       T 0x0836485c
//   StageMapList::compare(...)                                     T 0x08364848
//   APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript(
//       char const*, CActionPointEtcParameter&)                    T 0x08894996
//   eventReward::ImportRewardScript(char const*, char const*)      T 0x080ef3a2
//   importUpgradeTable(char const*, upgrade_table_t*)              T 0x08900c26
//   stLevelUpRewardItemScript::clear()                             W 0x083711d6（无报告，objdump）
//   seriaRoomDecoAniEventScript::clear()                           W 0x083732a4（无报告，objdump）
//   rewardSubKey::{rewardSubKey,clear,operator<}                   W 0x080f1190/0x080f11de/0x080f11a2
//   rewardKey::{rewardKey,clear,operator<}                         W 0x080f11f0/0x080f12ac/0x080f121a
//       （objdump 逐指令转写；布局见 CDataManager.h rewardSubKey/rewardKey 区）
//
// 报告路径：
//   docs/class_func_reports/_lt_global_gt_/{convertTabTypeStringToEnum,
//       loadNPCCommonData,importRegenerationRandomOptionScript,
//       importUpgradeTable}.md（## 汇编 + ## 反编译 C 全文已读）
//   docs/class_func_reports/StrikerScript/importScript.md（同上）
//   docs/class_func_reports/StageMapList/{loadList,compare}.md（同上）
//   docs/class_func_reports/APSystem__CActionPointEtcParameter/
//       importActionPointSystemEtcScript.md（同上）
//   docs/class_func_reports/eventReward/ImportRewardScript.md（同上）
//   无报告三处先读调用者/同类报告：_lt_global_gt_/ImportLevelUpRewardItemScript.md、
//       _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md、
//       DisPatcher_RentAvatar/dispatch_sig.md、DisPatcher_BuyItem/get_data.md。
// ============================================================================

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

int getChangeStatusType(std::string& s);                            // 0x8892b0d
bool ConvertWeekdayStringToInt(std::string s, unsigned int& out);   // 0x088c284a
int getItemRarity(const char* name);                                // 0x088bfd97
int getCharacterJobToEnumValue(std::string& job);                   // 0x089bbc0f

#include "CDataManager.h"        // stNPCCommonData_t / RandomOptionItem_Regenerator /
                                 // eventReward / StageMapList / APSystem 域权威声明
#include "APSystemTypes.h"       // CActionPointEx / _MedalRewardItem 权威布局
#include "StrikerScript.h"       // StrikerScript 及子类型（ContentsType 等）
#include "CItemAmplifier.h"      // upgrade_table_t / upgrade_info_t 权威布局
#include "CItem.h"               // CItem::GetMedalPrice 声明与 m_field14
#include "DNFLexWrapper.h"       // loadRDARScriptFile / ScanType / ScanInt / ScanFloat / ScanStr
#include "CUserProc.h"          // APSystem::_ActionGroupIndex 完整定义
#include "CEnvironment.h"        // less_TCHAR / SpinLock
#include "LogManager.h"          // LogManager::logFormat

// ORIG B 全局（定义 InitScriptProducers_5.cpp；同布局视图访问：
// STScriptFileList +0x00 即 std::map<int,std::string>，sizeof 0x18）。
namespace
{
class StageScriptFileListView
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
};
}  // namespace
extern StageScriptFileListView g_stageMapScriptFileList;    // B 0x09494e20
extern char g_npcScriptBaseDirectory[0x100];                // B 0x094fe680

// ============================================================================
// convertTabTypeStringToEnum —— ORIG T 0x08aa7d98
// 报告 _lt_global_gt_/convertTabTypeStringToEnum.md 全文转写。
// 函数级静态 __lookup(map<char const*,int,less_TCHAR>)/__init_lock(SpinLock)/
// __inited 三件套：首调登记 general/specific/despair/event → 0x11/0x14/0x17/0x1a；
// 之后查表命中返回 0..3，未命中返回 4。
// ============================================================================
int convertTabTypeStringToEnum(const char* text)
{
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static char __inited = 0;

    for (;;)
    {
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            __lookup.insert(std::make_pair("general", 0x11));
            __lookup.insert(std::make_pair("specific", 0x14));
            __lookup.insert(std::make_pair("despair", 0x17));
            __lookup.insert(std::make_pair("event", 0x1a));
            __init_lock.leave();
            __inited = 1;
            continue;   // ORIG jmp 回入口走 find 派发
        }

        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(text);
        if (it == __lookup.end())
        {
            return 4;
        }
        switch (it->second)
        {
        case 0x11:
            return 0;   // general
        case 0x14:
            return 1;   // specific
        case 0x17:
            return 2;   // despair
        case 0x1a:
            return 3;   // event
        default:
            return 4;
        }
    }
}


// ============================================================================
// stNPCCommonData_t::clear —— ORIG W 0x089fae9e（objdump 全文核实）：
// 仅清标量——循环 i=0..9 写 favorExp[i]（movw $0,0x2(%eax,%edx,2)，cmpl $0x9/setle），
// 再按 +0x00→+0x34→+0x30→+0x32 顺序清四个标量；无任何容器 clear。
// ============================================================================
void stNPCCommonData_t::clear()
{
    for (int i = 0; i <= 9; ++i)
    {
        m_favorExp[i] = 0;
    }
    m_maxFavorExp = 0;
    m_maxFavorLevelNpc = 0;
    m_favorSystemBeginQuest = 0;
    m_favorSystemBeginLevel = 0;
}

// ============================================================================
// loadNPCCommonData —— ORIG T 0x089fafc5
// 报告 _lt_global_gt_/loadNPCCommonData.md 全文转写。
// 返回：0x3c=脚本打开失败；其余路径一律 0（解析失败与干净 EOF 同值，ORIG 如此）。
// ============================================================================
int loadNPCCommonData(stNPCCommonData_t& out, const char* path)
{
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, path))
    {
        return 0x3c;
    }

    std::string token;
    int idx = 0;
    bool ok = false;

    out.clear();

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[npc favor exp]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorExp[idx] = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                out.m_maxFavorExp =
                    std::max(out.m_maxFavorExp, out.m_favorExp[idx]);
            }
        }
        else if (token == "[favorable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorableNpcList.push_back(idx);
            }
        }
        else if (token == "[illustration variable npc list]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_illustrationVariableNpcList.push_back(idx);
            }
        }
        else if (token == "[max favor level npc]")
        {
            out.m_maxFavorLevelNpc = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin quest]")
        {
            out.m_favorSystemBeginQuest = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor system begin level]")
        {
            out.m_favorSystemBeginLevel = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok)
            {
                break;
            }
        }
        else if (token == "[favor level reward npc]")
        {
            for (;;)
            {
                int npcId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out.m_favorLevelRewardNpcList.push_back(npcId);
            }
        }
        else if (token == "[favor duration buff list]")
        {
            for (;;)
            {
                std::vector<stNPCCommonData_t::stOneBuff_t> buffs;
                int first = ScanInt(&ok);
                if (!ok)
                {
                    break;                          // 回主循环（buffs 析构）
                }
                int count = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int i = 0; i < count; ++i)
                {
                    std::string s;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    int discardedType = getChangeStatusType(s);   // ORIG 存局部后弃用
                    int value = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    stNPCCommonData_t::stOneBuff_t buff;
                    buff.m_changeStatusType = getChangeStatusType(s);   // 第二次入元素
                    buff.m_value = value;
                    if (!ScanStr(&s))
                    {
                        return 0;
                    }
                    if (s == "+")
                    {
                        buff.m_isNotPlus = 0;
                    }
                    else
                    {
                        buff.m_isNotPlus = 1;
                    }
                    buffs.push_back(buff);
                }
                out.m_favorDurationBuffList.push_back(std::make_pair(first, buffs));
            }
        }
    }
    return 0;
}

// ============================================================================
// importRegenerationRandomOptionScript —— ORIG T 0x08a7e610
// 报告 _lt_global_gt_/importRegenerationRandomOptionScript.md 全文转写。
// 返回：0=成功/文件打开失败（ORIG 同值）；0x1d=[choose part] 首扫描失败；
//       0x5d=[regeneration price] 组重复。
// ============================================================================
int importRegenerationRandomOptionScript(RandomOptionItem_Regenerator& reg,
                                         const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    std::string token;
    std::string s;
    bool ok = false;
    int part = 0;

    while (ScanType(token, true))
    {
        if (token == "[choose part]")
        {
            part = ScanInt(&ok);
            if (!ok)
            {
                return 0x1d;
            }
            ScanStr(&s);
            str_index_to_itemgroup& entry = reg.m_map0[part];
            entry.m_name4 = s;
            while (ScanStr(&s))
            {
                entry.m_vec8.push_back(s);
            }
        }
        else if (token == "[adjust quantity ratio]")
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            for (int i = 0; i < 3; ++i)
            {
                reg.m_vec30.push_back(ScanInt(&ok));
            }
        }
        else if (token == "[adjust value ratio]")
        {
            part = ScanInt(&ok);            // ORIG 不校验 ok
            ScanStr(&s);
            reg.m_map0[part].m_name4 = s;
            _str_option_value_ratio ratio(0.0f, 0.0f, 0);  // ORIG 无默认 ctor 符号
            ratio.m_scaleRange0 = 0.0f;
            ratio.m_scaleBase4 = 0.0f;
            ratio.m_weight8 = 0;
            while (ScanFloat(&ratio.m_scaleRange0))
            {
                ScanFloat(&ratio.m_scaleBase4);
                ratio.m_weight8 = static_cast<unsigned int>(
                    ScanInt(reinterpret_cast<int*>(&ratio.m_weight8)));
                reg.m_vec3c.push_back(ratio);
                reg.m_field48 += ratio.m_weight8;
            }
        }
        else if (token == "[regeneration price]")
        {
            int limit = ScanInt(&ok);       // ORIG 不校验 ok
            for (;;)
            {
                int group;
                if (!ScanInt(&group))
                {
                    break;
                }
                if (reg.m_map18.find(group) != reg.m_map18.end())
                {
                    printf("random option item script error %u, group : %d\n",
                           0x5c, group);
                    return 0x5d;
                }
                std::map<int, _str_cost>& costs = reg.m_map18[group];
                int key;
                do
                {
                    ScanInt(&key);          // 三连 ScanInt(int*)，均不校验（ORIG 同）
                    int a;
                    int b;
                    ScanInt(&a);
                    ScanInt(&b);
                    _str_cost cost(a, b);
                    costs[key] = cost;
                } while (key < limit);
            }
        }
    }
    return 0;
}

// ============================================================================
// StrikerScript::importScript —— ORIG T 0x08a9ddb2
// 报告 StrikerScript/importScript.md 全文转写。成功置 m_loaded=1 并返回 true。
// ============================================================================
bool StrikerScript::importScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string s;
    bool ok = false;

    while (ScanType(token, true))
    {
        if (token == "[tag mode]")
        {
            ScanInt(&ok);                           // 仅存局部（ORIG 同）
        }
        else if (token == "[tag cost time]")
        {
            m_tagCostTime = ScanInt(&ok);
        }
        else if (token == "[tag character setting]")
        {
            m_tagCharacterSetting = ScanInt(&ok);
        }
        else if (token == "[striker combo]")
        {
            m_strikerCombo = ScanInt(&ok);
        }
        else if (token == "[striker start time]")
        {
            m_strikerStartTime = ScanInt(&ok);
        }
        else if (token == "[striker skill data up]")
        {
            importSkillDataUpVector();
        }
        else if (token == "[striker skill attack penalty]")
        {
            importSkillAttackPenalty();
        }
        else if (token == "[striker skill cooltime penalty]")
        {
            importSkillCoolTimePenalty();
        }
        else if (token == "[striker level penalty]")
        {
            importParentLevelPenlaty();
        }
        else if (token == "[striker module impossible]")
        {
            m_moduleImpossible.clear();
            while (ScanStr(&s))
            {
                m_moduleImpossible.push_back(
                    static_cast<ContentsType::T>(ContentsType::getType(s.c_str())));
            }
        }
        else if (token == "[striker dungeon impossible]")
        {
            m_dungeonImpossible.clear();
            for (;;)
            {
                int dungeon;
                if (!ScanInt(&dungeon))
                {
                    break;
                }
                m_dungeonImpossible.push_back(dungeon);
            }
        }
        else if (token == "[striker end time]")
        {
            m_strikerEndTime = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[position]")
        {
            m_positionType = ScanInt(static_cast<bool*>(0));   // +0xac
            m_field38 = ScanInt(static_cast<bool*>(0));        // +0x38
            m_field3c = ScanInt(static_cast<bool*>(0));        // +0x3c
        }
        else if (token == "[striker skill]")
        {
            for (;;)
            {
                StrikerSkill skill;
                int first;
                if (!ScanInt(&first))
                {
                    break;                              // 扫描失败退出该段
                }
                int second = ScanInt(static_cast<bool*>(0));
                skill.m_field00 = ScanInt(static_cast<bool*>(0));
                skill.m_field04 = ScanInt(static_cast<bool*>(0));
                skill.m_field08 = ScanInt(static_cast<bool*>(0));
                ScanStr(&s);
                skill.m_name = s;
                // map 键 = 两个截断 ushort 打包（栈 -0x5c/-0x5a 对）
                int key = static_cast<int>(
                    static_cast<unsigned short>(first) |
                    (static_cast<unsigned int>(static_cast<unsigned short>(second))
                     << 16));
                std::map<int, std::vector<StrikerSkill>*>::iterator it =
                    m_skillMap.find(key);
                if (it != m_skillMap.end())
                {
                    it->second->push_back(skill);
                }
                else
                {
                    std::vector<StrikerSkill>* vec = new std::vector<StrikerSkill>;
                    vec->push_back(skill);
                    m_skillMap[key] = vec;
                }
            }
        }
        else if (token == "[striker skill macro]")
        {
            m_macroCommands.clear();
            for (;;)
            {
                int index;
                if (!ScanInt(&index))
                {
                    break;
                }
                StrikerSkillAddCommand cmd;
                cmd.m_field00 = index;
                cmd.m_field04 = ScanInt(static_cast<bool*>(0));
                cmd.setType(ScanInt(static_cast<bool*>(0)));
                m_macroCommands.push_back(cmd);
            }
        }
        else if (token == "[common cool time]")
        {
            m_commonCoolTime = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[direction type]")
        {
            m_direction.setType(ScanInt(static_cast<bool*>(0)));
        }
        else if (token == "[contents revision]")
        {
            m_contentsRevision.clear();
            while (ScanStr(&s))
            {
                ContentsRevision rev;
                rev.m_type = ContentsType::getType(s.c_str());
                rev.m_field04 = ScanInt(static_cast<bool*>(0));
                rev.m_field08 = ScanInt(static_cast<bool*>(0));
                rev.m_field0c = ScanInt(static_cast<bool*>(0));
                m_contentsRevision.push_back(rev);
            }
        }
        else if (token == "[upgrade revision]")
        {
            m_upgradeRevision = ScanInt(static_cast<bool*>(0));
        }
    }

    m_loaded = 1;
    return true;
}

// ============================================================================
// StageMapList::compare —— ORIG T 0x08364848（比较 +0 uniqueId）。
// ============================================================================
bool StageMapList::compare(const _StageMapSummary& a, const _StageMapSummary& b)
{
    return a.m_uniqueId < b.m_uniqueId;
}

// ---- stagemap 域镜像（布局取自 loadList 报告汇编偏移；类型 ORIG 真实存在，
//      完整建模归 stagemap 域批次）----
namespace stagemap
{

struct StageData
{
    int m_field0;                            // +0x00
    int m_field4;                            // +0x04 (ORIG StageData stride 0x10)
    int m_field8;                            // +0x08
    int m_fieldC;                            // +0x0c
};

struct DistrictData
{
    int m_field0;                            // +0x00
    std::vector<StageData> m_vec4;           // +0x04 (ORIG StageData vector)
    std::string m_appendImage;              // +0x10 (ORIG std::string size 4)
    int m_field14;                           // +0x14
};

struct AreaData
{
    int m_field0;                            // +0x00
    char m_pad04[0x1c];                      // +0x04
    std::vector<DistrictData> m_vec20;       // +0x20
    int m_field2c;                           // +0x2c
};

class StageMapScript
{
public:
    StageMapScript();                        // ORIG W 0x08373488
    ~StageMapScript();                       // ORIG W 0x0837f782
    void clear();                            // ORIG W 0x08373428

    char m_pad00[0x14];                      // +0x00（未还原字段区）
    std::vector<AreaData> m_areaVec;         // +0x14
};

}  // namespace stagemap

// ORIG T 0x088a9f28（定义待 stagemap 脚本域批次；本 TU 仅声明引用）
bool importStageMapScript(stagemap::StageMapScript* out, const char* path);

// ============================================================================
// StageMapList::loadList —— ORIG T 0x0836485c
// 报告 StageMapList/loadList.md 全文转写。
// ============================================================================
bool StageMapList::loadList()
{
    std::vector<_StageMapSummary> summaries;

    for (std::map<int, std::string>::iterator it =
             g_stageMapScriptFileList.m_fileMap.begin();
         it != g_stageMapScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        if (name == 0)
        {
            break;
        }

        stagemap::StageMapScript script;
        if (!importStageMapScript(&script, name))
        {
            printf("Stage Map Script Error: %s\n", name);
            return false;
        }

        for (size_t a = 0; a < script.m_areaVec.size(); ++a)
        {
            stagemap::AreaData& area = script.m_areaVec[a];
            int districtId = 0;
            for (size_t d = 0; d < area.m_vec20.size(); ++d)
            {
                stagemap::DistrictData& district = area.m_vec20[d];
                int stageIdx = 0;
                for (size_t s = 0; s < district.m_vec4.size(); ++s)
                {
                    _StageMapSummary sum;
                    sum.m_nextId = 0;
                    sum.m_stageId = district.m_vec4[s].m_field0;
                    sum.m_uniqueId =
                        makeStageMapUniqueId(area.m_field0, district.m_field0,
                                             stageIdx);
                    summaries.push_back(sum);
                    ++stageIdx;
                }
                districtId = district.m_field0;
            }
            districtId += 1;
            int stageIdx2 = 0;
            if (0 < area.m_field2c)
            {
                _StageMapSummary sum;
                sum.m_nextId = 1;
                sum.m_stageId = area.m_field2c;
                sum.m_uniqueId =
                    makeStageMapUniqueId(area.m_field0, districtId, stageIdx2);
                summaries.push_back(sum);
            }
        }
    }

    std::sort(summaries.begin(), summaries.end(), compare);

    size_t count = summaries.size();
    for (size_t i = 0; i < count; ++i)
    {
        if (i == 0)
        {
            m_firstStageId = summaries[0].m_stageId;
        }
        if (i != count - 1)
        {
            summaries[i].m_nextId = summaries[i + 1].m_stageId;
        }
        m_map.insert(std::make_pair(summaries[i].m_stageId, summaries[i]));
    }
    return true;
}

// ============================================================================
// APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript
// ORIG T 0x08894996（静态成员）。报告
// APSystem__CActionPointEtcParameter/importActionPointSystemEtcScript.md 全文转写。
// 返回 false=成功/文件失败；错误路径亦为 false（调用方仅判零，语义兼容；
// ORIG 各行级错误码见注释）。
// ============================================================================
namespace APSystem
{

bool CActionPointEtcParameter::importActionPointSystemEtcScript(
    const char* path, CActionPointEtcParameter& out)
{
    std::string token;
    bool ok = false;

    out.ClearData();
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            // EOF：把各组元素指针登记进 +0x18 引用表（ORIG 尾段循环）
            for (std::map<unsigned short, std::vector<CActionPointEx> >::iterator it =
                     out.m_actionPoints.begin();
                 it != out.m_actionPoints.end(); ++it)
            {
                std::vector<CActionPointEx>& vec = it->second;
                for (size_t i = 0; i < vec.size(); ++i)
                {
                    out.m_actionPointRefs.insert(
                        std::make_pair(it->first,
                                       static_cast<const CActionPointEx*>(&vec[i])));
                }
            }
            return false;
        }

        if (token == "[action point list]")
        {
            for (;;)
            {
                CActionPointEx ex;
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;                              // 回主循环
                }
                ex.m_field00 = static_cast<unsigned short>(v1);

                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ex.m_field2c = static_cast<unsigned char>(v2);

                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                *reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]) =
                    static_cast<unsigned short>(key);              // +0x0c map 键

                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }

                *reinterpret_cast<int*>(&ex.m_bytes02[0]) = v4;   // +0x02 int（组索引）
                if (!IsValidActionGroupIndex((_ActionGroupIndex)v4))
                {
                    return false;                       // ORIG 错误码 0x62
                }

                int v5 = ScanInt(&ok);
                if (!ok)
                {
                    return false;                       // 0x65
                }
                ex.m_bytes02[6] = static_cast<unsigned char>(v5);   // +0x08（后被覆写）

                for (int i = 0; i <= 4; ++i)            // days 区 B+0x06..+0x0a
                {
                    int day = ScanInt(&ok);
                    if (!ok)
                    {
                        return false;                   // 0x6d
                    }
                    if (day != 0)
                    {
                        ex.m_bytes02[4 + i] =
                            static_cast<unsigned char>(-static_cast<char>(day));
                    }
                }

                std::string itemList;
                if (!ScanStr(&itemList))
                {
                    return false;                       // 0x76
                }
                {
                    // strtok_r 按 "," 切分 atoi 入 vector<int>（B+0x14）
                    char* context = 0;
                    char* buf = &itemList[0];
                    for (char* tok = strtok_r(buf, ",", &context); tok != 0;
                         tok = strtok_r(0, ",", &context))
                    {
                        ex.m_itemList.push_back(atoi(tok));
                    }
                }

                std::string weekdayStr;
                if (!ScanStr(&weekdayStr))
                {
                    return false;                       // 0x9c
                }
                unsigned int weekday = 0;
                if (!ConvertWeekdayStringToInt(weekdayStr, weekday))
                {
                    weekday = 7;
                }
                ex.m_weekday = static_cast<int>(weekday);

                int b9 = ScanInt(&ok);
                if (!ok)
                {
                    return false;                       // 0xa4
                }
                ex.m_bytes02[7] = static_cast<unsigned char>(b9);   // +0x09（覆盖 days[3]）

                int b10 = ScanInt(&ok);
                if (!ok)
                {
                    return false;                       // 0xa7
                }
                ex.m_bytes02[0x0e] = static_cast<unsigned char>(b10);   // +0x10

                if (!ScanStr(&ex.m_str24))
                {
                    return false;                       // 0xaa
                }
                if (!ScanStr(&ex.m_str28))
                {
                    return false;                       // 0xad
                }

                std::map<unsigned short, std::vector<CActionPointEx> >::iterator fit =
                    out.m_actionPoints.find(*reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]));
                if (fit != out.m_actionPoints.end())
                {
                    fit->second.push_back(ex);
                }
                else
                {
                    std::vector<CActionPointEx> vec;
                    vec.push_back(ex);
                    out.m_actionPoints.insert(
                        std::make_pair(
                        *reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]), vec));
                }
            }
        }
        else if (token == "[action point user message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return false;                       // ORIG 错误码 199(0xc7)
                }
                out.m_rewardMessages.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point system message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return false;                       // ORIG 错误码 0xd5
                }
                out.m_rewardMessages2.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point medal reward]")
        {
            for (;;)
            {
                _MedalRewardItem medal;
                medal.m_field00 = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                medal.m_field02 = static_cast<unsigned short>(ScanInt(&ok));
                medal.m_field04 = ScanInt(&ok);
                medal.m_field08 = static_cast<unsigned short>(ScanInt(&ok));
                out.m_medalRewards.push_back(medal);
            }
        }
        else if (token == "[action point today reward]")
        {
            for (;;)
            {
                APSystem::_TodayRewardItem today;   // Wave5：表行=顶层类(0xe)
                int raw = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                today.m_itemIdx = raw - 1;
                if (!IsValidTodayRewardItem(*reinterpret_cast<const _SIG_LOAD_ACTION_POINT::_TodayRewardItem*>(&today)))  // 嵌套形参按值4B,首int同位(顶层0xe表行,ORIG import 五列中间态见 ScriptLeafA2 批)
                {
                    return false;                       // ORIG 错误码 0xfa
                }
                ScanInt(&ok);                           // 四个丢弃列（ORIG 存栈即弃）
                ScanInt(&ok);
                ScanInt(&ok);
                ScanInt(&ok);
                out.m_todayRewards.push_back(today);
            }
        }
    }
}

}  // namespace APSystem

// ============================================================================
// eventReward::ImportRewardScript —— ORIG T 0x080ef3a2
// 报告 eventReward/ImportRewardScript.md 全文转写。
// 令牌派发表为函数级静态 __lookup/__init_lock/__inited 三件套
// （[/reward]=0x8f [job type]=0x93 [sub type]=0xa3 [option]=0xaf
//   [equipment item]=0xb9 [stackable item]=0xd8 [/sub type]=0xfc
//   [/job type]=0x102 [/option]=0x107；未知 -1 忽略）。
// 返回：1=[/reward] 收尾或自然 EOF 且最后 ok=true；0=文件/解析失败
// （自然 EOF 返回最后一次 ScanInt 的 ok 态，ORIG ebx=local_18d 同语义）。
// ============================================================================
int eventReward::ImportRewardScript(const char* dir, const char* file)
{
    if (!loadRDARScriptFile(dir, file))
    {
        return 0;
    }

    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static char __inited = 0;

    std::string token;
    std::string tok2;
    bool ok = false;

    rewardKey key;                                  // 5 字节栈键（-0x1a9..-0x1a5）
    std::map<int, std::vector<itemData> > subMap;   // 每 jobtype 局部表
    itemData curItem;                               // [option] 累积区

    while (ScanType(token, true))
    {
        int id;
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            id = 0;                                 // 首遍登记态
        }
        else
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }

        if (!__inited)
        {
            // 首遍：登记全部键值（顺序与 ORIG 级联一致），随后按当前令牌派发
            __lookup.insert(std::make_pair("[/reward]", 0x8f));
            __lookup.insert(std::make_pair("[job type]", 0x93));
            __lookup.insert(std::make_pair("[sub type]", 0xa3));
            __lookup.insert(std::make_pair("[option]", 0xaf));
            __lookup.insert(std::make_pair("[equipment item]", 0xb9));
            __lookup.insert(std::make_pair("[stackable item]", 0xd8));
            __lookup.insert(std::make_pair("[/sub type]", 0xfc));
            __lookup.insert(std::make_pair("[/job type]", 0x102));
            __lookup.insert(std::make_pair("[/option]", 0x107));
            __init_lock.leave();
            __inited = 1;

            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }

        switch (id)
        {
        case 0x8f:      // [/reward]
            return 1;

        case 0x93:      // [job type]
        {
            ScanStr(&tok2);
            key.m_job = static_cast<unsigned char>(getCharacterJobToEnumValue(tok2));
            key.m_a = static_cast<unsigned char>(ScanInt(&ok));
            if (!ok)
            {
                return 0;
            }
            key.m_b = static_cast<unsigned char>(ScanInt(&ok));
            if (!ok)
            {
                return 0;
            }
            break;
        }

        case 0xa3:      // [sub type]
        {
            ScanStr(&tok2);
            key.m_sub.m_type = static_cast<unsigned char>(getSubKeyType(tok2));
            if (!ScanStr(&tok2))
            {
                return 0;
            }
            key.m_sub.m_action = static_cast<unsigned char>(getSubKeyAction(tok2));
            break;
        }

        case 0xaf:      // [option]
        {
            ScanStr(&tok2);
            getOptionType(tok2, curItem);
            break;
        }

        case 0xb9:      // [equipment item]
        {
            defaultOption(key.m_sub.m_action, curItem);
            for (;;)
            {
                itemData item = curItem;            // 栈副本
                const int findKey = ScanInt(&ok);   // ORIG 键为独立局部 int（不占 itemData）
                if (!ok)
                {
                    goto next_token;                // 回主循环
                }
                item.m_itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::map<int, std::vector<itemData> >::iterator fit =
                    subMap.find(findKey);
                if (fit == subMap.end())
                {
                    std::vector<itemData> vec;
                    vec.push_back(item);
                    subMap.insert(std::make_pair(findKey, vec));
                }
                else
                {
                    fit->second.push_back(item);
                }
            }
        }

        case 0xd8:      // [stackable item]
        {
            curItem.m_count = 1;                    // 段首一次性置 1（ORIG 同）
            defaultOption(key.m_sub.m_action, curItem);
            for (;;)
            {
                itemData item = curItem;
                const int findKey = ScanInt(&ok);   // ORIG 键为独立局部 int
                if (!ok)
                {
                    goto next_token;
                }
                item.m_itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                item.m_count = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::map<int, std::vector<itemData> >::iterator fit =
                    subMap.find(findKey);
                if (fit == subMap.end())
                {
                    std::vector<itemData> vec;
                    vec.push_back(item);
                    subMap.insert(std::make_pair(findKey, vec));
                }
                else
                {
                    fit->second.push_back(item);
                }
            }
        }

        case 0xfc:      // [/sub type]
        {
            m_map0.insert(std::make_pair(key, subMap));
            key.m_sub.clear();                      // rewardSubKey::clear 仅清子键
            subMap.clear();
            break;
        }

        case 0x102:     // [/job type]
        {
            key.clear();                            // rewardKey::clear
            subMap.clear();
            break;
        }

        case 0x107:     // [/option]
        {
            curItem.clear();                        // itemData::clear（W 0x080f1162，call 实证）
            break;
        }

        default:        // 未知令牌忽略
            break;
        }

    next_token:;
    }
    // 自然 EOF：返回最后一次 ok 态（ORIG ebx=local_18d）
    return ok ? 1 : 0;
}

// ============================================================================
// importUpgradeTable —— ORIG T 0x08900c26
// 报告 _lt_global_gt_/importUpgradeTable.md 全文转写。
// 返回 1=成功（含 EOF）；0=失败（文件打开失败/关键扫描失败/键重复/异常捕获）。
//
// 【batch-15/16 验收标注（§6.2 口径）】orig 1119 / ours 1166（gap -132 → -47）。
//   ① 调用点差集（P167）**已归零**：direct 138 ↔ 136，missing 14 / extra 12 全部
//      是 C2/D2 ↔ C1/D1 同体构造/析构符号名对称差 + 1 个 _Unwind_Resume，
//      无任何真实调用点缺口；indirect 0/0。
//   ② 段边界切片法（difflib 在 N82 帧号重排下不可信）逐 token 段核对，修掉两处
//      真语义偏差：[cost weight by upgrade level] 的取数通道（ORIG 用
//      ScanInt(bool*) 返回值 + 循环头 ok 复位，非 ScanInt(int*) 出参形）、
//      [amplification const] 循环入口缺 `ok = true` 复位（orig local_cb）；
//      并删除匿名 namespace 的 inline helper（-O0 逐点发 call，见 N-I 延伸）。
//   ③ 余 -47 集中在三处已定位的 4.1 专属形：0x089003fa 的守卫 bool 汇聚岛
//      （实测命名 bool 反而 +12，已回退）、0x08900d5a 的双段 catch
//      （begin_catch/end_catch + 清理后 rethrow 的 EH 展开表形）、
//      0x0890124x 的成功出口 jmp/nop 中继 ⇒ N3/N82/setcc-fold/跨版本 EH
//      命中 ⇒ 判**语义完成、byte 层封顶**。
//   ④ 三处「看起来更像 ORIG 但实测更远」的改写已回退并在原地留数值记录
//      （get_upgrade_info 逐行重发 gain 471、dword 下标视图 326、
//        命名 bool 汇聚岛 1177），避免后续批次重试。
// ============================================================================
// upgrade_info_t 的表行写入使用**真实字段下标直访**（batch-15 N-I 延伸：
// 匿名 namespace 的 inline helper 在 -O0 下同样逐点发 call，不豁免）；偏移→字段
// 映射按 upgrade_info_t 已建分段具名成员（m_prob00[8]/m_prob20[3]/+0x2c..34 三项/
// m_w38[5]/m_w4c[4]/m_w5c[4]，合计恰 0x6c）。

int importUpgradeTable(const char* path, upgrade_table_t* table)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    try
    {
        std::string token;
        std::string rarityName;
        bool ok = false;

        for (;;)
        {
            if (!ScanType(token, true))
            {
                return 1;                               // EOF → 成功
            }

            if (token == "[table]")
            {
                table->m_infos.clear();
                for (;;)
                {
                    upgrade_info_t info;
                    info.m_prob00[0] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[5] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[6] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_prob00[4] = 0;
                    info.m_prob00[7] = ScanInt(&ok);
                    info.m_prob20[0] = ScanInt(&ok);
                    info.m_prob20[1] = ScanInt(&ok);
                    info.m_prob20[2] = ScanInt(&ok);
                    info.m_recipeItem = ScanInt(&ok);
                    info.m_meterialCount = 0;
                    info.m_w38[4] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    info.m_w5c[0] = 0.0f;
                    info.m_w4c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_w5c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_w4c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_w5c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_w4c[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    table->m_infos.push_back(info);
                }
            }
            else if (token == "[amplification const]")
            {
                size_t index = 0;
                ok = true;                    // ORIG local_cb = true（入循环前置真）
                for (;;)
                {
                    // ORIG 条件本体是 `ok == false || size() <= index` 的合并短路
                    // （0x08901020 起 + bool 汇聚岛）。ok 复位为 true 的语义已补（下方）；
                    // 汇聚岛在 4.4 下改命名 bool 反而多 12 insn（实测 1165→1177），
                    // 故保留直判形，岛形差记 N3/setcc-fold。
                    if (index >= table->m_infos.size())
                    {
                        break;
                    }
                    upgrade_info_t& info = table->m_infos[index];
                    ++index;
                    for (int i = 0; i <= 4; ++i)
                    {
                        if (i == 0)
                        {
                            info.m_w38[i] = 0;
                        }
                        else
                        {
                            info.m_w38[i] = ScanInt(&ok);
                        }
                        if (!ok)
                        {
                            break;
                        }
                    }
                }
                if (!ScanType(token, true))
                {
                    LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                          __FUNCTION__, 0xb3a,
                                          "ERROR [amplification const] count %d",
                                          static_cast<int>(index));
                }
            }
            else if (token == "[cost weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x1b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb48, "importUpgradeTable - invalid cost rarity table");
                    }
                }
            }
            else if (token == "[stat weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x20) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb5a, "importUpgradeTable - invalid stat rarity table");
                    }
                }
            }
            else if (token == "[type]")
            {
                for (int i = 10; i <= 0x15; ++i)
                {
                    UPG_TBL_F32(table, i + 0x4) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb6c, "importUpgradeTable - invalid type table");
                    }
                }
            }
            else if (token == "[level]")
            {
                for (int i = 0; i <= 3; ++i)
                {
                    UPG_TBL_F32(table, i) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb7e, "importUpgradeTable - invalid level table");
                    }
                }
            }
            else if (token == "[cost]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_I32(table, i + 0x26) = ScanInt(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb90, "importUpgradeTable - invalid gold table");
                    }
                }
            }
            else if (token == "[notice]")
            {
                UPG_TBL_I32(table, 0x8a) = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xb9e, "importUpgradeTable - invalid notice tag");
                }
            }
            else if (token == "[repair cost rate by upgrade level]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_F32(table, i + 0x9b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        if (i != 0x63)
                        {
                            LogManager::logFormat(
                                0, "../RDARScriptEtcParameter.cpp",
                                __FUNCTION__, 0xbac,
                                "ERROR : [repair cost rate by upgrade level] size %d / %d",
                                i, 100);
                        }
                        break;
                    }
                }
            }
            else if (token == "[destroy level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0xfe) = level;
                }
            }
            else if (token == "[max upgrade level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0x104) = level;
                }
            }
            else if (token == "[disjoint]")
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                    {
                        throw static_cast<unsigned int>(0xbdc);
                    }
                    if (token == "[/disjoint]")
                    {
                        break;
                    }
                    if (token == "[disjoint bonus item]")
                    {
                        UPG_TBL_I32(table, 0x8b) = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                    }
                    else if (token == "[equip level const]")
                    {
                        UPG_TBL_I32(table, 0x8c) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbea);
                        }
                    }
                    else if (token == "[upgrade failed bonus weight by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            float weight = ScanFloat(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xbf4);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xbf7);
                            }
                            UPG_TBL_F32(table, rarity + 0x8d) = weight;
                        }
                    }
                    else if (token == "[upgrade const for bonus item count]")
                    {
                        UPG_TBL_I32(table, 0x93) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbff);
                        }
                    }
                    else if (token == "[correction grade by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            int grade = ScanInt(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xc09);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xc0c);
                            }
                            UPG_TBL_I32(table, rarity + 0x94) = grade;
                        }
                    }
                }
            }
            else if (token == "[destruction result]")
            {
                for (int t = 10; t <= 0x15; ++t)
                {
                    UPG_TBL_F32(table, t + 0x10b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc26, "importUpgradeTable - invalid type table");
                    }
                }
                int destroyIdx = ScanInt(&ok);
                if (!ok)
                {
                    return 1;                           // ORIG jmp EOF 成功出口
                }
                table->m_destruction.m_field58 = destroyIdx;
                for (;;)
                {
                    STResultItemCnt cnt;
                    cnt.m_a = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_b = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_c = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    table->m_destruction.m_vec.push_back(cnt);
                }
            }
            else if (token == "[cost weight by upgrade level]")
            {
                for (;;)
                {
                    // ORIG 0x08901e81 起：每轮循环头先 `local_d9 = false` 复位 ok，
                    // 数值走 **ScanInt(bool*)/ScanFloat(bool*) 的返回值通道**
                    // （Ghidra `local_e0 = (float)ScanInt(&local_d9)`、
                    // `lVar8 = (longdouble)ScanFloat(&local_d9)`），ok 仅作
                    // 「!= true → 退出/返回 0」判定。此前写成 ScanInt(int*) 的
                    // 出参通道（P170：mangled 参数类型须 nm 实查），使本函数
                    // 引用了 ORIG 未调用的 _Z7ScanIntPi。
                    ok = false;
                    int level = ScanInt(&ok);
                    if (ok != true)
                    {
                        break;
                    }
                    float weight = ScanFloat(&ok);
                    if (ok != true)
                    {
                        return 0;
                    }
                    std::pair<std::map<int, float>::iterator, bool> ins =
                        table->m_floatMap.insert(std::make_pair(level, weight));
                    if (!ins.second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc54,
                            "[cost weight by upgrade level] key is duplicated]");
                        return 0;
                    }
                }
            }
            else if (token == "[upgrade machine frame base time]")
            {
                if (!ScanInt(&table->m_zero4b4[0]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[1]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[2]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[3]))
                {
                    return 0;
                }
            }
            else if (token == "[upgrade machine frame rate]")
            {
                for (;;)
                {
                    int key;
                    if (!ScanInt(&key))
                    {
                        break;
                    }
                    upgrade_table_t::MachineFrameInfo info;
                    if (!ScanFloat(&info.m_f00))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f04))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f08))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f0c))
                    {
                        return 0;
                    }
                    if (!ScanInt(&info.m_i10))
                    {
                        return 0;
                    }
                    // ORIG 0x08901e35：直接就地构造 value_type
                    // （_ZNSt4pairIKiN15upgrade_table_t16MachineFrameInfoEEC1IRiRS2_EEOT_OT0_
                    //  = pair<const int,MachineFrameInfo>::pair<int&,MachineFrameInfo&>）
                    // 后 _Hashtable::insert(value_type const&)，无 make_pair 转换临时。
                    // （上面 m_floatMap 那处 ORIG 确为 make_pair+转换，保持不动。）
                    table->m_machineFrames.insert(
                        std::unordered_map<int, upgrade_table_t::MachineFrameInfo>::value_type(
                            key, info));
                }
            }
        }
    }
    catch (unsigned int errLine)
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xc14, "Error: (%s):%d",
                              "importUpgradeTable", static_cast<int>(errLine));
        return 0;
    }
}

// ============================================================================
// stLevelUpRewardItemScript::clear —— ORIG W 0x083711d6
// 【报告缺失】无专属函数报告；已先读调用者报告
// _lt_global_gt_/ImportLevelUpRewardItemScript.md 后按 nm 地址 objdump 转写：
//   83711dc mov 0x8(%ebp),%eax ; push eax
//   83711e2 call std::map<LevelUpRewardItemTableKey,LevelUpRewardItemData>::clear
// 即 m_map0.clear()。
// ============================================================================
void stLevelUpRewardItemScript::clear()
{
    m_map0.clear();
}

// ============================================================================
// seriaRoomDecoAniEventScript::clear —— ORIG W 0x083732a4
// 【报告缺失】无专属函数报告；已先读同类报告
// _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md 与 ORIG ctor 0x083731ca
// （objdump）确认五成员布局后转写：
//   vector@+0x00 clear / map@+0x0c clear / map@+0x24 clear /
//   map@+0x40 clear / string@+0x3c clear。
// ============================================================================
void seriaRoomDecoAniEventScript::clear()
{
    m_rewardList.clear();
    m_itemMap.clear();
    m_aniList.clear();
    m_finishAniMap.clear();
    m_finishSound.clear();
}

// ============================================================================
// CItem::GetMedalPrice —— ORIG W 0x0822c862
// 【报告缺失】无专属函数报告；已先读调用者报告
// DisPatcher_RentAvatar/dispatch_sig.md 与 DisPatcher_BuyItem/get_data.md
// （与 GetCashPrice/get_price 并列的价格访问器）后按 nm 地址 objdump 转写：
//   mov 0x8(%ebp),%eax ; mov 0x14(%eax),%eax ; ret —— 返回 this+0x14 int。
// ============================================================================
int CItem::GetMedalPrice() const
{
    return m_field14;   // +0x14
}
