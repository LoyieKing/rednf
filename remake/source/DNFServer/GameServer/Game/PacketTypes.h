#ifndef DNF_GAME_PACKET_TYPES_H_
#define DNF_GAME_PACKET_TYPES_H_

// ============================================================================
// df_game_r 还原 —— MSG / Param 类型定义（批量生成）
// 各类型成员布局待细化；当前用 char 数组占位以生成符号。
// 所有 MSG 继承 MSG_BASE，所有 Param 继承 ParamBase。
// ============================================================================

#include "PacketDispatcher.h"
#include "CGameOption.h"      // CETCOption / CQuickChattingOption（MSG_SAVE_GAME_OPTION_* / MSG_QUICK_CHAT 子对象）
#include "CPartyTelePort.h"   // stPartyTelePort（MSG_SOLO_TELEPORT +0x38 reset()）
#include "CUserCharacInfo.h"  // CharacterOption（MSG_SAVE_CHARACTER_OPTION @+0x11）
#include <vector>
#include <string>


// ---------------------------------------------------------------------------
// 辅助类型（ORIG nm/objdump 核实）
// ---------------------------------------------------------------------------
// _ZN12buySkillInfoC1Ev 0x82309de：byte@0=0, byte@1=0, int@4=-1, int@8=-1（size 0xc）
struct buySkillInfo
{
    buySkillInfo() : m_0(0), m_1(0), m_4(-1), m_8(-1) {}
    unsigned char m_0;
    unsigned char m_1;
    int m_4;
    int m_8;
};

// RentalInfo/RentOutput/DeletedRentItem 权威定义见 InstanceRentalSystem.h（§7 合并）。
#include "InstanceRentalSystem.h"
// ---------------------------------------------------------------------------
// MSG 类型（占位大小 0x100，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
class MSG_ABNORMAL_USE_STACKABLE : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线，read 首读 +0x10）
    unsigned int m_field10;   // +0x10 异常使用 stackable 参数1【推断】
                              //（Dispatcher_AbnormalUseStackable/read.md get_int@msg+0x10）
    unsigned int m_field14;   // +0x14 参数2【推断】（get_int@msg+0x14）
};
static_assert(sizeof(MSG_ABNORMAL_USE_STACKABLE) == 0x18, "MSG_ABNORMAL_USE_STACKABLE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ACHIEVEMENT_TRIGGER : public MSG_BASE
{
public:
    unsigned int m_field0d;   // +0x0d 成就触发主参数【推断】
                              //（Dispatcher_AchievementTrigger/read.md get_int@msg+0xd）
    unsigned short m_field11; // +0x11 参数2【推断】（get_short@msg+0x11）
    unsigned short m_field13; // +0x13 参数3【推断】（get_short@msg+0x13）
    unsigned short m_field15; // +0x15 参数4【推断】（get_short@msg+0x15）
};
static_assert(sizeof(MSG_ACHIEVEMENT_TRIGGER) == 0x17, "MSG_ACHIEVEMENT_TRIGGER size");
#pragma pack(pop)

class MSG_ADD_FRIEND : public MSG_BASE
{
public:
    char m_pad[0x22];         // +0xd..（b28d-solo-S153：ORIG 0x82577fc dispatch 帧距实测
                              //   sizeof=0x2f [m=-0x43/p=-0x14]，原 0x21 少 1 字节）
};

#pragma pack(push, 1)
class MSG_APPROVE_JOIN_GUILD : public MSG_BASE
{
public:
    unsigned int m_characNo;  // +0x0d 被批准入会申请的角色编号【推断】
                              //（Dispatcher_ApproveJoinGuild/read.md get_int(uint&)@msg+0xd）
};
static_assert(sizeof(MSG_APPROVE_JOIN_GUILD) == 0x11, "MSG_APPROVE_JOIN_GUILD size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_AVATAR_DISJOINT_RANDOM_REWARD : public MSG_BASE
{
public:
    char m_gapD;                   // +0x0d 包格式 1 字节间隙（ORIG process 读 slot@+0xe）
    unsigned short m_slotE;        // +0x0e 槽位（ORIG 实证；NEW 此前 +0xd 错位 1 字节）
    int m_itemIdx10;               // +0x10 物品 idx（同上）
};
static_assert(sizeof(MSG_AVATAR_DISJOINT_RANDOM_REWARD) == 0x14, "MSG_AVATAR_DISJOINT_RANDOM_REWARD size");
#pragma pack(pop)

class MSG_BINGO_QUIZ : public MSG_BASE
{
public:
    MSG_BINGO_QUIZ();
    // ORIG C1 0x0822dc66：+0xd int 清零（其余未出线）
    int m_0d __attribute__((packed));  // +0x0d
    // pad removed
};

class MSG_BLUE_MARBLE : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE();
    // ORIG C1：+0xd byte 清零
    char m_0d;                // +0x0d（ORIG get_byte(char&)）
    // pad removed
};

class MSG_BLUE_MARBLE_CONFIRM_INFO : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE_CONFIRM_INFO();
    // ORIG C1：+0xd byte 清零、+0xe word=-1
    unsigned char m_0d;       // +0x0d
    unsigned short m_0e;      // +0x0e（ctor = -1）
    // pad removed
};

#pragma pack(push, 1)
class MSG_BOSS_DIE_CHECK : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    unsigned short m_field0e; // +0x0e 队伍/角色类标识【推断】
                              //（Dispatcher_BossDieCheck/read.md get_short@msg+0xe；==-1 时 addServerHackCnt 0xcb）
    unsigned short m_field10; // +0x10 短整型参数【推断】（get_short@msg+0x10；process 复制进包）
    int m_skillCheckSum;      // +0x14 技能校验和【推断】（get_int@msg+0x14；与
                              //  CUser::getCurCharacSkillCheckSum 比较）
    char m_pad18[2];          // +0x16..+0x17（b28d-solo-S161：ORIG 0x8256094 域 msg 槽
                              //  -0x24 ⇒ 帧距 0x18，原 0x16 差 2）
};
static_assert(sizeof(MSG_BOSS_DIE_CHECK) == 0x18, "MSG_BOSS_DIE_CHECK size");
#pragma pack(pop)

class MSG_BREAK_AWAY : public MSG_BASE
{
public:
    MSG_BREAK_AWAY();
    unsigned char m_checkFlag;  // +0x0d（read get_byte 写入；0=走重置/通知分支）
    // pad removed
};

#pragma pack(push, 1)
class MSG_BUY_ITEM : public MSG_BASE
{
public:
    // ORIG DisPatcher_BuyItem（process/get_data/check_error 逐字节）：
    unsigned long m_itemIdx;      // +0x0d（ORIG read 调 get_int(unsigned long&)ERm；
                                  //  get_data find_item 输入）
    unsigned int m_buyCount;      // +0x11（ORIG read 调 get_int(unsigned&)ERj；购买数量；
                                  //  >1 时 get_nMaxBuyableCount 校验）
    unsigned int m_deliveryType;  // +0x15（ORIG read 调 get_int(unsigned&)ERj；
                                  //  eventDeliveryBuyItem 首参回传）
};
#pragma pack(pop)

#pragma pack(push, 1)
// DisPatcher_BuySkill::read（ORIG 0x081bed4a）逐字节布局：
//   +0x0d sbyte 技能树 kind、+0x0e u8 条目数（1..70）、+0xf 起每条目 3 字节交错记录。
struct MSG_BUY_SKILL_entry
{
    unsigned char m_skillIdx;    // +0（条目基址 0xf+3*i）
    unsigned char m_level;       // +1（read 时 <1 置 1）
    unsigned char m_refundFlag;  // +2（0=购买 非0=退还）
};
class MSG_BUY_SKILL : public MSG_BASE
{
public:
    MSG_BUY_SKILL();
    signed char m_treeKind;              // +0x0d
    unsigned char m_count;               // +0x0e
    MSG_BUY_SKILL_entry m_entries[70];   // +0x0f..+0xe0
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CANCEL_JOIN_GUILD : public MSG_BASE
{
public:
    unsigned int m_characNo;  // +0x0d 撤销入会申请的角色编号【推断】
                              //（Dispatcher_CancelJoinGuild/read.md get_int(uint&)@msg+0xd）
};
static_assert(sizeof(MSG_CANCEL_JOIN_GUILD) == 0x11, "MSG_CANCEL_JOIN_GUILD size");
#pragma pack(pop)

class MSG_CHANGE_ANOTHER_SKILL_TREE : public MSG_BASE
{
public:
    MSG_CHANGE_ANOTHER_SKILL_TREE();
    // ORIG C1：+0xd byte = 0xff（未选中的技能树哨兵）
    char m_0d;                // +0x0d（ORIG get_byte(char&)：按有符号 char 读）
    // pad removed
};
// ORIG 布局（ctor 0x0822e0cc 直写实证 + Dispatcher_AvatarOptionChange/read.md）：
// +0x0d short 消耗品槽位 / +0x0f int 消耗品 itemIdx / +0x13 short 目标 avatar 槽位 /
// +0x15 int 目标装备 itemIdx / +0x19 byte option 值；总长 0x10d（MSG_BASE 0xd）。
#pragma pack(push, 1)
class MSG_CHANGE_AVATAR_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_AVATAR_OPTION();
    unsigned short m_consumeSlot;   // +0x0d（ctor -1）
    int m_consumeItemIdx;           // +0x0f（ctor -1）
    unsigned short m_avatarSlot;    // +0x13（ctor -1）
    int m_equipItemIdx;             // +0x15（ctor -1）
    unsigned char m_option;         // +0x19（ctor 0xff）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CHANGE_EMOTION : public MSG_BASE
{
public:
    short m_emotionIdx;       // +0x0d 表情索引（Dispatcher_ChangeEmotion/read.md get_short@msg+0xd；
                              //  经 isVaildEmotionValue 校验后 SetCurCharacEmotion）
};
static_assert(sizeof(MSG_CHANGE_EMOTION) == 0xf, "MSG_CHANGE_EMOTION size");
#pragma pack(pop)

class MSG_CHANGE_HOST_WARROOM : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x824fd96 域 msg 槽 -0x1e ⇒ 帧距 0x12，原 0x11 差 1。
    char m_pad[0x5];
};

#pragma pack(push, 1)
class MSG_CHANGE_LETTER_STAT : public MSG_BASE
{
public:
    int m_mailIdx;            // +0x0d 邮件编号（Dispatcher_ChangeLetterStat/read.md get_int@msg+0xd）
    short m_letterStat;       // +0x11 新邮件状态【推断】（get_short(s16&)@msg+0x11）
};
static_assert(sizeof(MSG_CHANGE_LETTER_STAT) == 0x13, "MSG_CHANGE_LETTER_STAT size");
#pragma pack(pop)
class MSG_CHANGE_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_RANDOM_OPTION();
    // ORIG C1：+0xe/+0x10 word 清零、+0x12 byte 清零（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    unsigned short m_10;      // +0x10
    unsigned char m_12;       // +0x12
    // pad removed
};

