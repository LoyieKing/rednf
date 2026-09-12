#ifndef DNF_GAME_SIG_TYPES_C_H_
#define DNF_GAME_SIG_TYPES_C_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 C（CHARAC_DATA/TAG_*/SIG_LOAD_* 域）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================

#include <string>  // std::string（CHARAC_LOAD_MERCENARY::GetRewardItem 形参）
#include <bitset>  // std::bitset<32>（SIG_LOAD_ETC::m_bitset，ORIG 类型）

#include <cstddef>  // offsetof static_assert
// SIG_LOAD_PVP_BUDDY 载荷引用 shared 权威 STPvPBuddyDBInfo（0x2b 步长）。
#include <string.h>  // user_gold_info::reset/toCopy（ORIG 头内联，W 弱符号）
#include "CCharacter.h"   // _Additioal_info 权威定义（CHARAC_DATA 真实子对象 +0x60）
#include "Inven_Item.h"   // Inven_Item 权威定义（CHARAC_DATA 真实成员 m_items[21]）
#include "MasteredSkill.h"  // _Mastered_skill（TAG_CHARAC_ETC_DATA/SIG_LOAD_SKILL 数组成员构造）
#include "STSkillCommandData.h"  // STSkillCommandData::STCommand（SIG_LOAD_SKILL 命令区）
#include "QuestClear.h"   // WongWork::STClearedQuest（SIG_LOAD_QUEST 完成位图元素，1 字节）
#include "GameResultType.h"  // GameResultType（SIG_LOAD_ETC +0x3c8 记录区，0x50 步长）
#include "PvpResultType.h"   // PvpResultType（SIG_LOAD_ETC +0x340 区）
#include "GamePODTypes.h"    // AvatarDetailInfo（m_avatarDetail 真实元素类型，0x4b）
#include "CTitleBook.h"      // stItemLockInfo（SIG_LOAD_INVENTORY +0x14784 锁区，8 字节）
                             //  + stTitleBook（SIG_LOAD_ETC +0x466a 称号簿真实子对象，0x6b3a）
#include "CAchievement.h"    // stAchievement_DBData（SIG_LOAD_ETC +0xb1a4 成就真实子对象，0x1808）
#include "Redeem_Item__CRedeemItem.h"  // Redeem_Item::Inven_Item_Expand_RedeemInfo（SIG_REDEEMINFO_LIST_TO_DB 元素，0x44）
#include "SIG_SAVE_EXPAND_EQUIPSLOT.h"  // SIG_SAVE_EXPAND_EQUIPSLOT（SIG_LOAD_INVENTORY +0x14aa4 子对象）
#include "CGameOption.h"     // CHotkeyOption（SIG_LOAD_CHARAC_HOTKEY_OPTION +0x05 子对象，0xbc）
#include "../../../shared/packet/include/STPvPBuddyDBInfo.h"

// ---- CHARAC_LOAD_MERCENARY 方法声明依赖的前向（完整定义见 CUser.h/PacketTypes.h）----
struct CUser;
struct competition_reward_result;
class Inven_Item;

// ORIG sizeof = 0x703（CACHE_CHARACTER_TYPE@+0x0 → +0x703）。
// CHARAC_DATA（SIG_LOAD_CHARAC 的 +0xc 区，0x67a 字节）：getCharacInfoDetail @0x864bdba
// 依偏移填充（源为 _Charac_info 成员；具名字段见 CUser.cpp 实现）。
// 尾部 +0x676（SIG 内绝对偏移 0x682）为 school no：RequestDBCharac @0x81c6ca0
// 写 CUser::GetSchoolNo；CHARAC_DATA::Reset（W 0x822ec9e）不覆盖该 4 字节。
// TAG_CHARAC_DATA（+0x68b 区，0x78 字节）：getTagCharacInfo @0x86904f0 填充。
#pragma pack(push, 1)
struct CHARAC_DATA
{
    // 前段 0x60：getCharacInfoDetail @0x0864bdba（ORIG 汇编逐条宽度实证，
    // 报告 docs/class_func_reports/CUser/getCharacInfoDetail.md）；m_data 保留
    // 兼容既有 blob 视图消费者（Main 协调 2026-09-04，union 叠加）。
    union
    {
        char m_data[0x60];
        struct
        {
            int            m_characNo;   // +0x00（mov %edx,(%eax)；←src+0x00）
            char           m_name[0x1e]; // +0x04（strncpy 0x1e；←src m_name）
            // 2026-09-05 §4.3（DBCharac 批次）：DB 绑定别名 union——get_* callee
            // 即 ORIG 引用类型实证（_getCharacInfo/_getCharacStat 报告：
            // get_byte(char&)@0x080fde54 / get_short(short&)@0x080fddec /
            // get_uint@0x080e22f2）；旧名旧类型保留，消费者零改动。
            union { unsigned char  m_field22;   // +0x22（movb；←src+0x22）
                            char           m_village;      // +0x22 charac_stat.village
        };
            union { unsigned char  m_field23;   // +0x23（movb；←src+0x25）
                            char           m_villagePrev;  // +0x23 village_prev
    };
            union { unsigned char  m_field24;   // +0x24（movb；←src+0x26 job）
                            char           m_job;          // +0x24 charac_info.job（get_byte char&）
            };
            union { unsigned short m_field25;   // +0x25（mov %dx 2B；←src+0x27 movzwl）
                            short          m_lev;          // +0x25 lev（get_short short&）
            };
            union { unsigned char  m_field27;   // +0x27（movb；←src+0x29）
                            char           m_growType;     // +0x27 grow_type（get_byte char&）
            };
            union { int            m_field28;   // +0x28（movl；←src+0x2b）
                            unsigned int   m_exp;          // +0x28 exp（get_uint）
            };
            unsigned short m_field2c;    // +0x2c（movzwl 0x33）
            unsigned short m_field2e;    // +0x2e（movzwl 0x35）
            unsigned short m_field30;    // +0x30（movzwl 0x37）
            union { unsigned short m_field32;   // +0x32（movzwl 0x4f）
                            short          m_fatigueBattery; // +0x32 fatigue_battery_charging（get_short short&）
            };
            char           m_field34[0x19]; // +0x34（CommonTime::GetCreateTime 出参区至 +0x4d）
            union { int            m_field4d;   // +0x4d（movl；←src+0x7b CharLastPlayTick）
                            unsigned int   m_lastPlayTime; // +0x4d last_play_time（get_uint）
            };
            char           m_pad51[5];   // +0x51..+0x55（本函数无写点）
            union { int            m_guildId;   // +0x56（movl；←src m_guildId@0xdbe）
                            unsigned int   m_guildIdUint;  // +0x56 guild_id（get_uint 绑定视图）
            };
            unsigned char  m_field5a;    // +0x5a（movb；←src+0xdc2）
            unsigned short m_field5b;    // +0x5b（movzwl 0x77）
            unsigned short m_field5d;    // +0x5d（movzwl 0x79）
            unsigned char  m_field5f;    // +0x5f（movb $0/$1；←src+0xe22!=0）
        };
    };
    _Additioal_info m_add;      // +0x060（pack1，0x52）
    // +0x0b2..+0x137 未还原字段组件视图（ORIG Reset 0x822ec9e 尾部字节簇 + dword）
    struct UnknownB2
    {
        // m_pad0 union 叠加 getCharacInfoDetail 实证写字段（宽度=ORIG store 指令）。
        union
        {
            char m_pad0[0x72];  // +0x0b2..+0x123
            struct
            {
                unsigned char  m_fieldb2;   // +0x0b2（movb；←src m_stamina@0x75）
                unsigned short m_fieldb3;   // +0x0b3（movzwl 0x4d）
                // 2026-09-05 §4.3（DBCharac 批次）：charac_stat DB 列绑定别名
                // union——callee 实证见 _getCharacStat 报告（get_uint@0x080e22f2 /
                // get_int(int&)@0x0811692c / get_byte(char&)@0x080fde54 /
                // get_short(short&)@0x080fddec）；旧名旧类型保留，消费者零改动。
                union { int            m_fieldb5;   // +0x0b5（movl；←src+0x51）
                        unsigned int   m_tutorialFlag;      // +0x0b5 tutorial_flag（get_uint）
                };
                char           m_fieldb9[0x10]; // +0x0b9（memcpy 0x10；←src+0x55）
                union { int            m_fieldc9;   // +0x0c9（CReliablePerson::GetTradeGoldTotal）
                        unsigned int   m_tradeGoldTotal;    // +0x0c9 trade_gold_total（get_uint）
                };
                int            m_fieldcd;   // +0x0cd（GetTradeGoldTotalBillion）
                union { int            m_fieldd1;   // +0x0d1（GetTradeGoldDaily）
                        unsigned int   m_tradeGoldDaily;    // +0x0d1 trade_gold_daily（get_uint）
                };
                int            m_fieldd5;   // +0x0d5（←src+0xe49）
                int            m_fieldd9;   // +0x0d9（←src+0xe4d）
                int            m_fielddd;   // +0x0dd（←src+0xe51）
                int            m_fielde1;   // +0x0e1（←src+0xe55）
                int            m_fielde5;   // +0x0e5（←src+0xe59）
                int            m_fielde9;   // +0x0e9（←src+0xe5d）
                int            m_fielded;   // +0x0ed（←src+0xe45）
                int            m_fieldf1;   // +0x0f1（←src+0xe68）
                union { unsigned char  m_fieldf5;   // +0x0f5（movb；←src+0xe70）
                        char           m_openFlag;          // +0x0f5 open_flag（get_byte char&）
                };
                int            m_fieldf6;   // +0x0f6（←src+0xe71）
                int            m_fieldfa;   // +0x0fa（←src+0xe79）
                union { int            m_fieldfe;   // +0x0fe（←src+0xdf4）
                        unsigned int   m_helpAbuseRatio;    // +0x0fe help_abuse_ratio（get_uint）
                };
                union { int            m_field102;  // +0x102（←src+0xdf8）
                        unsigned int   m_helpAbuseExp;      // +0x102 help_abuse_exp（get_uint）
                };
                int            m_field106;  // +0x106（←src+0xe7d）
                int            m_field10a;  // +0x10a（←src+0xe81）
                union { unsigned short m_field10e;  // +0x10e（movzwl 0x121f）
                        short          m_powerWarPoint;     // +0x10e power_war_point（get_short）
                };
                union { int            m_field110;  // +0x110（←src+0x1221）
                        unsigned int   m_pwAssaultCount;    // +0x110 power_war_assault_count（get_uint）
                };
                union { int            m_field114;  // +0x114（←src+0x1225；ORIG 重复写两次）
                        unsigned int   m_pwAssaultVicCount; // +0x114 power_war_assault_victory_count（get_uint）
                };
                union { int            m_field118;  // +0x118（←src+0x7f）
                        unsigned int   m_lastPlayTimePW;    // +0x118 last_play_time_powerwar（get_uint）
                };
                union { unsigned short m_field11c;  // +0x11c（movzwl 0x122c）
                        short          m_emotion;           // +0x11c emotion（get_short）
                };
                union { unsigned char  m_field11e;  // +0x11e（movb；←src+0x1239 movzbl）
                        char           m_skillTreeIndex;    // +0x11e skill_tree_index（get_byte char&）
                };
                union { unsigned char  m_field11f;  // +0x11f（movb；←src+0x123e）
                        char           m_addSlotFlag;       // +0x11f add_slot_flag（get_byte char&）
                };
                union { unsigned char  m_field120;  // +0x120（movb；←src+0x123f）
                        char           m_eventCharacLevel;  // +0x120 event_charac_level（get_byte char&）
                };
                union { unsigned char  m_field121;  // +0x121（movb；←src+0x1241）
                        char           m_memberDungeonFlag; // +0x121 member_dungeon_flag（get_byte char&）
                };
                union { unsigned short m_field122;  // +0x122（movzwl 0x126c）
                        short          m_memberBonusFatigue;// +0x122 member_bonus_fatigue（get_short）
                };
            };
        };
        char m_b124;            // +0x124
        union
        {
            char m_pad125[4];   // +0x125..+0x128
            int  m_i125;        // +0x125（movl；←src+0x126f，getCharacInfoDetail 实证）
            unsigned int m_lastPlayDungeonIndex; // +0x125 last_play_dungeon_index（get_uint）
        };
        char m_b129;            // +0x129
        char m_b12a;            // +0x12a
        char m_b12b;            // +0x12b
        union { char m_b12c;            // +0x12c
                unsigned char m_giveRewardFlag;  // +0x12c give_reward_flag（_LoadEventBringUpInfo uchar 存）
        };
        union { char m_b12d;            // +0x12d
                unsigned char m_useMaterialCnt;  // +0x12d use_material_cnt（_LoadEventBringUpInfo uchar 存）
        };
        char m_b12e;            // +0x12e（ORIG Reset 不清）
        union { char m_b12f;            // +0x12f
                unsigned char m_growthPowerReward; // +0x12f growth_power_reward（get_ubyte uchar&）
        };
        int  m_i130;            // +0x130
        union
        {
            char m_pad134[4];   // +0x134..+0x137
            int  m_i134;        // +0x134（movl；=get_acc_id()，getCharacInfoDetail 实证）
            unsigned int m_mId;             // +0x134 charac_info.m_id（get_uint）
        };
    };
    UnknownB2 m_unkB2;          // +0x0b2（sizeof 0x86）
    // ORIG C1 0x822ec4c 循环 esi=0x15 起减到 0 共 22 次 → m_items[22]
    // （0x138 + 22×0x3d = 0x676；Reset memset(0x138,0,0x53e) 覆盖到 +0x676）。
    Inven_Item m_items[22];     // +0x138（22×0x3d = 0x53e，至 +0x676）
    unsigned int m_schoolNo;    // +0x676（RequestDBCharac 写入；Reset 不清）

