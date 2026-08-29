// ============================================================================
// df_game_r 还原 —— G5 任务/城镇脚本域（STQuestScript.cpp）
// 对照 docs/class_func_reports/QuestScript.md 与 ORIG 反汇编：
//   QuestScript        ctor 0x0836e4e2 / copy 0x0837adc8 / dtor 0x0836eaf0
//                      clear 0x0836deba / destroy 0x08a6130a
//                      getName 0x08a611fc / getRequestMessage 0x08a61240
//                      getConditionMessage 0x08a61284 / getSolveMessage 0x08a612c6
//   TownScript         clear 0x086d1e2e / ctor 0x086d1f04 / dtor 0x086d2ab8
//   TrainingQuestScript ctor 0x08371cc8 / clear 0x08371dd4 / dtor 0x0837d488
//                      computeDayIndex 0x08aafc64 / get_random_fake_value 0x08aafc90
//                      suffleTrainingQuests 0x08aafcbc / getTrainingQuest 0x08aafb44
//                      getApplyLevel 0x08aaf8b6
// 解析辅助（loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "STQuestScript.h"

#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Quest.h"
#include "MyRandom.h"          // suffleTrainingQuests 的 random_shuffle 随机数器

#include "DNFLexWrapper.h"

// loadRDARScriptListFile 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77，
// mangled _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE）。
// STScriptFileList 无权威头，此处仅前置声明 + 正常 C++ 自由函数声明（非 asm 桥）。
// STScriptFileList 权威定义见 NpcScript.h（经 Quest.h include 链可见）。
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* map);

// ---- ORIG 全局脚本文件列表缓冲（initQuestScript/initTownScript 使用）。
// g_questScriptBaseDirectory 定义见下（ORIG B 0x9500380，供 EpisodeScript 等
// loadRDARScriptFile 首参，另一处 0x580 行已定义）；此处仅 town 目录定义。
char g_townScriptBaseDirectory[0x100];    // ORIG B 0x9500da0
extern std::map<int, std::string> g_townScriptFileList;  // 定义于 GameWorld.cpp（ORIG B 0x9500ea0）
// g_questScriptFileList 权威定义见 InitScriptProducers_4.cpp（extern 声明 + loadRDARScriptListFile 填充）。
        // ORIG B 0x9500480（initQuestScript 填充）

namespace
{
// 目录补齐：末尾非 '\' 且非 '/' 时补 "\\"（ORIG Init*/init* 族共用该字符）。
void AppendDirSeparator(char* dir)
{
    size_t len = strlen(dir);
    if (len == 0)
    {
        return;
    }
    char last = dir[len - 1];
    if (last != '\\' && last != '/')
    {
        dir[len] = '\\';
        dir[len + 1] = '\0';
    }
}
}

// ===================== QuestScript =====================

QuestScript::QuestScript()
{
    memset(m_jobNames, 0, sizeof(m_jobNames));
    memset(m_jobRequests, 0, sizeof(m_jobRequests));
    memset(m_jobConditions, 0, sizeof(m_jobConditions));
    memset(m_jobSolves, 0, sizeof(m_jobSolves));
    clear();
}

// ORIG D1 0x08a70700（成员自动析构）。
QuestScript::~QuestScript()
{
}


void QuestScript::clear()
{
    for (int i = 0; i <= 1; ++i)
        m_levelMin[i] = 0;
    m_field108 = 0;
    m_intDataList.clear();
    m_antiQuestList.clear();
    m_questCountLimit = 0;
    m_field130 = 0;
    m_clearItems.clear();
    m_killItems.clear();
    m_appearMap.clear();
    m_rewardSelectList.clear();
    m_triggerType = 0;
    m_triggerSubType = 0;
    m_triggerData.clear();
    m_field18c.clear();
    m_rewardType = 0;
    m_titleReward.clear();
    m_field1e4 = -1;
    m_field20c.clear();
    m_field218 = 0;
    m_rewardData.clear();
    m_field228 = -1;
    m_field22c = 0;
    m_field23d = 0;
    m_field240 = 100;
    m_str244 = "";
    m_field248 = -1;
    m_field24c = -1;
    m_field250 = -1;
    m_field254 = -1;
    m_mailQuestFlag = -1;
    m_mailTitle.clear();
    m_mailContents.clear();
    m_mailItems.clear();
    m_field1f4.clear();
    m_selectReward.clear();
    m_eventDialogs.clear();
    m_field22d = 1;
    m_field22e = 0;
    for (int i = 0; i <= 2; ++i)
        m_field230[i] = -1;
    m_field23c = 0;
    m_field270 = 0;
    m_field271 = 1;
    m_powerSide = 0xff;
    m_field274 = -1;
    m_field278 = 0;
    m_field27c = 0;
    m_questIdx = 0;
    for (int i = 0; i <= 10; ++i)
    {
        m_jobNames[i] = 0;
        m_jobRequests[i] = 0;
        m_jobConditions[i] = 0;
        m_jobSolves[i] = 0;
    }
    m_field280 = 0;
    m_field282 = 0;
    m_field284 = -1;
    m_field288 = -1;
    m_field28c = -1;
    m_field290 = -1;
    m_str294.clear();
    m_field2a8 = 0;
    m_field2ac = -1;
    m_2c8 = 0;
    m_str2cc = "";
    m_exceptionList.clear();
    m_field128 = 0;
    m_field12c = 0;
    m_dailySchedule.clear();
    m_field12c = 0;
    m_field128 = 0;
    m_vec2f0.clear();
    m_vec2fc.clear();
    m_308 = 0;
}

