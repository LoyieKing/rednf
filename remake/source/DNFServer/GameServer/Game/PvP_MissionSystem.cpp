#include "CDataManager.h"
#include "CMission.h"
#include "PvPTypes.h"          // CMTRand
#include "DNFLexWrapper.h"     // loadRDARScriptFile / Scan* helper
#include <stdio.h>
#include "SyncScriptDeps.h"   // checkValiedJobName / getCharacterJob
#include "NpcScript.h"

// ============================================================================
// PvP_MissionSystem 装载支撑（G5 批次）。
// ORIG 各函数地址/语义见 docs/class_func_reports/PvP_MissionSystem/*.md。
// ============================================================================

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局 STScriptFileList（PvP 任务脚本列表，BSS 0x0949…）。由
// CDataManager::init 的 loadRDARScriptListFile 填充；此处提供符号。
STScriptFileList g_PvpMissionScriptFileList;
// ORIG 全局（BSS 0x09500100，0x100 字节）：PvP 任务脚本根目录（运行期由
// 部署环境填充；BSS 初值即空串）。文件列表全局见上方 g_PvpMissionScriptFileList。
char g_PvpMissionScriptBaseDirectory[0x100];

// ---- ORIG _ZN13MissionScript5clearEv @0x08a5bbb2 ----
void MissionScript::clear()
{
    m_gradeMin = 0;              // +0x10/+0x14 pair(0,0)
    m_gradeMax = 0;
    m_field20 = 4;               // +0x20
    m_job = 0xb;                 // +0x24（无职业）
    m_field28.clear();
    m_field34 = 0x10;            // +0x34（默认 kind）
    m_field00 = -1;              // +0x00（[next mission]）
    m_prevMissionIndex = -1;     // +0x04（[prev mission]）
    m_str38.clear();
    m_str3c.clear();
    m_rewardItems.clear();
    m_expRate = 0.0f;            // +0x40
    m_stealingSkill = 0;         // +0x5c
    m_rewardSkill = 0;           // +0x5d
    m_rewardSp = 0;              // +0x5e
    m_comboMap.clear();
    m_field08 = 0;
    m_field0c = 0;
}

// ---- ORIG _ZN22MissionParameterScript5clearEv @0x08a5bca2 ----
void MissionParameterScript::clear()
{
    m_dailyIndices.clear();      // +0x00
    m_field0c = 0;
    m_dailyList.clear();         // +0x10
    m_field18 = 0;
    m_baseExp.clear();           // +0x1c
}

// ---- ORIG _Z13_ReadThreeIntP13MissionScript @0x08a5ab91 ----
// 条件参数三连读：+0x78 / +0x7c / +0x80。
void _ReadThreeInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
}

// ---- ORIG _Z12_ReadFourIntP13MissionScript @0x08a5abd2 ----
// [within] 条件四连读：+0x78 / +0x7c / +0x80 / +0x84。
void _ReadFourInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
    script->m_field84 = ScanInt((bool*)0);
}