    CHARAC_DATA() { Reset(); }  // ORIG W 0x822ec4c _ZN11CHARAC_DATAC1Ev
    void Reset();               // ORIG W 0x822ec9e _ZN11CHARAC_DATA5ResetEv
};
static_assert(sizeof(CHARAC_DATA) == 0x67a, "CHARAC_DATA ABI drift");
#pragma pack(pop)

#pragma pack(push, 1)
struct TAG_CHARAC_DATA
{
    // ORIG C1 0x822edae 实证：+0x26 构造 _Additioal_info（0x52）后调 clear()。
    // 前段 0x26 字段化：getTagCharacInfo @0x086904f0（ORIG 汇编宽度实证，
    // 报告 docs/class_func_reports/CUser/getTagCharacInfo.md）；m_data 保留兼容。
    union
    {
        char m_data[0x26];      // +0x00..+0x25（blob 前段视图）
        struct
        {
            int            m_characNo;  // +0x00（movl；←src+0x00）
            char           m_name[0x1d]; // +0x04（strncpy 0x1d；←src m_name）
            // 2026-09-05 §4.3（DBCharac 批次）：_GetTagCharacInfo（0x08409fa6 报告）
            // get_byte(char&)@0x080fde54 / get_short(short&)@0x080fddec 绑定别名；
            // 旧名旧类型保留（getTagCharacInfo/setTagCharacInfo 消费者零改动）。
            union { unsigned char  m_field21;   // +0x21（movb；←src+0x26 movzbl）
                    char           m_job;       // +0x21 charac_info.job（get_byte char&）
            };
            union { unsigned short m_field22;   // +0x22（mov %dx；←src+0x27 movzwl）
                    short          m_lev;       // +0x22 lev（get_short short&）
            };
            union { unsigned char  m_field24;   // +0x24（movb；←src+0x29）
                    char           m_growType;  // +0x24 grow_type（get_byte char&）
            };
            union { unsigned char  m_field25;   // +0x25（movb；←src+0x1239）
                    char           m_skillTreeIndex; // +0x25 skill_tree_index（get_byte char&）
            };
        };
    };
    _Additioal_info m_add;      // +0x26（pack1 0x52）

    TAG_CHARAC_DATA() { clear(); }  // ORIG W 0x822edae _ZN15TAG_CHARAC_DATAC1Ev
    void clear();              // ORIG W 0x822ed84 _ZN15TAG_CHARAC_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_DATA) == 0x78, "TAG_CHARAC_DATA ABI drift");
#pragma pack(pop)

// SIG_LOAD_INVENTORY +0xc 的 TAG_CHARAC_INVEN_DATA（0x4024 字节）：getTagCharacInventory @0x869067c 填充。
#pragma pack(push, 1)
struct TAG_CHARAC_INVEN_DATA
{
    // ORIG C1 0x0848f774：Inven_Item[0x16]@+0（0x16×0x3d=0x52c）+ Inven_Item[0x69]@+0x53e
    // （0x69×0x3d=0x18f9）两组倒计数构造后调 clear()（P151）。
    Inven_Item m_items[0x16];         // +0x000（0x16×0x3d = 0x53e）
    Inven_Item m_items2[0x69];        // +0x53e（0x69×0x3d = 0x1905 → +0x1e43）
    // +0x1e43 区：getTagCharacInventory @0x0869067c（报告 ## 汇编）实证
    // CAvatarItemMgr::GetItem(out+0x1e47 avatar 明细, out+0x1e43 int* 计数)、
    // GetClearAvatar(out+0x3ff8, 0x28B)、movl cargo->GetCapacity()→out+0x4020。
    // m_pad1e43 保留兼容，union 叠加具名。
    union
    {
        char m_pad1e43[0x4024 - 0x1e43];  // +0x1e43（0x21e1）
        struct
        {
            int  m_avatarCount;                    // +0x1e43（GetItem int* 出参）
            // b28d-solo-CUser：char[0x21b1]→AvatarDetailInfo[0x73]（0x73×0x4b=0x21b1）。
            //   ORIG getTagCharacInventory @0x08690708 该出参为单条 `lea 0x1e47(%eax),%ebx`；
            //   我方 `(AvatarDetailInfo*)&out->m_avatarDetail[0]`（char 数组+cast）4.4 恒拆
            //   `add $0x1e47,%eax; mov %eax,%ebx`（探针 /tmp/probe_lea2.cpp g1/g11/g14/g18/g19
            //   五形全败；g17 真实类型数组退化出双 lea）。真实类型 + 数组名退化即复现。
            //   sizeof/偏移/外层 0x4024 断言不变。
            AvatarDetailInfo m_avatarDetail[0x73]; // +0x1e47（0x73×0x4b = 0x21b1）
            // b28c-L5：char[0x28]→unsigned long[10]。ORIG getTagCharacInventory
            //   @0x08690708 该出参装载为单条 `lea 0x3ff8(%eax),%ebx`，而调用点写
            //   `reinterpret_cast<unsigned long*>(out->m_clearAvatar)`（char 数组 +
            //   显式 cast）时 4.4 恒拆 `add $0x3ff8,%eax; mov %eax,%ebx`（+1 条）。
            //   callee 权威形参即 `GetClearAvatar(unsigned long*)`（CInventory.h，
            //   C28L2 已实证该参数侧同型化）⇒ 成员按真实类型建模即可免 cast、
            //   复现 lea 单条。sizeof 0x28/偏移/外层 0x4024 断言均不变。
            unsigned long m_clearAvatar[10];       // +0x3ff8（GetClearAvatar 出参）
            unsigned int m_cargoCapacity;          // +0x4020（movl）
        };
    };

    TAG_CHARAC_INVEN_DATA() { clear(); }  // ORIG C1 0x0848f774
    void clear();              // ORIG W 0x822edd0 _ZN21TAG_CHARAC_INVEN_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_INVEN_DATA) == 0x4024, "TAG_CHARAC_INVEN_DATA ABI drift");
#pragma pack(pop)

// RequestDBETC @0x81c7602 在 SIG_LOAD_ETC +0x10 调用的组合 tag 区：
// 覆盖 tagSkillSlot[0x198] + tagSkillCombo[0x198] = 0x330 字节
//（ORIG W 0x822edf4 _ZN19TAG_CHARAC_ETC_DATA5clearEv = memset(this,0,0x330)）。
// ORIG C1 0x0848f7dc：外层 2 轮 × 内层初值 0xcb（=204 元素，步长 2）的嵌套构造
// 循环、行步长 0x198，之后调 clear()。
#pragma pack(push, 1)
struct TAG_CHARAC_ETC_DATA
{
    // [Lane6 2026-09-10] 真实二维数组成员（ORIG 0x0848f7dc 的嵌套构造循环 = 编译器对
    // `T m[2][204]` 的隐式生成：外层 2 轮、内层初值 0xcb 即 204 元素、行步长 0x198）。
    // 旧「平铺 [0xcb]+pad[2] ×2」既少构造 1 个元素/行（真语义 bug），又使 4.4 发两个
    // 独立循环（+1 静态 call-site）。探针 /tmp/l6probe/p1.cpp 实证 4.4.7 二维形逐条同形。
    // 行 0 = 旧 m_skillSlot（+0x000），行 1 = 旧 m_skillCombo（+0x198）；sizeof 0x330 不变。
    _Mastered_skill m_skills[2][204];    // +0x000..0x32f