void QuestScript::destroy()
{
    for (int i = 0; i < 0xb; ++i)
    {
        if (m_jobNames[i])
        {
            delete m_jobNames[i];
            m_jobNames[i] = 0;
        }
        if (m_jobRequests[i])
        {
            delete m_jobRequests[i];
            m_jobRequests[i] = 0;
        }
        if (m_jobConditions[i])
        {
            delete m_jobConditions[i];
            m_jobConditions[i] = 0;
        }
        if (m_jobSolves[i])
        {
            delete m_jobSolves[i];
            m_jobSolves[i] = 0;
        }
    }
}

std::string& QuestScript::getName(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobNames[job])
    {
        return *m_jobNames[job];
    }
    return m_name;
}

std::string& QuestScript::getRequestMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobRequests[job])
    {
        return *m_jobRequests[job];
    }
    return m_requestDefault;
}

std::string& QuestScript::getConditionMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobConditions[job])
    {
        return *m_jobConditions[job];
    }
    return m_conditionDefault;
}

std::string& QuestScript::getSolveMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobSolves[job])
    {
        return *m_jobSolves[job];
    }
    return m_solveDefault;
}

// ===================== TownScript =====================

TownScript::TownScript()
{
    clear();
}

TownScript::~TownScript()
{
}

void TownScript::clear()
{
    m_field0 = 0;
    m_str4 = "";
    m_str8 = "";
    m_strc = "";
    m_str10 = "";
    m_14 = 0;
    m_18 = -1;
    m_1c = -1;
    m_areas.clear();
    m_str38 = "";
    m_40 = 0;
    m_3c = m_40;
}

// ===================== TrainingQuestScript =====================

ST_TrainingQuest_Select_Prob::ST_TrainingQuest_Select_Prob()
{
    m_count = 0;
    m_pairs.clear();   // ORIG 正常路径仅 m_count=0 + m_pairs.clear()；数组由隐式构造初始化
}

ST_TrainingQuest_Select_Prob::~ST_TrainingQuest_Select_Prob()
{
}

TrainingQuestScript::TrainingQuestScript()
{
    // ORIG C1 0x8371cc8：成员数组隐式构造（70 个 vector）后仅调 clear()。
    clear();
}

TrainingQuestScript::~TrainingQuestScript()
{
}

void TrainingQuestScript::clear()
{
    m_selectProb.m_count = 0;
    m_selectProb.m_pairs.clear();
    m_applyCount = 0;
    for (int i = 0; i <= 0x45; ++i)
    {
        m_questProb[i].clear();
    }
    m_sameApplyQuest.clear();
}

void TrainingQuestScript::reset_same_apply_level_cnt()
{
    m_applyCount = 0;
}

int TrainingQuestScript::computeDayIndex()
{
    time_t now = time(0);
    struct tm* t = localtime(&now);
    return t->tm_yday;
}

int TrainingQuestScript::get_random_fake_value(int idx, int day)
{
    return m_selectProb.m_dayProb[day].at(idx);
}

void TrainingQuestScript::suffleTrainingQuests()
{
    int day = computeDayIndex();
    int count = m_selectProb.m_dayProb[day].at(0);
    // ORIG 0x08aafcbc：MyRandom(count) 作随机数器（3 参 random_shuffle）。
    MyRandom rnd(count);
    for (int i = 0; i <= 0x45; ++i)
    {
        if (!m_questProb[i].empty())
        {
            std::random_shuffle(m_questProb[i].begin(), m_questProb[i].end(), rnd);
        }
    }
}