#pragma pack(push, 1)
class MSG_CHANGE_SKILLSLOT : public MSG_BASE
{
public:
    unsigned char m_head0d;   // +0xd（包头由基类 MSG_BASE::m_head[0xd] 提供）
    // ORIG read/check_error 读 msg+0xd；process：param+4/5 ← msg[0xe]/msg[0xf]
    unsigned char m_curSlot;   // +0x0e（原技能槽）
    unsigned char m_newSlot;   // +0x0f（目标技能槽）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CHARAC_SELECT : public MSG_BASE
{
public:
    MSG_CHARAC_SELECT();
    signed char m_slot;      // +0xd（请求选择的角色槽位）
    unsigned int m_0e;       // +0xe（process 从 _Charac_info.m_field14b8 回填）
    unsigned char m_12;      // +0x12（_Charac_info.m_field14b7）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CHECK_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_CHECK_GUILD_JOIN();
    // ORIG C1：+0xd int 清零、memset(this+0x11,0,0x17)
    int m_0d;                 // +0x0d
    char m_11[0x17];          // +0x11..+0x27（ctor memset 全零）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CLIENT_SPEC_STATISTIC : public MSG_BASE
{
public:
    MSG_CLIENT_SPEC_STATISTIC();
    // ORIG C1 0x082556a0：MSG_BASE::C1(this) + Client_Spec::C1(this+0xe)。
    // [b28d-Main·检查点14] Client_Spec 已修正为 0xbf8（见 CUser.h）⇒ 本类
    // sizeof = 0xe + 0xbf8 = 0xc06，与 ORIG dispatch_template 帧实证一致。
    char m_padD;               // +0x0d
    Client_Spec m_spec;        // +0xe（sizeof 0xbf8）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_COLLECT_ITEMS : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    short m_itemIdx;          // +0x0e 回收物品索引（Dispatcher_CollectItems/read.md get_short@msg+0xe；
                              //  process 传 CInventory::delete_item）
    unsigned short m_count;   // +0x10 回收数量（get_short@msg+0x10）
};
static_assert(sizeof(MSG_COLLECT_ITEMS) == 0x12, "MSG_COLLECT_ITEMS size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_COMBO_SKILL : public MSG_BASE
{
public:
    MSG_COMBO_SKILL();
    char m_pad0d;      // +0x0d
    // +0x0e std::map<short, std::vector<int> >（ORIG C1 调 map C1 + clear）
    std::map<short, std::vector<int>, std::less<short>,
             std::allocator<std::pair<short const, std::vector<int> > > > m_comboMap;
};

#pragma pack(pop)
class MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET : public MSG_BASE
{
public:
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET();
    // ORIG C1：+0xd byte 清零
    unsigned char m_0d;       // +0x0d
    // pad removed
};

#pragma pack(push, 1)
class MSG_COMPATIBILITY_INDEX : public MSG_BASE
{
public:
    MSG_COMPATIBILITY_INDEX();
    // ORIG C1 0x0822e174 偏移实证：+0xf 为 gap，int@+0x10、word@+0x14 起
    unsigned char m_0d;   // +0x0d
    unsigned char m_0e;   // +0x0e
    char m_padF;          // +0x0f（ORIG gap）
    int m_10;             // +0x10
    short m_14;           // +0x14
    short m_16;           // +0x16
    short m_18;           // +0x18
    short m_1a;           // +0x1a
    unsigned char m_1c;   // +0x1c
    unsigned char m_1d;   // +0x1d
    char m_pad1e[2];      // +0x1e..+0x1f（b28d-solo-S153：ORIG 0x824b272 帧距 sizeof=0x20
                          //   [msg 深 m=-0x30/p=-0x10]；线上字段止于 +0x1d）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CREATE_CHARACTER : public MSG_BASE
{
public:
    unsigned char m_slot;    // +0xd
    char m_padE[4];          // +0xe..+0x11（ORIG name 前间隙）
    char m_name[0x1e];       // +0x12..0x2f（ORIG strncpy 长 0x1e；末字节即 +0x2f 标志位）
};
#pragma pack(pop)

class MSG_CREATE_DISJOINT_STORE : public MSG_BASE
{
public:
    char m_pad[0x10f];  // b28d-solo-S153：ORIG 0x82549c2 帧距 sizeof=0x11c [m=-0x130/p=-0x14]
};

#pragma pack(push, 1)
class MSG_DECREASE_DURABILITY : public MSG_BASE
{
public:
    unsigned char m_0d;     // +0x0d（UseEquip process：use_equip 槽位，回显至 param+0x8）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_CHARACTER : public MSG_BASE
{
public:
    signed char m_slot;      // +0xd
    char m_padE[0x22];        // +0xe..（b28d-solo-S153：ORIG 0x825a2ce 帧距 sizeof=0x30
                              //   [m=-0x44/p=-0x14]，原 0x21 少 1 字节）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_ITEM : public MSG_BASE
{
public:
    // 布局证据：DisPatcher_DeleteItem::process(0x081c1dda)/exercise_mode/
    // restricted_check/condition_check 对 msg 的全部读取（i<count<=100）：
    //   +0x0d byte itemSpace、+0x0e sbyte count、+0x0f short opType[i]、
    //   +0xd7 short position[i]、+0x19f int itemIdx[i]、+0x32f int countArr[i]。
    // [b28d-Main·检查点14] +0x4bf int m_checkSum：ORIG check_error 0x081c1a66
    //   `mov 0x4bf(%edx),%edx` 单指令直读包尾校验和，且 dispatch_template 帧
    //   实证 sizeof=0x4c3（msg 槽 -0x7fb vs 0x4bf 版的 -0x7f7）⇒ 该 4B 属本体
    //   （此前 DisPatcherCore 的 DeleteItemMsgTailView 派生视图已删除并入本体）。
    char m_itemSpace;                 // +0x0d（ORIG get_byte(char&)；char 非 unsigned char）
    signed char m_count;              // +0x0e
    short m_opType[100];              // +0x0f（==1 用途删除；2/3/4 方块统计）
    short m_position[100];            // +0xd7（槽位）
    unsigned long m_itemIdx[100];     // +0x19f（ORIG get_int(unsigned long&)；x86 32 位 unsigned long==int 宽度）
    int m_countArr[100];              // +0x32f
    int m_checkSum;                   // +0x4bf（包尾校验和；sizeof=0x4c3）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_TO_BLACKLIST : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线）
    int m_nameLen;            // +0x10 角色名长度【推断】（Dispatcher_DeleteToBlackList/read.md get_int@msg+0x10）
    char m_characName[0x1e];  // +0x14 移出黑名单的角色名（get_str(buf,0x1e)@msg+0x14）
    char m_pad34[2];          // +0x32..+0x33（b28d-solo-S153：ORIG 0x8255cac 帧距 sizeof=0x34
                              //   [m=-0x48/p=-0x14]；线上字段止于 +0x31）
};
static_assert(sizeof(MSG_DELETE_TO_BLACKLIST) == 0x34, "MSG_DELETE_TO_BLACKLIST size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DENY_JOIN_GUILD : public MSG_BASE
{
public:
    unsigned int m_characNo;  // +0x0d 被拒绝入会申请的角色编号【推断】
                              //（Dispatcher_DenyJoinGuild/read.md get_int(uint&)@msg+0xd）
};
static_assert(sizeof(MSG_DENY_JOIN_GUILD) == 0x11, "MSG_DENY_JOIN_GUILD size");
#pragma pack(pop)

class MSG_DIE_CHARACTER : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x8259f68 域 msg 槽 -0x1d ⇒ 帧距 0x11，原 0x10 差 1。
    char m_pad[0x4];         // +0xd..+0x10（帧推导总长 0x11）
};

#pragma pack(push, 1)
class MSG_DONATE_GUILD_FUND : public MSG_BASE
{
public:
    int m_donateMoney;        // +0x0d 公会基金捐赠金额【推断】
                              //（Dispatcher_DonateGuildFund/read.md get_int@msg+0xd）
};
static_assert(sizeof(MSG_DONATE_GUILD_FUND) == 0x11, "MSG_DONATE_GUILD_FUND size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DROP_ITEM : public MSG_BASE
{
public:
    short m_0d;             // +0x0d（drop_item itemIdx，s16）
    short m_0f;             // +0x0f（drop count，s16）
    unsigned char m_11;     // +0x11（inven space，回显至 param+0x8）
    short m_12;             // +0x12（s16）
    int m_14;               // +0x14
    char m_padTail[1];        // +0x18（帧推导总长 0x19）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ENCHANT : public MSG_BASE
{
public:
    MSG_ENCHANT();
    // [P321 T1 直证] ORIG 全消费点 `movzwl 0xd(%eax),%eax; movzwl %ax,%edx`
    //   （ProcUpgrade 0x0854a1d7/0x0854a38f/0x0854a65f，check_error 0x081d34b3）
    //   零扩展 ⇒ 真型 unsigned short（旧 short 使 4.4 产 movswl/cwtl）。
    unsigned short m_0d;  // +0x0d
    int m_0f;             // +0x0f
    unsigned short m_13;  // +0x13（同上，0x0854a249/0x0854a53d/0x081d3537 movzwl）
    int m_15;             // +0x15
    unsigned char m_19;   // +0x19
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ENCHANT_BY_BEAD : public MSG_BASE
{
public:
    signed char m_field0d;    // +0x0d OnEnchantByBead 第2参【推断】（Dispatcher_EnchantByBead/read.md get_byte@msg+0xd）
    short m_field0e;          // +0x0e 第3参【推断】（get_short@msg+0xe）
    signed char m_field10;    // +0x10 第4参【推断】（get_byte@msg+0x10）
    char m_gap11;             // +0x11（ORIG 未出线）
    short m_field12;          // +0x12 第5参【推断】（get_short@msg+0x12）
};
static_assert(sizeof(MSG_ENCHANT_BY_BEAD) == 0x14, "MSG_ENCHANT_BY_BEAD size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ENTER_DISJOINT_STORE : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    short m_field0e;          // +0x0e 进入目标标识 slot/idx【推断】
                              //（Dispatcher_EnterDisjointStore/read.md get_short@msg+0xe）
};
static_assert(sizeof(MSG_ENTER_DISJOINT_STORE) == 0x10, "MSG_ENTER_DISJOINT_STORE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_EQUIPSLOT_SWITCH : public MSG_BASE
{
public:
    signed char m_field0d;    // +0x0d EquipslotSwitch 第3参【推断】（Dispatcher_Equipslot_Switch/read.md get_byte@msg+0xd）
    signed char m_field0e;    // +0x0e 第4参【推断】（get_byte@msg+0xe）
    signed char m_field0f;    // +0x0f 第2参【推断】（get_byte@msg+0xf）
};
static_assert(sizeof(MSG_EQUIPSLOT_SWITCH) == 0x10, "MSG_EQUIPSLOT_SWITCH size");
#pragma pack(pop)

class MSG_EXCHANGE_RANDOM_ITEM_REWARD : public MSG_BASE
{
public:
    MSG_EXCHANGE_RANDOM_ITEM_REWARD();
    // ORIG C1：std::vector<int> 构造于 +0x10（+0xd..+0xf 未出线）
    char m_padD[3];                    // +0x0d..+0x0f
    std::vector<int> m_rewardItems;    // +0x10（4 对齐推入；ORIG 单 call 实证）
};

#pragma pack(push, 1)
class MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE : public MSG_BASE
{
public:
    signed char m_flag;       // +0x0d 装备栏扩展标志（Dispatcher_Expand_Equipslot_Flag_Update/read.md
                              //  get_byte@msg+0xd；process 传 CUser::UpdateEquipslotFlag）
};
static_assert(sizeof(MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE) == 0xe, "MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_EXPERT_EXTRACTION : public MSG_BASE
{
public:
    MSG_EXPERT_EXTRACTION();
    // ORIG C1：+0xd byte 清零、+0x10 word 清零、+0xe ← word(+0x10)（拷贝链）
    unsigned char m_0d;       // +0x0d
    unsigned short m_0e;      // +0x0e（= m_10）
    unsigned short m_10;      // +0x10（ctor = 0）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_FATIGUE_ATTENDANCE : public MSG_BASE
{
public:
    MSG_FATIGUE_ATTENDANCE();
    // ORIG C1：+0xd int 清零
    int m_0d;                 // +0x0d
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_FINISH_LOADING : public MSG_BASE
{
public:
    int m_0d;             // +0x0d（客户端 inven checksum，ORIG process 与
                          //  CUser::getCurCharacInvenCheckSum 比对，不等记 0x25b）
    int m_11;             // +0x11（skill checksum，与 getCurCharacSkillCheckSum 比对）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_FLOAT_RDATA_MODULATION : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线，read 首读 +0x10）
    unsigned int m_encoded1;  // +0x10 加密值1，process 解码 xor 0x59ab991f【推断】
                              //（Dispatcher_Float_RDATA_Modulation/read.md get_int@msg+0x10）
    unsigned int m_encoded2;  // +0x14 加密值2，process 解码 xor 0x1f93712b【推断】（get_int@msg+0x14）
};
static_assert(sizeof(MSG_FLOAT_RDATA_MODULATION) == 0x18, "MSG_FLOAT_RDATA_MODULATION size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GET_ITEM : public MSG_BASE
{
public:
    unsigned short m_0d;    // +0x0d（item serial；GetItemSerialChecker 第三参）
    unsigned char m_0f;     // +0x0f（==1 布尔标志，传 CParty::get_item/handlePickupItem）
    unsigned char m_10;     // +0x10（槽位/pos，sbyte 传 get_item 与 0x56 广播分支判定 2/3/4）
    unsigned short m_11;    // +0x11
    unsigned short m_13;    // +0x13
    unsigned short m_15;    // +0x15（%7 反外挂校验）
    unsigned char m_17;     // +0x17（!=1 时计 hack 0x3bb）
    union { unsigned char m_pad18[2]; unsigned short m_18; };   // +0x18（ORIG 081c3566 get_shortERt@0x18 → ushort；m_pad18 兼容旧消费者）
    unsigned short m_1a;    // +0x1a（拾取 x）
    unsigned short m_1c;    // +0x1c（拾取 y）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GOLD_TAKE_INCREASING_AMOUNT : public MSG_BASE
{
public:
    int m_overSkillTime;      // +0x0d 宠物/精灵过技能时间值（Dispatcher_GoldTakeIncreasingAmount/read.md
                              //  get_int@msg+0xd；process 传 CCreatureMgr::SetOverSkillTime）
};
static_assert(sizeof(MSG_GOLD_TAKE_INCREASING_AMOUNT) == 0x11, "MSG_GOLD_TAKE_INCREASING_AMOUNT size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GROWTH_EQUIPMENT : public MSG_BASE
{
public:
    MSG_GROWTH_EQUIPMENT();
    // ORIG C1：+0xd/+0x11 int 各清零
    int m_0d;                 // +0x0d
    int m_11;                 // +0x11
    // pad removed
};
#pragma pack(pop)

class MSG_GROWTH_POWER : public MSG_BASE
{
public:
    MSG_GROWTH_POWER();
    // ORIG C1：+0xd byte 清零、memset(this+0xe,0,5)
    unsigned char m_0d;       // +0x0d
    char m_0e[5];             // +0x0e..+0x12（ctor 全零）
    // pad removed
};

#pragma pack(push, 1)
class MSG_GUILD_ATTENDANCE : public MSG_BASE
{
public:
    signed char m_field0d;    // +0x0d 公会出勤信息类型【推断】
                              //（Dispatcher_GuildAttendanceInfo/read.md get_byte@msg+0xd）
};
static_assert(sizeof(MSG_GUILD_ATTENDANCE) == 0xe, "MSG_GUILD_ATTENDANCE size");
#pragma pack(pop)

// ORIG 布局（PacketTypes keeper 协作批次）：STGuildBoardDBInfo 嵌于 +0xd，
// 具名字节 +0xb2（ORIG 写 0xa5 = sizeof(STGuildBoardDBInfo)）。
class STGuildMemberCharacData
{
public:
    STGuildMemberCharacData();       // bff@0/bff@1/b0@2 + memset(this+3,0,0x1e)
    unsigned char m_0;
    unsigned char m_1;
    unsigned char m_2;
    char m_pad3[0x1e];
} __attribute__((packed));  // sizeof = 0x21（packed；非 pack 时因 alignment=4 → 0x24，致 MSG_GUILD_BOARD m_b2 偏移 +3）

class STGuildBoardDBInfo
{
public:
    STGuildBoardDBInfo();            // members C1@0x84、ints 0x78/7c/80 清零、memset(this,0,0x78)
    char m_pad78[0x78];
    int m_78;
    int m_7c;
    int m_80;
    STGuildMemberCharacData m_members;   // +0x84
} __attribute__((packed));  // sizeof = 0xa5

#pragma pack(push, 1)
class MSG_GUILD_BOARD : public MSG_BASE
{
public:
    MSG_GUILD_BOARD();
    STGuildBoardDBInfo m_info;       // +0xd..+0xb1（pack 后真落 0xd；ORIG add $0xd 实证）
    unsigned char m_b2;              // +0xb2（ORIG C1 清零）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GUILD_CARGO_MOVE_ITEM : public MSG_BASE
{
public:
    unsigned short m_field0d;  // +0x0d 货运参数1【推断】
                               //（Dispatcher_GuildCargoMoveItem/read.md get_short@msg+0xd；process.md:153）
    unsigned short m_field0f;  // +0x0f 货运参数2【推断】（get_short@msg+0xf；process.md:154）
    int m_field11;             // +0x11 货运参数3【推断】（get_int@msg+0x11；process.md:155）
    int m_field15;             // +0x15 货运参数4【推断】（get_int@msg+0x15；process.md:156）
};
static_assert(sizeof(MSG_GUILD_CARGO_MOVE_ITEM) == 0x19, "MSG_GUILD_CARGO_MOVE_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GUILD_CARGO_POP_ITEM : public MSG_BASE
{
public:
    unsigned short m_field0d;  // +0x0d 槽位【推断】
                               //（Dispatcher_GuildCargoPopItem/read.md get_short@msg+0xd；process.md:187）
    int m_itemIdx;             // +0x0f 物品索引（get_int@msg+0xf；process.md find_item）
    int m_field13;             // +0x13 int 参数【推断】（get_int@msg+0x13；process.md:189）
    unsigned char m_field17;   // +0x17 byte flag【推断】（get_byte@msg+0x17；process.md 与 is_stackable 并用）
};
static_assert(sizeof(MSG_GUILD_CARGO_POP_ITEM) == 0x18, "MSG_GUILD_CARGO_POP_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_GUILD_CARGO_PUSH_ITEM : public MSG_BASE
{
public:
    unsigned char m_field0d;   // +0x0d byte 参数【推断】
                               //（Dispatcher_GuildCargoPushItem/read.md get_byte@msg+0xd；process.md:245）
    unsigned short m_field0e;  // +0x0e ushort 参数【推断】（get_short@msg+0xe；process.md:246）
    int m_field10;             // +0x10 int 参数【推断】（get_int@msg+0x10；process.md:242）
    int m_field14;             // +0x14 int 参数【推断】（get_int@msg+0x14；process.md:244）
    unsigned short m_field18;  // +0x18 ushort 参数【推断】（get_short@msg+0x18；process.md:243）
};
static_assert(sizeof(MSG_GUILD_CARGO_PUSH_ITEM) == 0x1a, "MSG_GUILD_CARGO_PUSH_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_HATCH_CREATURE : public MSG_BASE
{
public:
    unsigned char m_itemSpace;  // +0x0d 物品空间类型（Dispatcher_HatchCreature/read.md get_byte@msg+0xd；
                                //  check_error GetInvenTypeFromItemSpace + CheckItemLock）
    short m_slotIdx;            // +0x0e 待孵化生物槽索引（get_short@msg+0xe；CheckItemLock 第2参）
};
static_assert(sizeof(MSG_HATCH_CREATURE) == 0x10, "MSG_HATCH_CREATURE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_INFORM_NOTICE : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE();
    // ORIG C1：+0xd int = 0x80（notice 类型哨兵）
    int m_noticeFlag;         // +0x0d 通知 flag（ctor = 0x80 哨兵；Dispatcher_InformNotice/read.md
                              //  get_int@msg+0xd；process handleInformNoticeFlag）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_INFORM_NOTICE_2ND : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE_2ND();
    // ORIG C1：+0xd int = 0x80
    int m_noticeFlag;         // +0x0d 通知 flag（ctor = 0x80；Dispatcher_InformNotice_2nd/read.md
                              //  get_int@msg+0xd；process CCharacInformNotice::handleInformNoticeFlag）
    // pad removed
};
#pragma pack(pop)

class MSG_ITEM_DICTIONARY : public MSG_BASE
{
public:
};

class MSG_ITEM_HYPER_LINK_MESSAGE : public MSG_BASE
{
public:

    // ORIG 布局（Dispatcher_ItemHyperLinkMessage/read 0x081d98c4、process 报告）
    unsigned char m_typeD;             // +0x0d 消息频道类型
    unsigned short m_itemSpaceE;       // +0x0e INVEN space（use_stackable 用）
    unsigned int m_slot10;             // +0x10 槽位/子类型
    int m_textLen14;                   // +0x14 文本长度
    char m_text18[0x100];              // +0x18..0x117 文本（get_str 上限 0x100）
    int m_nameLen118;                  // +0x118 名字长度（type==1||0x26 才读）
    char m_name11c[0x1e];              // +0x11c..0x139 名字（get_str 上限 0x1e）
    char m_field13a;                   // +0x13a（type==1||0x26 才读，signed char）
    unsigned char m_linkCount13b;      // +0x13b 链接物品数（read 校验 1..3）
    unsigned char m_links13c[3][0x68]; // +0x13c..0x273 每项 0x68 字节块
};

#pragma pack(push, 1)
class MSG_MULTI_MAILBOX_SEND : public MSG_BASE
{
public:

    // ORIG 布局依据：Dispatcher_MultiMailBoxSend/dispatch_sig @0x082262a2 全部读写偏移，
    // sizeof 由 CUser::set_MultimailMsgData memcpy(this+0x8e246, msg, 0x1a6) 实证定案。
    int m_flagD;                  // +0x0d get_int #1
    char m_title11[0x1e];         // +0x11 收件人名（get_str 上限 0x1e）
    unsigned int m_gold2f;        // +0x2f 附金币
    unsigned char m_count33;      // +0x33 附件条目数（<=10）
    // 条目区 stride 0xb（并行视图；逐字段相对偏移见 MultiMailBoxSend dispatch_sig 实现注释）：
    char m_body34[0x6e];          // +0x34..0xa1
    int m_msgLenA2;               // +0xa2 消息长度
    char m_msgTextA6[0x100];      // +0xa6..0x1a5 消息正文
};                                // sizeof 0x1a6
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ITEM_UPGRADE : public MSG_BASE
{
public:
    MSG_ITEM_UPGRADE();
    // ORIG C1：+0x25 word 清零、memset(this+0x28,0,0x81)、+0x27 byte 清零
    // +0x0d..+0x24 字段化（ORIG ProcUpgrade @0x0854779c 逐 disp 实证，UpC11 请求/Main 代落；
    // 净长 0xd+1+2+4+2+2=0x18 与旧 padD 等长，sizeof 0x10d 不变）
    char m_padD[0xd];              // +0x0d..0x19（ORIG 未出线）
    unsigned char m_upgradeType;   // +0x1a 升级(0)/增幅(1) movzbl 分派
    unsigned short m_targetSlot;   // +0x1b movzwl（GetInvenSlot/update_item/delete_item 第2参）
    unsigned int m_targetItemIdx;  // +0x1d 与 Inven_Item+0x02 一致性校验
    unsigned short m_ticketSlot;   // +0x21 保护券槽 0xffff=无券
    unsigned short m_materialSlot; // +0x23 材料槽（delete_item 第2参）
    unsigned short m_25;      // +0x25
    unsigned char m_27;       // +0x27
    char m_28[0x81];          // +0x28..+0xa8（ctor memset 全零）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_LOG_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_LOG_INGAME_ADVERTISEMENT();
    signed char m_field0d;    // +0x0d 广告日志类型【推断】
                              //（Dispatcher_LogIngameAdvertisement/read.md get_byte@msg+0xd）
    short m_field0e;          // +0x0e 广告日志参数【推断】（get_short@msg+0xe）
    signed char m_field10;    // +0x10 标志字节【推断】（get_byte@msg+0x10）
    unsigned int m_field11;   // +0x11 计数值【推断】（get_int@msg+0x11）
    // pad removed
};
#pragma pack(pop)
#pragma pack(push, 1)
class MSG_MAILBOX_EXTRACT_ITEM : public MSG_BASE
{
public:
    unsigned int m_mailIdx;   // +0x0d 提取的邮件编号【推断】
                              //（Dispatcher_MailBox_Extract/read.md get_int(uint&)@msg+0xd）
    // b28d-solo-S161：ORIG 0x8256668 域 msg 槽 -0x22 ⇒ 帧距 0x16，原 0x14 差 2。
    char m_lane10pad[0x5];
};
static_assert(sizeof(MSG_MAILBOX_EXTRACT_ITEM) == 0x16, "MSG_MAILBOX_EXTRACT_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MAILBOX_SEND : public MSG_BASE
{
public:

    // ORIG 布局依据：Dispatcher_MailBox_Send/read @0x081cc3da 全部读写偏移：
    int m_nameLenD;            // +0x0d 收件人名长度（read get_int #1）
    char m_name11[0x1d];       // +0x11 收件人名（get_str 上限 0x1d）
    unsigned int m_gold2e;     // +0x2e 附金币
    unsigned int m_guildKey32; // +0x32 公会邮键（非 0 走公会邮件；后读）
    unsigned char m_space36;   // +0x36 附件物品空间
    unsigned short m_slot37;   // +0x37 附件槽位
    unsigned long m_itemId39;  // +0x39 附件物品 id
    int m_extra3d;             // +0x3d 附件附加字段
    int m_titleLen41;          // +0x41 标题长度
    char m_text45[0x100];      // +0x45..0x144 邮件文本（get_str 上限 0x100）
};                             // sizeof 0x145
static_assert(sizeof(MSG_MAILBOX_SEND) == 0x145, "MSG_MAILBOX_SEND size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MEMBER_ENTER_REPLY : public MSG_BASE
{
public:
    unsigned char m_reply;    // +0x0d 进入请求答复【推断:0=拒绝 1=同意】
                              //（Dispatcher_MemberEnterReply/read.md get_byte@msg+0xd）
};
static_assert(sizeof(MSG_MEMBER_ENTER_REPLY) == 0xe, "MSG_MEMBER_ENTER_REPLY size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MEMBER_SECEDE : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线）
    int m_nameLen;            // +0x10 角色名长度【推断】（Dispatcher_MemberSecede/read.md get_int@msg+0x10）
    char m_characName[0x1e];  // +0x14 脱离公会的成员角色名（get_str(buf,0x1e)@msg+0x14）
    char m_pad34[2];          // +0x32..+0x33（b28d-solo-S153：ORIG 0x8257038 帧距 sizeof=0x34
                              //   [m=-0x48/p=-0x14]）
};
static_assert(sizeof(MSG_MEMBER_SECEDE) == 0x34, "MSG_MEMBER_SECEDE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MONSTERCARD_BIND : public MSG_BASE
{
public:
    MSG_MONSTERCARD_BIND();
    // ORIG C1：word 拷贝链 +0x11 ← 0，+0xf ← +0x11，+0xd ← +0xf
    unsigned short m_0d;      // +0x0d（= m_0f）
    unsigned short m_0f;      // +0x0f（= m_11）
    unsigned short m_11;      // +0x11（ctor = 0）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MOTION_HACK : public MSG_BASE
{
public:
    unsigned char m_field0d;  // +0x0d 移动外挂检测类型【推断】
                              //（Dispatcher_MotionHack/read.md get_byte@msg+0xd）
    unsigned int m_field0e;   // +0x0e 检测数据1【推断】（get_int@msg+0xe）
    unsigned int m_field12;   // +0x12 检测数据2【推断】（get_int@msg+0x12）
};
static_assert(sizeof(MSG_MOTION_HACK) == 0x16, "MSG_MOTION_HACK size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MOVE_ITEMSPACE : public MSG_BASE
{
public:
    // 布局证据：DisPatcher_MoveItem::process(0x081c5904) 对 msg 的全部读取：
    //   +0x0d byte 源 space、+0x0e short 源槽位、+0x10/+0x14 双 u32 组成
    //   exchange_interspace 的 ulong 数量实参（低/高）、+0x18 byte 目的 space、
    //   +0x19 short 目的槽位、+0x1b u32 itemIdx、+0x23/+0x24 byte expand 参数。
    char m_fromSpace;                 // +0x0d（ORIG get_byte(char&)）
    short m_fromPos;                  // +0x0e
    unsigned long m_count;            // +0x10（ORIG get_int(unsigned long&)；x86 32 位==u32 宽度）
    int m_field14;                    // +0x14（ORIG get_int(int&) —— §119 误设 unsigned long，callee ERm→ERi 修正）
    char m_toSpace;                   // +0x18（ORIG get_byte(char&)）
    short m_toPos;                    // +0x19
    unsigned long m_itemIdx;          // +0x1b（ORIG get_int(unsigned long&)）
    unsigned char m_pad1f[4];         // +0x1f..+0x22（ORIG 未读取区）
    char m_expanda;                   // +0x23（ORIG get_byte(char&)）
    char m_expandb;                   // +0x24
};   // MSG_MOVE_ITEMSPACE
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MOVE_MAP : public MSG_BASE
{
public:
    signed char m_x;              // +0x0d（move_map 目标格 x，movsbl 有符号扩展）
    signed char m_y;              // +0x0e（move_map 目标格 y）
    unsigned int m_fieldF;        // +0x0f（Secu_HackLogCheckByParty::
                                  //  CheckMoveMapFitInPathHackCnt 第 3 实参）
    unsigned int m_field13;       // +0x13（同上第 4 实参）
    unsigned char m_field17;      // +0x17（==1 时 CParty::move_map 先 checkLayeredMap）
    // ORIG 0x081c4b7a `movzwl 0x18(%eax),%ecx` 2B 读（A822Y 证，CheckTrapBitsHackCnt %7 判据）
    unsigned short m_trapBits;    // +0x18..+0x19（trap 位图，%7 整除性 hack 检查）
    unsigned char m_1a;           // +0x1a（ORIG process 校验 ==1 跳过 0x25a hack 记数）
    unsigned short m_1b[4];       // +0x1b..+0x22（[0]=mapIndex：CBossTowerFunc::
                                  //   MoveMap 以 (ushort*)(msg+0x1b) 传入；
                                  //   [i] 亦为 CParty::checkMoveMapHackCnt
                                  //   按成员槽读取的击杀数 short 数组）
    unsigned int m_23[4];         // +0x23..+0x33（每成员 playtime tick，
                                  //  CheckOneMapPlayTimeHackCnt 逐槽修正/上报）
    char m_pad33[2];              // +0x33..+0x34
    unsigned int m_field35;       // +0x35..+0x38（ORIG 0x85a8b8f `mov 0x35(%eax),%eax`
                                  //  单 dword 直读；checkMoveMapHackCnt 判 0 换单人单尾。
                                  //  W6Shop19-2 证，Main 代落；memcpy 假调用清除见其源改）
    unsigned char m_39;           // +0x39（process 回写 CParty::get_party_seatno）
};
#pragma pack(pop)

class MSG_OBJECT_BRING_UP : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x824b856 域 msg 槽 -0x1c ⇒ 帧距 0x10，原 0xf 差 1。
    char m_pad[0x3];
};

#pragma pack(push, 1)
class MSG_OPEN_CERAPACKAGE : public MSG_BASE
{
public:
    MSG_OPEN_CERAPACKAGE();
    // ORIG C1：+0xe word 清零、memset(this+0x14,0,0x118)
    char m_padD;              // +0x0d
    unsigned short m_0e;      // +0x0e
    char m_pad10[4];          // +0x10..+0x13
    char m_14[0x118];         // +0x14..+0x12b（ctor memset 全零；总长 0x12c 真实布局）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE : public MSG_BASE
{
public:
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE();
    // ORIG C1：+0xd byte、+0xe int、+0x12 int 清零；GetInBuffer 传 sizeof=0x16
    unsigned char m_0d;       // +0x0d
    int m_0e;                 // +0x0e（pack 后紧贴；ORIG 单指令 movl 写入）
    int m_12;                 // +0x12（read get_int 第三字段）
};
#pragma pack(pop)
class MSG_P2P_STATISTICS : public MSG_BASE
{
public:
    MSG_P2P_STATISTICS();
    // ORIG C1 0x0822e1d2 偏移实证：+0xf gap，其余全部落在自然对齐位
    // （不加 pack：int/short 自然对齐即复现 ORIG 布局）
    unsigned char m_0d;   // +0x0d
    unsigned char m_0e;   // +0x0e
    char m_padF;          // +0x0f（ORIG gap）
    short m_10;           // +0x10
    short m_12;           // +0x12
    short m_14;           // +0x14
    int m_18;             // +0x18（自然对齐推入）
    int m_1c;             // +0x1c
    int m_20;             // +0x20
    int m_24;             // +0x24
    short m_28;           // +0x28
    short m_2a;           // +0x2a
    short m_2c;           // +0x2c
    int m_30;             // +0x30（自然对齐推入）
    int m_34;             // +0x34
    int m_38;             // +0x38
    int m_3c;             // +0x3c
};

// ORIG 布局（SetPartyInfoUI 0x0859b234 读取实证）：+0x0d titleIndex、
#pragma pack(push, 1)
class MSG_PARTY_SET_INFO : public MSG_BASE
{
public:
    unsigned char m_titleIndex;   // +0x0d
    int m_titleLen;               // +0x0e..0x11（SetPartyInfo::read 单级 int 字段装载 0xe(%eax)，DCoreB 证据；4B 同宽 sizeof 0x3e 不变）
    char m_title[0x20];           // +0x12（setTitle 拷贝源，0x20 同 CParty::m_title）
    unsigned char m_userMax;      // +0x32
    unsigned short m_dungIndex;   // +0x33
    unsigned char m_dungDiffi;    // +0x35
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_PARTY_TELEPORT : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT();
    // ORIG C1：+0xd/+0xe byte、+0xf/+0x11 word、+0x13 byte 全零；
    // read（Dispatcher_PartyTeleport/read.md）同偏移：byte@0xd/0xe、short@0xf/0x11、byte@0x13【推断】
    unsigned char m_field0d;  // +0x0d
    unsigned char m_field0e;  // +0x0e
    unsigned short m_field0f; // +0x0f
    unsigned short m_field11; // +0x11
    unsigned char m_field13;  // +0x13
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_PARTY_TELEPORT_CONFIRM : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT_CONFIRM();
    unsigned char m_confirmFlag;  // +0x0d 队伍传送确认/应答标志【推断】
                                  //（Dispatcher_PartyTeleportConfirm/read.md get_byte@msg+0xd）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_PURIFY_ITEM : public MSG_BASE
{
public:
    MSG_PURIFY_ITEM();
    // ORIG C1：+0xd word、+0xf int、+0x13 word、+0x15 int 清零（非自然对齐 → pack）
    unsigned short m_0d;      // +0x0d
    int m_0f;                 // +0x0f
    unsigned short m_13;      // +0x13
    int m_15;                 // +0x15
    // pad removed
};
#pragma pack(pop)

class MSG_PVP_MISSION_COMBO_CLEAR : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x8250a88 域 msg 槽 -0x1b ⇒ 帧距 0xf，原 0xe 差 1。
    char m_pad[0x2];
};

#pragma pack(push, 1)
class MSG_PVP_MISSION_HP_PERCENT : public MSG_BASE
{
public:
    MSG_PVP_MISSION_HP_PERCENT();
    ~MSG_PVP_MISSION_HP_PERCENT();
    unsigned char m_0d;                    // +0x0d
    std::vector<short> m_hpPercents;       // +0x0e
    std::vector<int> m_characIdxs;         // +0x1a
};
#pragma pack(pop)

class MSG_PVP_TIMEOUT : public MSG_BASE
{
public:
    char m_pad[0x2a];         // +0xd..（b28d-solo-S153：ORIG 0x8258f28 帧距 sizeof=0x37
                              //   [m=-0x4b/p=-0x14]，原 0x29 少 1 字节）
};

class MSG_QUEST_SHOP : public MSG_BASE
{
public:
    MSG_QUEST_SHOP();
    // ORIG C1：+0xd/+0xe byte 清零
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    // pad removed
};

#pragma pack(push, 1)
class MSG_QUICK_CHAT : public MSG_BASE
{
public:
    MSG_QUICK_CHAT();
    // ORIG C1：+0xd int、+0x11 byte 清零；CQuickChattingOption 构造于 +0x12
    int m_0d;                      // +0x0d
    unsigned char m_11;            // +0x11
    bool isLastMessage();          // ORIG W 0x0822dc98（m_11==1）
    CQuickChattingOption m_quick;  // +0x12（sizeof 0x12c；总长 0x13e 真实布局）
};
#pragma pack(pop)

class MSG_QUICK_PARTY : public MSG_BASE
{
public:
    MSG_QUICK_PARTY();
    // ORIG C1：+0xe word 清零、+0x10 byte = 0xff（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    unsigned char m_10;       // +0x10（ctor = 0xff）
    // pad removed
};

#pragma pack(push, 1)
class MSG_REFUND_SKILL : public MSG_BASE
{
public:
    MSG_REFUND_SKILL();
    // ORIG C1：+0xd int 清零、+0x11..+0x13 byte 清零
    int m_0d;                 // +0x0d
    unsigned char m_11;       // +0x11
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_REGENERATION_RANDOM_OPTION : public MSG_BASE
{
public:
    // ORIG Dispather_RegenerationRandomOption::process：
    //   两装备槽 u16@0xd/0xf；两源选项序号 s16@0x11/0x13（Regenerate 第 3/4 参）
    unsigned short m_equipSlot0;  // +0x0d
    unsigned short m_equipSlot1;  // +0x0f
    short m_srcOptIndex0;         // +0x11
    short m_srcOptIndex1;         // +0x13
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_REGISTER_TO_BLACKLIST : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线）
    int m_nameLen;            // +0x10 角色名长度【推断】（Dispatcher_RegisterToBlackList/read.md get_int@msg+0x10）
    char m_characName[0x1e];  // +0x14 加入黑名单的目标角色名（get_str(buf,0x1e)@msg+0x14）
    char m_pad34[2];          // +0x32..+0x33（b28d-solo-S153：ORIG 0x8255ea0 帧距 sizeof=0x34
                              //   [m=-0x48/p=-0x14]）
};
static_assert(sizeof(MSG_REGISTER_TO_BLACKLIST) == 0x34, "MSG_REGISTER_TO_BLACKLIST size");
#pragma pack(pop)

class MSG_REMOVE_FRIEND : public MSG_BASE
{
public:
    char m_pad[0x22];         // +0xd..（b28d-solo-S153：ORIG 0x825760e 帧距 sizeof=0x2f
                              //   [m=-0x43/p=-0x14]，原 0x21 少 1 字节）
};

#pragma pack(push, 1)
class MSG_RENAME_CREATURE : public MSG_BASE
{
public:
    MSG_RENAME_CREATURE();
    // ORIG C1：memset(this+0x14,0,0xd)（即 m_name 区）
    signed char m_field0d;    // +0x0d【推断】（Dispatcher_RenameCreature/read.md get_byte@msg+0xd）
    short m_field0e;          // +0x0e【推断】（get_short@msg+0xe）
    int m_nameLen;            // +0x10 新名字字节长度（get_int@msg+0x10；get_str len 参数）
    char m_name[0xd];         // +0x14..+0x20 新生物名称（get_str(buf,0xd,len)@msg+0x14；ctor memset 全零）
    char m_pad21[3];          // +0x21..+0x23（b28d-solo-S153：ORIG 0x8256470 帧距 sizeof=0x24
                              //   [m=-0x38/p=-0x14]；ctor memset 仍 0xd 字节不变）
};
#pragma pack(pop)

class MSG_RENT_ITEM : public MSG_BASE
{
public:
    MSG_RENT_ITEM();
    // ORIG C1：+0xd/+0xe byte 清零、memset(this+0xf,0,2)
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    char m_0f[2];             // +0x0f..+0x10（ctor memset 全零）
    // pad removed
};

#pragma pack(push, 1)
class MSG_REPAIR_EQUIP : public MSG_BASE
{
public:
    unsigned char m_0d;     // +0x0d（invenType，传 Store::repair_equip char 参）
    short m_0e;             // +0x0e（slot，s16；成功后回显 param+0xa）
    unsigned short m_10;    // +0x10（count/耐久参，u16）
};
#pragma pack(pop)

class MSG_REQUEST_CHARAC_SKILL_INFO : public MSG_BASE
{
public:
    MSG_REQUEST_CHARAC_SKILL_INFO();
    // ORIG C1：+0xd byte = 0xff（无请求哨兵）
    unsigned char m_0d;       // +0x0d
    // pad removed
};

#pragma pack(push, 1)
class MSG_REQUEST_DISJOINT_ITEM : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    short m_field0e;          // +0x0e slot【推断】（Dispatcher_RequestDisjointItem/read.md get_short@msg+0xe）
    short m_field10;          // +0x10 第二 slot/idx【推断】（get_short@msg+0x10）
    signed char m_field12;    // +0x12 byte flag【推断】（get_byte@msg+0x12）
    // b28d-solo-S161：ORIG 0x82547d4 域 msg 槽 -0x20 ⇒ 帧距 0x14，原 0x13 差 1。
    char m_pad13;
};
static_assert(sizeof(MSG_REQUEST_DISJOINT_ITEM) == 0x14, "MSG_REQUEST_DISJOINT_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_REQUEST_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_REQUEST_GUILD_JOIN();
    // ORIG C1：+0xd int、+0x28 int 清零；memset(this+0x11,0,0x17)、memset(this+0x2c,0,0x1a)
    int m_0d;                 // +0x0d
    char m_11[0x17];          // +0x11..+0x27（ctor memset 全零）
    int m_28;                 // +0x28
    char m_2c[0x1a];          // +0x2c..+0x45（ctor memset 全零）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_REQUEST_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_REQUEST_INGAME_ADVERTISEMENT();
    short m_field0d;          // +0x0d 广告请求类型【推断】
                              //（Dispatcher_RequestIngameAdvertisement/read.md get_short@msg+0xd）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_REQUEST_MEMBER_ENTER : public MSG_BASE
{
public:
    char m_gap0d[3];          // +0x0d（ORIG 未出线）
    int m_nameLen;            // +0x10 角色名长度【推断】（Dispatcher_RequestMemberEnter/read.md get_int@msg+0x10）
    char m_characName[0x1e];  // +0x14 请求进入成员的角色名（get_str(buf,0x1e)@msg+0x14）
    char m_pad34[2];          // +0x32..+0x33（b28d-solo-S153：ORIG 0x8257420 帧距 sizeof=0x34
                              //   [m=-0x48/p=-0x14]）
};
static_assert(sizeof(MSG_REQUEST_MEMBER_ENTER) == 0x34, "MSG_REQUEST_MEMBER_ENTER size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_RESET_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_RESET_RANDOM_OPTION();
    // ORIG DispatcherResetRandomOption::process 与 C1 偏移一致：+0xd 为 gap
    char m_padD;
    unsigned short m_equipSlot;    // +0x0e（目标装备槽；GetInvenRef(1, slot)）
    unsigned short m_scrollSlot;   // +0x10（再铸卷轴槽；use_item 消耗）
    unsigned char m_rarityChoice;  // +0x12（玩家选择稀有度，须 <=2）
    char m_padTail[1];        // +0x13（帧推导总长 0x14）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_RIGHT_OF_CHANGE_GROW_TYPE : public MSG_BASE
{
public:
    MSG_RIGHT_OF_CHANGE_GROW_TYPE();
    // ORIG C1：+0xd word、+0xf byte 清零（packed：基类 0xd 非对齐长度，
    // 无 pack 时 m_0d 被对齐到 +0xe —— 与 ORIG 偏移不符）
    unsigned short m_0d;      // +0x0d
    unsigned char m_0f;       // +0x0f
    // pad removed
};
#pragma pack(pop)

class MSG_SAVE_CHARACTER_OPTION : public MSG_BASE
{
public:
    MSG_SAVE_CHARACTER_OPTION();
    // ORIG C1：CharacterOption 构造于 +0x11（CUserCharacInfo.h，ctor memset 0xff,0x200；0x200 字节）
    char m_padD[4];            // +0x0d..+0x10
    CharacterOption m_option;  // +0x11（sizeof 0x200）
};

#pragma pack(push, 1)
class MSG_SAVE_GAME_OPTION_1 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_1();
    // ORIG C1：CETCOption 构造于 +0x11（CGameOption.h，inline clear）
    unsigned int m_optionSize;  // +0x0d 选项二进制长度，read 校验 <0x49
                                //（Dispatcher_SaveGameOption_1/read.md get_int@msg+0xd；get_binary@msg+0x11）
    CETCOption m_option;        // +0x11（sizeof 0x48；binary 覆盖区与 CETCOption 缓冲等价）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SAVE_GAME_OPTION_2 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_2();
    // ORIG C1：CHotkeyOption 构造于 +0x11（CGameOption.h，inline clear；0xbc 字节）。
    // process 以 CHotkeyOption(0xbc) 语义 memcpy；read 校验 len<0xbd 后裸字节写入 m+0x11。
    unsigned int m_optionSize;  // +0x0d 选项二进制长度，read 校验 <0xbd【推断】
    CHotkeyOption m_option;     // +0x11（sizeof 0xbc）
};
#pragma pack(pop)
#pragma pack(push, 1)
class MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON : public MSG_BASE
{
public:
    // ORIG 嵌套类型（ctor/dtor 实证）：byte@0、byte@1、std::string@2，sizeof 0xa
    struct _slot_String
    {
        _slot_String();
        _slot_String(const _slot_String&);
        _slot_String& operator=(const _slot_String&);
        ~_slot_String();
        // ORIG 0x8239f16/0x8239e66：C1/op= 首 2 字节走一次 16 位字拷贝（movzwl/movw）
        // ⇒ 源为单一 unsigned short（两 byte 分立字段 O0 必出两次 byte 拷贝，形不符）。
        // 消费点仅经 reinterpret_cast<short*>（Dispatchers 21123/21154），同 2 字节无歧义。
        unsigned short m_slot;  // +0x00
        std::string m_str;      // +0x02
    } __attribute__((packed));

    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON();
    // ORIG C1：vector<_slot_String> 构造于 +0xf
    short m_count;                  // +0x0d emoticon slot 数量（Dispatcher_SaveGameOption_ChattingEmoticon/
                                    //  read.md get_short@msg+0xd，循环上界）
    std::vector<_slot_String> m_slots;   // +0xf

};
#pragma pack(pop)
#pragma pack(push, 1)
class MSG_SAVE_QUEST_NOTIFY : public MSG_BASE
{
public:
    MSG_SAVE_QUEST_NOTIFY();
    // ORIG C1：memset(this+0xd,0,0x28)
    char m_0d[0x28];          // +0x0d..+0x34（ctor memset 全零）
    // pad removed
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SCAN_DLL : public MSG_BASE
{
public:
    int m_field0d;            // +0x0d DLL 扫描结果码【推断】
                              //（Dispatcher_ScanBotByDll/read.md get_int@msg+0xd）
    signed char m_field11;    // +0x11 扫描标志【推断】（get_byte@msg+0x11）
    unsigned short m_field12; // +0x12 扫描计数【推断】（get_short@msg+0x12）
};
static_assert(sizeof(MSG_SCAN_DLL) == 0x14, "MSG_SCAN_DLL size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SCAN_DRV : public MSG_BASE
{
public:
    int m_field0d;            // +0x0d 驱动扫描结果码【推断】
                              //（Dispatcher_ScanBotByDrv/read.md get_int@msg+0xd）
    signed char m_field11;    // +0x11 扫描标志【推断】（get_byte@msg+0x11）
};
static_assert(sizeof(MSG_SCAN_DRV) == 0x12, "MSG_SCAN_DRV size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SCRIPT_HASH : public MSG_BASE
{
public:
    signed char m_field0d;    // +0x0d 脚本哈希校验类型1【推断】
                              //（Dispatcher_ValidateScriptHash/read.md get_byte@msg+0xd）
    signed char m_field0e;    // +0x0e 校验类型2【推断】（get_byte@msg+0xe）
    int m_field0f;            // +0x0f 脚本哈希值【推断】（get_int@msg+0xf）
};
static_assert(sizeof(MSG_SCRIPT_HASH) == 0x13, "MSG_SCRIPT_HASH size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SECRET_SHOP_BUY_ITEM : public MSG_BASE
{
public:
    int m_itemIdx;            // +0x0d 购买物品索引（Dispatcher_SecretShopBuyItem/read.md get_int@msg+0xd；
                              //  process 传 secretshop::CSecretShop::BuyItem）
    int m_count;              // +0x11 购买数量，read 校验 >0；非堆叠物品 BuyItem 强制为 1
                              //（get_int@msg+0x11；BuyItem.md is_stackable 分支）
};
static_assert(sizeof(MSG_SECRET_SHOP_BUY_ITEM) == 0x15, "MSG_SECRET_SHOP_BUY_ITEM size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SECRET_SHOP_OPEN_CLOSE : public MSG_BASE
{
public:
    unsigned char m_openFlag;  // +0x0d 购买开关（Dispatcher_SecretShopOpenClose/read.md get_byte@msg+0xd；
                               //  process 传 SECRET_SHOP_DATA::SetBuying(pos,bool)）
};
static_assert(sizeof(MSG_SECRET_SHOP_OPEN_CLOSE) == 0xe, "MSG_SECRET_SHOP_OPEN_CLOSE size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SECURITY_LOG_PACKET : public MSG_BASE
{
public:
    MSG_SECURITY_LOG_PACKET();
    int m_field0d;            // +0x0d 安全日志主标识【推断】
                              //（Dispatcher_SecurityLog/read.md get_int→局部→回写 msg+0xd）
    unsigned int m_logCounts[5];  // +0x11..+0x24 五个安全日志计数【推断】
                              //（read 循环 5 次 get_int @msg+0x11/0x15/0x19/0x1d/0x21）
    // pad removed
};
#pragma pack(pop)

class MSG_SECU_RPY : public MSG_BASE
{
public:
    char m_pad[0x1007];       // +0x0d..（ORIG 权威尺寸 0x1014：数据区 +0x0f..+0x100e、
                              //  长度 +0x100f、应答类型 +0x1013；dispatch_template 帧实证）
};

#pragma pack(push, 1)
class MSG_SELECT_DUNGEON : public MSG_BASE
{
public:
    // 布局证据：DisPatcher_SelectDungeon::process(0x081c8102)：
    //   +0x0d short 副本 idx、+0x15?? 无 —— 实际读取 +0xd word、
    //   +0xf/+0x10/+0x11 byte（dungeon_start/handleStartGame 参数与 party 标记）。
    short m_dungeonIdx;               // +0x0d
    signed char m_dungeonType;        // +0x0f
    signed char m_field10;            // +0x10
    unsigned char m_field11;          // +0x11（写入 CParty::m_partyType）
};
#pragma pack(pop)

class MSG_SELECT_STRIKER : public MSG_BASE
{
public:
    MSG_SELECT_STRIKER();
    // ORIG C1：+0xd byte = 0xff、+0xe byte 清零
    unsigned char m_0d;       // +0x0d（ctor = 0xff）
    unsigned char m_0e;       // +0x0e
    // pad removed
};

#pragma pack(push, 1)
class MSG_SERIAROOM_DECO_EVENT : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    unsigned short m_decoEventId;  // +0x0e 赛丽亚房装饰事件编号
                                   //（Dispatcher_SeriaRoom_Deco_Event/read.md get_short@msg+0xe）
};
static_assert(sizeof(MSG_SERIAROOM_DECO_EVENT) == 0x10, "MSG_SERIAROOM_DECO_EVENT size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SET_READY_STATE : public MSG_BASE
{
public:
    unsigned char m_0d;   // +0x0d（ready 标志，ORIG process 以 (m_0d!=0) 传入
                          //  PvP_Room::set_ready_state）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SET_USER_AREA : public MSG_BASE
{
public:
    signed char m_0d;     // +0x0d（vill）
    signed char m_0e;     // +0x0e（area）
    unsigned short m_0f;  // +0x0f（x）
    unsigned short m_11;  // +0x11（y）
    signed char m_13;     // +0x13（z，sbyte 符号扩展后传 move_area）
    unsigned short m_14;  // +0x14（prevVill）
    unsigned short m_16;  // +0x16（prevArea）
};
#pragma pack(pop)

class MSG_SKILL_COMMAND : public MSG_BASE
{
public:
    MSG_SKILL_COMMAND();
    char m_pad[0x191];
};

class MSG_SKILL_INIT : public MSG_BASE
{
public:
    MSG_SKILL_INIT();
    // ORIG C1：+0xd byte 清零
    char m_0d;                // +0x0d（ORIG get_byte(char&)）
    // pad removed
};

#pragma pack(push, 1)
class MSG_SOLO_TELEPORT : public MSG_BASE
{
public:
    MSG_SOLO_TELEPORT();
    // ORIG C1：memset(this+0xd,0,0x1e)；+0x2c/+0x30 int 清零；+0x34 = -1；
    // stPartyTelePort::reset() 于 +0x38（CPartyTelePort.h）；+0x44 int 清零
    char m_0d[0x1e];          // +0x0d..+0x2a（ctor memset 全零）
    char m_pad2b;             // +0x2b
    int m_2c;                 // +0x2c
    int m_30;                 // +0x30
    int m_34;                 // +0x34（ctor = -1）
    stPartyTelePort m_tp;     // +0x38（sizeof 现 0xa：ORIG set_party_teleport 0x08283d98 仅拷 10 字节）
    char m_pad42[2];          // +0x42（m_tp 收缩补位，保持 m_44 于已验证 +0x44）
    int m_44;                 // +0x44
};  // 总长 0x48 真实布局
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SORT_ITEM : public MSG_BASE
{
public:
    // 布局证据：DisPatcher_SortItem::process(0x081c9c54)：仅读 +0x0d byte。
    char m_invenType;                 // +0x0d（ORIG get_byte(char&)）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_STACKABLE_ACTION_USE : public MSG_BASE
{
public:
    MSG_STACKABLE_ACTION_USE();
    short m_0d;           // +0x0d
    unsigned char m_0f;   // +0x0f
    int m_10;             // +0x10
    unsigned char m_14;   // +0x14
    int m_15;             // +0x15
    int m_19;             // +0x19
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_TELEPORT : public MSG_BASE
{
public:
    char m_gap0d;             // +0x0d（ORIG 未出线，read 首读 +0xe）
    short m_field0e;          // +0x0e【推断】（Dispatcher_Teleport/read.md get_short@msg+0xe）
    unsigned int m_field10;   // +0x10【推断】（get_int@msg+0x10）
    signed char m_field14;    // +0x14【推断】（get_byte@msg+0x14）
    signed char m_field15;    // +0x15【推断】（get_byte@msg+0x15）
    // b28d-solo-S161：ORIG 0x8253e2e 域 msg 槽 -0x24 ⇒ 帧距 0x18，原 0x16 差 2。
    char m_pad16[2];
};
static_assert(sizeof(MSG_TELEPORT) == 0x18, "MSG_TELEPORT size");
#pragma pack(pop)

class MSG_TEMP_STRUCT : public MSG_BASE
{
public:
    MSG_TEMP_STRUCT();
    // pad removed
};

#pragma pack(push, 1)
class MSG_TITLEBOOK_GET : public MSG_BASE
{
public:
    int m_itemSpace;          // +0x0d ENUM_ITEMSPACE（Dispatcher_TitleBookGet/read.md get_int@msg+0xd；
                              //  process getTitle 第5参）
    int m_field11;            // +0x11 getTitle 第6参 int& 回填【推断】（get_int@msg+0x11）
    int m_field15;            // +0x15 getTitle 第4参【推断】（get_int@msg+0x15）
    int m_category;           // +0x19 ENUM_TITLE_BOOK_CATEGORY（get_int@msg+0x19；getTitle 第2参）
    int m_field1d;            // +0x1d getTitle 第3参【推断】（get_int@msg+0x1d）
};
static_assert(sizeof(MSG_TITLEBOOK_GET) == 0x21, "MSG_TITLEBOOK_GET size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_TITLEBOOK_OTHERUSER : public MSG_BASE
{
public:
    unsigned short m_targetIdx;  // +0x0d 目标用户/称号索引【推断】
                                 //（Dispatcher_TitleBookOtherUser/read.md get_short@msg+0xd）
};
static_assert(sizeof(MSG_TITLEBOOK_OTHERUSER) == 0xf, "MSG_TITLEBOOK_OTHERUSER size");
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_TITLEBOOK_PUT : public MSG_BASE
{
public:
    int m_itemSpace;          // +0x0d【推断】与 TITLEBOOK_GET 同构 5 int（Dispatcher_TitleBookPut/read.md）
    int m_field11;            // +0x11【推断】（get_int@msg+0x11）
    int m_field15;            // +0x15【推断】（get_int@msg+0x15）
    int m_field19;            // +0x19【推断】（get_int@msg+0x19；对应 GET 的 category 位）
    int m_field1d;            // +0x1d【推断】（get_int@msg+0x1d）
};
static_assert(sizeof(MSG_TITLEBOOK_PUT) == 0x21, "MSG_TITLEBOOK_PUT size");
#pragma pack(pop)

class MSG_UPDATE_CONTRACT_OF_CUBE_INFO : public MSG_BASE
{
public:
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO();
    // ORIG C1：+0xd/+0xe byte 清零；read（Dispatcher_UpdateContractOfCubeInfo/read.md）同偏移
    unsigned char m_useFlag;   // +0x0d 是否使用服务的布尔开关【推断】（read get_byte@msg+0xd；process setne）
    unsigned char m_selectIdx; // +0x0e 选择的服务索引【推断】（read get_byte@msg+0xe；
                               //  process 传 cUserHistoryLog::SelectCubeForPremiumService）
    // pad removed
};

#pragma pack(push, 1)
class MSG_ADD_SOCKET_TO_AVATAR
{
public:
    MSG_ADD_SOCKET_TO_AVATAR();
    // ORIG C1 W 0x0822dd2c（21B，无基类）：三字段清零（Dispatcher_AddSocketToAvatar::
    // dispatch_sig get_short/get_int 写入点；报告 Dispatcher_AddSocketToAvatar 全文）。
    char m_head[0xd];           // +0x00 包头区
    unsigned short m_slotA;     // +0x0d（avatar 槽，GetInvenSlot(2,·)）
    unsigned long m_addInfo;    // +0x0f（宝石 item addInfo）
    unsigned short m_slotB;     // +0x13（inventory 槽，GetInvenSlot(1,·)）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USERINFO_REQ : public MSG_BASE
{
public:
    unsigned short m_targetId;  // +0xd（GameWorld::get_user_info 目标 id）
    char m_slot;                // +0xf（ORIG read 用 get_byte(char&) 重载）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USER_HISTORY_LOG : public MSG_BASE
{
public:
    MSG_USER_HISTORY_LOG();
    // ORIG C1：+0xd byte、+0x10 int 清零、memset(this+0x14,0,0x100)
    unsigned char m_0d;       // +0x0d
    char m_pad0e[2];          // +0x0e..+0x0f
    int m_10;                 // +0x10
    char m_14[0x100];         // +0x14..+0x113（ctor memset 全零；总长 0x114 真实布局）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USER_SET_POS : public MSG_BASE
{
public:
    unsigned short m_0d;  // +0x0d（x）
    unsigned short m_0f;  // +0x0f（y）
    signed char m_11;     // +0x11（z，sbyte）
    unsigned short m_12;  // +0x12（dir，传 GameWorld::move_position 第 5 参）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USE_COIN : public MSG_BASE
{
public:
    unsigned short m_0d;    // +0x0d（UseCoin process 回显至 param+0x8；u16 @ msg+0x0d）
};
#pragma pack(pop)

class MSG_USE_DYE : public MSG_BASE
{
public:
    MSG_USE_DYE();
    // ORIG C1：+0xe/+0x10 word 清零（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    unsigned short m_10;      // +0x10
    // pad removed
};

#pragma pack(push, 1)
class MSG_USE_LIMIT_CUBE : public MSG_BASE
{
public:
    MSG_USE_LIMIT_CUBE();
    short m_slotA;            // +0x0d 限定魔方源槽位A（Dispatcher_UseLimitCube/read.md get_short@msg+0xd；
                              //  process GetInvenSlot(1, slotA)）
    unsigned int m_itemIdx;   // +0x0f 限定魔方物品索引（get_int@msg+0xf）
    short m_slotB;            // +0x13 目标槽位B（get_short@msg+0x13；GetInvenSlot(1, slotB)）
    // pad removed
};
#pragma pack(pop)

class MSG_USE_RENAME_CARD : public MSG_BASE
{
public:
    MSG_USE_RENAME_CARD();
    // ORIG C1：+0xe word 清零（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    // pad removed
};

#pragma pack(push, 1)
class MSG_USE_SKILL : public MSG_BASE
{
public:
    char m_gapD;              // +0x0d（ORIG read 首读 get_byte@+0xd，未消费）
    // ORIG DisPatcher_UseSkill::process：技能槽 u8@0xe；目标 u16@0xf（use_skill 第 3 参）
    unsigned char m_slot;        // +0x0e
    unsigned short m_targetId;   // +0x0f（SKILL_USING_VALIDATE_INFO 槽位 / CParty::use_skill 目标）
    char m_padTail[2];        // +0x11..+0x12（帧推导总长 0x13）
};
#pragma pack(pop)

class MSG_WALKOUT_PARTY : public MSG_BASE
{
public:
    char m_walkoutSlot;       // +0xd（包头后首字节：被踢成员槽位，walkout_member 第二参）
};

#pragma pack(push, 1)
class MSG_WARROOM_WP_PER_MONSTER : public MSG_BASE
{
public:
    int m_field0d;            // +0x0d HandleWpPerMonster 第2参【推断】
                              //（Dispatcher_WarRoomWpPerMonster/read.md get_int@msg+0xd）
    int m_field11;            // +0x11 第3参【推断】（get_int@msg+0x11）
    unsigned short m_killerUid;   // +0x15 击杀者 uid（get_short@msg+0x15；-1=无，否则
                                  //  GameWorld::find_from_world 找 CUser）
};
static_assert(sizeof(MSG_WARROOM_WP_PER_MONSTER) == 0x17, "MSG_WARROOM_WP_PER_MONSTER size");
#pragma pack(pop)

class MsgClearQuestIndex : public MSG_BASE
{
public:
    MsgClearQuestIndex();
    MsgClearQuestIndex& operator=(const MsgClearQuestIndex&);  // ORIG W 0x08235266
    char m_gap[3];                 // +0x0d..+0x0f（ORIG 拷贝赋值跳过区）
    int m_count;                   // +0x10（read 读入的请求数；Debug_Clear 响应回写实际清空数）
    std::vector<int> m_questIdxs;  // +0x14
};

class MsgLimitNpcBuyItem : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x824ba44 域 msg 槽 -0x21 ⇒ 帧距 0x15，原 0x14 差 1。
    char m_pad[0x8];
};

class MsgMercenaryCompetition : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x824d24a 域 msg 槽 -0x24 ⇒ 帧距 0x18，原 0x17 差 1。
    char m_pad[0xb];
};

class MsgMercenaryReturn : public MSG_BASE
{
public:
    // b28d-solo-S161：ORIG 0x824d69c 域 msg 槽 -0x20 ⇒ 帧距 0x14，原 0x13 差 1。
    char m_pad[0x7];
};

#pragma pack(push, 1)
#pragma pack(push, 1)
class MsgRequestAssaultPrice : public MSG_BASE
{
public:
    MsgRequestAssaultPrice();
    // ORIG C1：+0xd word = -1（未定价哨兵）
    short m_0d;               // +0x0d（ctor = -1）
    // pad removed
};
#pragma pack(pop)
#pragma pack(pop)

class MsgTournamentRewardSelect : public MSG_BASE
{
public:
    MsgTournamentRewardSelect();
    // ORIG C1：+0xd/+0xe byte = 0xff（未选择哨兵）
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    // pad removed
};

// ---------------------------------------------------------------------------
// Param 类型（占位大小 0x40，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
class AbnormalUseStackable : public ParamBase
{
public:
    AbnormalUseStackable();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
};
#pragma pack(pop)

class BuyItemParam : public ParamBase
{
public:
    BuyItemParam();
    Inven_Item m_item;          // +0x04（ORIG C1 调 Inven_Item C1）
    Inven_Item m_resultItem;    // +0x41
private:
    char m_gap7e[2];            // +0x7e 对齐填充
public:
    // ORIG C1 直接六连清零 +0x80..+0x94（ORIG 无独立 Mid 类型/符号 —— 平铺字段）
    unsigned int m_80;          // +0x80（b28c C28L19 代落：ORIG 0x081becdd 单步直落 callee-saved vs 同函数 +0xb0 int 三段，P211）
    int m_84;                   // +0x84
    int m_88;                   // +0x88
    unsigned int m_8c;          // +0x8c（b28d-Main 代落 P211档③：ORIG DisPatcher_BuyItem::send 0x081be20a `mov 0x8c(%eax),%eax; mov %eax,%edx` RHS-first+edx 中转需字段与 int 返回异型）
    int m_90;                   // +0x90
    int m_94;                   // +0x94
    std::pair<int, int> m_pair; // +0x98
    int m_a0;                   // +0xa0（ctor = -1）
    int m_a4;                   // +0xa4（ctor = -1）
    CItem* m_a8;                // +0xa8（get_data find_item 结果商城物品；ORIG 布局 int 位宽不变）
    CItem* m_ac;                // +0xac（材料 item idx 的 find_item 结果）
    int m_b0;                   // +0xb0
};

#pragma pack(push, 1)
class CheckSecurityProtection : public ParamBase
{
public:
    CheckSecurityProtection();
    // ORIG C1：+0x04 byte 清零（CSecu_ProtectionField::Check 首数据字节）
    unsigned char m_4;       // +0x04
};
#pragma pack(pop)

class InformNoticeData : public ParamBase
{
public:
    InformNoticeData();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class InformNoticeData2nd : public ParamBase
{
public:
    InformNoticeData2nd();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

#pragma pack(push, 1)
class LimitNpcBuyItem : public ParamBase
{
public:
    LimitNpcBuyItem();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad8[3];          // +0x08..+0x0a（b28d-solo-S153：ORIG 0x824ba44 帧距 param sizeof=0xb
                             //   [m=-0x21/p=-0x2c]；pack(1) 使 sizeof=0xb 不被 int 对齐撑到 0xc）
};
#pragma pack(pop)

class PcRoomPlayTimeRewardParam : public ParamBase
{
public:
    PcRoomPlayTimeRewardParam();
    int m_result;             // +0x04 事件结果码（Dispatcher_PcRoomPlayTimeReward/process.md
                              //  0x081e4fca/0x081e5016 param+4 写入；0xb2=无事件实体）
};
static_assert(sizeof(PcRoomPlayTimeRewardParam) == 0x8, "PcRoomPlayTimeRewardParam size");

class ReqDebugClearQuestResult : public ParamBase
{
public:
    ReqDebugClearQuestResult();
    MsgClearQuestIndex m_index;  // +0x04（ORIG C1 调 MsgClearQuestIndex C1）
};

class ReqResult : public ParamBase
{
public:
    ReqResult();
    unsigned char m_found;     // +0x04（Dispatcher_Req_Urgent_Quest：是否已随机选出发任务）
    char m_pad05[3];           // +0x05..+0x07
    int m_questIdx;            // +0x08（选中的紧急任务 idx / -1）
    int m_errorCode;           // +0x0c（1=等级或状态不符；4=无候选任务）
};

class STQuestShopBuyParam : public ParamBase
{
public:
    STQuestShopBuyParam();
    // ORIG C1：+0x04 byte、+0x06 word、+0x08 int 清零
    unsigned char m_4;       // +0x04
    short m_6;               // +0x06（自然对齐）
    int m_8;                 // +0x08
    // pad removed
};

class STQuestShopParam : public ParamBase
{
public:
    STQuestShopParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class STUdateItemParam : public ParamBase
{
public:
    STUdateItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class ScanBotByDllParam : public ParamBase
{
public:
    ScanBotByDllParam();
    // ORIG C1：+0x04 int、+0x08 byte、+0x0a word 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    char m_pad09;            // +0x09
    short m_a;               // +0x0a
};

class ScanBotByDrvParam : public ParamBase
{
public:
    ScanBotByDrvParam();
    // ORIG C1：+0x04 int、+0x08 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
};

class ScriptHashParam : public ParamBase
{
public:
    ScriptHashParam();
    // ORIG C1：+0x04/+0x05 byte、+0x08 int 清零
    unsigned char m_4;       // +0x04
    unsigned char m_5;       // +0x05
    int m_8;                 // +0x08（自然对齐推入）
};

#pragma pack(push, 1)
class SecuCardRpyParam : public ParamBase
{
public:
    SecuCardRpyParam();
    // ORIG C1：+0x04/+0x05 byte 清零
    unsigned char m_4;       // +0x04
    unsigned char m_5;       // +0x05
};
#pragma pack(pop)

class competition_reward_result
{
public:
    competition_reward_result() throw();  // b28c C28L32 代落：P216——ORIG SendMercenaryReturnResult::C1 0x0824d624 14 条无 landing，4.4 对成员构造布 cleanup+_Unwind_Resume（29 条）
    int m_0;
    int m_4;
    unsigned char m_8;
};

#pragma pack(push, 1)
class SendMercenaryCompetitionResult : public ParamBase
{
public:
    SendMercenaryCompetitionResult();
    unsigned char m_isMyCharac;  // +0x04 是否我方角色（Dispatcher_Mercenary_Competition/process.md
                                 //  0x081dddb5 mov %al,0x4(%edx)）
    unsigned int m_characNo;     // +0x08 竞技对象角色 No（msg+0x10 回写；0x081dddc1）
    unsigned char m_charac;      // +0x0c（msg[0x15] 回写；0x081dddce）【推断】
    unsigned char m_charac2;     // +0x0d（msg[0x14] 回写；0x081ddddb）【推断】
    char m_pad0e[5];             // +0x0e..+0x12（b28d-solo-S153：ORIG 0x824d24a 帧距
                                 //   param sizeof=0x10 [m=-0x24/p=-0x34]）
};
static_assert(sizeof(SendMercenaryCompetitionResult) == 0x10, "SendMercenaryCompetitionResult size");
#pragma pack(pop)

#pragma pack(push, 1)
class SendMercenaryReturnResult : public ParamBase
{
public:
    SendMercenaryReturnResult();
    // ORIG C1：competition_reward_result 出线构造于 +0x0c
    char m_pad04[8];                     // +0x04..+0x0b
    competition_reward_result m_result;  // +0x0c（类型定义已前移至本类上方）
};
#pragma pack(pop)

class TempParam : public ParamBase
{
public:
    TempParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class saveQuestNotifyParam : public ParamBase
{
public:
    saveQuestNotifyParam();
    int m_errorCode;   // +0x04（Dispatcher_SaveQuestNotify::process 写入 check_error 结果）
    // pad removed
};

// stAddFriendParam：ORIG _ZN16stAddFriendParamC1Ev 0x8230f64 —— ParamBase(vfptr@0)
// + 清零 int @+0x04，sizeof 8；RTTI _ZTI16stAddFriendParam 存在。
class stAddFriendParam : public ParamBase
{
public:
    stAddFriendParam();

    int m_field4;   // +0x04（ORIG ctor 清零）
};

class stAvatarDisjointRandomReward : public ParamBase
{
public:
    stAvatarDisjointRandomReward();
    int m_4;                                    // +0x04
    int m_8;                                    // +0x08
    std::vector<std::pair<int, int> > m_rewards; // +0x0c
};

class stBingoQuiz : public ParamBase
{
public:
    stBingoQuiz();
    int m_result;             // +0x04 结果码（Dispatcher_BingoQuiz/process.md 0x0825e9e6/0x0825eaa8）
    unsigned int m_bingoData; // +0x08 CUser::getBingoData() 首 dword（BingoData*）【推断】
                              //（process.md 0x0825eaba-0x0825eabf）
};
static_assert(sizeof(stBingoQuiz) == 0xc, "stBingoQuiz size");

class stBingoReward : public ParamBase
{
public:
    stBingoReward();
    int m_result;             // +0x04 check_error 结果码（Dispatcher_BingoReward/process.md 0x0825e6b4）
    unsigned int m_bingoData; // +0x08 CUser::getBingoData() 首 dword（BingoData*）【推断】
                              //（process.md 0x0825e6c8-0x0825e6cd）
};
static_assert(sizeof(stBingoReward) == 0xc, "stBingoReward size");

class stBlueMarble : public ParamBase
{
public:
    stBlueMarble();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stBlueMarbleConfirmInfo : public ParamBase
{
public:
    stBlueMarbleConfirmInfo();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

#pragma pack(push, 1)
class stBossDieCheckParam : public ParamBase
{
public:
    stBossDieCheckParam();
    // ORIG C1：+0x04 int、+0x08 byte、+0x0a word、+0x0c int 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    char m_pad09;            // +0x09
    short m_a;               // +0x0a
    int m_c;                 // +0x0c
};
#pragma pack(pop)

class stBreakAwayParam : public ParamBase
{
public:
    stBreakAwayParam();
    int m_resultCode;  // +0x04（process：突破检查未完成时写 1；send 读之回包）
    // pad removed
};

class stCallMemberListParam : public ParamBase
{
public:
    stCallMemberListParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stChangeAnotherSkillTree : public ParamBase
{
public:
    stChangeAnotherSkillTree();
    int m_result;             // +0x04 check_error 结果码
                              //（Dispatcher_ChangeAnotherSkillTree/process.md 0x081d1ff2/0x081d200b）
    unsigned char m_skillTreeIndex;  // +0x08 SetCurCharacSkillTreeIndex 参数回写 0/1（0x081d2062）
};

class stChangeAvatarOption : public ParamBase
{
public:
    stChangeAvatarOption();
    int m_result;             // +0x04 结果码（Dispatcher_AvatarOptionChange/process.md
                              //  0x081e07a1/0x081e08de；0x11=GetInvenRef 0、0x16=delete_item 失败）
    unsigned short m_srcSlot;     // +0x08 变更源 avatar 槽（msg+0xd 回写；0x081e09db）
    unsigned short m_dstSlot;     // +0x0a 目标槽（msg+0x13 回写；0x081e09e6）
    unsigned char m_optionValue;  // +0x0c 变更后 avatar option 字节（msg[0x19] 回写；0x081e09f4）
};

#pragma pack(push, 1)
class stChangeEmotionParam : public ParamBase
{
public:
    stChangeEmotionParam();
    // ORIG C1：+0x04 word 清零
    short m_4;               // +0x04
    char m_pad6[1];          // +0x06（b28d-solo-S153：ORIG 0x82532a4 帧距 param sizeof=0x7
                             //   [m=-0x1b/p=-0x22]）
};
#pragma pack(pop)

#pragma pack(push, 1)
class stChangeHostWarRoom : public ParamBase
{
public:
    stChangeHostWarRoom();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad8[2];          // +0x08..+0x09（b28d-solo-S153：ORIG 0x824fd96 帧距 param sizeof=0xa
                             //   [m=-0x1e/p=-0x28]）
};
#pragma pack(pop)

class stChangeLetterStatParam : public ParamBase
{
public:
    stChangeLetterStatParam();
    int m_result;             // +0x04 check_error/ReqChangeLetterStat 结果码
                              //（Dispatcher_ChangeLetterStat/process.md 0x081ce226/0x081ce29f）
};
static_assert(sizeof(stChangeLetterStatParam) == 0x8, "stChangeLetterStatParam size");
class stCharacSlotExtendEffect : public ParamBase
{
public:
    stCharacSlotExtendEffect();
    // ORIG C1：+0x04 int、+0x08/+0x09 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    unsigned char m_9;       // +0x09
};

#pragma pack(push, 1)
class stClientSpecStatisticParam : public ParamBase
{
public:
    stClientSpecStatisticParam();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_spec;    // +0x04 客户端 spec 类型字节（msg[0xd] 拷入；ORIG C1 清零；
                             //  Dispatcher_Client_Spec_Statistic/process.md 0x081ce724）
};
#pragma pack(pop)

class stCloseDisjointStoreParam : public ParamBase
{
public:
    stCloseDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stCollectItems : public ParamBase
{
public:
    stCollectItems();
    // ORIG C1：+0x04 int、+0x08/+0x0a word 清零
    int m_4;                 // +0x04
    short m_8;               // +0x08
    short m_a;               // +0x0a
};

class stComboSkill : public ParamBase
{
public:
    stComboSkill();
    // ORIG C1：+0x04 int 清零
    int m_result;            // +0x04 check_error 结果码（ORIG C1 清零；Dispatcher_ComboSkill/process.md
                             //  0x0825f326/0x0825f34a；combo map 在 MSG_COMBO_SKILL msg +0xe）
};
class stCreateCharac : public ParamBase
{
public:
    stCreateCharac();
    int m_errNo;         // +0x04（process 错误码；G2/G3 统一命名约定）
};

class stComboSkillExtensionQuickSlotReset : public ParamBase
{
public:
    stComboSkillExtensionQuickSlotReset();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stCreateDisjointStoreParam : public ParamBase
{
public:
    stCreateDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stDeleteCharac : public ParamBase
{
public:
    stDeleteCharac();
    int m_errNo;         // +0x04（process 错误码）
};

class stDeleteItemParam : public ParamBase
{
public:
    stDeleteItemParam();
    int m_4;            // +0x04
    unsigned char m_8;  // +0x08
    unsigned char m_9;  // +0x09
    short m_slotsA[100];   // +0x0a（ctor 循环清零）
    short m_slotsD2[100];  // +0xd2
    int m_counts19c[100];  // +0x19c
};

class stDeleteToBlackListParam : public ParamBase
{
public:
    stDeleteToBlackListParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

#pragma pack(push, 1)
class stDieCharacInDungeon : public ParamBase
{
public:
    stDieCharacInDungeon();
    int m_errNo;    // +0x04（ORIG process 写 check_error 结果/0x7fffffff）
    char m_pad8[3]; // +0x08..+0x0a（b28d-solo-S153：ORIG 0x8259f68 帧距 param sizeof=0xb
                    //   [m=-0x1d/p=-0x28]；若 check_tu 不 AE 则回退本行）
};
#pragma pack(pop)

class stDropItemParam : public ParamBase
{
public:
    stDropItemParam();
    int m_errNo;            // +0x04（check_error / drop_item>0 时的返回码）
    unsigned char m_8;      // +0x08（回显 MSG_DROP_ITEM m_11）
    int m_c;                // +0x0c（ORIG C1 清零）
};

class stEnchantByBeadParam : public ParamBase
{
public:
    stEnchantByBeadParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

#pragma pack(push, 1)
class stEnchantParam : public ParamBase
{
public:
    stEnchantParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    short m_pad8;            // +0x08..+0x09（b28d-solo-S153：ORIG 0x82523a0 帧距 param
                             //   sizeof=0xa [m=-0x26/p=-0x30]；short 令类对齐=2，
                             //   MSG_BASE 实例 param 起点取整到 -0x24）
} __attribute__((aligned(2)));   // b28d-solo-S153：ORIG 该类对齐 2（pack(1) 会压成 1）
#pragma pack(pop)

class stEnterDisjointStoreParam : public ParamBase
{
public:
    stEnterDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

#pragma pack(push, 1)
class stEquipslotSwitch : public ParamBase
{
public:
    stEquipslotSwitch();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_result;  // +0x04 CExpandEquipslot::EquipslotSwitch 返回 bool（ORIG C1 清零；
                             //  Dispatcher_Equipslot_Switch/process.md 0x081d62b7 mov %dl,0x4）
};
#pragma pack(pop)

class stExchangeRandomItemReward : public ParamBase
{
public:
    stExchangeRandomItemReward();
    int m_4;                                     // +0x04
    unsigned char m_8;                           // +0x08
    unsigned char m_9;                           // +0x09
    std::vector<std::pair<int, int> > m_rewards; // +0x0c
    std::vector<int> m_items;                    // +0x18
};

#pragma pack(push, 1)
class stExpandEquipslotFlagUpdate : public ParamBase
{
public:
    stExpandEquipslotFlagUpdate();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
};
#pragma pack(pop)

class stExpertExtraction : public ParamBase
{
public:
    stExpertExtraction();
    int m_4;               // +0x04
    short m_8;             // +0x08
    std::vector<int> m_list; // +0x0c
};

class stFatigueAttendance : public ParamBase
{
public:
    stFatigueAttendance();
    int m_result;            // +0x04 结果码（Dispatcher_FatigueAttendance/process.md 0x081db036 等）
    unsigned int m_recordType;   // +0x08 msg+0xd 拷入：ingame event history record type（0x081db014）
    unsigned int m_historyBits;  // +0x0c setBit 置位后 get_ingame_event_history 位图回写（0x081db244）
    int m_todayTime;             // +0x10 CEventScriptMng::get_today_time() 写入（0x081db265）
};

class stFinishLoadParam : public ParamBase
{
public:
    stFinishLoadParam();
    int m_errNo;          // +0x04（check_error 结果；<0 时 process 走 LineFunc 路径）
};

class stGetItemParam : public ParamBase
{
public:
    stGetItemParam();
    int m_errNo;            // +0x04（check_error 结果；0/0xd8/0x13/0x7fffffff 状态码）
};

class stGoldTakeIncreasingAmount : public ParamBase
{
public:
    stGoldTakeIncreasingAmount();
    // ORIG C1：+0x04 int、+0x08 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
};

class stGrowthCreature : public ParamBase
{
public:
    stGrowthCreature();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stGrowthEquipment : public ParamBase
{
public:
    stGrowthEquipment();
    int m_4;             // +0x04
    int m_8;             // +0x08
    int m_c;             // +0x0c
    short m_10;          // +0x10
    int m_14;            // +0x14
};

class stGrowthPowerParam : public ParamBase
{
public:
    stGrowthPowerParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stGuildBoardParam : public ParamBase
{
public:
    stGuildBoardParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stGuildCargoHistoryParam : public ParamBase
{
public:
    stGuildCargoHistoryParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stGuildCargoMoveItemParam : public ParamBase
{
public:
    stGuildCargoMoveItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stGuildCargoParam : public ParamBase
{
public:
    stGuildCargoParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stGuildCargoPopItemParam : public ParamBase
{
public:
    stGuildCargoPopItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stGuildCargoPushItemParam : public ParamBase
{
public:
    stGuildCargoPushItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stHatchCreature : public ParamBase
{
public:
    stHatchCreature();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stImageCommunicationEquipmentUse : public ParamBase
{
public:
    stImageCommunicationEquipmentUse();
    // ORIG C1：+0x04/+0x08/+0x0c int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    int m_c;                 // +0x0c
};

class stItemDictionary : public ParamBase
{
public:
    stItemDictionary();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stItemHyperLinkMessage : public ParamBase
{
public:
    stItemHyperLinkMessage();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stLeavePartyParam : public ParamBase
{
public:
    stLeavePartyParam();
    int m_errNo;              // +0x4（ORIG process 写入 check_error/业务结果码，send 消费）
};

class stLogIngameAdvertisement : public ParamBase
{
public:
    stLogIngameAdvertisement();
    int m_result;            // +0x04 check_error 结果码（Dispatcher_LogIngameAdvertisement/process.md
                             //  0x081e4e1d；msg+0xd..0x14 直接传 WriteLog 不回写 param）
    // pad removed
};

#pragma pack(push, 1)
class stMailboxExtractParam : public ParamBase
{
public:
    stMailboxExtractParam();
    // ORIG C1：+0x04/+0x08 int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    char m_padC[2];          // +0x0c..+0x0d（b28d-solo-S153：ORIG 0x8256668 帧距 param
                             //   sizeof=0xe [m=-0x22/p=-0x30]）
};
#pragma pack(pop)

class stMailboxOpenParam : public ParamBase
{
public:
    stMailboxOpenParam();
    // ORIG C1：+0x04/+0x08 int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
};

class stMailboxSendParam : public ParamBase
{
public:
    stMailboxSendParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stMemberEnterReplyParam : public ParamBase
{
public:
    stMemberEnterReplyParam();
    int m_4;                 // +0x04 ORIG C1：清零
};
class stMemberSecedeParam : public ParamBase
{
public:
    stMemberSecedeParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stMonstercardBind : public ParamBase
{
public:
    stMonstercardBind();
    int m_4;                 // +0x04
    short m_8;               // +0x08
    short m_a;               // +0x0a
    short m_c;               // +0x0c
    std::vector<int> m_list; // +0x10
};

class stMoveItemParam : public ParamBase
{
public:
    stMoveItemParam();
    int m_4;              // +0x04
    unsigned char m_8;    // +0x08
    short m_a;            // +0x0a
    int m_c;              // +0x0c
    unsigned char m_10;   // +0x10
    int m_14;             // +0x14
};

class stMoveMapParam : public ParamBase
{
public:
    stMoveMapParam();
    int m_errNo;          // +0x04（check_error 结果；<0 时 process 走 LineFunc 路径）
};

#pragma pack(push, 1)
class stObjectBringUp : public ParamBase
{
public:
    stObjectBringUp();
    int m_4;                  // +0x04
    short m_8;                // +0x08
    unsigned char m_a;        // +0x0a
    unsigned char m_b;        // +0x0b
    std::vector<int> m_items; // +0x0c
};
#pragma pack(pop)

class stOnBuySkillParam : public ParamBase
{
public:
    stOnBuySkillParam();
    unsigned char m_4;         // +0x04
    buySkillInfo m_infos[70];  // +0x08..+0x34f（ORIG 循环 70 次 buySkillInfo C1）
    int m_350;                 // +0x350（ctor = -1）
    unsigned char m_354;       // +0x354
    unsigned char m_355;       // +0x355
};

#pragma pack(push, 1)
class stOnChangeSkillParam : public ParamBase
{
public:
    stOnChangeSkillParam();
    // ORIG DisPatcher_ChangeSkill::process 写入（skill_move 前快照，send 回读）
    unsigned char m_curSlot;   // +0x04
    unsigned char m_newSlot;   // +0x05
};
#pragma pack(pop)

class stOnGetUserInfoParam : public ParamBase
{
public:
    stOnGetUserInfoParam();
    // ORIG C1：+0x04 word、+0x06 byte 清零
    short m_4;               // +0x04
    unsigned char m_6;       // +0x06
};

class stOnPvPTimeOut : public ParamBase
{
public:
    stOnPvPTimeOut();
    int m_timeOutResult;      // +0x4（ORIG PvPTimeout::process 写入 time_out 结果码，send 消费）
};

class stOnRecoverStaminaParam : public ParamBase
{
public:
    stOnRecoverStaminaParam();
    int m_errNo;              // +0x4（ORIG process 写入 serviceOfShusia 结果码，send 消费）
};
class stOnSetPvPReadyParam : public ParamBase
{
public:
    stOnSetPvPReadyParam();
    int m_errNo;          // +0x04（PvP_Room::set_ready_state 结果码；<0 时 start_pvp）
};

class stOnSortItemParam : public ParamBase
{
public:
    stOnSortItemParam();
    char m_invenType;                 // +0x04（回显 msg+0xd；ORIG movsbl 符号扩展）
    unsigned char m_gap5[3];          // +0x05..+0x07
    int m_result;                     // +0x08（sort 返回值；ctor 清零）
};

class stOnUseEquipParam : public ParamBase
{
public:
    stOnUseEquipParam();
    unsigned int m_errNo;   // +0x04（use_equip 返回码；P211 异型化触发转换中转=ORIG 0x081c0e4a 形，b28 BL12 代落）
    unsigned char m_8;      // +0x08（回显 MSG_DECREASE_DURABILITY m_0d）
};

class stOpenCeraPackage : public ParamBase
{
public:
    stOpenCeraPackage();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stPartyTeleport : public ParamBase
{
public:
    stPartyTeleport();
    char m_pad[0xc];
};

class stPartyTeleportConfirm : public ParamBase
{
public:
    stPartyTeleportConfirm();
    // ORIG：+0x04 int m_result、+0x08 unsigned char m_bConfirm（构造体见 PacketTypes.cpp）
    int m_result;              // +0x04
    unsigned char m_bConfirm;  // +0x08
};

class stQuickChatParam : public ParamBase
{
public:
    stQuickChatParam();
    // pad removed
};

class stQuickPartyParam : public ParamBase
{
public:
    stQuickPartyParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stRefundSkill : public ParamBase
{
public:
    stRefundSkill();
    char m_pad[0x18];
};

class stRegenROIResult : public ParamBase
{
public:
    stRegenROIResult();
    // ORIG Dispather_RegenerationRandomOption::process 写入、send 消费：
    int m_errNo;          // +0x04（0=成功；否则 SendCmdErrorPacket/use_money 错码）
    short m_insertedSlot; // +0x08（insertItemIntoInventory 返回槽位，>=0 时有效）
};

class stRegisterToBlackListParam : public ParamBase
{
public:
    stRegisterToBlackListParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stRemoveFriendParam : public ParamBase
{
public:
    stRemoveFriendParam();
    int m_errNo;    // +0x04（ORIG process 写 check_error 结果）
};

class stRenameCreature : public ParamBase
{
public:
    stRenameCreature();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stRentItemParam : public ParamBase
{
public:
    stRentItemParam();
    int m_4;                                  // +0x04
    InstanceRentalSystem::RentOutput m_output; // +0x08
};

class stRepairDisjointMachineParam : public ParamBase
{
public:
    stRepairDisjointMachineParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stRepairEquipParam : public ParamBase
{
public:
    stRepairEquipParam();
    int m_errNo;                // +0x04（repair_equip 返回码）
    unsigned char m_8;          // +0x08（成功时回显 MSG_REPAIR_EQUIP m_0d）
    unsigned char m_pad9;       // +0x09
    unsigned short m_a;         // +0x0a（成功时回显 MSG_REPAIR_EQUIP m_0e 原始位型）
    unsigned short m_c;         // +0x0c（repair_equip 出参 ushort&，耐久）
};

class stRequestAssaultPrice : public ParamBase
{
public:
    stRequestAssaultPrice();
    int m_4;                  // +0x04（check_error 结果；ORIG 实证）
    int m_8;                  // +0x08（突袭费用；ORIG 实证）
    // pad removed
};

class stRequestBlackListParam : public ParamBase
{
public:
    stRequestBlackListParam();
    int m_4;                 // +0x04 ORIG C1：清零
};
class stRequestCharacSkillInfo : public ParamBase
{
public:
    stRequestCharacSkillInfo();
    int m_4;             // +0x04 错误码（send 读低字节）
    // pad removed
};

#pragma pack(push, 1)
class stRequestDisjointItemParam : public ParamBase
{
public:
    stRequestDisjointItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};
#pragma pack(pop)

class stRequestIngameAdvertisement : public ParamBase
{
public:
    stRequestIngameAdvertisement();
    // ORIG 实证字段（Dispatcher_RequestIngameAdvertisement process/read.md）：
    // +0x04 起内嵌 stNotifyIngameADInfo 相关字段；总长 0x648。
    unsigned int m_code;          // +0x04（WriteLog 第 5 参 uint / send 首字段）
    char m_pad08[0x230];          // +0x08 .. +0x237
    unsigned long m_itemID;       // +0x238（奖品 idx A）
    char m_pad23c[0xff];          // +0x23c .. +0x33a
    unsigned char m_adShape;      // +0x33b 广告形态
    unsigned char m_logFlag;      // +0x33c 日志开关
    unsigned char m_prizeShape;   // +0x33d 奖品形态
    char m_pad33e[0x1fe];         // +0x33e .. +0x53b
    unsigned long m_itemID2;      // +0x53c（奖品 idx B）
    char m_pad540[0x100];         // +0x540 .. +0x63f
    int m_errorCode;              // +0x640
    short m_adType;               // +0x644
    char m_pad646[2];             // +0x646 .. +0x647（总长 0x648）
};

class stRequestMemberEnterParam : public ParamBase
{
public:
    stRequestMemberEnterParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stRightOfChangeGrowType : public ParamBase
{
public:
    stRightOfChangeGrowType();
    char m_pad[0x4];
};

class stSaveCharacOption : public ParamBase
{
public:
    stSaveCharacOption();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stSecretShopBuyItemParam : public ParamBase
{
public:
    stSecretShopBuyItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stSecretShopOpenCloseParam : public ParamBase
{
public:
    stSecretShopOpenCloseParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stSecurityLogParam : public ParamBase
{
public:
    stSecurityLogParam();
    // pad removed
};

class stSelectCharac : public ParamBase
{
public:
    stSelectCharac();
    int m_errNo;         // +0x04（process 错误码）
};

class stSelectDungeonParam : public ParamBase
{
public:
    stSelectDungeonParam();
    int m_4;    // +0x04
    int m_8;    // +0x08
    int m_c;    // +0x0c
    int m_10;   // +0x10（ctor = 2）
    int m_14;   // +0x14
};

class stSelectStriker : public ParamBase
{
public:
    stSelectStriker();
    // b28d-solo-S161：ORIG dispatch_template 0x824a3d8 域 param 槽距 = 9
    // （msg@-0x1b / param@-0x24 ⇒ param 尾 -0x1c 紧贴 msg）⇒ 4(pad)+5=9。
    char m_pad[0x5];
};

class stSendClearedMission : public ParamBase
{
public:
    stSendClearedMission();
    std::vector<int> m_missionIdxs;  // +0x04
};

class stSeriaRoomDecoEvent : public ParamBase
{
public:
    stSeriaRoomDecoEvent();
    // ORIG C1：+0x04 int、+0x08/+0x0a word 清零
    int m_4;                 // +0x04
    short m_8;               // +0x08
    short m_a;               // +0x0a
};

class stSetAreaParam : public ParamBase
{
public:
    stSetAreaParam();
    int m_errNo;          // +0x04（三参 check_error 结果；>=1 直接返回 0）
    signed char m_8;      // +0x08（ORIG SetArea::check_error 0x081c93e8 +0x44 movzbl 0xd,Q;mov %dl,0x8 单步直存需 LHS/RHS 全等 schar=schar；DCoreC8 probe）
    signed char m_9;      // +0x09（同上）
};

class stSetPartyInfo : public ParamBase
{
public:
    stSetPartyInfo();
    int m_errNo;              // +0x4（ORIG process 写入 check_error/业务结果码，send 消费）
};

class stSkillCommandParam : public ParamBase
{
public:
    stSkillCommandParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stSkillInit : public ParamBase
{
public:
    stSkillInit();
    // ORIG C1：+0x04 int、+0x08 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
};

class stSoloTeleportParam : public ParamBase
{
public:
    stSoloTeleportParam();
    // ORIG C1：+0x04/+0x08/+0x0c int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    int m_c;                 // +0x0c
    // pad removed
};

class stStackableActionUse : public ParamBase
{
public:
    stStackableActionUse();
    // b28d-solo-S161：ORIG 0x826033e 域 param 槽距 = 0xf（msg@-0x29 / param@-0x38）
    // ⇒ 4(pad)+0xb=0xf（原 0x7 得 0xb，差 4）。
    char m_pad[0xb];
};

class stStartGameParam : public ParamBase
{
public:
    stStartGameParam();
    int m_errNo;                      // +0x04（check_error 结果；ORIG process 写入）
    int m_8;                          // +0x08（ORIG C1 清零）
    int m_c;                          // +0x0c（ORIG C1 = 2）
};

#pragma pack(push, 1)
class stTeleportParam : public ParamBase
{
public:
    stTeleportParam();
    int m_4;              // +0x04
    short m_8;            // +0x08
    char m_gapA[6];       // +0x0a..+0x0f（ORIG 未初始化区）
    unsigned char m_10;   // +0x10
    unsigned char m_11;   // +0x11
    // [b28d-Main 代落·S162 真语义 bug] ORIG ctor 0x08231362 写 +0x14 int、
    // Dispatcher_Teleport::send 0x081d09b3 读 +0x14、process 0x081d0723 写 +0x14
    // （+0x12..+0x13 不写）⇒ pack(1) 下 int 须前置 2 字节 pad 才落 +0x14；
    // 原 m_14 落 +0x12（读/写错位）。sizeof 仍 0x18（0x14+4，尾部 pad 删除）。
    char m_pad12[2];      // +0x12..+0x13（ORIG 未初始化区）
    int m_14;             // +0x14
};
#pragma pack(pop)

class stTournamentRewardSelect : public ParamBase
{
public:
    stTournamentRewardSelect();
    // b28d-solo-S161：ORIG 0x822e0aa 域 param 槽距 = 9（msg@-0x1b / param@-0x24）
    // ⇒ 4(pad)+5=9（原 0x4 得 8，差 1）。
    char m_pad[0x5];
};

class stTournamentRewardSelectState : public ParamBase
{
public:
    stTournamentRewardSelectState();
    char m_pad[0x7];   // b28d-solo-S153：ORIG 0x824c162 帧距 param sizeof=0xb [m=-0x19/p=-0x24]；
                       // 若 check_tu 不 AE 则回退为 0x3
};

class stUpdateContractOfCubeInfo : public ParamBase
{
public:
    stUpdateContractOfCubeInfo();
    // ORIG C1：+0x04 int、+0x08/+0x09 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    unsigned char m_9;       // +0x09
};

class stUpgradeDisjointMachineParam : public ParamBase
{
public:
    stUpgradeDisjointMachineParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

class stUseCoinParam : public ParamBase
{
public:
    stUseCoinParam();
    int m_errNo;            // +0x04（check_error 结果）
    unsigned short m_8;     // +0x08（回显 MSG_USE_COIN m_0d）
};

class stUseDye : public ParamBase
{
public:
    stUseDye();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stUseLimitCube : public ParamBase
{
public:
    stUseLimitCube();
    // ORIG C1：+0x04 byte、+0x08/+0x0c int、+0x10 word 清零
    unsigned char m_4;       // +0x04
    char m_pad05[3];         // +0x05..+0x07
    int m_8;                 // +0x08
    int m_c;                 // +0x0c
    short m_10;              // +0x10
};

class stUseRenameCard : public ParamBase
{
public:
    stUseRenameCard();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    // pad removed
};

class stUseSkillParam : public ParamBase
{
public:
    stUseSkillParam();
    // ORIG DisPatcher_UseSkill::process 写入 check_error 结果（send 消费）：
    int m_errNo;        // +0x04（0=可施放；0x7fffffff=状态非 5）
};

class stWalkoutPartyParam : public ParamBase
{
public:
    stWalkoutPartyParam();
    int m_errNo;              // +0x4（ORIG process 写入 walkout_member 结果码，send 消费）
};

class stWarRoomWp : public ParamBase
{
public:
    stWarRoomWp();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
};

#endif  // DNF_GAME_PACKET_TYPES_H_