    TAG_CHARAC_ETC_DATA() { clear(); }   // ORIG C1 0x0848f7dc
    void clear();              // ORIG W 0x822edf4
};
static_assert(sizeof(TAG_CHARAC_ETC_DATA) == 0x330, "TAG_CHARAC_ETC_DATA ABI drift");
#pragma pack(pop)

#pragma pack(push, 1)
struct SIG_LOAD_CHARAC
{
    unsigned int     m_accId;           // +0x000
    unsigned int     m_characNo;        // +0x004
    int              m_result;          // +0x008
    CHARAC_DATA      m_characData;      // +0x00c（getCharacInfoDetail 写；尾部含 m_schoolNo）
    // b28c-L5：unsigned int→int。ORIG GetCharacInfo @0x0864bd1x 该槽 store 为
    //   `mov 0xc(%ebp),%edx; mov %eax,0x686(%edx)`（值驻 %eax + base 占 %edx 的
    //   **同型直存**），而 getCurCharacTagCharacNo() 权威返回 int（CUserCharacInfo.h:959）
    //   ⇒ unsigned 目标使 4.4 多插一段 `mov %eax,%edx` 异型中转（P211 双向开关）。
    //   另一消费点 CUser.cpp:11021 本就是无 cast 的 int 直赋值，换型后同型。
    int              m_tagCharacNo;     // +0x686
    unsigned char    m_strikerSkillIndex; // +0x68a
    TAG_CHARAC_DATA   m_tagCharacData;  // +0x68b（0x78 字节；getTagCharacInfo 写 + CUser.h:7469 直接
                                        //   透传，clear() 为 ORIG W 0x822ed84；类型化保 .clear() 调用）
};
static_assert(sizeof(SIG_LOAD_CHARAC) == 0x703, "SIG_LOAD_CHARAC ABI drift");
// 注：SIG_LOAD_CHARAC 含 non-trivial 成员（CHARAC_DATA 用户构造），GCC4.4
// offsetof 对其未定义，偏移由成员声明序 + pack(1) 保证，删除 offsetof 断言。
#pragma pack(pop)

struct SIG_LOAD_CHARAC_HOTKEY_OPTION
{
    // +0x00 uint 视图（DB 载荷 accId，Dispatcher_SaveGameOption_2::process
    // 0x081cee64 store `call get_acc_id; mov -0xc,%edx; mov %eax,(%edx)`
    // 具名字段同型直存形实证，探针 store.cpp hC；pad 解引用形反产 LHS 早载
    // 跨 call %ebx 缓存）。sizeof 0xc4 不变。
    // [Lane31W8LTDBTaskView] 补 +0x04 hotkeyType（ORIG DB_LoadCharacHotKeyOption
    // ::dispatch 0x0842fd42 `movzbl 0x4(%eax)`）与 +0x05 hotkey 字节区（0xbc，
    // uncompress_zip 目标，ORIG `lea 0x5(%eax)` 实证）。CHotkeyOption 有构造函数，
    // C++03 禁止其作 union 成员 ⇒ 头内以字节区表达；消费点按地址转 CHotkeyOption*
    // 调用其成员（clear）保留 ORIG 的 CHotkeyOption::clear 弱符号调用。
    // m_pad 字节视角保留（Dispatchers.cpp m_pad[4]/[5] 不动）。
    union
    {
        char m_pad[0xc4];
        struct
        {
            unsigned int m_uid;          // +0x00
            unsigned char m_hotkeyType;  // +0x04
            char m_hotkey[0xbc];         // +0x05（CHotkeyOption 字节视角）
            char m_pad_c1[3];            // +0xc1..+0xc3
        };
    };
};
static_assert(sizeof(SIG_LOAD_CHARAC_HOTKEY_OPTION) == 0xc4,
              "SIG_LOAD_CHARAC_HOTKEY_OPTION ABI drift");
static_assert(0x005 + sizeof(((SIG_LOAD_CHARAC_HOTKEY_OPTION*)0)->m_hotkey) == 0xc1,
              "SIG_LOAD_CHARAC_HOTKEY_OPTION m_pad_c1 at +0xc1");

struct SIG_LOAD_CLEANPAD_POINT
{
    char m_pad[0x3cf0];
};

// 字段化（LinkClosureB-2，DB_LoadDimension::makeRequest 0x08431d8c 证据，
// sizeof 不变）：
struct SIG_LOAD_DIMENSION
{
    unsigned int m_field00;        // +0x00（makeRequest 写 accId 类参数）
    char m_dimensionInout[6];      // +0x04..+0x09（get_dimensionInout(0..5)）
    char m_pad0a[2];               // +0x0a..+0x0b
    char m_limitInoutCount;        // +0x0c（get_limit_inout_count(2)）
    char m_pad0d[3];
};

struct SIG_LOAD_DNF_HACK_LOG
{
    char m_pad[0x3cf4];
};

// ORIG sizeof = 0x1810（CACHE_CHARACTER_TYPE@+0x1d2f3 → +0x1eb03）。
// CUser::GetDungeonClear @0x864de58：+0xc 清零 int，+0x10 起 CDungeonClear::toString(buf,0x1800)。
struct SIG_LOAD_DUNGEON_CLEAR
{
    unsigned int m_characNo;       // +0x000（RequestDBDungeonClear @0x81c7436 ← get_charac_no）
    unsigned int m_accId;          // +0x004（同上 ← get_acc_id）
    char         m_pad8[4];        // +0x008
    int      m_selected;           // +0x00c
    char     m_clearData[0x1800];  // +0x010
};
static_assert(sizeof(SIG_LOAD_DUNGEON_CLEAR) == 0x1810, "SIG_LOAD_DUNGEON_CLEAR ABI drift");
static_assert(offsetof(SIG_LOAD_DUNGEON_CLEAR, m_selected) == 0x00c, "SIG_LOAD_DUNGEON_CLEAR m_selected offset drift");

// ORIG sizeof = 0x10（CACHE_CHARACTER_TYPE@+0x1eb03 → +0x1eb13）。
// RequestBlackCount @0x81c79f2：memset 0x10 后 +0x0 写 CUser::get_charac_no(slot)。
// CUser::GetBlackListCount @0x867f1d4：+0x4 清零，+0x8 = getCurrCharacBlackCount()。
struct SIG_REQ_BLACK_COUNT
{
    unsigned int m_characNo;   // +0x000（RequestBlackCount 写入）
    int      m_count;          // +0x004
    unsigned int m_blackCount; // +0x008（P211/P274 异型化：ORIG store 出
                               //  `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x8(%eax)`
                               //  RHS-first 中转形；同宽同偏移，消费点仅赋值/直读）
    char     m_pad_c[0x10 - 0xc];
};
static_assert(sizeof(SIG_REQ_BLACK_COUNT) == 0x10, "SIG_REQ_BLACK_COUNT ABI drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_characNo) == 0x000, "SIG_REQ_BLACK_COUNT m_characNo offset drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_count) == 0x004, "SIG_REQ_BLACK_COUNT m_count offset drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_blackCount) == 0x008, "SIG_REQ_BLACK_COUNT m_blackCount offset drift");

// ORIG sizeof = 0x11（GetEtcInfo 传 &sig+0x14a99，下一字段 combo +0x14aaa）。
// CUser::GetCharacCashAttendanceInfo @0x86910a6 仅清零首字节；
// Inter_LoadEtc::processAttendAnceEvent @0x84e807e 把首字节读作启用标志(bool)。
// 其余 0x10 字节仅作为 blob 在 DB/Inter_LoadEtc 间传递，无字段级访问 → 保留 pad。
struct SIG_ATTENDANCE_EVENT
{
    // [b28d-solo-Inter_LoadEtc_impl] P248 档②：ORIG processAttendAnceEvent 0x084e8078
    // `movzbl; xor $1; test %al,%al; jne` ⇒ bool 字段（uchar 恒多 cmp/setne 形态）。
    bool m_isEnabled;            // +0x00（启用标志；GetCharacCashAttendanceInfo 清零）
    unsigned char m_pad[0x10];   // +0x01（opaque blob，无 ORIG 字段级证据）

    // ORIG W 0x0848fb84：仅 movb $0,(%eax)（只清首字节）。
    SIG_ATTENDANCE_EVENT() { m_isEnabled = 0; }
};
// ============================================================================
// stNPCRelationship_t（ORIG C1 0x0848fa50 W）：4×ushort + int 清零，sizeof 0xc。
// 权威定义（自 OnlyOrigBatch7_Force.cpp 局部定义提升，2026-08-30）。
// ============================================================================
struct stNPCRelationship_t
{
    stNPCRelationship_t()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
        m_d = 0;
        m_e = 0;
    }
    unsigned short m_a;   // +0x00
    unsigned short m_b;   // +0x02
    unsigned short m_c;   // +0x04
    unsigned short m_d;   // +0x06
    int m_e;              // +0x08
};

// ============================================================================
// SIG_LOAD_PREMIUM_LETHE（ORIG ctor W 0x0848ff7c：203×2B _Mastered_skill 数组
// 循环构造，0x196 + 3 pad = 0x199）。权威定义（自 CUserOnlyOrig.cpp 提升）。
// ============================================================================
struct SIG_LOAD_PREMIUM_LETHE
{
    SIG_LOAD_PREMIUM_LETHE() {}   // 空体让数组成员自动逐元素构造（同 ORIG）
    // ORIG ctor 0x0848ff7c `mov $0xcb,%esi`：gcc 数组构造循环计数=N-1 ⇒ N=0xcc。
    _Mastered_skill m_skills[0xcc];   // +0x00（204 × 2B = 0x198）
    char m_pad196[1];                 // +0x198（sizeof = 0x199）
};

// ORIG sizeof = 0x14b60（CACHE_CHARACTER_TYPE@+0x1fd20 → +0x34880 cacheTime）。

// ORIG sizeof = 0x14b60（CACHE_CHARACTER_TYPE@+0x1fd20 → +0x34880 cacheTime）。
// CUser::GetEtcInfo @0x867794c 依此偏移填充；子对象区（skill/expand/aura/option）
// 为可重叠序列化区，经命名数组 + 文档化偏移访问（同 GetContinuousItemInfo 约定）。
// 字节打包：ORIG 在 +0xc9b3（奇数偏移）写 dword，故必须 pack(1)。
// ============================================================================
// [R18arr 2026-09-13] 为 SIG_LOAD_ETC 真实成员化而**上移**的三段类型定义（纯移动，
// 逐字节不变；原分片 SigTypes_D.h / SigTypes_E.h 内的同定义同步删除）。
// 理由：ORIG SIG_LOAD_ETC::C1（0x08491f86）在 +0x2308/+0x24a0/+0x40ce/+0xc9af 处
// 构造这三类子对象，而分片顺序为 A→C→D→E→B ⇒ 定义必须位于 SIG_LOAD_ETC 之前。
// ============================================================================
// ---- 冷却道具载入结构（SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO +0x4 区）----
// ORIG C1 0x0848fa0e：movb 0→+0；for i<=0x31 循环清零 {int,uint}@+4+i*8 两段
// （setle 有符号界，P2/P51）。CUser::LoadCoolTimeItemInfo（ORIG 0x08658ff6）
// 读 count@+0 与 entries{int itemIdx; uint endSec}。sizeof = 4+0x32*8 = 0x194。
struct CONT_ENTRY
{
    int m_itemIdx;          // +0x00
    unsigned int m_expireTime;  // +0x04
};
struct CONTINUOUS_ITEM_INFO
{
    // [R18arr 2026-09-13] 匿名 union 双视角（P235/P260）：整体字节视图 m_raw 供
    // SIG_LOAD_ETC +0x230c 的 get_binary 实参（数组名退化 ⇒ 4.4 单条 `lea 0x230c(%eax),%edx`；
    // 取址/成员取址形恒折 `add+mov` 两步 ⇒ DIFF）。布局/尺寸零变化（0x194）。
    union
    {
        struct
        {
            unsigned char m_count;          // +0x00
            char m_pad1[3];                 // +0x01（对齐）
            CONT_ENTRY m_entries[0x32];     // +0x04（0x32×8 = 0x190）
        };
        char m_raw[0x194];                  // +0x00（同址整体视图，数组退化直传）
    };