int TrainingQuestScript::getTrainingQuest(int dayIndex, int questCount, int job)
{
    if (questCount == job)
    {
        ++m_applyCount;
    }
    else
    {
        m_applyCount = 0;
    }
    if (questCount == -100)
    {
        if (m_applyCount >= m_sameApplyQuest.size())
        {
            return 0x7fffffff;
        }
        return m_sameApplyQuest.at(m_applyCount);
    }
    if (dayIndex - 1 + questCount < 0)
    {
        return 0x7fffffff;
    }
    if (m_applyCount >= m_questProb[dayIndex - 1 + questCount].size())
    {
        return 0x7fffffff;
    }
    return m_questProb[dayIndex - 1 + questCount].at(m_applyCount);
}

bool TrainingQuestScript::getApplyLevel(std::multiset<int>* out)
{
    if (m_selectProb.m_count <= 0 || m_selectProb.m_pairs.empty())
    {
        return false;
    }
    int day = computeDayIndex();
    std::vector<int> tmp;
    for (int i = 0; i < m_selectProb.m_count; ++i)
    {
        int val = get_random_fake_value(i, day);
        int acc = 0;
        for (std::vector<std::pair<int, int> >::iterator it =
                 m_selectProb.m_pairs.begin();
             it != m_selectProb.m_pairs.end(); ++it)
        {
            acc += it->second;
            if (val < acc)
            {
                tmp.push_back(it->first);
                break;
            }
        }
    }
    for (int level = 1; level <= 0x46; ++level)
    {
        for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it)
        {
            if (*it == -100)
            {
                out->insert(*it);
            }
            else if (*it + level >= 1)
            {
                out->insert(*it);
            }
        }
    }
    return true;
}

// ===================== 脚本导入 =====================

// 权威定义见 InitScriptProducers_4.cpp（g_questScriptFileList B 0x9500480）。
extern std::map<int, std::string> g_questScriptFileList;

int initQuestScript(const char* dir, const char* list)
{
    strncpy(g_questScriptBaseDirectory, dir, 0x100);
    AppendDirSeparator(g_questScriptBaseDirectory);
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_questScriptFileList),
                                g_questScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

int importQuestScript(QuestScript* script, const char* path)
{
    // ORIG importQuestScript 的函数局部辅助类 QuestData（报告
    // importQuestScript_QuestScript___char_const____QuestData/：
    //   C1 0x08a616fe / D1 0x08a617a6）。重建版解析不复用该辅助结构，
    // 按报告布局语义落地并 force 实例化以发射 _ZZ17importQuestScript...
    // QuestData C1/C2/D1/D2 符号。
    struct QuestData
    {
        std::string m_str0;                     // +0x00
        std::vector<int> m_vec4;                // +0x04
        int m_int10;                            // +0x10
        int m_int14;                            // +0x14
        char m_byte18;                          // +0x18
        char m_byte19;                          // +0x19
        char m_byte1a;                          // +0x1a
        char m_byte1b;                          // +0x1b
        char m_byte1c;                          // +0x1c
        std::vector<MonsterKillItem> m_vec20;   // +0x20
        char m_byte2c;                          // +0x2c

        QuestData()
        {
            m_int14 = 0;   // ORIG 先清 +0x14
            m_int10 = 0;   // 再清 +0x10
            m_byte18 = 0;
            m_byte19 = 0;
            m_byte1a = 0;
            m_byte1b = 0;
            m_byte1c = 0;
            m_byte2c = 0;
        }

        ~QuestData()
        {
        }
    };
    {
        QuestData qd;
        (void)qd;
    }

    if (!loadRDARScriptFile("Script/QuestScript", path))
    {
        return 0;
    }
    script->clear();

    std::string line;
    if (!ScanType(line, true))
    {
        return 0;
    }
    int value = 0;
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_questIdx = value;
    if (!ScanType(line, true))
    {
        return 0;
    }
    script->m_name = line;
    if (!ScanType(line, true))
    {
        return 0;
    }
    script->m_requestDefault = line;
    if (!ScanType(line, true))
    {
        return 0;
    }
    script->m_conditionDefault = line;
    if (!ScanType(line, true))
    {
        return 0;
    }
    script->m_solveDefault = line;
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_npcIdx = value;
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_levelMin[0] = value;
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_levelMin[1] = value;
    return 1;
}