// ============================================================================
// importPvpMissionScript（ORIG _Z22importPvpMissionScriptP13MissionScriptPKc，
// 0x8a5ac28）。反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionScript.md。返回 false=装载/解析失败；true=成功或 EOF 正常收尾。
// ============================================================================
bool importPvpMissionScript(MissionScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_PvpMissionScriptBaseDirectory, path))
        return false;

    script->clear();
    script->m_field0c =
        g_PvpMissionScriptFileList.FindIndexByFullScan(path);

    std::string token;
    std::string sub;
    std::string text;

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

        if (token == "[rank range]")
        {
            script->m_gradeMin = ScanInt((bool*)0);   // +0x10
            script->m_gradeMax = ScanInt((bool*)0);   // +0x14
        }
        else if (token == "[kind]")
        {
            script->m_field08 = ScanInt((bool*)0);    // +0x08
        }
        else if (token == "[rate range]")
        {
            script->m_rateMin = ScanInt((bool*)0);    // +0x18
            script->m_rateMax = ScanInt((bool*)0);    // +0x1c
        }
        else if (token == "[grade]")
        {
            ScanStr(&token);   // ORIG 复用同一 token 缓冲
            if (token == "[normal]")
                script->m_field20 = 0;
            else if (token == "[repeatable]")
                script->m_field20 = 1;
            else if (token == "[daily]")
                script->m_field20 = 2;
            else if (token == "[rank up]")
                script->m_field20 = 3;
        }
        else if (token == "[target character]")
        {
            if (!ScanStr(&sub))
                return true;                     // ORIG 扫描失败→正常收尾
            if (!checkValiedJobName(sub.c_str()))
            {
                printf("parsing error : %s\n", sub.c_str());
                return false;
            }
            script->m_job = getCharacterJob(sub.c_str());   // +0x24
        }
        else if (token == "[pre required mission]")
        {
            for (;;)
            {
                bool ok = false;
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_field28.push_back(idx);       // +0x28
            }
        }
        else if (token != "[condition]")
        {
            if (token == "[prev mission]")
                script->m_prevMissionIndex = ScanInt((bool*)0);   // +0x04
            else if (token == "[next mission]")
                script->m_field00 = ScanInt((bool*)0);            // +0x00
            else if (token == "[name_text]")
            {
                ScanStr(&text);
                script->m_str38 = text;                           // +0x38
            }
            else if (token == "[cond_text]")
            {
                ScanStr(&text);
                script->m_str3c = text;                           // +0x3c
            }
            else if (token == "[reward item]")
            {
                for (;;)
                {
                    bool ok = false;
                    int itemIdx = ScanInt(&ok);
                    if (!ok)
                        break;
                    int count = ScanInt(&ok);
                    if (!ok)
                        return false;
                    script->m_rewardItems.push_back(
                        std::make_pair(itemIdx, count));          // +0x44
                }
            }
            else if (token == "[reward skill]")
                script->m_rewardSkill = 1;                        // +0x5d
            else if (token == "[reward sp]")
                script->m_rewardSp = 1;                           // +0x5e
            else if (token == "[difficulty_ratio]")
                script->m_expRate = ScanFloat((bool*)0);          // +0x40
            else if (token == "[stealing skill]")
                script->m_stealingSkill = 1;                      // +0x5c
        }
        else
        {
            // ---- [condition] 子段 ----
            if (!ScanStr(&sub))
                return true;                 // ORIG 扫描失败→正常收尾

            if (sub == "[winning count]")         { script->m_field34 = 0x0; _ReadThreeInt(script); }
            else if (sub == "[winning streak]")   { script->m_field34 = 0x1; _ReadThreeInt(script); }
            else if (sub == "[join]")             { script->m_field34 = 0x2; _ReadThreeInt(script); }
            else if (sub == "[revenge]")          { script->m_field34 = 0x7; _ReadThreeInt(script); }
            else if (sub == "[kill]")             { script->m_field34 = 0x9; _ReadThreeInt(script); }
            else if (sub == "[kill count]")       { script->m_field34 = 0x3; _ReadThreeInt(script); }
            else if (sub == "[hereafter join]")   { script->m_field34 = 0x4; _ReadThreeInt(script); }
            else if (sub == "[hereafter winning count]") { script->m_field34 = 0x5; _ReadThreeInt(script); }
            else if (sub == "[remain HP]")        { script->m_field34 = 0x6; _ReadThreeInt(script); }
            else if (sub == "[hereafter kill count]") { script->m_field34 = 0x8; _ReadThreeInt(script); }
            else if (sub == "[within]")           { script->m_field34 = 0xb; _ReadFourInt(script); }
            else if (sub == "[remainS HP]")       { script->m_field34 = 0xc; _ReadThreeInt(script); }
            else if (sub == "[move_channel]")     { script->m_field34 = 0xd; _ReadThreeInt(script); }
            else if (sub == "[move_channel_total]") { script->m_field34 = 0xe; _ReadThreeInt(script); }
            else if (sub == "[combo clear]")
            {
                script->m_field34 = 0xf;
                std::string comboTok;
                for (;;)
                {
                    if (!ScanType(comboTok, true))
                        break;               // 内层 EOF → 回外层关键字循环

                    combo_script_key key;    // ctor：job=0xb/growType=0
                    combo_script_data data;  // ctor

                    if (comboTok == "[job]")
                    {
                        if (!ScanStr(&sub))
                        {
                            printf("parsing error %d, %s\n", 0x129,
                                   sub.c_str());
                            return false;
                        }
                        if (!checkValiedJobName(sub.c_str()))
                        {
                            printf("parsing error : %s\n", sub.c_str());
                            return false;
                        }
                        key.m_job = (char)getCharacterJob(sub.c_str());
                        key.m_growType = (short)ScanInt((bool*)0);
                        int n = ScanInt((bool*)0);
                        std::pair<int, int> fill;
                        data.m_rewardList.resize(n, fill);
                        for (int i = 0; i < n; ++i)
                        {
                            data.m_rewardList[i].first = ScanInt((bool*)0);
                            data.m_rewardList[i].second = ScanInt((bool*)0);
                        }

                        if (!ScanType(comboTok, true))
                            continue;        // ORIG esi=1 → 循环头再读一次

                        if (comboTok == "[reward item]")
                        {
                            // ORIG 读入两个 int 后未使用（忠实转写）。
                            (void)ScanInt((bool*)0);
                            (void)ScanInt((bool*)0);
                        }
                        (void)ScanType(comboTok, true);   // 再读一枚，结果忽略

                        if (script->m_comboMap.find(key) ==
                            script->m_comboMap.end())
                            script->m_comboMap.insert(
                                std::make_pair(key, data));
                        // 非 [job] 分支与插入完成均继续内层循环。
                    }
                }
            }
        }
    }
}