    CONTINUOUS_ITEM_INFO()          // ORIG W 0x0848fa0e（C1/C2 同址）
    {
        m_count = 0;
        for (int i = 0; i <= 0x31; ++i)
        {
            m_entries[i].m_itemIdx = 0;
            m_entries[i].m_expireTime = 0;
        }
    }
};

// ORIG ctor 0x0848f894：+0x4 int=0 → Redeem_Item::Inven_Item_Expand_RedeemInfo
// C1 × 0x15（esi=0x14 倒计数，步长 0x44：0x15×0x44 = 0x594 → +0x4 = 0x598 → 尾
// 0x59c）。前 4 字节 ctor 不触碰。
#pragma pack(push, 1)
struct SIG_REDEEMINFO_LIST_TO_DB
{
    char m_pad0[4];                                // +0x000
    int  m_field4;                                 // +0x004（ctor 清零）
    Redeem_Item::Inven_Item_Expand_RedeemInfo m_items[0x15];  // +0x008（0x594）

    SIG_REDEEMINFO_LIST_TO_DB() : m_field4(0) {}   // ORIG W 0x0848f894（C1/C2 同址）：m_items[0x15] 自动数组先于 m_field4=0，故置 init-list
};
#pragma pack(pop)
static_assert(sizeof(SIG_REDEEMINFO_LIST_TO_DB) == 0x59c,
              "SIG_REDEEMINFO_LIST_TO_DB ABI drift");

// ORIG ctor 0x08491eca：仅构造 +0x4 的 CONTINUOUS_ITEM_INFO（0x194；count@+0 +
// entries{int,uint}[0x32]）。
#pragma pack(push, 1)
struct SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO
{
    union
    {
        char m_pad0[4];             // +0x000
        // UpdateCoolTimeItemInfo 0x08658bfb / UpdateEffectItemInfo：
        // `mov %eax,(sig)` 4B 写 getCurCharacNo（Ghidra `*(undefined4*)local_18`）。
        unsigned int m_characNo;    // +0x000
    };
    CONTINUOUS_ITEM_INFO m_info;    // +0x004（0x194）

    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO()  // ORIG W 0x08491eca（C1/C2 同址）
        : m_info()
    {
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO) == 0x198,
              "SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO ABI drift");

// +0/+4/+8；ORIG ctor _ZN26SIG_SAVE_PC_ROOM_PLAY_TIMEC1Ev @0x0848fb64 清零三段；
// ORIG GetInBuffer 传 sizeof=9 → pack(1)。
#pragma pack(push, 1)
struct SIG_SAVE_PC_ROOM_PLAY_TIME
{
    unsigned int m_accId;        // +0x00 ←get_acc_id()
    // [R18arr 2026-09-13] +0x04 双视角：ORIG SIG_LOAD_ETC 该槽为 int
    // （DB_LoadEtc/LoadPcRoomPlayTimeInfo 历史判词实证「m_totalPcRoomPlayTime(int)」，
    //  get_uint 出参走 u、store 走与字段同型的 i ⇒ 单段 store），
    // 而 SAVE 侧（makeRequest）按 getPcRoomPlayTimeRewardCnt 语义为 unsigned。
    // union 双视角同尺寸 ⇒ sizeof/偏移/mangled 零变化（P235/P260 范式）。
    union
    {
        unsigned int m_totalTime;        // +0x04
        int          m_totalTimeSigned;  // +0x04（SIG_LOAD_ETC +0xc9b3 视图：int 同型直存）
    };
    unsigned char m_rewardCnt;   // +0x08 ←pcRoomPlayTimeRewardCnt