int initTownScript(char* dir, char* list)
{
    strncpy(g_townScriptBaseDirectory, dir, 0x100);
    AppendDirSeparator(g_townScriptBaseDirectory);
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(&g_townScriptFileList),
                                g_townScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

bool importTownScript(TownScript* script, const char* path)
{
    // ORIG T 0x8aada90（382 条）：键值解析器。ScanType 失败=文件读完（break，
    // 非错误）；ScanStr/ScanInt 返回值不检查（ORIG 无防御，Fix7_I 2026-08-30 修正）。
    if (!loadRDARScriptFile("Script/TownScript", path))
    {
        return false;
    }
    script->clear();

    // ORIG：getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory)
    getDirectoryPath(script->m_str38, "%s%s", g_townScriptBaseDirectory);

    std::string line;
    std::string sub;
    int value = 0;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[name]")
        {
            ScanStr(&script->m_str4);
            continue;
        }
        if (line == "[entering title]")
        {
            ScanStr(&script->m_str8);
            continue;
        }
        if (line == "[entering title next]")
        {
            ScanStr(&script->m_str10);
            continue;
        }
        if (line == "[cutscene image]")
        {
            ScanStr(&script->m_strc);
            script->m_14 = ScanInt((bool*)0);   // ORIG：ScanInt 返回直接存（无检查）
            continue;
        }
        if (line == "[dungeon what must be cleared]")
        {
            script->m_18 = ScanInt((bool*)0);
            continue;
        }
        if (line == "[only server parsing dungeon what must be cleared]")
        {
            script->m_1c = ScanInt((bool*)0);
            continue;
        }
        if (line == "[area]")
        {
            TownAreaScript area;
            area.clear();
            bool flag = false;
            area.m_areaIndex = ScanInt(&flag);      // ORIG 8aadc87 无检查
            ScanStr(&area.m_mapName);               // ORIG 8aadc9f 无检查
            ScanStr(&sub);                          // ORIG 8aadca4 ScanStr（非 ScanType）
            if (sub == "[minimap rect]")
            {
                // ORIG 8aadcca..8aadd75：T_Rect 复用 + 多个 rect，
                // 每个 rect 至多 4 值（m_00/m_08/m_04/m_0c），以 flag 失败终止。
                T_Rect r;
                for (;;)
                {
                    value = ScanInt(&flag);         // 8aadcd5
                    if (!flag)
                        break;                      // 8aadd63 flag 检查
                    int i = 0;
                    do
                    {
                        if (i == 0)
                            r.m_00 = value;
                        else if (i == 1)
                            r.m_08 = value;
                        else if (i == 2)
                            r.m_04 = value;
                        else
                            r.m_0c = value;
                        value = ScanInt(&flag);     // 8aadd1e
                        ++i;
                    } while (i <= 3 && flag);       // 8aadd30
                    area.m_rects.push_back(r);      // 8aadd4e
                    if (!flag)
                        break;                      // 8aadd63：读到新值则继续新 rect
                }
                ScanStr(&sub);                      // ORIG 8aadd6f ScanStr
            }
            if (sub == "[use private store]")
            {
                value = ScanInt(&flag);             // ORIG 8aadd97
                area.m_field24 = (value != 0) ? 1 : 0;  // ORIG 8aadda2 setne
                ScanStr(&sub);                      // ORIG 8aaddb0 ScanStr
            }
            // 区域类型：normal/gate/fishing place/pub/dungeon gate/
            //          safe zone against a / safe zone against b
            if (sub == "[normal]")
                area.m_areaType = 0;
            else if (sub == "[gate]")
                area.m_areaType = 1;
            else if (sub == "[fishing place]")
                area.m_areaType = 2;
            else if (sub == "[pub]")
                area.m_areaType = 3;
            else if (sub == "[dungeon gate]")
                area.m_areaType = 4;
            else if (sub == "[safe zone against a]")
                area.m_areaType = 5;
            else if (sub == "[safe zone against b]")
                area.m_areaType = 6;
            else
                area.m_areaType = 0;
            // ORIG 8aade99..8aadec7：循环 ScanInt 直到 flag 失败，全部 push
            for (;;)
            {
                value = ScanInt(&flag);
                if (!flag)
                    break;
                area.m_field18.push_back(value);
            }
            script->m_areas[area.m_areaIndex] = area;
            continue;
        }
        if (line == "[limit level]")
        {
            script->m_3c = ScanInt((bool*)0);
            continue;
        }
        if (line == "[need quest]")
        {
            script->m_40 = ScanInt((bool*)0);
            continue;
        }
        // 未知键：忽略（回到循环读下一行）
    }
    // ORIG 尾部：script->m_field0 = g_townScriptFileList.FindIndexByFullScan(path)
    script->m_field0 = reinterpret_cast<STScriptFileList*>(&g_townScriptFileList)
                           ->FindIndexByFullScan(path);
    return true;
}

