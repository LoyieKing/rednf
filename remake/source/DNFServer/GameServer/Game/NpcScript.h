#ifndef GAME_NPCSCRIPT_STRUCT_H
#define GAME_NPCSCRIPT_STRUCT_H

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "randomMessageBalloonInfo.h"   // NpcScript+0x6a4 内嵌对象（ORIG ctor 0x8a00c08）
#include "NpcMovieData.h"               // NpcScript+0x6bc 内嵌对象（ORIG ctor 0x89fb78e）

// ============================================================================
// NpcScript —— NPC 对话脚本结构（ORIG 0x089fb942 ctor / 0x089fbf22 clear /
// 0x089fc264 importNpcScript 逐字段写回还原）。
// 偏移均对照 docs/class_func_reports/NpcScript/{NpcScript,clear}.md 与
// docs/class_func_reports/_lt_global_gt_/importNpcScript.md 的 ORIG 汇编。
// 本头为 NpcScript 及其子类型的唯一声明点。
// ============================================================================

// ORIG 枚举（mangled 标签与 ORIG 一致）：NpcScript+0x4c 的
// std::map<ENUM_NPC_ROLE,int> 键类型（isExistRole 符号
// _ZNK10CNPCScript11isExistRoleE13ENUM_NPC_ROLE 实证）。
enum ENUM_NPC_ROLE
{
    ENUM_NPC_ROLE_0 = 0
};

// ---- ORIG 枚举（mangled 标签与 ORIG 一致；值据 ORIG 汇编）----
// getNpcMoodFromString（0x089faf46）返回 0/1/2/3（"bad"/"normal"/"good"）。
enum ENUM_NPC_MOOD
{
    ENUM_NPC_MOOD_BAD = 0,
    ENUM_NPC_MOOD_NORMAL = 1,
    ENUM_NPC_MOOD_GOOD = 2,
    ENUM_NPC_MOOD_3 = 3
};

// getGiftRewardItem 以 change 索引 stRewardItemInfo_t::m_byDir[3]（0..2）。
enum ENUM_NPC_FAVOR_CHANGE_CONST
{
    ENUM_NPC_FAVOR_CHANGE_0 = 0,
    ENUM_NPC_FAVOR_CHANGE_1 = 1,
    ENUM_NPC_FAVOR_CHANGE_2 = 2
};

// giveGiftItem 在超出每日限额时向引用写 1。
enum ENUM_NPC_FAVOR_GIFT_RESULT_CONST
{
    ENUM_NPC_FAVOR_GIFT_RESULT_OK = 0,
    ENUM_NPC_FAVOR_GIFT_RESULT_FAIL = 1
};

// ORIG 全局（nm: g_npcScriptBaseDirectory B 0x094fe680 [0x100]，
//             g_npcScriptFileList B 0x094fe780 [0x18]）。定义待全局区 TU 提供。
extern char g_npcScriptBaseDirectory[0x100];

struct STScriptFileList
{
    STScriptFileList();                       // ORIG 0x088bacf8
    void Clear();                             // ORIG 0x088bad3a
    std::map<int, std::string> m_fileMap;     // +0x00，sizeof 0x18
    int FindIndexByFullScan(const char* path) const;  // ORIG 0x088bae56 T
    const char* GetFileName(int idx) const;           // ORIG 0x088bad4e T
    int GetIndex(const char* path);  // ORIG T 0x088badba
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
// [dialog]：InteractiveDialogScript（ORIG ctor 0x08a009f2 / clear 0x08a0098a /
// 拷贝 ctor 0x083d7fa4 逐偏移实证：+0/+4/+8 int、+0xc vector<int>、
// +0x18 byte、+0x1c int、+0x20 byte、+0x24 string、+0x28 vector<int>；
// 默认对齐恰好匹配，sizeof 0x34）。
struct InteractiveDialogScript
{
    int m_a;                       // +0x00
    int m_b;                       // +0x04
    int m_c;                       // +0x08
    std::vector<int> m_conditions; // +0x0c
    char m_flag0;                  // +0x18（byte；clear 置 0）
    int m_d;                       // +0x1c
    bool m_flag;                   // +0x20
    std::string m_str;             // +0x24
    std::vector<int> m_list2;      // +0x28

    InteractiveDialogScript();     // ORIG W C1 0x08a009f2（成员 C1 + clear）
    void clear();                  // ORIG T 0x08a0098a
};

// [favor_dialog]：PresentResponceScript。
struct PresentResponceScript
{
    int m_a;            // +0x00
    int m_b;            // +0x04
    int m_c;            // +0x08
    std::string m_str;  // +0x0c

    // ORIG _ZN21PresentResponceScriptC1Ev W 0x8a00aba；
    // clear ORIG 0x8a00a86：三 int 清零 + m_str.clear()
    PresentResponceScript();
    void clear();
};

// [favor_dialog_exception]
struct PresentResponceScriptException
{
    int m_a;            // +0x00
    std::string m_str;  // +0x04

    // ORIG _ZN30PresentResponceScriptExceptionC1Ev W 0x8a00b22；
    // clear 0x8a00b02：m_a=0 + m_str.clear()
    PresentResponceScriptException();
    void clear();
};

// [event_favor_dialog]
struct PresentResponceScriptEvent
{
    int m_a;            // +0x00
    std::string m_str;  // +0x04