    // ORIG W 0x0848fb64：movl 0→+0 / movl 0→+4 / movb 0→+8。
    SIG_SAVE_PC_ROOM_PLAY_TIME()
    {
        m_accId = 0;
        m_totalTime = 0;
        m_rewardCnt = 0;
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
// SIG_LOAD_ETC +0x263c 受限道具条目（16B；GetLimitItemUsage @0x08688dc4 字段实证）。
struct SIG_ETC_LIMIT_ITEM
{
    unsigned char m_used;                // 元素+0x00（←LimitItemUsage::m_field10）
    char          m_pad1[3];
    int           m_itemIdx;             // 元素+0x04（←m_fieldc）
    int           m_field8;              // 元素+0x08（←m_field15）
    int           m_fieldc;              // 元素+0x0c（←m_field19）
};
// SIG_LOAD_ETC +0x2d3c event_server_message 条目（0x58；DB_LoadEtc/
// LoadEventServerMessageInfo.md 写点实证：+0 movb kind、+1 movb index、
// +4 movl 串长、+8 strncpy（上限 $0x32=50，字段区 0x34=52）、+0x3c movl、
// +0x40 strncpy name[20]、+0x54 movl update_time）。
struct SIG_ETC_EVENT_MESSAGE
{
    unsigned char  m_kind;        // 元素+0x00
    unsigned char  m_index;       // 元素+0x01
    char           m_pad2[2];     // 元素+0x02
    unsigned int   m_messageLen;  // 元素+0x04
    char           m_message[52]; // 元素+0x08
    unsigned int   m_nameLen;     // 元素+0x3c
    char           m_name[20];    // 元素+0x40
    int            m_updateTime;  // 元素+0x54
};
static_assert(sizeof(SIG_ETC_EVENT_MESSAGE) == 0x58, "SIG_ETC_EVENT_MESSAGE ABI drift");
struct SIG_LOAD_ETC
{
    unsigned int m_accId;                              // +0x000（RequestDBETC ← get_acc_id）
    unsigned int m_characNo;                           // +0x004（RequestDBETC ← get_charac_no）
    int      m_field8;                                 // +0x008（GetEtcInfo 先清零）
    unsigned int m_fieldC;                             // +0x00c（RequestDBETC ← MSG_CHARAC_SELECT.m_0e，
                                                       //       同 SIG_LOAD_CHARAC.m_tagCharacNo 数据源）
    TAG_CHARAC_ETC_DATA m_tagEtc;                    // +0x010（ORIG C1 构造：2 行 × _Mastered_skill[0xcb]）
    PvpResultType m_pvpResult;                       // +0x340（0x84）
    int      m_recordCount;                          // +0x3c4
    GameResultType m_records[0x64];                  // +0x3c8（0x64×0x50 = 0x1f40）
    // +0x2308 / +0x24a0 两段 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（ORIG C1 @0x08491f86
    // 在 +0x2308 与 +0x24a0 各发 `mov this,%eax; add $0x2308|0x24a0,%eax;
    // mov %eax,(%esp); call SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO::C1Ev`）。
    // [R18arr 2026-09-13] blob/union 视图 → 真实成员（sizeof 0x198 与区长度逐字节
    // 相等；成员构造由编译器自动发射，消除手写 placement new 的 `_ZnwjPv`+`_ZdlPvS_`）。
    // 视图对应（旧名 → 新访问）：
    //   m_cooltimeCharacNo/m_effectCharacNo → .m_characNo      @+0x2308 / +0x24a0
    //     （CUserCharacInfo::GetContinuousItemInfo 0x08677f24/08677f43 写点）
    //   m_cooltimeItemCount/m_effectItemCount → .m_info.m_count  @+0x230c / +0x24a4
    //     （08677f67/08678008 `mov %dl` 截断字节）
    //   m_cooltimeItem/m_effectItem（0x194 blob）→ .m_info            @+0x230c / +0x24a4
    //     （DB_LoadEtc/LoadCharacItemStat.md：get_binary 目标 cap 0x194；
    //      0x24a4+0x194 = +0x2638 精确衔接 count）
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO m_contItemCooltime;  // +0x2308（0x198）
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO m_contItemEffect;    // +0x24a0（0x198）
    // +0x2638 受限道具区：GetLimitItemUsage @0x08688dc4 实证 count@+0x2638（movl $0/
    // 写 index）、条目 0x263c 起 stride 0x10（dst=(idx+0x263)*0x10；字段 dst+0xc
    // movb ←LimitItemUsage::m_field10、dst+0x10 movl ←m_fieldc、dst+0x14 ←
    // m_field15、dst+0x18 ←m_field19）。
    int  m_limitItemCount;                   // +0x2638
    SIG_ETC_LIMIT_ITEM m_limitItems[(0x29fc - 0x263c) / 0x10];  // +0x263c
    unsigned char m_oneDayLetheFlag;                 // +0x29fc
    // [K2 2026-09-11 Main 批准] char→unsigned char：ORIG 0x084e8c92 段读该 blob 尾哨
    // 为 `movzbl 0x2b95(%eax),%edx`（单字节零扩展直读），`(unsigned char)char_arr[i]`
    // 在 4.4 恒多 1 条 `mov %eax,%edx`（探针 /tmp/k2p/ucmp2.cpp p7 vs /tmp/k2p/ucmp.cpp p1）
    // ⇒ 原码字段为无符号字节。同宽 1B ⇒ sizeof(SIG_LOAD_ETC)/全部偏移/mangled 不变。
    // [R18arr 2026-09-13] blob → 真实成员：ORIG C1 @0x08491f86 在 +0x29fd/+0x2b97
    // 各发 `mov this,%eax; add $0x29fd|0x2b97,%eax; mov %eax,(%esp);
    // call SIG_LOAD_PREMIUM_LETHE::C1Ev`（无 operator new）⇒ 原码是真实具名成员，
    // 由编译器自动构造。我方旧 char blob + ctor 内手写 placement new 在 4.4 下
    // 每处多发 `_ZnwjPv` call + 判空 + `_ZdlPvS_` EH 岛（N83 形态差）。
    // sizeof(SIG_LOAD_PREMIUM_LETHE)=0x199 与区长度逐字节相等，偏移零变化。
    SIG_LOAD_PREMIUM_LETHE m_backupSkill;            // +0x29fd（0x199）
    unsigned char m_oneDayLetheFlag2ND;              // +0x2b96
    SIG_LOAD_PREMIUM_LETHE m_backupSkill2ND;         // +0x2b97（0x199；0x2d2f 尾哨）
    char     m_pad2d30[0x2d30 - (0x2b97 + 0x199)];     // +0x2d30 前垫
    unsigned int m_serverGroup;                        // +0x2d30（RequestDBETC ← G_CEnvironment()->m_serverEnvir.m_serverGroup）
    int      m_channelNo;                              // +0x2d34（RequestDBETC ← CEnvironment::get_channel_no）
    // +0x2d38 区：LoadEventServerMessageInfo.md（计数 movb 截断 @0x2d38，条目
    // 基址 0x2d3c 步长 imul $0x58；ORIG 无行数上限，written 超 50 项落 npc 区 =
    // 如实转写，O0 地址连续与 ORIG 同行为）；_loadNPCRelasionship.md（npc_cnt get_uint @0x3e6c、npc_data
    // get_binary cap 0x258=600 @0x3e70 → +0x40c8）。
    unsigned char m_eventMessageCount;                 // +0x2d38
    char     m_pad2D39[3];                             // +0x2d39
    SIG_ETC_EVENT_MESSAGE m_eventMessages[50];         // +0x2d3c（50×0x58 = 0x1130
                                                       //   → +0x3e6c 精确衔接 npc）
    unsigned int m_npcCount;                           // +0x3e6c
    // [R18arr 2026-09-13] blob → 真实成员：ORIG C1 @0x08491f86 在 +0x3e70 发
    // 倒计数构造循环（`mov $0x31,%esi` / `call stNPCRelationship_t::C1Ev` /
    // `add $0xc,%ebx` / `sub $0x1,%esi` / `cmp $0xffffffff; setne`）⇒ 原码是
    // 真实数组成员。sizeof=0xc × 50 = 0x258 与区长度相等，偏移零变化。
    stNPCRelationship_t m_npcData[50];                 // +0x3e70（0x258）
    unsigned short m_curConditionEventStep;          // +0x40c8
    unsigned short m_curConditionEventRewardStep;    // +0x40ca
    unsigned short m_properDungeonClearCount;        // +0x40cc
    // [R18arr 2026-09-13] blob → 真实成员：ORIG C1 @0x08491f86 在 +0x40ce 发
    // `add $0x40ce,%eax; mov %eax,(%esp); call SIG_REDEEMINFO_LIST_TO_DB::C1Ev`。
    // sizeof(SIG_REDEEMINFO_LIST_TO_DB)=0x59c 与区长度逐字节相等 ⇒ 偏移零变化；
    // 旧名 m_expandData6（char[0x59c]）的消费点改为 (char*)&m_redeemInfo。
    SIG_REDEEMINFO_LIST_TO_DB m_redeemInfo;          // +0x40ce（type 6，0x59c）
    // type 0xe 区 = charac_titlebook 4 解压列（DB_LoadEtc/LoadTitleBook.md：
    // uncompress_zip 目标 0x466a/0x5718/0x86c0/0xa358，outLen 0x10ae/0x2fa8/
    // 0x1c98/0xe4c 精确衔接）。
    // [R18arr 2026-09-13] blob → 真实成员：ORIG C1 @0x08491f86 在 +0x466a 发
    // `mov this,%eax; add $0x466a,%eax; mov %eax,(%esp); call stTitleBook::C1Ev`
    // （无 operator new）⇒ 原码是真实具名成员。sizeof(stTitleBook)=0x6b3a 与区
    // 长度逐字节相等；m_category0/1/2/3 依次落 +0x466a/0x5718/0x86c0/0xa358，
    // 与原 union 内四段解压列偏移逐项一致 ⇒ 全部偏移零变化。
    stTitleBook m_titlebook;                         // +0x466a（type 0xe，0x6b3a）
    // type 0xf 区 = charac_achievement（DB_LoadEtc/LoadAchievement.md：解压目标
    // 0xb1a4 cap 0x1800；条目 stride 12 首 dword 判零 ≤512 条；计数 movl
    // @0xc9a4；last_update get_uint @0xc9a8 → +0xc9ac 接具名字段）。
    // [R18arr 2026-09-13] blob → 真实成员：ORIG C1 @0x08491f86 在 +0xb1a4 发
    // `add $0xb1a4,%eax; call stAchievement_DBData::C1Ev`（无 operator new）。
    // sizeof(stAchievement_DBData)=0x1808；m_entries[512]@+0（=0xb1a4，0x1800）、
    // m_count@+0x1800（=0xc9a4）、m_saveTime@+0x1804（=0xc9a8）与原三字段偏移
    // 逐项一致 ⇒ 全部偏移零变化。
    stAchievement_DBData m_achievement;              // +0xb1a4（type 0xf，0x1808）
    unsigned char m_curVillageAttackCount;           // +0xc9ac
    unsigned char m_curRevengeDungeonCount;          // +0xc9ad
    unsigned char m_flagPresentAvengerTitle;         // +0xc9ae
    // [R18arr 2026-09-13] blob+标量 → 真实成员：ORIG C1 @0x08491f86 在 +0xc9af 发
    // `add $0xc9af,%eax; mov %eax,(%esp); call SIG_SAVE_PC_ROOM_PLAY_TIME::C1Ev`
    // （pack(1) sizeof=9 ⇒ 覆盖 +0xc9af..+0xc9b7）。
    // 视图对应：旧 m_totalPcRoomPlayTime → .m_totalTime @+0xc9b3、
    //          旧 m_pcRoomPlayTimeRewardCnt → .m_rewardCnt @+0xc9b7。
    SIG_SAVE_PC_ROOM_PLAY_TIME m_pcRoomPlayTime;     // +0xc9af（9 字节）
    int      m_curCharacUsedGiftFatigueQuantity;     // +0xc9b8
    std::bitset<32> m_bitset;                        // +0xc9bc（ORIG bitset<32>，4 字节；set/reset 成员调用）
    // +0xc9c0 混合区（skill + expand8/7/b/10/d/10 + aura + option）：
    // 前 0x414 为 SIG_LOAD_SKILL 视图（CUser::GetEtcInfo/Inter_LoadEtc 消费，
    // 保留经 m_mixedRegion 基名访问）；其后各段按 DB_LoadEtc 报告写点字段化
    // （证据：GetUserSkill.md cdd1/cdd4 movb；LoadPvPMissionList.md cdd5 cap
    // 0x198 与 fair_pvp 0xcf23 重叠序列化；LoadFairPvPScore.md cf23..cf4a
    // 10×get_uint、cf4b blob 0x650、d59b get_uint/d59f get_int/d5a3 get_ubyte、
    // movl $0 清 d59b；LoadQuestShop/_LoadQuestShop.md d5a7、d651 双实例 0xaa、
    // d6fb movb；LoadItemMakingSkill.md d700 起 11×get_short；LoadTODUserState.md
    // d71c/d720/d728/d72c 4B、d724/d726 2B；LoadAuraAvatarOption.md d730
    // reset=memset 0xc + 3×int；LoadCharacItemStat.md d740 blob 5000、eac8
    // get_binary 0x200；LoadBoosterGage.md eccc get_int；LoadCharacOption.md
    // ecd4 movl、ecd8 blob 24000、ecdc+4i 5×dword 钳位）。union 保 sizeof/偏移。
    union
    {
        char m_mixedRegion[0x14a98 - 0xc9c0];        // +0xc9c0
        struct
        {
            char           m_padC9C0[0x411];         // +0xc9c0（SIG_LOAD_SKILL 视图区）
            unsigned char  m_scriptVersion;          // +0xcdd1（get_ubyte）
            char           m_padCDD2[2];             // +0xcdd2
            unsigned char  m_pvpSkillInsertFlag;     // +0xcdd4（insert 占位行置 1）
            char           m_pvpMissionInfo[0x14e];  // +0xcdd5（解压 cap 0x198，尾部
                                                     //   与 m_fairPvpScore 重叠 =
                                                     //   ORIG 序列化复用，后写覆盖）
            unsigned int   m_fairPvpScore[10];       // +0xcf23（private_win..daily_play_count）
            char           m_fairPvpGiveItem[0x650]; // +0xcf4b（give_item blob）
            unsigned int   m_fairPvpLastPlayTime;    // +0xd59b（unix(last_play_time)）
            int            m_fairPvpLastPlaySec;     // +0xd59f（get_int 重读）
            unsigned char  m_fairPvpGiveFlag;        // +0xd5a3（get_ubyte）
            char           m_padD5A4[3];             // +0xd5a4
            char           m_questShopMain[0xaa];    // +0xd5a7（CHARAC_QUEST_SHOP_DATA 视图）
            char           m_questShopTag[0xaa];     // +0xd651（tag 双实例）
            unsigned char  m_questShopTagLoaded;     // +0xd6fb（tag 装载成功置 1）
            char           m_padD6FC[4];             // +0xd6fc
            unsigned short m_itemMakingSkill[11];    // +0xd700（weapon..magic_stone）
            char           m_padD716[6];             // +0xd716
            int            m_todFirstLayerStartDate; // +0xd71c
            int            m_todTodayEnterCount;     // +0xd720
            unsigned short m_todLastClearLayer;      // +0xd724
            short          m_todEnterCountByWeek;    // +0xd726（get_short，
                                                     //   LoadTODUserState.md 布局
                                                     //   int,int,ushort,short,int,int）
            int            m_todDate;                // +0xd728
            int            m_todLastClearDate;       // +0xd72c
            // [b28d-Main·检查点14 P235/P322] +0xd730 12B 同址 union 视角：
            //   ORIG LoadAuraAvatarOption 0x084084af `mov -0x1c(%ebp),%ecx; add $0x35cc,%ecx;
            //   mov -0x20(%ebp),%edx; mov %edx,(%eax,%ecx,4)`（4B store + 无符号索引序）
            //   探针 /tmp/dble_probe/p8.cpp k1（union 视角 unsigned 元素）逐条全等。
            //   sizeof(SIG_LOAD_ETC)/全部偏移/mangled 零变化；原字段名保留。
            union
            {
                char         m_auraAvatarOption[0xc];  // +0xd730（stAuraAvatarOption 视图）
                unsigned int m_auraOptionWords[3];     // 同址 3×4B 视角
            };
            char           m_padD73C[4];             // +0xd73c
            char           m_itemDictionary[5000];   // +0xd740（check_flag blob cap 5000）
            char           m_characOptions[0x200];   // +0xeac8（options get_binary）
            char           m_padECC8[4];             // +0xecc8
            int            m_boosterGage;            // +0xeccc（member_booster_gage）
            char           m_padECD0[4];             // +0xecd0
            int            m_bestClearTimeCount;     // +0xecd4（条目计数 movl）
            char           m_bestClearTime[24000];   // +0xecd8（blob；条目 stride 0x18）
        };
    };
    unsigned char m_blueMarbleEnterCount;            // +0x14a98
    SIG_ATTENDANCE_EVENT m_attendanceEvent;          // +0x14a99
    char     m_comboSkill[0x14b60 - 0x14aaa];        // +0x14aaa

    SIG_LOAD_ETC();   // ORIG W 0x08491f86（定义 SigTypes_C.cpp：子对象逐区构造）
    // ORIG D1/D2 W 0x084921f0：GameResultType[0x64] 析构循环 + PvpResultType 析构。
    ~SIG_LOAD_ETC() {}
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_ETC) == 0x14b60, "SIG_LOAD_ETC ABI drift");
// [K2 2026-09-11] char→unsigned char 同宽替换的偏移锁（Main 批准条件）。
static_assert(offsetof(SIG_LOAD_ETC, m_backupSkill) == 0x29fd,
              "SIG_LOAD_ETC m_backupSkill offset drift");
static_assert(offsetof(SIG_LOAD_ETC, m_backupSkill2ND) == 0x2b97,
              "SIG_LOAD_ETC m_backupSkill2ND offset drift");
// [R18arr 2026-09-13] 真实成员化的偏移锁（ORIG 构造链偏移 ↔ 成员 offsetof 对照）：
static_assert(offsetof(SIG_LOAD_ETC, m_npcData) == 0x3e70,
              "SIG_LOAD_ETC m_npcData offset drift");
static_assert(offsetof(SIG_LOAD_ETC, m_titlebook) == 0x466a,
              "SIG_LOAD_ETC m_titlebook offset drift");
static_assert(offsetof(SIG_LOAD_ETC, m_achievement) == 0xb1a4,
              "SIG_LOAD_ETC m_achievement offset drift");
static_assert(offsetof(SIG_LOAD_ETC, m_curVillageAttackCount) == 0xc9ac,
              "SIG_LOAD_ETC m_curVillageAttackCount offset drift");

struct SIG_LOAD_EVENTITEM
{
    char m_pad[0x5c];
};

// ---- 临时生物期限（ORIG C1 W 0x0848f976：两 int 均置 -1）----
// 使用点：CInventory::LoadCreatureTempPeriod（CInventory.h 前向声明）、
// SIG_LOAD_INVENTORY m_tempPeriods[0x14]（ORIG ctor 步长 8）。
struct SIG_POSTAL_CREATURE_TEMP_PERIOD
{
    int m_uid;           // +0x00
    int m_tempPeriod;    // +0x04

