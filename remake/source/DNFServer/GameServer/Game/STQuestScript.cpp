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

// ---- ORIG 全局脚本文件列表缓冲（initQuestScript/initTownScript 使用） ----
static char g_questScriptDir[0x100];   // ORIG 0x9500380
static char g_townScriptDir[0x100];    // ORIG 0x9500da0

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

QuestScript::QuestScript(const QuestScript& other)
    : m_name(other.m_name),
      m_requestDefault(other.m_requestDefault),
      m_conditionDefault(other.m_conditionDefault),
      m_solveDefault(other.m_solveDefault),
      m_fieldC8(other.m_fieldC8),
      m_targetCharacters(other.m_targetCharacters),
      m_intDataList(other.m_intDataList),
      m_antiQuestList(other.m_antiQuestList),
      m_clearItems(other.m_clearItems),
      m_killItems(other.m_killItems),
      m_appearMap(other.m_appearMap),
      m_rewardSelectList(other.m_rewardSelectList),
      m_triggerData(other.m_triggerData),
      m_titleReward(other.m_titleReward),
      m_selectReward(other.m_selectReward),
      m_eventDialogs(other.m_eventDialogs),
      m_field20c(other.m_field20c),
      m_rewardData(other.m_rewardData),
      m_mailTitle(other.m_mailTitle),
      m_mailContents(other.m_mailContents),
      m_mailItems(other.m_mailItems),
      m_postalReward(other.m_postalReward),
      m_map2b0(other.m_map2b0),
      m_str2cc(other.m_str2cc),
      m_exceptionList(other.m_exceptionList),
      m_dailySchedule(other.m_dailySchedule),
      m_str2e8(other.m_str2e8),
      m_vec2f0(other.m_vec2f0),
      m_vec2fc(other.m_vec2fc)
{
    m_index = other.m_index;
    m_questIdx = other.m_questIdx;
    m_questType = other.m_questType;
    m_fieldD8 = other.m_fieldD8;
    m_jobChangeQuest = other.m_jobChangeQuest;
    m_fieldE0 = other.m_fieldE0;
    m_npcIdx = other.m_npcIdx;
    m_fieldE8 = other.m_fieldE8;
    m_fieldEC = other.m_fieldEC;
    m_fieldF0 = other.m_fieldF0;
    m_levelMin = other.m_levelMin;
    m_levelMax = other.m_levelMax;
    m_field108 = other.m_field108;
    m_questCountLimit = other.m_questCountLimit;
    m_field128 = other.m_field128;
    m_field12c = other.m_field12c;
    m_field130 = other.m_field130;
    m_field134 = other.m_field134;
    m_triggerType = other.m_triggerType;
    m_triggerSubType = other.m_triggerSubType;
    m_field1bc = other.m_field1bc;
    m_field1c0 = other.m_field1c0;
    m_field1c4 = other.m_field1c4;
    m_rewardType = other.m_rewardType;
    m_field1e4 = other.m_field1e4;
    m_field218 = other.m_field218;
    m_field228 = other.m_field228;
    m_field22c = other.m_field22c;
    m_field22d = other.m_field22d;
    m_field22e = other.m_field22e;
    m_field23c = other.m_field23c;
    m_field23d = other.m_field23d;
    m_field240 = other.m_field240;
    m_str244 = other.m_str244;
    m_field248 = other.m_field248;
    m_field24c = other.m_field24c;
    m_field250 = other.m_field250;
    m_field254 = other.m_field254;
    m_mailQuestFlag = other.m_mailQuestFlag;
    m_field270 = other.m_field270;
    m_powerSide = other.m_powerSide;
    m_field274 = other.m_field274;
    m_field278 = other.m_field278;
    m_field27c = other.m_field27c;
    m_field280 = other.m_field280;
    m_field282 = other.m_field282;
    m_field284 = other.m_field284;
    m_field288 = other.m_field288;
    m_field28c = other.m_field28c;
    m_field290 = other.m_field290;
    m_field2a8 = other.m_field2a8;
    m_field2ac = other.m_field2ac;
    m_2c8 = other.m_2c8;
    m_2ec = other.m_2ec;
    m_308 = other.m_308;
    for (int i = 0; i < 11; ++i)
    {
        m_jobNames[i] = other.m_jobNames[i]
                            ? new std::string(*other.m_jobNames[i])
                            : 0;
        m_jobRequests[i] = other.m_jobRequests[i]
                               ? new std::string(*other.m_jobRequests[i])
                               : 0;
        m_jobConditions[i] = other.m_jobConditions[i]
                                 ? new std::string(*other.m_jobConditions[i])
                                 : 0;
        m_jobSolves[i] = other.m_jobSolves[i]
                             ? new std::string(*other.m_jobSolves[i])
                             : 0;
    }
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
    m_levelMin = 0;
    m_levelMax = 0;
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

const std::string& QuestScript::getName(int job) const
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobNames[job])
    {
        return *m_jobNames[job];
    }
    return m_name;
}

const std::string& QuestScript::getRequestMessage(int job) const
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobRequests[job])
    {
        return *m_jobRequests[job];
    }
    return m_requestDefault;
}

const std::string& QuestScript::getConditionMessage(int job) const
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobConditions[job])
    {
        return *m_jobConditions[job];
    }
    return m_conditionDefault;
}

const std::string& QuestScript::getSolveMessage(int job) const
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
    for (int i = 0; i < 69; ++i)
    {
        m_questProb[i].clear();
    }
    m_sameApplyQuest.clear();
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
        if (m_applyCount >= (int)m_sameApplyQuest.size())
        {
            return 0x7fffffff;
        }
        return m_sameApplyQuest.at(m_applyCount);
    }
    int idx = questCount - 1 + dayIndex;
    if (idx < 0)
    {
        return 0x7fffffff;
    }
    if (m_applyCount >= (int)m_questProb[idx].size())
    {
        return 0x7fffffff;
    }
    return m_questProb[idx].at(m_applyCount);
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
    int level = 1;
    for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it)
    {
        if (*it != -100 && *it + level > 0)
        {
            out->insert(*it);
        }
        ++level;
    }
    return true;
}

// ===================== 脚本导入 =====================

int initQuestScript(const char* dir, const char* list)
{
    strncpy(g_questScriptDir, dir, 0x100);
    AppendDirSlash(g_questScriptDir);
    if (!loadRDARScriptListFile(0, g_questScriptDir, list, 0))
    {
        return 0;
    }
    return 1;
}

int importQuestScript(QuestScript* script, const char* path)
{
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
    script->m_levelMin = value;
    if (!ScanInt(&value))
    {
        return 0;
    }
    script->m_levelMax = value;
    return 1;
}

int initTownScript(char* dir, char* list)
{
    strncpy(g_townScriptDir, dir, 0x100);
    AppendDirSlash(g_townScriptDir);
    if (!loadRDARScriptListFile(0, g_townScriptDir, list, 0))
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
