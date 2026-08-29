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

#include "DNFLexWrapper.h"

// loadRDARScriptListFile 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77，
// mangled _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE）。
// STScriptFileList 无权威头，此处仅前置声明 + 正常 C++ 自由函数声明（非 asm 桥）。
class STScriptFileList;
int loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                           const char* listPath, std::map<int, int>* map);

// ---- ORIG 全局脚本文件列表缓冲（initQuestScript/initTownScript 使用）。
// g_questScriptBaseDirectory 定义见下（ORIG B 0x9500380，供 EpisodeScript 等
// loadRDARScriptFile 首参，另一处 0x580 行已定义）；此处仅 town 目录定义。
char g_townScriptBaseDirectory[0x100];    // ORIG B 0x9500da0

namespace
{
// 目录补齐：末尾非 '\' 且非 '/' 时补 "/"。
void AppendDirSlash(char* dir)
{
    size_t len = strlen(dir);
    if (len == 0)
    {
        return;
    }
    char last = dir[len - 1];
    if (last != '\\' && last != '/')
    {
        dir[len] = '/';
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


QuestScript::~QuestScript()
{
    destroy();
}

void QuestScript::clear()
{
    m_index = -1;
    m_name = "";
    m_requestDefault = "";
    m_conditionDefault = "";
    m_solveDefault = "";
    m_fieldC8.clear();
    m_str2e8 = "";
    m_2ec = 0;
    m_questType = 4;
    m_npcIdx = 0;
    m_fieldE8 = -1;
    m_field134 = -1;
    m_jobChangeQuest = 0;
    m_field1b0.clear();
    m_field1bc = 0;
    m_field1c0 = -1;
    m_postalReward.clear();
    m_field1c4 = 4;
    m_targetCharacters.clear();
    m_fieldE0 = -1;
    m_fieldD8 = 3;
    m_fieldEC = 0xb;
    m_fieldF0 = -1;
    m_levelMin[0] = 0;
    m_levelMin[1] = 0;
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
    m_rewardType = 0;
    m_titleReward.clear();
    m_field1e4 = -1;
    m_selectReward.clear();
    m_field218 = 0;
    m_rewardData.clear();
    m_field228 = -1;
    m_field22c = 0;
    m_field22d = 1;
    m_field22e = 0;
    m_field23c = 0;
    m_field23d = 0;
    m_field240 = 100;
    m_str244 = "";
    m_field248 = -1;
    m_field24c = -1;
    m_field250 = -1;
    m_field254 = -1;
    m_mailTitle.clear();
    m_mailQuestFlag = -1;
    m_mailContents.clear();
    m_mailItems.clear();
    m_field270 = 0;
    m_powerSide = 1;
    m_field274 = -1;
    m_field278 = 0;
    m_field27c = 0;
    m_questIdx = 0;
    m_field280 = 0;
    m_field282 = 0;
    m_field284 = -1;
    m_field288 = -1;
    m_field28c = -1;
    m_field290 = -1;
    m_field2a8 = 0;
    m_field2ac = -1;
    m_2c8 = 0;
    m_str2cc = "";
    m_exceptionList.clear();
    m_field128 = 0;
    m_field12c = 0;
    m_dailySchedule.clear();
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
    for (int i = 0; i < 365; ++i)
    {
        m_dayProb[i].clear();
    }
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
    (void)count;
    for (int i = 0; i <= 0x45; ++i)
    {
        if (!m_questProb[i].empty())
        {
            std::random_shuffle(m_questProb[i].begin(), m_questProb[i].end());
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

int initQuestScript(const char* dir, const char* list)
{
    strncpy(g_questScriptBaseDirectory, dir, 0x100);
    AppendDirSlash(g_questScriptBaseDirectory);
    if (!loadRDARScriptListFile(0, g_questScriptBaseDirectory, list, 0))
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
    AppendDirSlash(g_townScriptBaseDirectory);
    if (!loadRDARScriptListFile(0, g_townScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
}

bool importTownScript(TownScript* script, const char* path)
{
    if (!loadRDARScriptFile("Script/TownScript", path))
    {
        return false;
    }
    script->clear();

    std::string line;
    if (!ScanType(line, true))
    {
        return false;
    }
    script->m_field0 = 0;
    if (!ScanType(line, true))
    {
        return false;
    }
    script->m_str4 = line;
    if (!ScanType(line, true))
    {
        return false;
    }
    script->m_str8 = line;
    int value = 0;
    if (!ScanInt(&value))
    {
        return false;
    }
    script->m_14 = value;
    return true;
}

int importTrainingQuestScript(TrainingQuestScript* script, const char* path)
{
    if (!loadRDARScriptFile("Script/TrainingQuestScript", path))
    {
        return 0;
    }
    script->clear();

    std::string line;
    int value = 0;
    if (!ScanType(line, true))
    {
        return 0;
    }
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_selectProb.m_count = value;
    for (int i = 0; i < script->m_selectProb.m_count && i < 365; ++i)
    {
        if (!ScanType(line, true))
        {
            return 0;
        }
        if (!ScanInt(&value))
        {
            return 0;
        }
        script->m_selectProb.m_dayProb[i].push_back(value);
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