int importTrainingQuestScript(TrainingQuestScript* script, const char* path)
{
    // ORIG T 0x8aaf542（258 条）逐指令转写：键值解析器。
    // 键：[select prob] count+pairs；[rand value] dayProb[366]；
    // [level] 递增序号段 → m_questProb[prev]，否则（含 -100）→ m_sameApplyQuest；
    // [training quest end] 终止。
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    script->clear();

    std::string line;
    std::string sub;
    int prev = 0;
    for (;;)
    {
        if (!ScanType(line, true))
        {
            return 0;
        }
        if (line == "[select prob]")
        {
            if (!ScanInt(&script->m_selectProb.m_count))
                return 0;
            for (;;)
            {
                int a = 0, b = 0;
                if (!ScanInt(&a))
                    break;
                if (!ScanInt(&b))
                    break;
                script->m_selectProb.m_pairs.push_back(std::make_pair(a, b));
                if (!ScanType(sub, true))
                    break;
                if (sub == "[/select prob]")
                    break;
            }
            continue;
        }
        if (line == "[rand value]")
        {
            for (int i = 0; i <= 0x16d; ++i)
            {
                int j = 0;
                for (;;)
                {
                    int v = 0;
                    if (!ScanInt(&v))
                        break;
                    script->m_selectProb.m_dayProb[i].push_back(v);
                    ++j;
                    if (j >= script->m_selectProb.m_count)
                        break;
                }
                if (!ScanType(sub, true))
                    break;
                if (sub == "[/rand value]")
                    break;
            }
            continue;
        }
        if (line == "[level]")
        {
            int v = 0;
            if (!ScanInt(&v))
                return 0;
            if (v != -100 && v == prev + 1)
            {
                // 递增序号：值进 m_questProb[prev]
                for (;;)
                {
                    int x = 0;
                    if (!ScanInt(&x))
                        break;
                    script->m_questProb[prev].push_back(x);
                    if (!ScanType(sub, true))
                        break;
                    if (sub == "[/level]")
                        break;
                }
                prev = v;
            }
            else
            {
                // 非递增（含 -100）：值进 m_sameApplyQuest
                for (;;)
                {
                    int x = 0;
                    if (!ScanInt(&x))
                        break;
                    script->m_sameApplyQuest.push_back(x);
                    if (!ScanType(sub, true))
                        break;
                    if (sub == "[/level]")
                        break;
                }
            }
            continue;
        }
        if (line == "[training quest end]")
            break;
        // 未知键：忽略（回循环读下一行）
    }
    return 1;
}

// ORIG B 0x09500380：脚本根目录（EpisodeScript 等 loadRDARScriptFile 首参）。
char g_questScriptBaseDirectory[0x100];

// ORIG T 0x8a60f78（0x95）：0x14 字节标量区清零（+0x10 为字节）+ m_str20 构造
// + m_field24 清零 + 11 个 dialog 指针清零（i=0..10，P151）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0), m_field10(0),
      m_field14(0), m_field18(0), m_field1c(0), m_str20(), m_field24(0)
{
    for (int i = 0; i <= 10; ++i)
        m_dialogs[i] = 0;
}

// ORIG T 0x8a6100e：逐字段拷贝 + 深拷贝 11 个 dialog 串
// （new std::string + operator=，非拷贝构造；ORIG asm _Znwj→SsC1→SsaSERKSs 实证）。
dungeonEventHanaseStruct::dungeonEventHanaseStruct(
    const dungeonEventHanaseStruct& o)
    : m_field0(o.m_field0), m_field4(o.m_field4), m_field8(o.m_field8),
      m_fieldc(o.m_fieldc), m_field10(o.m_field10), m_field14(o.m_field14),
      m_field18(o.m_field18), m_field1c(o.m_field1c), m_str20(o.m_str20),
      m_field24(o.m_field24)
{
    for (int i = 0; i <= 10; ++i)
    {
        m_dialogs[i] = 0;
        if (o.m_dialogs[i] != 0)
        {
            m_dialogs[i] = new std::string;
            *m_dialogs[i] = *o.m_dialogs[i];
        }
    }
}

// ORIG T 0x8a615fe（0x44）：m_field24!=0 且 0<=job<=10 且 m_dialogs[job]!=0 →
// 返回 m_dialogs[job]；否则返回 &m_str20（+0x20）。
std::string* dungeonEventHanaseStruct::getNewDialog(ENUM_CHARACTERJOB job)
{
    if (m_field24 != 0 && static_cast<int>(job) >= 0 &&
        static_cast<int>(job) <= 10 && m_dialogs[job] != 0)
    {
        return m_dialogs[job];
    }
    return &m_str20;
}
