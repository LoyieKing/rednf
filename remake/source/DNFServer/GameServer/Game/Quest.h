#ifndef GAME_QUEST_H_
#define GAME_QUEST_H_

// df_game_r 任务模板 Quest 类（G3-2 批次，2026-08-17）。
// 布局依据 ORIG Quest ctor 0x837aa28 / set_quest 0x835233e 反汇编推导，
// sizeof(Quest) = 0x150；QuestScript 布局按 set_quest 读取偏移推导。
// CListTables.cpp 的 QuestList 依赖本类的真实符号。

#include <string>
#include <map>
#include <utility>
#include <vector>

class CUser;
class CDataManager;

// ---- 枚举（ORIG mangled 名要求类型名一致） ----
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};

enum ENUM_QUEST_ENEMY_TYPE
{
    ENUM_QUEST_ENEMY_TYPE_0 = 0
};

enum ENUM_QUEST_EXCEPTION_TYPE
{
    ENUM_QUEST_EXCEPTION_TYPE_0 = 0
};

enum _DailyScheduleType
{
    _DailyScheduleType_0 = 0
};

// ---- 任务目标职业/成长（12 字节） ----
struct stQuestTargetCharacter_t
{
    int m_job;        // +0x00
    int m_firstGrow;  // +0x04
    int m_secondGrow; // +0x08
};

// ---- 副本通关道具（24 字节） ----
struct DungeonClearItem
{
    int m_itemIdx;    // +0x00
    int m_dungeonIdx; // +0x04（-1 = 任意）
    int m_rewardItem; // +0x08
    int m_count;      // +0x0c
    int m_percent;    // +0x10
    int m_field14;    // +0x14

    void clear()
    {
        m_itemIdx = 0;
        m_dungeonIdx = -1;
        m_rewardItem = 0;
        m_count = 0;
        m_percent = 0;
        m_field14 = 0;
    }
};

// ---- 击杀怪物（32 字节） ----
struct MonsterKillItem
{
    int m_enemyType;   // +0x00（ENUM_QUEST_ENEMY_TYPE，默认 1）
    int m_killCount;   // +0x04
    int m_mapIdx;      // +0x08（-1 = 任意）
    int m_dungeonIdx;  // +0x0c（-1 = 任意）
    int m_field10;     // +0x10
    int m_field14;     // +0x14
    int m_field18;     // +0x18
    int m_field1c;     // +0x1c（-1）

    void clear()
    {
        m_enemyType = 1;
        m_killCount = 0;
        m_mapIdx = -1;
        m_dungeonIdx = -1;
        m_field10 = 0;
        m_field14 = 0;
        m_field18 = 0;
        m_field1c = -1;
    }
};

// ---- 奖励道具信息（16 字节） ----
struct RewardItemInfo
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    int m_job;       // +0x08（0xb = 无职业限制）
    int m_growType;  // +0x0c（-1 = 无成长限制）

    RewardItemInfo()
    {
        m_count = 0;
        m_itemIdx = 0;
        m_job = 0xb;
        m_growType = -1;
    }
};

// ---- 出现地图（16 字节） ----
struct AppearMap
{
    int m_mapA;       // +0x00
    int m_mapB;       // +0x04（-1 = 单地图判定）
    int m_appearMap;  // +0x08（判定通过返回的地图）
    int m_percent;    // +0x0c

    AppearMap() { clear(); }
    void clear()
    {
        m_mapA = -1;
        m_mapB = -1;
        m_appearMap = -1;
        m_percent = -1;
    }
};

// ---- 邮件奖励（16 字节：标题/内容/类型） ----
struct PostalReward
{
    std::string m_title;     // +0x00
    std::string m_contents;  // +0x04
    int m_rewardType;        // +0x0c

    PostalReward() : m_rewardType(0) {}
    void clear()
    {
        m_title.clear();
        m_contents.clear();
        m_rewardType = 0;
    }
};

// ---- 副本事件对话（0x54 字节） ----
struct dungeonEventHanaseStruct
{
    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldc;       // +0x0c
    char m_field10;     // +0x10
    int m_field14;      // +0x14
    int m_field18;      // +0x18
    int m_field1c;      // +0x1c
    std::string m_str20; // +0x20
    int m_field24;      // +0x24
    int m_field28[11];  // +0x28..+0x50
};

// ---- 任务触发状态（8 字节：3 个 9 位值） ----
struct stQuestTriggerState_t
{
    short m_trigger0;  // +0x00
    short m_trigger1;  // +0x02
    short m_trigger2;  // +0x04
};

// ---- 任务选择参数（QuestList / check_possible 共用） ----
struct stSelectQuestParam
{
    int m_level;          // +0x00
    int m_job;            // +0x04
    int m_firstGrow;      // +0x08
    int m_secondGrow;     // +0x0c
    int m_pvpGrade;       // +0x10
    char m_powerSide;     // +0x14
    char m_pad15[3];
    int m_questIdx;       // +0x18
    int m_expertJobType;  // +0x1c
    int m_expertJobLevel; // +0x20
    int m_questCount;     // +0x24
};

