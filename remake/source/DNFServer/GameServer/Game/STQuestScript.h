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

class QuestScript;

// ---- 城镇区域脚本（map<int, TownAreaScript> 值类型，布局从简） ----
class TownAreaScript
{
public:
    TownAreaScript();
    TownAreaScript(const TownAreaScript& other);
    TownAreaScript& operator=(const TownAreaScript& other);
    ~TownAreaScript();

    int m_areaIndex;    // +0x00
    int m_mapIndex;     // +0x04
};

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
    std::vector<int> m_dayProb[365];               // +0x10
    char m_pad112c[0xc];                           // +0x112c..+0x1137
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
    std::vector<int> m_questProb[69];            // +0x1138
    std::vector<int> m_sameApplyQuest;           // +0x1480
    int m_applyCount;                            // +0x148c
};

// ---- 脚本导入/初始化（ORIG 符号） ----
int initQuestScript(const char* dir, const char* list);
int importQuestScript(QuestScript* script, const char* path);
int initTownScript(char* dir, char* list);
bool importTownScript(TownScript* script, const char* path);
int importTrainingQuestScript(TrainingQuestScript* script, const char* path);

#endif  // GAME_STQUESTSCRIPT_H_
