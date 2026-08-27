#ifndef GAME_NPCSCRIPT_STRUCT_H
#define GAME_NPCSCRIPT_STRUCT_H

// ============================================================================
// NpcScript —— NPC 对话脚本结构（ORIG 0x089fb942 ctor / 0x089fbf22 clear /
// 0x089fc264 importNpcScript 逐字段写回还原）。
// 偏移均对照 docs/class_func_reports/NpcScript/{NpcScript,clear}.md 与
// docs/class_func_reports/_lt_global_gt_/importNpcScript.md 的 ORIG 汇编。
// 本头为 NpcScript 及其子类型的唯一声明点。
// ============================================================================

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

// ORIG 全局（nm: g_npcScriptBaseDirectory B 0x094fe680 [0x100]，
//             g_npcScriptFileList B 0x094fe780 [0x18]）。定义待全局区 TU 提供。
extern char g_npcScriptBaseDirectory[0x100];

struct STScriptFileList
{
    char m_pad[0x18];  // ORIG 大小 0x18；内部字段未还原 —「推断」
    int FindIndexByFullScan(const char* path) const;  // ORIG 0x088bae56 T
};
extern STScriptFileList g_npcScriptFileList;

// ---- 外部解析辅助（ORIG 真实符号，定义在各自 TU）----
bool loadRDARScriptFile(const char* directory, const char* path);  // 0x088bbd29
void getDirectoryPath(std::string& out, const char* fmt, ...);     // 0x088bbc80
int getNpcRoleByString(std::string name);                          // 0x089fecee
int getNpcMoodFromString(const char* name);                        // 0x089faf46
int getCharacterJob(const char* name);                             // 0x088be57e

// ---- 子类型（字段按 importNpcScript 解析顺序；大小为「推断」）----

// [click mark]：type==0 时附带一个参数 int。
struct ClickMarkInfo
{
    int m_type;    // +0x00
    int m_param;   // +0x04
};

// [dialog]：InteractiveDialogScript（ORIG ctor 0x08a009f2）。
struct InteractiveDialogScript
{
    int m_a;                       // +0x00
    int m_b;                       // +0x04
    int m_c;                       // +0x08
    std::vector<int> m_conditions; // +0x0c（count==-1 时置 size1 标记，推断）
    int m_d;                       // +0x18
    bool m_flag;                   // +0x1c
    std::string m_str;             // +0x20
    std::vector<int> m_list2;      // +0x2c
};

// [favor_dialog]：PresentResponceScript。
struct PresentResponceScript
{
    int m_a;            // +0x00
    int m_b;            // +0x04
    int m_c;            // +0x08
    std::string m_str;  // +0x0c
};

// [favor_dialog_exception]
struct PresentResponceScriptException
{
    int m_a;            // +0x00
    std::string m_str;  // +0x04
};

// [event_favor_dialog]
struct PresentResponceScriptEvent
{
    int m_a;            // +0x00
    std::string m_str;  // +0x04
};

// [prefer item] 等的礼物信息（解析顺序：int, int, short）—「推断」
struct stGiftItemInfo_t
{
    int            m_a;   // +0x00
    int            m_b;   // +0x04
    short          m_c;   // +0x08
};

// [prefer item] 等的礼物表：ORIG 每张 0x78 = 5 个相邻
// std::map<unsigned long, stGiftItemInfo_t>（旧 libstdc++ sizeof(map)=0x18；
// stride 见 ORIG clear() 的 +l*0x18 + {0x28, 0xa0, 0x118, 0x190}），按 slot 索引取表。
struct stGiftTable_t
{
    std::map<unsigned long, stGiftItemInfo_t> m_maps[5];
};
static_assert(sizeof(stGiftTable_t) == 0x78, "stGiftTable_t size mismatch");


// stFavorableRelationShip_t —— 内嵌于 NpcScript +0x170，总大小 0x514
// （+0x170 .. +0x684）。偏移为相对本结构的偏移。
// 嵌套类型名/生命周期按 ORIG mangled（_ZN25stFavorableRelationShip_t*）权威建模：
//   stRewardItemElement_t D1/D2 @0x08a06f28、stBuffRewardInfo C1 @0x085818f0 / D1 @0x08a073d4、
//   stLevelRewardInfo C1/C2ERKS @0x0858188a / 0x08a04160、operator= @0x08a04268、D1 @0x085823c6、
//   stPeriodRewardInfo C1 @0x08a00c70 / D1 @0x085819e4、stRewardItemInfo_t C1 @0x08a00d58 /
//   D1 @0x08581a80 / clear T @0x089fb4c0、stRewardItem_t C1 @0x08a00dcc / D1 @0x08581aae /
//   clear T @0x089fb508。
class stFavorableRelationShip_t
{
public:
    stFavorableRelationShip_t() {}
    ~stFavorableRelationShip_t() {}
    void clear();  // ORIG 0x089fb56c T（本 TU 提供）