    SIG_POSTAL_CREATURE_TEMP_PERIOD()   // ORIG W 0x0848f976
    {
        m_uid = -1;
        m_tempPeriod = -1;
    }
};

struct SIG_LOAD_GAME_OPTION
{
    char m_pad[0x4b0];
};

// ORIG SIG_SAVE_PC_ROOM_RENT_ITEM（CUser::GetPcRoomRentInfo @0x86769ea 填充）：
// 容器 2 个 0xc 条目；条目类型权威名 SIG_SAVE_PC_ROOM_RENT_INFO
//（W ctor 0x848f98e / clear 0x0848f9a2：+0 int=0 / +4,+5,+6 byte=0 / +8 int=-1）。
// ITEM ctor 0x8491cbe 仅构造 entry[0]；ITEM clear 0x8694c9a 仅清 entry[0]。
#pragma pack(push, 1)
struct SIG_SAVE_PC_ROOM_RENT_INFO
{
    int  m_itemIndex;   // +0x00
    char m_field4;      // +0x04
    char m_field5;      // +0x05
    char m_inEquip;     // +0x06
    char m_pad7;        // +0x07
    int  m_field8;      // +0x08

    SIG_SAVE_PC_ROOM_RENT_INFO() { clear(); }   // ORIG W 0x848f98e（内联，转调 clear）
    /* ORIG W 0x0848f9a2（weak ⇒ 头内 inline 定义）。类内 inline 使调用方
     * （ITEM::clear 等）按"外部 callee"帧策略分配栈帧，与 ORIG 一致
     * （/tmp/p26_slot4.cpp probeH 实证；类外定义时 4.4 塌缩帧）。 */
    void clear()
    {
        m_itemIndex = 0;
        m_field4 = 0;
        m_field5 = 0;
        m_inEquip = 0;
        m_field8 = -1;
    }
};
struct SIG_SAVE_PC_ROOM_RENT_ITEM
{
    SIG_SAVE_PC_ROOM_RENT_INFO m_entry[1];
    SIG_SAVE_PC_ROOM_RENT_ITEM();                 // ORIG W 0x8491cbe（定义 SigTypes.cpp，生成导出符号）
    void clear();                                 // ORIG W 0x08694c9a
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_PC_ROOM_RENT_ITEM) == 0xc, "SIG_SAVE_PC_ROOM_RENT_ITEM ABI drift");
static_assert(sizeof(SIG_SAVE_PC_ROOM_RENT_INFO) == 0xc, "SIG_SAVE_PC_ROOM_RENT_INFO ABI drift");

// ORIG SIG_SAVE_DIMENSION_RENT_ITEM（CUser::GetDimensionRentInfo @0x8676bec 填充）：
// 容器 12 个 8 字节条目；条目类型权威名 SIG_SAVE_DIMENSION_RENT_INFO
//（W ctor 0x848f9d0 / clear 0x0848f9e4：+0 int=0 / +4..+7 byte=0）。
// ITEM ctor 0x8491cf2 仅构造 entry[0..10]；ITEM clear 0x8694cd2 仅清 entry[0..10]。
#pragma pack(push, 1)
struct SIG_SAVE_DIMENSION_RENT_INFO
{
    int  m_itemIndex;   // +0x00
    char m_field4;      // +0x04
    char m_field5;      // +0x05
    char m_inEquip;     // +0x06
    char m_field7;      // +0x07

    SIG_SAVE_DIMENSION_RENT_INFO() { clear(); }   // ORIG W 0x848f9d0（内联，转调 clear）
    /* ORIG W 0x0848f9e4（weak ⇒ 头内 inline 定义）。类内 inline 使调用方
     * （ITEM::clear 等）按"外部 callee"帧策略分配栈帧，与 ORIG 一致
     * （NL26 probeH 实证；类外定义时 4.4 塌缩帧）。 */
    void clear()
    {
        m_itemIndex = 0;
        m_field4 = 0;
        m_field5 = 0;
        m_inEquip = 0;
        m_field7 = 0;
    }
};
struct SIG_SAVE_DIMENSION_RENT_ITEM
{
    SIG_SAVE_DIMENSION_RENT_INFO m_entry[11];
    SIG_SAVE_DIMENSION_RENT_ITEM();               // ORIG W 0x8491cf2（定义 SigTypes.cpp，生成导出符号）
    void clear();                                 // ORIG W 0x08694cd2
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_ITEM) == 0x58, "SIG_SAVE_DIMENSION_RENT_ITEM ABI drift");
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_INFO) == 8, "SIG_SAVE_DIMENSION_RENT_INFO ABI drift");

// ORIG sizeof = 0x1544c（CACHE_CHARACTER_TYPE@+0x703 → +0x15b4f）。
// 填充函数与关键偏移（GetInventoryInfo @0x864d4ba / GetInventory @0x864d646 /
// GetCreatureItem @0x833b80c / GetRentInfo @0x86769b4）：
//   +0x0000 背包序列化区起 / +0x000c TAG_CHARAC_INVEN_DATA（getTagCharacInventory）/
//   +0x4030 result(0 初始,失败 1) / +0x4034 money / +0x4038 coin / +0x403c payCoin /
//   +0x4040 eventCoin / +0x4044 invenType2(0x1905) / +0x5949 invenType1(0x4a58) /
//   +0xa3a1 装备镜像(0x53e) / +0xa8e0 cargo 容量 / +0xa8e4 cargo / +0xcd1c avatar 计数 /
//   +0xcd20 avatar / +0xeed1 invenType3(0x39aa) / +0x12787 宠物尾(0xf4) /
//   +0x1287c creature 计数 / +0x12880 creature(0x1ed8) / +0x14758 clearAvatar /
//   +0x14780 avatarCoin / +0x14784 expand type2 / +0x14aa4 expand type9 /
//   +0x1533c inventory_capacity / +0x15340 creature2 计数 / +0x153e4 pcroom rent /
//   +0x153f0 dimension rent / +0x15448 maxEquipLevel。字节打包（+0x5949 等奇偏移）。
#pragma pack(push, 1)
struct SIG_LOAD_INVENTORY
{
    unsigned int m_accId;                          // +0x000（RequestDBInventory_forchina ← get_acc_id）
    unsigned int m_characNo;                       // +0x004（同上 ← get_charac_no）
    unsigned int m_field8;                         // +0x008（← MSG_CHARAC_SELECT.m_0e）
    TAG_CHARAC_INVEN_DATA m_tagInven;              // +0x00c（getTagCharacInventory 填充；
                                                   //   RequestDBInventory_forchina 调 clear）
    int      m_result;                            // +0x4030
    unsigned int m_money;                         // +0x4034（P211 异型化：get_money 返 int，ORIG 0x864d646 store 带 mov %eax,%edx 中转 ⇒ 字段 unsigned）
    unsigned int m_coin;                          // +0x4038
    unsigned int m_payCoin;                       // +0x403c
    unsigned int m_eventCoin;                     // +0x4040
    // ORIG C1 0x08491d26：TAG_CHARAC_INVEN_DATA → Inven_Item[0x69]@+0x4044 →
    // Inven_Item[0x138]@+0x5949 → Inven_Item[0x16]@+0xa3a1 → Inven_Item[0x98]@+0xa8e4 →
    // Inven_Item[0xf2]@+0xeed1 → stItemLockInfo[0x64]@+0x14784 →
    // SIG_SAVE_EXPAND_EQUIPSLOT@+0x14aa4 → SIG_POSTAL_CREATURE_TEMP_PERIOD[0x14]@+0x15344。
    Inven_Item m_invenType2[0x69];                // +0x4044（0x69×0x3d = 0x1905 → +0x5949）
    Inven_Item m_invenType1[0x138];               // +0x5949（0x138×0x3d = 0x4a58 → +0xa3a1）
    Inven_Item m_equipMirror[0x16];               // +0xa3a1（0x16×0x3d = 0x53e → +0xa8df）
    char m_padA8df[0xa8e0 - 0xa8df];              // +0xa8df（1 字节）
    unsigned int m_cargoCapacity;                 // +0xa8e0（CCargo::GetCapacity）
    Inven_Item m_cargo[0x98];                     // +0xa8e4（0x98×0x3d = 0x2438 → +0xcd1c）
    int      m_avatarCount;                       // +0xcd1c
    AvatarDetailInfo m_avatarDetail[0x73];        // +0xcd20（0x73×0x4b = 0x21b1；同 TAG 视图，b28d-solo-CUser）
    Inven_Item m_invenType3[0xf2];                // +0xeed1（0xf2×0x3d = 0x39aa → +0x1287b）
    char     m_pad1287b[0x1287c - 0xeed1 - 0x39aa]; // +0x1287b（creatureCount 前 1 字节）
    int      m_creatureCount;                     // +0x1287c
    char     m_creature[0x1ed8];                  // +0x12880（CCreatureMgr::GetCreatureItem）
    unsigned long m_clearAvatar[10];              // +0x14758（0x28；GetClearAvatar(unsigned long*) 同型，b28d-solo-CUser）
    unsigned int m_avatarCoin;                    // +0x14780（AvatarCoin::Get）
    stItemLockInfo m_locks[0x64];                 // +0x14784（0x64×8 = 0x320 → +0x14aa4）
    SIG_SAVE_EXPAND_EQUIPSLOT m_expandEquip;      // +0x14aa4（0x898 → +0x1533c）
    unsigned int m_inventoryCapacity;             // +0x1533c（P211 异型化：get_inventory_capacity 返 int，ORIG GetInventory 0x864d646 尾 store 带 mov %eax,%edx 中转 ⇒ 字段 unsigned；唯一写点 CUser.cpp GetInventory）
    int      m_creatureCount2;                    // +0x15340（GetCreatureItem 第二区计数）
    SIG_POSTAL_CREATURE_TEMP_PERIOD m_tempPeriods[0x14];  // +0x15344（0x14×8 = 0xa0 → +0x153e4）
    SIG_SAVE_PC_ROOM_RENT_ITEM m_rentPcRoom;      // +0x153e4（ORIG C1 0x8491cbe 构造）
    SIG_SAVE_DIMENSION_RENT_ITEM m_rentDimension; // +0x153f0（0x58 → +0x15448；ORIG C1 0x8491cf2）
    int      m_maxEquipLevel;                     // +0x15448