    // ORIG _ZN26PresentResponceScriptEventC1Ev W 0x8a00b8a；
    // clear 0x8a00b6a：m_a=0 + m_str.clear()
    PresentResponceScriptEvent();
    void clear();
};


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

    // [prefer item] 等的礼物信息（int@0 / short@4 / short@6）。
    struct stGiftItemInfo_t
    {
        unsigned long m_a;   // +0x00（ORIG make_pair<unsigned long&, GiftInfo&> 键取此字，
                             //  实证 importNpcScript 0x089fd6df `IRmRN...` 去重需 ulong 左值）
        short m_b;   // +0x04
        short m_c;   // +0x06
    };

    // [prefer item] 等的礼物表：ORIG 为 4 个独立 `map<unsigned long,
    // stGiftItemInfo_t> m_X[5]` 数组成员（旧 libstdc++ sizeof(map)=0x18）——
    // C1 W 0x08a00eb0 对 +0x28/+0xa0/+0x118/+0x190 各发一组 `mov $4,ebx`
    // 倒计数 ×5 map::C1 构造循环（P151，组距 0x78）；D1 逆序 4 组循环析构；
    // ORIG nm 无 stGiftTable_t 符号（该包装型系重建发明，P154 仅NEW，2026-09-05
    // EmoC11 数组化归正）。stride 亦见 ORIG clear() +l*0x18 × {0x28,0xa0,0x118,0x190}。
    typedef std::map<unsigned long, stGiftItemInfo_t> GiftMapArray_t;  // 仅命名元素型

    // [reward item]/[favor]：单个奖励元素（3 int）。ORIG 为 POD：无用户析构
    // （ORIG 08a06f28 W D1/D2 为 vector 析构链隐式实例；P12 实证 __uninitialized_copy<true>
    // 链要求 trivial，2026-08-28 OrigFavorRel 删除用户析构声明）。
    struct stRewardItemElement_t
    {
        int m_a;  // +0x00
        int m_b;  // +0x04
        int m_w;  // +0x08
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
    // ORIG +0x010 为匿名嵌套类型实例（Ghidra 伪名 ._140/._199；ORIG 符号
    // _ZN25stFavorableRelationShip_t5._140C1/D1 t 0x089fb8b6/0x089fb8fc 与
    // ._199D1/D2 t 0x0858046c）。GCC 无法声明 `class ._140`；改名 `_140` 自然
    // mangled `4_140` 缺点号不匹配。ctor/dtor 在 FavorableRelationDeps.cpp
    // 显式定义 + `.set` 汇编别名强制 ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。
    // 布局不变：成员经 m_fav. 前缀访问（现有调用方已用 m_fav.m_moodChanges/.m_keyItems）。
    class _140
    {
    public:
        _140();     // ORIG _ZN25stFavorableRelationShip_t5._140C1Ev t 0x089fb8b6（构造两 vector<pair>）
        ~_140();    // ORIG _ZN25stFavorableRelationShip_t5._140D1Ev t 0x089fb8fc（逆序析构）
        std::vector<std::pair<ENUM_NPC_MOOD, unsigned int> > m_moodChanges;   // +0x010（abs 0x180）
        std::vector<std::pair<unsigned long, unsigned int> > m_keyItems; // +0x01c（abs 0x18c）
    } m_fav;                                              // +0x010（sizeof 0x18）
    GiftMapArray_t m_preferItems[5];                  // +0x028（abs 0x198）
    GiftMapArray_t m_preferItemGroups[5];             // +0x0a0（abs 0x210）
    GiftMapArray_t m_unpreferItems[5];                // +0x118（abs 0x288）
    GiftMapArray_t m_unpreferItemGroups[5];           // +0x190（abs 0x300）
    stRewardItem_t m_rewardItems;                     // +0x208（abs 0x378）[level][mood] 全表
    float m_favorRate0;                               // +0x4d8（abs 0x648）
    float m_favorRate1;                               // +0x4dc（abs 0x64c）
    float m_favorRate2;                               // +0x4e0（abs 0x650）
    std::vector<stLevelRewardInfo> m_levelRewards;    // +0x4e4（abs 0x654）
    std::vector<stBuffRewardInfo>  m_buffRewards;     // +0x4f0（abs 0x660）
    // ORIG +0x4fc 为一个 stPeriodRewardInfo 实例（C1 @0x08a00c70 清零标量并
    // 构造三 string；D1 @0x085819e4）；按 ORIG 建模为子结构而非平铺成员，
    // 使 C1 自动清零 m_type/m_a/m_b（与 ORIG 逐字节一致）。
    stPeriodRewardInfo m_durationReward;              // +0x4fc（abs 0x66c，sizeof 0x18）
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
    std::string m_unknown3c;                          // +0x03c（ORIG ctor 0x89fb954 构造 string）
    std::vector<int> m_unknown40;                     // +0x040（ORIG ctor 0x89fba1e 构造 vector<int>）
    std::map<ENUM_NPC_ROLE, int> m_roleMap;           // +0x04c [role]（ORIG map<ENUM_NPC_ROLE,int>）
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
    randomMessageBalloonInfo m_balloonInfo;           // +0x6a4（ORIG ctor 0x8a00c08，sizeof 0x14）
    bool m_hide;                                      // +0x6b8 [hide]
    NpcMovieData m_movieData;                         // +0x6bc（ORIG ctor 0x89fb78e，sizeof 0x2c）
};

#endif  // GAME_NPCSCRIPT_STRUCT_H