    // [reward item]/[favor]：单个奖励元素（3 int）。ORIG 析构为空体 ret。
    struct stRewardItemElement_t
    {
        int m_a;  // +0x00
        int m_b;  // +0x04
        int m_w;  // +0x08
        ~stRewardItemElement_t();
    };

    // [reward item] 每 (favorLevel, mood) 槽：3 个方向 vector + 3 个权重计数，sizeof=0x30。
    // ORIG vector 槽 = 0x378 + dir*0xc + mood*0x30 + level*0x90；
    //      计数     = 0x39c + dir*4   + mood*0x30 + level*0x90。
    struct stRewardItemInfo_t
    {
        std::vector<stRewardItemElement_t> m_byDir[3];
        unsigned int m_weight[3];
        stRewardItemInfo_t();
        ~stRewardItemInfo_t();
        void clear();
    };

    // [reward item] 全表：5 个奖励等级 × 3 个 mood，sizeof=5×0x90=0x2d0。
    struct stRewardItem_t
    {
        stRewardItemInfo_t m_slots[5][3];  // [level][mood]
        stRewardItem_t();
        ~stRewardItem_t();
        void clear();
    };

    // [favor level reward mail] 条目，sizeof=0x14（旧 libstdc++ std::string 大小为 4）。
    struct stLevelRewardInfo
    {
        unsigned short m_level;   // +0x00
        unsigned int   m_flag0;   // +0x04
        unsigned int   m_flag1;   // +0x08
        std::string    m_name;    // +0x0c
        std::string    m_desc;    // +0x10
        stLevelRewardInfo();
        stLevelRewardInfo(const stLevelRewardInfo&);
        ~stLevelRewardInfo();
        stLevelRewardInfo& operator=(const stLevelRewardInfo&);
    };

    // [favor_duration_reward_buff] 条目（2×ushort + int），平凡析构。
    struct stBuffRewardInfo
    {
        unsigned short m_a;   // +0x00
        unsigned short m_b;   // +0x02
        unsigned int   m_c;   // +0x04
        stBuffRewardInfo();
        ~stBuffRewardInfo();
    };

    // 周期奖励条目，sizeof=0x18：
    // ORIG C1 顺序 = 三 string 默认构造(+0x4/+0x10/+0x14) → 标量清零(word@0,int@8,int@0xc)
    //              → 三个 string 赋空串；D1 逆序析构(0x14→0x10→0x4)。
    struct stPeriodRewardInfo
    {
        unsigned short m_type;        // +0x00
        std::string    m_name;        // +0x04
        int            m_a;           // +0x08
        int            m_b;           // +0x0c
        std::string    m_desc;        // +0x10
        std::string    m_param;       // +0x14
        stPeriodRewardInfo();
        ~stPeriodRewardInfo();
    };


    bool m_enabled;                                   // +0x000（abs 0x170）
    int  m_defaultFavor;                              // +0x004（abs 0x174）
    unsigned short m_maxGiftPerDay;                   // +0x008（abs 0x178）
    unsigned short m_dialogDayGap1;                   // +0x00a（abs 0x17a）
    unsigned short m_dialogDayGap2;                   // +0x00c（abs 0x17c）
    // +0x00e pad 2
    std::vector<std::pair<int, unsigned int> > m_moodChanges;   // +0x010（abs 0x180）
    std::vector<std::pair<unsigned long, unsigned int> > m_keyItems; // +0x01c（abs 0x18c）
    stGiftTable_t m_preferItems;                      // +0x028（abs 0x198）
    stGiftTable_t m_preferItemGroups;                 // +0x0a0（abs 0x210）
    stGiftTable_t m_unpreferItems;                    // +0x118（abs 0x288）
    stGiftTable_t m_unpreferItemGroups;               // +0x190（abs 0x300）
    stRewardItem_t m_rewardItems;                     // +0x208（abs 0x378）[level][mood] 全表
    float m_favorRate0;                               // +0x4d8（abs 0x648）
    float m_favorRate1;                               // +0x4dc（abs 0x64c）
    float m_favorRate2;                               // +0x4e0（abs 0x650）
    std::vector<stLevelRewardInfo> m_levelRewards;    // +0x4e4（abs 0x654）
    std::vector<stBuffRewardInfo>  m_buffRewards;     // +0x4f0（abs 0x660）
    unsigned short m_durationRewardType;              // +0x4fc（abs 0x66c）
    std::string m_durationRewardName;                 // +0x500（abs 0x670）
    int m_durationRewardA;                            // +0x504（abs 0x674）
    int m_durationRewardB;                            // +0x508（abs 0x678）
    std::string m_durationRewardStr1;                 // +0x50c（abs 0x67c）
    std::string m_durationRewardStr2;                 // +0x510（abs 0x680）
};