    SIG_LOAD_INVENTORY() {}                       // ORIG W 0x08491d26（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_INVENTORY) == 0x1544c, "SIG_LOAD_INVENTORY ABI drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_result) == 0x4030, "SIG_LOAD_INVENTORY m_result offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_money) == 0x4034, "SIG_LOAD_INVENTORY m_money offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_coin) == 0x4038, "SIG_LOAD_INVENTORY m_coin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_payCoin) == 0x403c, "SIG_LOAD_INVENTORY m_payCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_eventCoin) == 0x4040, "SIG_LOAD_INVENTORY m_eventCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_cargoCapacity) == 0xa8e0, "SIG_LOAD_INVENTORY m_cargoCapacity offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_avatarCount) == 0xcd1c, "SIG_LOAD_INVENTORY m_avatarCount offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_creatureCount) == 0x1287c, "SIG_LOAD_INVENTORY m_creatureCount offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_avatarCoin) == 0x14780, "SIG_LOAD_INVENTORY m_avatarCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_inventoryCapacity) == 0x1533c, "SIG_LOAD_INVENTORY m_inventoryCapacity offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_maxEquipLevel) == 0x15448, "SIG_LOAD_INVENTORY m_maxEquipLevel offset drift");

// ORIG sizeof = 0xafcc（RequestDBKillMonsterInfo @0x81c7836 memset 0xafcc；
// charac_no 写于 +0xafc8 尾部）。
// ORIG C1 0x0848fc8e：stMonsterInfo_t[0x5dc]@+0 → [0x1770]@+0x1d4c → [0x5dc]@+0x927c
// 三组倒计数构造（0x5dc×5=0x1d4c；0x1770×5=0x7530；步长 5，P151）。
// stMonsterInfo_t（pack(1) {uint mobIdx; uchar diff;}）与 CKillMonsterInfo_chain.cpp
// 的 TU 局部定义同构（ODR 相同）；该 cpp 不 include 本头，无冲突。
#pragma pack(push, 1)
struct stMonsterInfo_t
{
    // ORIG C1/C2 0x0848fc78（W）：mobIdx=0、diff=0。
    stMonsterInfo_t()
    {
        mobIdx = 0;
        diff = 0;
    }
    unsigned int  mobIdx;   // +0x00
    unsigned char diff;     // +0x04
};

struct SIG_LOAD_KILL_MONSTER_INFO
{
    stMonsterInfo_t m_boss[0x5dc];      // +0x000（0x1d4c）
    stMonsterInfo_t m_named[0x1770];    // +0x1d4c（0x7530 → +0x927c）
    stMonsterInfo_t m_apc[0x5dc];       // +0x927c（0x1d4c → +0xafc8）
    int m_characNo;                     // +0xafc8（ORIG KMI 0x081c7836 store 为
                                        // `mov -0xc,%edx; mov %eax,0xafc8(%edx)`
                                        // 直存形——int 成员实证；unsigned 成员在
                                        // int 值赋值时必出 3 行 swap，c6root 探针
                                        // 实证；广播 2026-09-05，W8C 无异议）

    // ORIG W 0x0848fc8e：数组成员隐式构造后，ctor 体 memset 三块数组清零并
    // m_characNo=0（ORIG 848fd0c/848fd27/848fd47/848fd67）。
    SIG_LOAD_KILL_MONSTER_INFO()
    {
        memset(m_boss, 0, sizeof(m_boss));
        memset(m_named, 0, sizeof(m_named));
        memset(m_apc, 0, sizeof(m_apc));
        m_characNo = 0;
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_KILL_MONSTER_INFO) == 0xafcc, "SIG_LOAD_KILL_MONSTER_INFO ABI drift");
static_assert(offsetof(SIG_LOAD_KILL_MONSTER_INFO, m_characNo) == 0xafc8, "SIG_LOAD_KILL_MONSTER_INFO m_characNo offset drift");

struct SIG_LOAD_NO_CACHE_DATA
{
    union {
        unsigned int m_characNo;   // +0x00（ORIG dispatch 0x08446e.. `movl %edx,(%eax)`
                                   //   int RHS→unsigned 字段 = P211 异型 RHS-first 形，
                                   //   b28-L11；协议 charac_no 非负，unsigned 仅表线格式）
        int m_characNoInt;         // +0x00（同址 int 视角：RHS 为 int 局部时同型直存，
                                   //   [Lane31W8LTDBTaskView] 消除视图 cast 反模式）
        char m_pad[0x2c];
    };
};

// [Lane31W8LTDBTaskView] 整体字段化 + 匿名 union 双视角：ORIG
// DB_LoadPowerWarStatueInfo::GetCharacInfo 0x08434a8a / GetAvatarInfo 0x08434886
// 按偏移逐字段读（+0x00 charac_no、+0x04 name[0x1d]、+0x22 lev、+0x23 job、
// +0x24 grow_type、+0x25 guild_name[0x28]、+0x50 guild_id、+0x54 equipment[0xa]、
// +0x7c dword）；m_pad 字节视角保留。sizeof 0x80 不变。
#pragma pack(push, 1)
struct SIG_LOAD_POWER_WAR_STATUE_INFO
{
    union
    {
        char m_pad[0x80];
        struct
        {
            int m_characNo;            // +0x00
            char m_name[0x1d];         // +0x04
            char m_pad21;              // +0x21（ORIG 空洞）
            char m_lev;                // +0x22
            char m_job;                // +0x23
            char m_growType;           // +0x24
            char m_guildName[0x28];    // +0x25
            char m_pad4d[3];           // +0x4d（ORIG 空洞）
            unsigned int m_guildId;    // +0x50
            int m_equipment[0xa];      // +0x54
            int m_field7c;             // +0x7c
        };
    };
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_POWER_WAR_STATUE_INFO) == 0x80,
              "SIG_LOAD_POWER_WAR_STATUE_INFO ABI drift");
static_assert(0x050 + sizeof(((SIG_LOAD_POWER_WAR_STATUE_INFO*)0)->m_guildId) == 0x054,
              "SIG_LOAD_POWER_WAR_STATUE_INFO m_equipment at +0x54");

struct SIG_LOAD_PU_USER_INFO
{
    char m_pad[0x1];
};

// ---- PvP 好友列表（Community SendLogin 载荷）----
// ORIG：CCommunityServerProxy::SendLogin 0x0846ca06 实证——
//   +0x00 int m_buddyCount（SendLogin 拷 param_2[0] → 包 +0x3e buddyCount）
//   +0x04 STPvPBuddyDBInfo[0x20]（SendLogin memcpy 0x560 → 包 +0x42 buddies）
//   sizeof = 0x564。DB 侧（Inter_LoadPvPBuddy 域）填充。
struct SIG_LOAD_PVP_BUDDY
{
    int m_buddyCount;                    // +0x00
    STPvPBuddyDBInfo m_buddies[0x20];    // +0x04（0x2b × 0x20 = 0x560）
};
static_assert(sizeof(SIG_LOAD_PVP_BUDDY) == 0x564, "SIG_LOAD_PVP_BUDDY ABI drift");
static_assert(offsetof(SIG_LOAD_PVP_BUDDY, m_buddyCount) == 0x00, "SIG_LOAD_PVP_BUDDY m_buddyCount offset drift");
static_assert(offsetof(SIG_LOAD_PVP_BUDDY, m_buddies) == 0x04, "SIG_LOAD_PVP_BUDDY m_buddies offset drift");
// ORIG sizeof = 0x7608（RequestDBQuest @0x81c70c2 memset 0x7608；布局与
// UserQuest.cpp 本地视图/get_quest@0x869067c 族一致：m_cleared 0x7530 位图 +
// m_authen 0x28）。
// ORIG C1 0x08491e92：+0xb0 起 WongWork::STClearedQuest C1 × 0x7530（步长 1，
// 倒计数 P151；STClearedQuest 为 1 字节 {uchar}）。
#pragma pack(push, 1)
struct SIG_LOAD_QUEST
{
    unsigned int m_characNo;        // +0x000（RequestDBQuest 写入）
    int          m_field04;         // +0x004
    char         m_pad08[4];        // +0x008
    int          m_quest[20];       // +0x00c
    int          m_trigger[20];     // +0x05c
    int          m_urgentQuest;     // +0x0ac
    WongWork::STClearedQuest m_cleared[0x7530]; // +0x0b0（任务完成位图，1 字节/项）
    char         m_authen[0x28];    // +0x75e0

    SIG_LOAD_QUEST() {}             // ORIG W 0x08491e92（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_QUEST) == 0x7608, "SIG_LOAD_QUEST ABI drift");
static_assert(offsetof(SIG_LOAD_QUEST, m_cleared) == 0xb0, "SIG_LOAD_QUEST m_cleared offset drift");

// 具名化（证据：docs/class_func_reports/CUser/GetSkill.md ORIG 0x864dcf2 反编译段：
// +0x4 清零；SkillSlot::get_remain_sp(skill,+8,+0x1a4,+0x340,+0x344)（本体 0x8694ec0
// 依序拷 SkillSlot+0x4/+0xc/+0x14/+0x18 → SP1/SP2/SFP1/SFP2）；
// SkillSlot::get_skill_slot(skill,+0xc,+0x1a8,0x198) 两 0x198 槽区；
// get_skill_command_buf 后 memcpy(+0x348, buf, 200)；
// quest_reward.md 0x0864xxxx 写 +0x410=1；+0x411 ← CDataManager+0xa850）。
// 0x358..0x410 无字段级证据保留 pad。
#pragma pack(push, 1)
struct SIG_LOAD_SKILL
{
    union
    {
        unsigned char m_pad0[4];    // +0x000
        unsigned int  m_characNo;   // +0x000（UpdateData 0x0865359x movl 写 getCurCharacNo；
                                    //   W8LT_DBTask.cpp DB_SaveSkillCharacView 同义视图）
    };
    int m_field04;                  // +0x004（GetSkill 清零）
    int m_remainSpTree1;            // +0x008（SkillSlot+0x4）
    // ORIG ctor 0x08491ee0 循环计数 0xcb ⇒ N=0xcc（memcpy 0x198 实证同宽）。
    _Mastered_skill m_skillSlotTree1[0xcc];  // +0x00c（0xcc×2 = 0x198 → +0x1a4）
    int m_remainSpTree2;            // +0x1a4（SkillSlot+0xc）
    _Mastered_skill m_skillSlotTree2[0xcc];  // +0x1a8（0x198 → +0x340）
    int m_remainSfpTree1;           // +0x340（SkillSlot+0x14）
    int m_remainSfpTree2;           // +0x344（SkillSlot+0x18）
    // ORIG ctor 循环计数 0x31 ⇒ N=0x32（0x348+0xc8=+0x410 = m_flag410，无 pad）。
    STSkillCommandData::STCommand m_commands[0x32];  // +0x348（0x32×4 = 0xc8 → +0x410）
    bool m_flag410;                 // +0x410（quest_reward 写 1；SaveSkill 传 get_skill_table_name 直读）
    unsigned char m_field411;       // +0x411（← G_CDataManager()+0xa850）
    char m_pad412[2];               // +0x412（对齐尾，ORIG memset 覆盖范围 0x414）

    // ORIG W 0x08491ee0：_Mastered_skill[0xcc]×2 + STCommand[0x32] 数组成员隐式构造。
    SIG_LOAD_SKILL() {}
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_SKILL) == 0x414, "SIG_LOAD_SKILL ABI drift");

