#ifndef GAME_STQUESTSCRIPT_H_
#define GAME_STQUESTSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— G5 任务/城镇脚本域（STQuestScript.h）
// QuestScript 权威定义见 Quest.h（本批补充职业文案数组与缺失尾部字段）。
// TrainingQuestScript 布局依据 ORIG ctor 0x08371cc8 / clear 0x08371dd4；
// TownScript 布局依据 ORIG clear 0x086d1e2e。
// ============================================================================

#include <set>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "TownAreaScript.h"   // TownAreaScript 唯一声明点（0x30，TownAreaScript.cpp 提供符号）

class QuestScript;

// ---- 城镇脚本（0x44 字节） ----
class TownScript
{
public:
    TownScript();
    ~TownScript();
    void clear();

    int m_field0;                        // +0x00
    std::string m_str4;                  // +0x04
    std::string m_str8;                  // +0x08
    std::string m_strc;                  // +0x0c
    std::string m_str10;                 // +0x10
    int m_14;                            // +0x14
    int m_18;                            // +0x18（-1）
    int m_1c;                            // +0x1c（-1）
    std::map<int, TownAreaScript> m_areas;  // +0x20
    std::string m_str38;                 // +0x38
    int m_3c;                            // +0x3c
    int m_40;                            // +0x40
};

// ---- 每日训练任务选择概率（0x1138 字节） ----
class ST_TrainingQuest_Select_Prob
{
public:
    ST_TrainingQuest_Select_Prob();
    ~ST_TrainingQuest_Select_Prob();

    int m_count;                                   // +0x00
    std::vector<std::pair<int, int> > m_pairs;     // +0x04
    std::vector<int> m_dayProb[366];               // +0x10..+0x1138（ORIG 构造循环 0x16d 计数=366 个）
};

// ---- 每日训练任务脚本（0x1490 字节） ----
class TrainingQuestScript
{
public:
    TrainingQuestScript();
    ~TrainingQuestScript();

    void clear();
    void reset_same_apply_level_cnt();
    int computeDayIndex();
    int get_random_fake_value(int idx, int day);
    void suffleTrainingQuests();
    int getTrainingQuest(int dayIndex, int questCount, int job);
    bool getApplyLevel(std::multiset<int>* out);

    ST_TrainingQuest_Select_Prob m_selectProb;   // +0x00
    // ORIG C1 0x8371cc8 构造 0x46=70 个 vector（+0x1138 起）；clear 0x8371dd4 循环
    // i<=0x45；reset_same_apply_level_cnt 0x8371e48 写 m_applyCount@+0x148c →
    // sizeof 0x1490（CDM m_dailyTrainingQuest@+0xa1c4 边界互证）。
    std::vector<int> m_questProb[70];            // +0x1138
    std::vector<int> m_sameApplyQuest;           // +0x1480
    int m_applyCount;                            // +0x148c
};

// ORIG BSS 全局 0x9500380（定义于 STQuestScript.cpp；importEpicQuestScript 经
// loadRDARScriptFile(g_questScriptBaseDirectory, path) 使用）。
extern char g_questScriptBaseDirectory[0x100];

int initQuestScript(const char* dir, const char* list);
int importTrainingQuestScript(TrainingQuestScript* script, const char* path);
int initTownScript(char* dir, char* list);
bool importTownScript(TownScript* script, const char* path);
int importQuestScript(QuestScript* script, const char* path);

#endif  // GAME_STQUESTSCRIPT_H_