// ---- 任务脚本（QuestScript，set_quest 读取偏移推导） ----
class QuestScript
{
public:
    QuestScript();
    QuestScript(const QuestScript& other);
    ~QuestScript();

    void clear();
    void destroy();
    const std::string& getName(int job) const;
    const std::string& getRequestMessage(int job) const;
    const std::string& getConditionMessage(int job) const;
    const std::string& getSolveMessage(int job) const;

    int m_index;                                  // +0x00
    std::string m_name;                           // +0x04
    std::string m_requestDefault;                 // +0x08
    std::string m_conditionDefault;               // +0x0c
    std::string m_solveDefault;                   // +0x10
    int m_questIdx;                               // +0x14
    std::string* m_jobNames[11];                  // +0x18
    std::string* m_jobRequests[11];               // +0x44
    std::string* m_jobConditions[11];             // +0x70
    std::string* m_jobSolves[11];                 // +0x9c
    std::vector<std::string> m_fieldC8;           // +0xc8
    int m_questType;                              // +0xd4
    int m_fieldD8;                                // +0xd8
    int m_jobChangeQuest;                         // +0xdc
    int m_fieldE0;                                // +0xe0
    int m_npcIdx;                                 // +0xe4
    int m_fieldE8;                                // +0xe8
    int m_fieldEC;                                // +0xec
    int m_fieldF0;                                // +0xf0
    std::vector<stQuestTargetCharacter_t> m_targetCharacters;  // +0xf4
    int m_levelMin;                               // +0x100
    int m_levelMax;                               // +0x104
    char m_field108;                              // +0x108
    char m_pad109[3];
    std::vector<std::vector<int> > m_intDataList; // +0x10c
    std::vector<int> m_antiQuestList;             // +0x118
    int m_questCountLimit;                        // +0x124
    int m_field128;                               // +0x128
    int m_field12c;                               // +0x12c
    char m_field130;                              // +0x130
    char m_pad131[3];                             // +0x131..+0x133
    int m_field134;                               // +0x134
    std::vector<DungeonClearItem> m_clearItems;   // +0x138
    std::vector<MonsterKillItem> m_killItems;     // +0x144
    AppearMap m_appearMap;                        // +0x150
    std::vector<std::pair<int, int> > m_rewardSelectList;  // +0x160
    char m_pad16c[0xc];                           // +0x16c..+0x177
    int m_triggerType;                            // +0x178
    int m_triggerSubType;                         // +0x17c
    std::vector<int> m_triggerData;               // +0x180
    char m_pad18c[0x24];                          // +0x18c..+0x1af
    std::vector<int> m_field1b0;                  // +0x1b0
    char m_field1bc;                              // +0x1bc
    char m_pad1bd[3];
    int m_field1c0;                               // +0x1c0
    int m_field1c4;                               // +0x1c4
    int m_rewardType;                             // +0x1d4
    std::vector<RewardItemInfo> m_titleReward;    // +0x1d8
    int m_field1e4;                               // +0x1e4
    std::vector<RewardItemInfo> m_selectReward;   // +0x1e8
    char m_pad1f4[0xc];                           // +0x1f4..+0x1ff
    std::vector<dungeonEventHanaseStruct> m_eventDialogs;  // +0x200
    std::vector<int> m_field20c;                  // +0x20c
    int m_field218;                               // +0x218
    std::vector<int> m_rewardData;                // +0x21c
    int m_field228;                               // +0x228
    char m_field22c;                              // +0x22c
    char m_field22d;                              // +0x22d
    char m_field22e;                              // +0x22e
    char m_pad22f[0xd];                           // +0x22f..+0x23b
    char m_field23c;                              // +0x23c
    char m_field23d;                              // +0x23d
    char m_pad23e[2];
    int m_field240;                               // +0x240
    std::string m_str244;                         // +0x244
    int m_field248;                               // +0x248
    int m_field24c;                               // +0x24c
    int m_field250;                               // +0x250
    int m_field254;                               // +0x254
    char m_mailQuestFlag;                         // +0x258
    char m_pad259[3];
    std::string m_mailTitle;                      // +0x25c
    std::string m_mailContents;                   // +0x260
    std::vector<int> m_mailItems;                 // +0x264
    char m_field270;                              // +0x270
    char m_pad271;
    char m_powerSide;                             // +0x272
    char m_pad273;
    int m_field274;                               // +0x274
    char m_field278;                              // +0x278
    char m_pad279[3];
    int m_field27c;                               // +0x27c
    short m_field280;                             // +0x280
    short m_field282;                             // +0x282
    int m_field284;                               // +0x284
    int m_field288;                               // +0x288
    int m_field28c;                               // +0x28c
    int m_field290;                               // +0x290
    PostalReward m_postalReward;                  // +0x298
    int m_field2a8;                               // +0x2a8
    int m_field2ac;                               // +0x2ac
    std::map<int, std::pair<int, int> > m_map2b0; // +0x2b0
    char m_2c8;                                   // +0x2c8
    std::string m_str2cc;                         // +0x2cc
    std::vector<ENUM_QUEST_EXCEPTION_TYPE> m_exceptionList;  // +0x2d0
    std::vector<int> m_dailySchedule;             // +0x2dc
    std::string m_str2e8;                         // +0x2e8
    char m_2ec;                                   // +0x2ec
    std::vector<int> m_vec2f0;                    // +0x2f0
    std::vector<int> m_vec2fc;                    // +0x2fc
    char m_308;                                   // +0x308
};