// ORIG sizeof = 0x19c（CACHE_CHARACTER_TYPE@+0x1d157 → +0x1d2f3）。
// CUser::GetTicket @0x867846e：+0x4 清零 / +0x8 count / +0xc int[100] 票表（list<int> 遍历，
// 上限 99）。字节打包（+0x0c 奇偏移起）。
#pragma pack(push, 1)
struct SIG_LOAD_TICKET
{
    union
    {
        char         m_pad0[0x4];   // +0x000
        unsigned int m_characNo;    // +0x000（RequestDBTicket 报告 ORIG 0x081c73a6：
                                    //   memset 后 `call get_charac_no; mov %eax,%edx;
                                    //   mov -0xc(%ebp),%eax; mov %edx,(%eax)`——
                                    //   缓冲首字即 charac_no；W8LT_DBTask.cpp
                                    //   DB_LoadTicketCharacView 为同偏移同义视图）
    };
    int      m_field4;        // +0x004（GetTicket 先清零）
    int      m_count;         // +0x008
    int      m_tickets[100];  // +0x00c
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_TICKET) == 0x19c, "SIG_LOAD_TICKET ABI drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_characNo) == 0x000, "SIG_LOAD_TICKET m_characNo offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_field4) == 0x004, "SIG_LOAD_TICKET m_field4 offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_count) == 0x008, "SIG_LOAD_TICKET m_count offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_tickets) == 0x00c, "SIG_LOAD_TICKET m_tickets offset drift");

// ---- 雇佣兵加载结构（vector 元素；ORIG 步长 0x54，见 _M_insert_aux/ix imul $0x54） ----
struct CHARAC_LOAD_MERCENARY
{
    // ORIG C1 W 0x08694c08：+0/+4/+0x24/+0x28/+0x2c 清零、+0x30/0x31=0xff、
    // +0x32=0xa、memset(+6,0,0x1e)、memset(+0x34,0,0x20)（定义在
    // CUserMissingA2.cpp，标量在前两 memset 在后的独立展开，勿调 Reset）。
    CHARAC_LOAD_MERCENARY();
    void Reset();                 // ORIG T 0x081b410a
    int GetCompetitionState() const;  // ORIG T 0x081b2e86
    // ---- 雇佣兵竞技域方法声明（CUserMissingA2.cpp 独占定义；ORIG 报告齐备）----
    int CalcItemDropRate(CUser*) const;                  // ORIG T 0x081b2d6a
    bool CheckCompetitionAreaIndex(char, int) const;     // ORIG T 0x081b2b34
    bool CheckCompetitionPeriodIndex(char) const;        // ORIG T 0x081b2bc4
    bool Competition(CUser*, char, char);                // ORIG T 0x081b3814
    bool GetRewardItem(CUser*, competition_reward_result&, Inven_Item&,
                       std::string&);                    // ORIG T 0x081b3276
    bool Reward(CUser*, competition_reward_result&);     // ORIG T 0x081b3b68
                                                         // SIGFIX b28c：ORIG 串
                                                         // "bool CHARAC_LOAD_MERCENARY::Reward(CUser*, competition_reward_result&)"
    int GetRewardGold(CUser*, competition_reward_result&) const;  // ORIG T 0x081b3046
    float GetAvatarBonus() const;                        // ORIG T 0x081b2c94
    float GetPeriodBonus() const;                        // ORIG T 0x081b2c1e
    float GetRewardCritical(int) const;                  // ORIG T 0x081b2d0a
    int GetCompetitionPeriodTerm(bool*) const;           // ORIG T 0x081b2f32
    int GetCompetitionPeriodSecond(char) const;          // ORIG T 0x081b2ed2
    void SendMessageToCUser(CUser*, char*) const;        // ORIG T 0x081b4190（空体）
    void alter();                                        // ORIG W 0x081b47c8（+0x24 写 1）
    void save_complete();                                // ORIG W 0x08694c8e（+0x24 写 0）

    // [CUserLane 2026-09-11 · S2 承接项/P265 档③] int → unsigned int（同宽 4B，
    //   偏移/sizeof/mangled 零变化）。ORIG SetMercenaryInfoPacket 0x0868e704 读点
    //   0x0868e7e2 `mov (%eax),%eax; mov %eax,%edx`（字段与 int 实参异型 ⇒ 两段中转；
    //   改前 4.4 恒折单段 `mov (%eax),%edx`）。P254 目标同效的 enum 化无对应枚举
    //   （ENUM_CHARACTERJOB 为职业枚举，禁发明）⇒ 用同谓词的无符号化实现。
    //   A/B：件 DIFF 190/190 nd1 → AE 190/191 nd0；CUserMissingA2/CUserOnlyOrig 四数全等。
    unsigned int m_characNo;      // +0x00（FindIndexByCharacNo 按首 dword 无符号比较）


// NOTE_MARKER_NOOP
    short m_level;                // +0x04（AddCurCharacMercenaryInfo 写 charac+0x27）
    char m_name[0x1e];            // +0x06..0x23（Reset memset(this+6,0,0x1e)）
    // [b28d-solo-CUserMissingA2] P292/档②：ORIG 0x0868e459 判脏标记为
    // `movzbl 0x24(%eax),%eax; test %al,%al; je`（无 setne 物化）⇒ 字段为 bool
    // （uchar 会多一条 `setne %al`）。宽度/偏移/sizeof 不变（1 字节）。
    bool m_field24;               // +0x24（Reset 写 0；alter 写 1/save_complete 写 0）
    char m_pad25[3];              // +0x25..0x27（Reset 不触碰）
    // [b28d-Main·检查点17 P321-S1] int→unsigned int（同宽 4B，偏移/sizeof/mangled 不变）：
    //   T1 仅定 4 字节（ORIG `mov DWORD PTR [eax+0x28],edx`）；T2 消费端无有符号指令；
    //   ORIG Competition 0x081b3a?? 段为 RHS-first+中转（`mov edx,eax; mov eax,[ebp+8];
    //   mov [eax+0x28],edx`），探针 /tmp/probe_m28.cpp 实测 unsigned 字段+int RHS 逐条复现。
    unsigned int m_field28;       // +0x28（Reset 写 0）
    int m_field2c;                // +0x2c（Reset 写 0；GetCompetitionState 竞技结束秒）
    unsigned char m_field30;      // +0x30（Reset 写 0xff；GetCompetitionState 未报名标志）
    unsigned char m_field31;      // +0x31（Reset 写 0xff）
    unsigned char m_avatarBonus;  // +0x32 avatar 等级加成：Reset 写 0xa=CalcMercenaryAvatarBonus
                                  //   初值；setAvatarTypeBonus(0x0868ebaa) 写、GetAvatarBonus
                                  //   (0x081b2c94) 读、AddCharac(0x081b4300) 按 m_avatarItems 重算
    unsigned char m_pad33;        // +0x33（Reset 不触碰）
    int m_avatarItems[8];         // +0x34..0x53（Reset memset(this+0x34,0,0x20)；AddCharac 循环读取重算 bonus）
};

static_assert(sizeof(CHARAC_LOAD_MERCENARY) == 0x54,
              "CHARAC_LOAD_MERCENARY ABI drift (ORIG vector stride 0x54 @0x81b4ba9)");
#ifndef DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
#define DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
// Wave5 APSystem 批调和 + Inter_LoginInfo/dispatch_sig 权威化（与 MoreTypes.h
// 同一守卫互斥；SetGrowthCreatureEventInfo(SIG const&) 0x8691cde stride 0x2c 循环，
// 缓冲区总长 0xb*0x2c = 0x1e4 = SIG_LOGIN_DATA+0x4154..0x4338）。
struct SIG_GIVE_GROWTH_CREATURE_EVENT
{
    struct eventCharacInfo
    {
        unsigned short m_serverGroup;   // +0x00
        int m_characNo;                 // +0x04
        int m_job;                      // +0x08
        char m_name[30];                // +0x0c..0x29

        eventCharacInfo() : m_serverGroup(0), m_characNo(0), m_job(0)
        {
            memset(m_name, 0, sizeof(m_name));   // ORIG memset(this+0xc, 0, 0x1e)
        }
    };
    static_assert(sizeof(eventCharacInfo) == 0x2c, "eventCharacInfo size");

    eventCharacInfo m_entries[11];      // stride 0x2c × 0xb = 0x1e4
};
static_assert(sizeof(SIG_GIVE_GROWTH_CREATURE_EVENT) == 0x1e4,
                            "SIG_GIVE_GROWTH_CREATURE_EVENT ABI drift");
#endif  // DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED

// ---- 登录域整包转发视图（dispatch_sig 只作整包指针转发；内部字段归各 owner 批次）
struct SIG_INGAME_EVENT_HISTORY       { unsigned char m_data[0x178]; };  // +0x3fac..0x4123
static_assert(sizeof(SIG_INGAME_EVENT_HISTORY) == 0x178, "SIG_INGAME_EVENT_HISTORY size");
struct SIG_GROWTH_WEAPON_EVENT        { unsigned char m_data[0x14]; };   // +0x4140..0x4153
static_assert(sizeof(SIG_GROWTH_WEAPON_EVENT) == 0x14, "SIG_GROWTH_WEAPON_EVENT size");
// [b28d-22] ORIG 0x08691dca setCharacLevelUpGift_AccountOnce 对 sig 用 scaled 索引
// `mov 0xc(%ebp),%eax; mov 0x4(%eax,%edx,8),%eax` ⇒ 条目数组自 sig+0 起、元素 {int,int}
// stride 8（0x20/8=4 条），.m_second 在条目内 +0x04；循环 i<11 越界读照抄 P171。
// 占位 m_data 令 4.4 折成 shl+add ⇒ 建具名数组；sizeof 0x20 不变。
struct SIG_LEVELUP_GIFT_ACCOUNT_ONCE
{
    struct Entry { int m_first; int m_second; };   // 条目内 +0x00/+0x04
    Entry m_entries[4];                            // +0x00..+0x1f（stride 8）
};   // +0x4338..0x4357
static_assert(sizeof(SIG_LEVELUP_GIFT_ACCOUNT_ONCE) == 0x20, "SIG_LEVELUP_GIFT_ACCOUNT_ONCE size");

// 金币控制统计结构（Secu_GoldControl 用），ORIG sizeof = 0x3c。
// 6 组 64 位值（gold/import/sub/importSub/auctionAdd/auctionSub），后随 6 个计数器。
// 方法 C1/C2/reset/toCopy 为 ORIG 头文件内联弱符号（nm W）：
//  - C1：6 组 64 位逐组 movl $0,(%eax)+movl $0,0x4(%eax)（每组共读一次 this）
//  - reset：memset(this,0,0x3c)；toCopy：memcpy(&other,this,0x3c)
// （2026-08-27 自本文件尾段上移：SIG_LOGIN_DATA::m_goldInfo 值成员需完整类型。）
struct user_gold_info
{
    unsigned long long m_gold;         // +0x00
    unsigned long long m_import;       // +0x08
    unsigned long long m_sub;          // +0x10
    unsigned long long m_importSub;    // +0x18
    unsigned long long m_auctionAdd;   // +0x20
    unsigned long long m_auctionSub;   // +0x28
    unsigned short m_count[6];         // +0x30..0x3a

    user_gold_info()
        : m_gold(0), m_import(0), m_sub(0), m_importSub(0),
          m_auctionAdd(0), m_auctionSub(0), m_count{0,0,0,0,0,0}
    {}
    void reset() { memset(this, 0, 0x3c); }
    void toCopy(user_gold_info& other) { memcpy(&other, this, 0x3c); }
};
static_assert(sizeof(user_gold_info) == 0x3c, "user_gold_info ABI drift");

// [R18arr 2026-09-13] CONT_ENTRY / CONTINUOUS_ITEM_INFO 已上移至本文件 SIG_LOAD_ETC
// 定义之前（SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO 成员化所需）；纯移动零改动。
#endif  // DNF_GAME_SIG_TYPES_C_H_