// ============================================================================
// importPvpMissionParameterScript（ORIG
// _Z31importPvpMissionParameterScriptR22MissionParameterScript，0x8a5b7ec）。
// 反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionParameterScript.md。
// ============================================================================
bool importPvpMissionParameterScript(MissionParameterScript& param)
{
    param.clear();
    if (!loadRDARScriptFile("Etc/", "MissionSystem.etc"))
        return false;

    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[frequent daily mission]")
        {
            int count = ScanInt(&ok);
            if (!ok)
                return false;
            for (int i = 0; i < count; ++i)
            {
                unsigned int idx = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
                param.m_dailyIndices.push_back(idx);      // +0x00
            }
        }
        else if (token == "[casual daily mission]")
        {
            param.m_field0c = ScanInt(&ok);               // +0x0c
            if (!ok)
                return false;
            for (;;)
            {
                int weight = ScanInt(&ok);
                if (!ok)
                    break;                                 // ORIG → 回外层循环
                int missionIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                param.m_dailyList.push_back(
                    std::make_pair((unsigned int)weight,
                                   (unsigned int)missionIdx));   // +0x10
                param.m_field18 += weight;                      // +0x18
            }
        }
        else if (token == "[mission reward exp table]")
        {
            param.m_baseExp.resize(0x1e);                 // +0x1c
            for (int i = 0; i < 0x1e; ++i)
            {
                unsigned int v = (unsigned int)ScanInt(&ok);
                param.m_baseExp[i] = v;
                if (!ok)
                    return false;
            }
        }
    }
    return true;
}

// ============================================================================
// 构造（ORIG 0x85e1abc）
// 顺序：map@+0x00、vector<list<CMission*>>@+0x18、vector<uint>@+0x24、
//       list<pair<uint,uint>>@+0x34、bitset@+0x40、vector<uint>@+0x48；
//       m_trand = new CMTRand(time(0))；m_field3c=0；reset bitset；
//       m_kindMissionList.resize(0x24)。
// ============================================================================
// ORIG 0x85e1abc：+0x30(m_field30)/+0x3c(m_field3c)/+0x44(m_trand) 仅
// 构造函数体内赋值一次，初始化列表不产生写入（m_field30 保持垃圾，与 ORIG 一致）。
PvP_MissionSystem::PvP_MissionSystem()
    : m_missionMap(),
      m_kindMissionList(),
      m_dailyMissionIndices(),
      m_dailyMissionList(),
      m_dailyMissionKind(),
      m_baseMissionExp()
{
    CMTRand* rand = new CMTRand();
    rand->seed((unsigned long)time(0));   // ORIG CMTRand(time(0))：initialize+reload 覆盖全部状态
    m_trand = rand;
    m_field3c = 0;
    m_dailyMissionKind.reset();
    m_kindMissionList.resize(0x24);   // ORIG resize 0x24 个 list<CMission*>
}

// ============================================================================
// 析构（ORIG 0x85e1c48）
// 顺序：delete m_trand；m_trand=0；遍历 m_missionMap delete 各 CMission；
//       m_missionMap.clear(); 其余成员默认析构。
// ============================================================================
PvP_MissionSystem::~PvP_MissionSystem()
{
    if (m_trand != 0)
        delete (CMTRand*)m_trand;
    m_trand = 0;
    puts("Mission System Delete");   // ORIG 0x85e1c72-0x85e1c79

    for (std::map<int, CMission*>::iterator it = m_missionMap.begin();
         it != m_missionMap.end(); ++it)
    {
        CMission* m = it->second;
        if (m != 0)
            delete m;
    }
    m_missionMap.clear();
}