// ---- Quest 任务模板（sizeof 0x150） ----
class Quest
{
public:
    Quest();
    ~Quest();

    // 接收条件 / 检测
    char exposeQuest() const;
    char check_possible(const stSelectQuestParam& param) const;
    char check_power_side(char side);
    bool checkPossibleJob(ENUM_CHARACTERJOB job) const;
    bool _isNeededCheckGrowType(ENUM_CHARACTERJOB job) const;
    bool checkMatchedGrowType(ENUM_CHARACTERJOB job, int firstGrow,
                              int secondGrow) const;
    bool check_clear_item(int itemIdx, int dungeonIdx) const;
    bool CheckKillMonster(int mapIdx, int dungeonIdx, int killCount,
                          ENUM_QUEST_ENEMY_TYPE enemyType,
                          std::vector<MonsterKillItem>& out) const;
    char check_clear_send() const;

    // 触发 / 奖励
    int get_init_trigger() const;
    bool get_init_achievement_trigger(unsigned short& t0, unsigned short& t1,
                                      unsigned short& t2) const;
    int get_title_reward() const;
    bool isRepeatableQuest() const;
    unsigned int ConvertRewardSelectIndex(const CUser* user, int index);
    std::vector<std::pair<int, int> > get_clear_itemlist(int dungeonIdx) const;

    // 排程 / 地图
    bool IsOpenScheduleQuest() const;
    int get_appearmap(int mapA, int mapB) const;

    // 装载
    void set_quest(QuestScript script);

    // ---- 成员（+0x00 起） ----
    std::string m_name;                        // +0x00
    int m_index;                               // +0x04
    int m_type;                                // +0x08
    int m_npcIdx;                              // +0x0c
    std::vector<stQuestTargetCharacter_t> m_targetCharacters;  // +0x10
    int m_jobChangeQuest;                      // +0x1c
    int m_levelMin;                            // +0x20
    int m_levelMax;                            // +0x24
    int m_questCountLimit;                     // +0x28
    std::vector<std::vector<int> > m_intDataList;  // +0x2c
    std::vector<int> m_antiQuestList;          // +0x38
    char m_clearSendFlag;                      // +0x44
    char m_pad45[3];
    std::vector<DungeonClearItem> m_clearItems;    // +0x48
    std::vector<MonsterKillItem> m_killItems;      // +0x54
    AppearMap m_appearMap;                         // +0x60
    std::vector<std::pair<int, int> > m_rewardSelectList;  // +0x70
    int m_triggerType;                         // +0x7c
    int m_triggerSubType;                      // +0x80
    std::vector<int> m_triggerData;            // +0x84
    std::vector<int> m_field90;                // +0x90
    PostalReward m_postalReward;               // +0x9c
    std::vector<std::pair<int, int> > m_triggerDataPairs;  // +0xac
    int m_fieldB8;                             // +0xb8
    int m_fieldBC;                             // +0xbc
    int m_fieldC0;                             // +0xc0
    std::vector<RewardItemInfo> m_titleReward; // +0xc4
    std::vector<RewardItemInfo> m_selectReward;// +0xd0
    std::vector<int> m_fieldDC;                // +0xdc
    int m_fieldE8;                             // +0xe8
    std::vector<RewardItemInfo> m_rewardItems; // +0xec
    int m_fieldF8;                             // +0xf8
    int m_fieldFC;                             // +0xfc
    char m_exposeQuest;                        // +0x100
    char m_field101;                           // +0x101
    char m_field102;                           // +0x102
    char m_pad103;                             // +0x103
    int m_field104;                            // +0x104
    char m_mailFlag;                           // +0x108
    char m_pad109[3];
    std::string m_mailTitle;                   // +0x10c
    std::string m_mailContents;                // +0x110
    std::vector<int> m_mailItems;              // +0x114
    char m_field120;                           // +0x120
    char m_pad121[3];
    std::vector<dungeonEventHanaseStruct> m_eventDialogs;  // +0x124
    char m_field130;                           // +0x130
    char m_powerSide;                          // +0x131
    char m_pad132[2];
    int m_field134;                            // +0x134
    short m_field138;                          // +0x138
    short m_field13a;                          // +0x13a
    int m_field13c;                            // +0x13c
    int m_field140;                            // +0x140
    std::vector<ENUM_QUEST_EXCEPTION_TYPE> m_exceptionList;  // +0x144
};

#endif  // GAME_QUEST_H_