// ---- NpcScript 主体 ----
class NpcScript
{
public:
    NpcScript();   // ORIG 0x089fb942 C1/C2
    ~NpcScript();  // ORIG 0x08581daa D1/D2（W）
    void clear();  // ORIG 0x089fbf22

    int m_scriptFileIndex;                            // +0x000
    std::string m_name;                               // +0x004 [name]
    std::string m_fieldName;                          // +0x008 [field name]
    std::string m_smallFace;                          // +0x00c [small face]
    int m_smallFaceId;                                // +0x010
    std::string m_bigFace;                            // +0x014 [big face]
    int m_bigFaceId;                                  // +0x018
    std::string m_popupFace;                          // +0x01c [popup face]
    int m_popupFaceId;                                // +0x020
    std::string m_fieldAnimation;                     // +0x024 [field animation]
    std::string m_fieldWav;                           // +0x028 [field wav]
    std::string m_popupWav[2];                        // +0x02c/+0x030 [popup wav]
    bool m_lookAround;                                // +0x034 [look around]
    int m_unknown38;                                  // +0x038 clear=-1 —「推断」
    char m_pad3c[0x10];                               // +0x03c..0x4b ctor 构造两个子对象，类型未还原 —「推断」
    std::map<int, int> m_roleMap;                     // +0x04c [role]
    std::string m_questSpeech;                        // +0x064 [quest speech]
    std::vector<int> m_skills[12];                    // +0x068 [skill]（0xb=common，job 索引）
    std::string m_moveTownLimitStr;                   // +0x0f8 [move town limit str]
    std::string m_moveTownConfirmStr;                 // +0x0fc [move town confirm str]
    std::string m_directoryPath;                      // +0x100 getDirectoryPath("%s%s")
    std::vector<std::string> m_stringData;            // +0x104 [string data]
    std::vector<int> m_intData;                       // +0x110 [int data]
    std::vector<std::string> m_balloonMessages;       // +0x11c [balloon message]
    std::vector<int> m_balloonTicks;                  // +0x128 [balloon message]
    std::vector<InteractiveDialogScript> m_dialogs;   // +0x134 [dialog]
    std::vector<PresentResponceScript> m_presentResponces; // +0x140 [favor_dialog]
    std::vector<std::string> m_firstFavorDialogs;     // +0x14c [1st favor dialog]
    std::vector<PresentResponceScriptException> m_presentResponceExceptions; // +0x158
    std::vector<PresentResponceScriptEvent> m_presentResponceEvents;         // +0x164
    stFavorableRelationShip_t m_favorableRelationShip;// +0x170 [favorable relationship]
    std::string m_favorDurationBuffDialog;            // +0x684 [favor_duration_reward_buff_dialog]
    bool m_visibleOnDungeonClear;                     // +0x688 [visible on dungeon clear]
    std::vector<std::string> m_webPages;              // +0x68c [web page]
    std::vector<ClickMarkInfo> m_clickMarks;          // +0x698 [click mark]
    std::string m_randomMessageAnimation;             // +0x6a4 [random message animation]
    int m_randomMessageProbability;                   // +0x6a8 [random message probability]
    std::vector<std::string> m_randomMessageList;     // +0x6ac [random message list]
    bool m_hide;                                      // +0x6b8 [hide]
    // ORIG +0x6bc 为内嵌 NpcMovieData 对象，其首成员即 movie path 队列 —「推断」
    std::deque<std::string> m_moviePaths;             // +0x6bc [movie path]
    int m_moviePathIndex;                             // +0x6e4 [movie path] 置 0
};

#endif  // GAME_NPCSCRIPT_STRUCT_H