// ============================================================================
// Factory（ORIG 0x85e2132）
// 依 script.m_field34（m_kind 的派生分派）switch 构造对应 CMission_* 子类。
// 各子类 ctor 实参布局（script 偏移）：
//   +0x7c count / +0x80 第二参数 / +0x84 第三参数 / +0x60 comboMap。
// ============================================================================
CMission* PvP_MissionSystem::Factory(const MissionScript& script)
{
    CMission* result = 0;
    switch (script.m_field34)
    {
    case 0:   result = new CMission_winning_count(script, script.m_field7c); break;
    case 1:   result = new CMission_winning_streak(script, script.m_field7c, script.m_field80); break;
    case 2:   result = new CMission_join(script, script.m_field7c); break;
    case 3:   result = new CMission_kill_count(script, script.m_field7c); break;
    case 4:   result = new CMission_hereafter_join(script, script.m_field7c); break;
    case 5:   result = new CMission_hereafter_winning_count(script, script.m_field7c); break;
    case 6:   result = new CMission_remain_hp(script, script.m_field7c, script.m_field80); break;
    case 7:   result = new CMission_revenge(script, script.m_field7c); break;
    case 8:   result = new CMission_hereafter_kill_count(script, script.m_field7c); break;
    case 9:   result = new CMission_kill(script, script.m_field7c, script.m_field80); break;
    case 0xb: result = new CMission_within(script, script.m_field7c, script.m_field80, script.m_field84); break;
    case 0xc: result = new CMission_remains_hp(script, script.m_field7c, script.m_field80); break;
    case 0xd: result = new CMission_move_channel(script, script.m_field7c); break;
    case 0xe: result = new CMission_move_channel_total(script, script.m_field7c); break;
    case 0xf: result = new CMission_combo_clear(script, script.m_comboMap); break;
    default:  result = 0; break;
    }
    return result;
}

// ============================================================================
// Load_list（ORIG 0x85e1dc6）
// 遍历 g_PvpMissionScriptFileList.m_fileMap，逐条 import，成功则 Factory、
// 按 GetMissionIndex 插入 m_missionMap、按 GetMissionKind 挂入 m_kindMissionList、
// kind==2 时置 m_dailyMissionKind 位；最后 sort_list。
// ============================================================================
bool PvP_MissionSystem::Load_list()
{
    MissionScript script;
    for (std::map<int, std::string>::const_iterator it =
             g_PvpMissionScriptFileList.m_fileMap.begin();
         it != g_PvpMissionScriptFileList.m_fileMap.end(); ++it)
    {
        const char* path = it->second.c_str();
        if (path == 0)
            break;
        if (!importPvpMissionScript(&script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        CMission* mission = Factory(script);
        if (mission == 0)
            return false;
        int idx = mission->GetMissionIndex();
        m_missionMap[idx] = mission;
        unsigned int kind = mission->GetMissionKind();
        if (kind < m_kindMissionList.size())
            m_kindMissionList[kind].push_back(mission);
        int grade = mission->GetMissionGrade();
        if (grade == 2)
            m_dailyMissionKind.set(
                (unsigned int)mission->GetMissionKind() & 0x1f);
    }
    sort_list();
    return true;
}

// ============================================================================
// Load_EtcParameter（ORIG 0x85e1fee）
// importPvpMissionParameterScript 成功则 copy_missionParameter。
// ============================================================================
bool PvP_MissionSystem::Load_EtcParameter()
{
    MissionParameterScript param;
    bool ok = importPvpMissionParameterScript(param);
    if (ok)
        copy_missionParameter(param);
    else
        puts("Mission Parameter Error!");
    return ok;
}

// ============================================================================
// copy_missionParameter（ORIG 0x85e206e）
//   m_dailyMissionIndices(+0x24) = param.m_dailyIndices(+0x00)
//   m_field30(+0x30)             = param.m_field0c
//   m_dailyMissionList(+0x34)    = param.m_dailyList(+0x10)
//   m_field3c(+0x3c)             = param.m_field18
//   m_baseMissionExp(+0x48)      = param.m_baseExp(+0x1c)
// ============================================================================
void PvP_MissionSystem::copy_missionParameter(const MissionParameterScript& param)
{
    m_dailyMissionIndices = param.m_dailyIndices;
    m_field30 = param.m_field0c;
    m_dailyMissionList = param.m_dailyList;
    m_field3c = param.m_field18;
    m_baseMissionExp = param.m_baseExp;
}

CMission* PvP_MissionSystem::find_mission(int idx)
{
    std::map<int, CMission*>::iterator it = m_missionMap.find(idx);
    return it == m_missionMap.end() ? 0 : it->second;
}

// ORIG 0x85e2f00：遍历 m_kindMissionList[0x1b]，首个 GetRankRange().gradeMin == idx
// 的 CMission 返回其 GetMissionIndex()；无匹配返回 0。
int PvP_MissionSystem::get_WithinMissionIndex(int idx) const
{
    const std::list<CMission*>& list = m_kindMissionList[0x1b];
    for (std::list<CMission*>::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        if (!*it)
            continue;
        MissionRankRange grade = (*it)->GetRankRange();
        if (grade.gradeMin == idx)
            return (*it)->GetMissionIndex();
    }
    return 0;
}

// ORIG 0x85e26a0：else 分支 operator[](0)（m_kindMissionList[0]，非静态空表）。
std::list<CMission*>& PvP_MissionSystem::get_kind_mission_list(unsigned int idx)
{
    return idx < m_kindMissionList.size() ? m_kindMissionList[idx] : m_kindMissionList[0];
}

std::bitset<32> PvP_MissionSystem::get_daily_mission_kind() const
{
    return m_dailyMissionKind;
}

// ORIG 0x85e2e74：越界时 cMyTrace 记录 InvalidRank 并返回 0。
int PvP_MissionSystem::get_BaseMissionExp_byRank(unsigned short rank) const
{
    if (rank >= m_baseMissionExp.size())
    {
        cMyTrace(
            "unsigned int PvP_MissionSystem::get_BaseMissionExp_byRank(short unsigned int) const",
            0x162, 5)
            ("[@missionSystem] <InvalidRank : %d>", rank);
        return 0;
    }
    return static_cast<int>(m_baseMissionExp[rank]);
}

void PvP_MissionSystem::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    // ORIG 0x85e26dc 逐指令转写：三段——①dailyMissionIndices→map 查 mission
    // 推 {kind,idx}；②按 m_dailyMissionList 加权随机补 m_field30 个；③bitset
    // 校验 kind 范围/重复，越界或重复打印 cMyTrace 并清空 out。
    out.clear();
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i)
    {
        const unsigned int id = m_dailyMissionIndices[i];
        std::map<int, CMission*>::iterator it =
            m_missionMap.find(static_cast<int>(id));
        if (it == m_missionMap.end())
            continue;
        CMission* mission = it->second;
        if (mission == 0)
            continue;
        out.push_back(std::make_pair(
            static_cast<unsigned int>(mission->GetMissionKind()),
            static_cast<unsigned int>(mission->GetMissionIndex())));
    }
    for (int j = 0; j < m_field30; ++j)
    {
        std::list<std::pair<unsigned int, unsigned int> > tmp(m_dailyMissionList);
        int weight = m_field3c;
        int r = static_cast<CMTRand*>(m_trand)->randInt() % weight;
        int sum = 0;
        for (std::list<std::pair<unsigned int, unsigned int> >::iterator li =
                 tmp.begin(); li != tmp.end(); ++li)
        {
            sum += static_cast<int>(li->first);
            if (r < sum)
            {
                const unsigned int missionIdx = li->second;
                std::map<int, CMission*>::iterator mi =
                    m_missionMap.find(static_cast<int>(missionIdx));
                if (mi != m_missionMap.end() && mi->second != 0)
                {
                    CMission* m = mi->second;
                    out.push_back(std::make_pair(
                        static_cast<unsigned int>(m->GetMissionKind()),
                        static_cast<unsigned int>(m->GetMissionIndex())));
                }
                weight -= static_cast<int>(li->first);
                tmp.erase(li);
                break;
            }
        }
        if (weight <= 0)
        {
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x123, 5)
                ("InvalidMaxDice %d", weight);
        }
    }
    std::bitset<32> dailyKind(0);
    for (std::vector<std::pair<unsigned int, unsigned int> >::size_type k = 0;
         k < out.size(); ++k)
    {
        const unsigned int kind = out[k].first;
        if (kind == 0)
            continue;
        if (kind > 0x20)
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x139, 5)
                ("[@missionSystem] OverKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else if (dailyKind[kind])
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x14d, 5)
                ("[@missionSystem] OverlappedKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else
        {
            dailyKind[kind] = true;
        }
    }
}

void PvP_MissionSystem::sort_list()
{
    for (std::vector<std::list<CMission*> >::iterator it = m_kindMissionList.begin();
         it != m_kindMissionList.end(); ++it)
        it->sort();
}
