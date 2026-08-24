#ifndef DNF_GAME_PACKET_TYPES_H_
#define DNF_GAME_PACKET_TYPES_H_

// ============================================================================
// df_game_r 还原 —— MSG / Param 类型定义（批量生成）
// 各类型成员布局待细化；当前用 char 数组占位以生成符号。
// 所有 MSG 继承 MSG_BASE，所有 Param 继承 ParamBase。
// ============================================================================

#include "PacketDispatcher.h"
#include <vector>


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
class MSG_ABNORMAL_USE_STACKABLE : public MSG_BASE
{
public:
    MSG_ABNORMAL_USE_STACKABLE();
    char m_pad[0x100];
};

class MSG_ACHIEVEMENT_TRIGGER : public MSG_BASE
{
public:
    MSG_ACHIEVEMENT_TRIGGER();
    char m_pad[0x100];
};

class MSG_ADD_FRIEND : public MSG_BASE
{
public:
    MSG_ADD_FRIEND();
    char m_pad[0x100];
};

class MSG_APPROVE_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_APPROVE_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_AVATAR_DISJOINT_RANDOM_REWARD : public MSG_BASE
{
public:
    MSG_AVATAR_DISJOINT_RANDOM_REWARD();
    char m_pad[0x100];
};

class MSG_BINGO_QUIZ : public MSG_BASE
{
public:
    MSG_BINGO_QUIZ();
    char m_pad[0x100];
};

class MSG_BLUE_MARBLE : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE();
    char m_pad[0x100];
};

class MSG_BLUE_MARBLE_CONFIRM_INFO : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE_CONFIRM_INFO();
    char m_pad[0x100];
};

class MSG_BOSS_DIE_CHECK : public MSG_BASE
{
public:
    MSG_BOSS_DIE_CHECK();
    char m_pad[0x100];
};

class MSG_BREAK_AWAY : public MSG_BASE
{
public:
    MSG_BREAK_AWAY();
    unsigned char m_checkFlag;  // +0x0d（read get_byte 写入；0=走重置/通知分支）
    char m_pad0e[0xff];
};

#pragma pack(push, 1)
class MSG_BUY_ITEM : public MSG_BASE
{
public:
    MSG_BUY_ITEM();
    // ORIG DisPatcher_BuyItem（process/get_data/check_error 逐字节）：
    int m_itemIdx;      // +0x0d（购买目标 item idx；get_data find_item 输入）
    int m_buyCount;     // +0x11（购买数量；>1 时 get_nMaxBuyableCount 校验）
    int m_deliveryType; // +0x15（eventDeliveryBuyItem 首参回传）
    char m_pad19[0xf4]; // +0x19..（总大小保持 0x10d 含 vptr 前基类 0xd）
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
    char m_pad[44];                      // +0xe1..（总大小保持占位版 0x10d）
};
#pragma pack(pop)

class MSG_CANCEL_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_CANCEL_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_CHANGE_ANOTHER_SKILL_TREE : public MSG_BASE
{
public:
    MSG_CHANGE_ANOTHER_SKILL_TREE();
    char m_pad[0x100];
};

class MSG_CHANGE_AVATAR_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_AVATAR_OPTION();
    char m_pad[0x100];
};

class MSG_CHANGE_EMOTION : public MSG_BASE
{
public:
    MSG_CHANGE_EMOTION();
    char m_pad[0x100];
};

class MSG_CHANGE_HOST_WARROOM : public MSG_BASE
{
public:
    MSG_CHANGE_HOST_WARROOM();
    char m_pad[0x100];
};

class MSG_CHANGE_LETTER_STAT : public MSG_BASE
{
public:
    MSG_CHANGE_LETTER_STAT();
    char m_pad[0x100];
};

class MSG_CHANGE_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_RANDOM_OPTION();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_CHANGE_SKILLSLOT : public MSG_BASE
{
public:
    MSG_CHANGE_SKILLSLOT();
    // ORIG DisPatcher_ChangeSkill::process：param+4/5 ← msg[0xe]/msg[0xf]
    unsigned char m_curSlot;   // +0x0e（原技能槽）
    unsigned char m_newSlot;   // +0x0f（目标技能槽）
    char m_pad10[0xf9];        // +0x10..（总大小保持 0x10d）
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
    char m_pad13[0xed];      // +0x13 → 负载总 0x100
};
#pragma pack(pop)

class MSG_CHECK_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_CHECK_GUILD_JOIN();
    char m_pad[0x100];
};

class MSG_CLIENT_SPEC_STATISTIC : public MSG_BASE
{
public:
    MSG_CLIENT_SPEC_STATISTIC();
    char m_pad[0x100];
};

class MSG_COLLECT_ITEMS : public MSG_BASE
{
public:
    char m_pad[0x100];
};

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
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_COMPATIBILITY_INDEX : public MSG_BASE
{
public:
    MSG_COMPATIBILITY_INDEX();
    unsigned char m_0d;   // +0x0d
    unsigned char m_0e;   // +0x0e
    int m_10;             // +0x10
    short m_14;           // +0x14
    short m_16;           // +0x16
    short m_18;           // +0x18
    short m_1a;           // +0x1a
    unsigned char m_1c;   // +0x1c
    unsigned char m_1d;   // +0x1d
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CREATE_CHARACTER : public MSG_BASE
{
public:
    MSG_CREATE_CHARACTER();
    unsigned char m_slot;    // +0xd
    char m_name[0x1e];       // +0x12..0x2f（ORIG strncpy 长 0x1e；末字节即 +0x2f 标志位）
    char m_pad30[0xd0];      // +0x30 → 负载总 0x100
};
#pragma pack(pop)

class MSG_CREATE_DISJOINT_STORE : public MSG_BASE
{
public:
    MSG_CREATE_DISJOINT_STORE();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_DECREASE_DURABILITY : public MSG_BASE
{
public:
    MSG_DECREASE_DURABILITY();
    unsigned char m_0d;     // +0x0d（UseEquip process：use_equip 槽位，回显至 param+0x8）
    char m_pad[0xff];
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_CHARACTER : public MSG_BASE
{
public:
    MSG_DELETE_CHARACTER();
    signed char m_slot;      // +0xd
    char m_padE[0xf3];       // +0xe → 负载总 0x100
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_ITEM : public MSG_BASE
{
public:
    MSG_DELETE_ITEM();
    // 布局证据：DisPatcher_DeleteItem::process(0x081c1dda)/exercise_mode/
    // restricted_check/condition_check 对 msg 的全部读取（i<count<=100）：
    //   +0x0d byte itemSpace、+0x0e sbyte count、+0x0f short opType[i]、
    //   +0xd7 short position[i]、+0x19f int itemIdx[i]、+0x32f int countArr[i]。
    // 尾部无保留区：ORIG 线上格式即止于 countArr[99]（sizeof=0x4bf）。
    unsigned char m_itemSpace;        // +0x0d
    signed char m_count;              // +0x0e
    short m_opType[100];              // +0x0f（==1 用途删除；2/3/4 方块统计）
    short m_position[100];            // +0xd7（槽位）
    int m_itemIdx[100];               // +0x19f
    int m_countArr[100];              // +0x32f
};
#pragma pack(pop)

class MSG_DELETE_TO_BLACKLIST : public MSG_BASE
{
public:
    MSG_DELETE_TO_BLACKLIST();
    char m_pad[0x100];
};

class MSG_DENY_JOIN_GUILD : public MSG_BASE
{
public:
    MSG_DENY_JOIN_GUILD();
    char m_pad[0x100];
};

class MSG_DIE_CHARACTER : public MSG_BASE
{
public:
    MSG_DIE_CHARACTER();
    char m_pad[0x100];
};

class MSG_DONATE_GUILD_FUND : public MSG_BASE
{
public:
    MSG_DONATE_GUILD_FUND();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_DROP_ITEM : public MSG_BASE
{
public:
    MSG_DROP_ITEM();
    short m_0d;             // +0x0d（drop_item itemIdx，s16）
    short m_0f;             // +0x0f（drop count，s16）
    unsigned char m_11;     // +0x11（inven space，回显至 param+0x8）
    short m_12;             // +0x12（s16）
    int m_14;               // +0x14
    char m_pad[0xf5];
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_ENCHANT : public MSG_BASE
{
public:
    MSG_ENCHANT();
    short m_0d;           // +0x0d
    int m_0f;             // +0x0f
    short m_13;           // +0x13
    int m_15;             // +0x15
    unsigned char m_19;   // +0x19
};
#pragma pack(pop)

class MSG_ENCHANT_BY_BEAD : public MSG_BASE
{
public:
    MSG_ENCHANT_BY_BEAD();
    char m_pad[0x100];
};

class MSG_ENTER_DISJOINT_STORE : public MSG_BASE
{
public:
    MSG_ENTER_DISJOINT_STORE();
    char m_pad[0x100];
};

class MSG_EQUIPSLOT_SWITCH : public MSG_BASE
{
public:
    MSG_EQUIPSLOT_SWITCH();
    char m_pad[0x100];
};

class MSG_EXCHANGE_RANDOM_ITEM_REWARD : public MSG_BASE
{
public:
    MSG_EXCHANGE_RANDOM_ITEM_REWARD();
    char m_pad[0x100];
};

class MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE : public MSG_BASE
{
public:
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE();
    char m_pad[0x100];
};

class MSG_EXPERT_EXTRACTION : public MSG_BASE
{
public:
    MSG_EXPERT_EXTRACTION();
    char m_pad[0x100];
};

class MSG_FATIGUE_ATTENDANCE : public MSG_BASE
{
public:
    MSG_FATIGUE_ATTENDANCE();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_FINISH_LOADING : public MSG_BASE
{
public:
    MSG_FINISH_LOADING();
    int m_0d;             // +0x0d（客户端 inven checksum，ORIG process 与
                          //  CUser::getCurCharacInvenCheckSum 比对，不等记 0x25b）
    int m_11;             // +0x11（skill checksum，与 getCurCharacSkillCheckSum 比对）
    char m_pad[0xf8];     // +0x15..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

class MSG_FLOAT_RDATA_MODULATION : public MSG_BASE
{
public:
    MSG_FLOAT_RDATA_MODULATION();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_GET_ITEM : public MSG_BASE
{
public:
    MSG_GET_ITEM();
    unsigned short m_0d;    // +0x0d（item serial；GetItemSerialChecker 第三参）
    unsigned char m_0f;     // +0x0f（==1 布尔标志，传 CParty::get_item/handlePickupItem）
    unsigned char m_10;     // +0x10（槽位/pos，sbyte 传 get_item 与 0x56 广播分支判定 2/3/4）
    unsigned short m_11;    // +0x11
    unsigned short m_13;    // +0x13
    unsigned short m_15;    // +0x15（%7 反外挂校验）
    unsigned char m_17;     // +0x17（!=1 时计 hack 0x3bb）
    unsigned char m_pad18[2];   // +0x18..+0x19
    unsigned short m_1a;    // +0x1a（拾取 x）
    unsigned short m_1c;    // +0x1c（拾取 y）
    char m_pad[0xef];
};
#pragma pack(pop)

class MSG_GOLD_TAKE_INCREASING_AMOUNT : public MSG_BASE
{
public:
    MSG_GOLD_TAKE_INCREASING_AMOUNT();
    char m_pad[0x100];
};

class MSG_GROWTH_EQUIPMENT : public MSG_BASE
{
public:
    MSG_GROWTH_EQUIPMENT();
    char m_pad[0x100];
};

class MSG_GROWTH_POWER : public MSG_BASE
{
public:
    MSG_GROWTH_POWER();
    char m_pad[0x100];
};

class MSG_GUILD_ATTENDANCE : public MSG_BASE
{
public:
    MSG_GUILD_ATTENDANCE();
    char m_pad[0x100];
};

class MSG_GUILD_BOARD : public MSG_BASE
{
public:
    MSG_GUILD_BOARD();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_MOVE_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_MOVE_ITEM();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_POP_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_POP_ITEM();
    char m_pad[0x100];
};

class MSG_GUILD_CARGO_PUSH_ITEM : public MSG_BASE
{
public:
    MSG_GUILD_CARGO_PUSH_ITEM();
    char m_pad[0x100];
};

class MSG_HATCH_CREATURE : public MSG_BASE
{
public:
    MSG_HATCH_CREATURE();
    char m_pad[0x100];
};

class MSG_INFORM_NOTICE : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE();
    char m_pad[0x100];
};

class MSG_INFORM_NOTICE_2ND : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE_2ND();
    char m_pad[0x100];
};

class MSG_ITEM_DICTIONARY : public MSG_BASE
{
public:
    MSG_ITEM_DICTIONARY();
    char m_pad[0x100];
};

class MSG_ITEM_HYPER_LINK_MESSAGE : public MSG_BASE
{
public:
    MSG_ITEM_HYPER_LINK_MESSAGE();
    char m_pad[0x100];
};

class MSG_ITEM_UPGRADE : public MSG_BASE
{
public:
    MSG_ITEM_UPGRADE();
    char m_pad[0x100];
};

class MSG_LOG_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_LOG_INGAME_ADVERTISEMENT();
    char m_pad[0x100];
};

class MSG_MAILBOX_EXTRACT_ITEM : public MSG_BASE
{
public:
    MSG_MAILBOX_EXTRACT_ITEM();
    char m_pad[0x100];
};

class MSG_MAILBOX_SEND : public MSG_BASE
{
public:
    MSG_MAILBOX_SEND();
    char m_pad[0x100];
};

class MSG_MEMBER_ENTER_REPLY : public MSG_BASE
{
public:
    MSG_MEMBER_ENTER_REPLY();
    char m_pad[0x100];
};

class MSG_MEMBER_SECEDE : public MSG_BASE
{
public:
    MSG_MEMBER_SECEDE();
    char m_pad[0x100];
};

class MSG_MONSTERCARD_BIND : public MSG_BASE
{
public:
    MSG_MONSTERCARD_BIND();
    char m_pad[0x100];
};

class MSG_MOTION_HACK : public MSG_BASE
{
public:
    MSG_MOTION_HACK();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_MOVE_ITEMSPACE : public MSG_BASE
{
public:
    MSG_MOVE_ITEMSPACE();
    // 布局证据：DisPatcher_MoveItem::process(0x081c5904) 对 msg 的全部读取：
    //   +0x0d byte 源 space、+0x0e short 源槽位、+0x10/+0x14 双 u32 组成
    //   exchange_interspace 的 ulong 数量实参（低/高）、+0x18 byte 目的 space、
    //   +0x19 short 目的槽位、+0x1b u32 itemIdx、+0x23/+0x24 byte expand 参数。
    unsigned char m_fromSpace;        // +0x0d
    short m_fromPos;                  // +0x0e
    unsigned int m_count;             // +0x10（ulong 实参低 32 位）
    unsigned int m_field14;           // +0x14（ulong 实参高 32 位）
    unsigned char m_toSpace;          // +0x18
    short m_toPos;                    // +0x19
    unsigned int m_itemIdx;           // +0x1b
    unsigned char m_pad1f[4];         // +0x1f..+0x22（ORIG 未读取区）
    unsigned char m_expanda;          // +0x23
    unsigned char m_expandb;          // +0x24
    char m_padTail[0xe8];             // +0x25..+0x10c（保持原 sizeof 0x10d）
};   // MSG_MOVE_ITEMSPACE
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MOVE_MAP : public MSG_BASE
{
public:
    MSG_MOVE_MAP();
    char m_pad_d[0xd];            // +0x0d..+0x1a（前段字段未还原，保持偏移）
    unsigned char m_1a;           // +0x1a（ORIG process 校验 ==1 跳过 0x25a hack 记数）
    unsigned short m_1b;          // +0x1b（mapIndex：CBossTowerFunc::MoveMap 以
                                  //  (ushort*)(msg+0x1b) 传入）
    char m_pad1d[6];              // +0x1d..+0x23
    unsigned int m_23[4];         // +0x23..+0x33（每成员 playtime tick，
                                  //  CheckOneMapPlayTimeHackCnt 逐槽修正/上报）
    char m_pad33[6];              // +0x33..+0x39
    unsigned char m_39;           // +0x39（process 回写 CParty::get_party_seatno）
    char m_padTail[0xd3];         // +0x3a..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

class MSG_OBJECT_BRING_UP : public MSG_BASE
{
public:
    MSG_OBJECT_BRING_UP();
    char m_pad[0x100];
};

class MSG_OPEN_CERAPACKAGE : public MSG_BASE
{
public:
    MSG_OPEN_CERAPACKAGE();
    char m_pad[0x100];
};

class MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE : public MSG_BASE
{
public:
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_P2P_STATISTICS : public MSG_BASE
{
public:
    MSG_P2P_STATISTICS();
    unsigned char m_0d;   // +0x0d
    unsigned char m_0e;   // +0x0e
    short m_10;           // +0x10
    short m_12;           // +0x12
    short m_14;           // +0x14
    int m_18;             // +0x18
    int m_1c;             // +0x1c
    int m_20;             // +0x20
    int m_24;             // +0x24
    short m_28;           // +0x28
    short m_2a;           // +0x2a
    short m_2c;           // +0x2c
    int m_30;             // +0x30
    int m_34;             // +0x34
    int m_38;             // +0x38
    int m_3c;             // +0x3c
};
#pragma pack(pop)

// ORIG 布局（SetPartyInfoUI 0x0859b234 读取实证）：+0x0d titleIndex、
// +0x12 title[0x20]、+0x32 userMax、+0x33 dungIndex(ushort)、+0x35 dungDiffi。
class MSG_PARTY_SET_INFO : public MSG_BASE
{
public:
    MSG_PARTY_SET_INFO();
    unsigned char m_titleIndex;   // +0x0d
    char m_pad0e[4];              // +0x0e..0x11
    char m_title[0x20];           // +0x12（setTitle 拷贝源，0x20 同 CParty::m_title）
    unsigned char m_userMax;      // +0x32
    unsigned short m_dungIndex;   // +0x33
    unsigned char m_dungDiffi;    // +0x35
    char m_pad36[0xca];           // +0x36..0xff（总尺寸 0x10d 不变）
};

class MSG_PARTY_TELEPORT : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT();
    char m_pad[0x100];
};

class MSG_PARTY_TELEPORT_CONFIRM : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT_CONFIRM();
    char m_pad[0x100];
};

class MSG_PURIFY_ITEM : public MSG_BASE
{
public:
    MSG_PURIFY_ITEM();
    char m_pad[0x100];
};

class MSG_PVP_MISSION_COMBO_CLEAR : public MSG_BASE
{
public:
    MSG_PVP_MISSION_COMBO_CLEAR();
    char m_pad[0x100];
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
    MSG_PVP_TIMEOUT();
    char m_pad[0x100];
};

class MSG_QUEST_SHOP : public MSG_BASE
{
public:
    MSG_QUEST_SHOP();
    char m_pad[0x100];
};

class MSG_QUICK_CHAT : public MSG_BASE
{
public:
    MSG_QUICK_CHAT();
    char m_pad[0x100];
};

class MSG_QUICK_PARTY : public MSG_BASE
{
public:
    MSG_QUICK_PARTY();
    char m_pad[0x100];
};

class MSG_REFUND_SKILL : public MSG_BASE
{
public:
    MSG_REFUND_SKILL();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_REGENERATION_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_REGENERATION_RANDOM_OPTION();
    // ORIG Dispather_RegenerationRandomOption::process：
    //   两装备槽 u16@0xd/0xf；两源选项序号 s16@0x11/0x13（Regenerate 第 3/4 参）
    unsigned short m_equipSlot0;  // +0x0d
    unsigned short m_equipSlot1;  // +0x0f
    short m_srcOptIndex0;         // +0x11
    short m_srcOptIndex1;         // +0x13
    char m_pad15[0xf4];           // +0x15..（总大小保持 0x10d）
};
#pragma pack(pop)

class MSG_REGISTER_TO_BLACKLIST : public MSG_BASE
{
public:
    MSG_REGISTER_TO_BLACKLIST();
    char m_pad[0x100];
};

class MSG_REMOVE_FRIEND : public MSG_BASE
{
public:
    MSG_REMOVE_FRIEND();
    char m_pad[0x100];
};

class MSG_RENAME_CREATURE : public MSG_BASE
{
public:
    MSG_RENAME_CREATURE();
    char m_pad[0x100];
};

class MSG_RENT_ITEM : public MSG_BASE
{
public:
    MSG_RENT_ITEM();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_REPAIR_EQUIP : public MSG_BASE
{
public:
    MSG_REPAIR_EQUIP();
    unsigned char m_0d;     // +0x0d（invenType，传 Store::repair_equip char 参）
    short m_0e;             // +0x0e（slot，s16；成功后回显 param+0xa）
    unsigned short m_10;    // +0x10（count/耐久参，u16）
    char m_pad[0xfb];
};
#pragma pack(pop)

class MSG_REQUEST_CHARAC_SKILL_INFO : public MSG_BASE
{
public:
    MSG_REQUEST_CHARAC_SKILL_INFO();
    char m_pad[0x100];
};

class MSG_REQUEST_DISJOINT_ITEM : public MSG_BASE
{
public:
    MSG_REQUEST_DISJOINT_ITEM();
    char m_pad[0x100];
};

class MSG_REQUEST_GUILD_JOIN : public MSG_BASE
{
public:
    MSG_REQUEST_GUILD_JOIN();
    char m_pad[0x100];
};

class MSG_REQUEST_INGAME_ADVERTISEMENT : public MSG_BASE
{
public:
    MSG_REQUEST_INGAME_ADVERTISEMENT();
    char m_pad[0x100];
};

class MSG_REQUEST_MEMBER_ENTER : public MSG_BASE
{
public:
    MSG_REQUEST_MEMBER_ENTER();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_RESET_RANDOM_OPTION : public MSG_BASE
{
public:
    MSG_RESET_RANDOM_OPTION();
    // ORIG DispatcherResetRandomOption::process：
    unsigned short m_equipSlot;    // +0x0e（目标装备槽；GetInvenRef(1, slot)）
    unsigned short m_scrollSlot;   // +0x10（再铸卷轴槽；use_item 消耗）
    unsigned char m_rarityChoice;  // +0x12（玩家选择稀有度，须 <=2）
    char m_pad13[0xf1];            // +0x13..（总大小保持 0x10d）
};
#pragma pack(pop)

class MSG_RIGHT_OF_CHANGE_GROW_TYPE : public MSG_BASE
{
public:
    MSG_RIGHT_OF_CHANGE_GROW_TYPE();
    char m_pad[0x100];
};

class MSG_SAVE_CHARACTER_OPTION : public MSG_BASE
{
public:
    MSG_SAVE_CHARACTER_OPTION();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_1 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_1();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_2 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_2();
    char m_pad[0x100];
};

class MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON();
    char m_pad[0x100];
};

class MSG_SAVE_QUEST_NOTIFY : public MSG_BASE
{
public:
    MSG_SAVE_QUEST_NOTIFY();
    char m_pad[0x100];
};

class MSG_SCAN_DLL : public MSG_BASE
{
public:
    MSG_SCAN_DLL();
    char m_pad[0x100];
};

class MSG_SCAN_DRV : public MSG_BASE
{
public:
    MSG_SCAN_DRV();
    char m_pad[0x100];
};

class MSG_SCRIPT_HASH : public MSG_BASE
{
public:
    MSG_SCRIPT_HASH();
    char m_pad[0x100];
};

class MSG_SECRET_SHOP_BUY_ITEM : public MSG_BASE
{
public:
    MSG_SECRET_SHOP_BUY_ITEM();
    char m_pad[0x100];
};

class MSG_SECRET_SHOP_OPEN_CLOSE : public MSG_BASE
{
public:
    MSG_SECRET_SHOP_OPEN_CLOSE();
    char m_pad[0x100];
};

class MSG_SECURITY_LOG_PACKET : public MSG_BASE
{
public:
    MSG_SECURITY_LOG_PACKET();
    char m_pad[0x100];
};

class MSG_SECU_RPY : public MSG_BASE
{
public:
    MSG_SECU_RPY();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_SELECT_DUNGEON : public MSG_BASE
{
public:
    MSG_SELECT_DUNGEON();
    // 布局证据：DisPatcher_SelectDungeon::process(0x081c8102)：
    //   +0x0d short 副本 idx、+0x15?? 无 —— 实际读取 +0xd word、
    //   +0xf/+0x10/+0x11 byte（dungeon_start/handleStartGame 参数与 party 标记）。
    short m_dungeonIdx;               // +0x0d
    signed char m_dungeonType;        // +0x0f
    signed char m_field10;            // +0x10
    unsigned char m_field11;          // +0x11（写入 CParty::m_partyType）
    char m_padTail[0xfb];             // +0x12..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

class MSG_SELECT_STRIKER : public MSG_BASE
{
public:
    MSG_SELECT_STRIKER();
    char m_pad[0x100];
};

class MSG_SERIAROOM_DECO_EVENT : public MSG_BASE
{
public:
    MSG_SERIAROOM_DECO_EVENT();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_SET_READY_STATE : public MSG_BASE
{
public:
    MSG_SET_READY_STATE();
    unsigned char m_0d;   // +0x0d（ready 标志，ORIG process 以 (m_0d!=0) 传入
                          //  PvP_Room::set_ready_state）
    char m_pad[0xff];     // +0x0e..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SET_USER_AREA : public MSG_BASE
{
public:
    MSG_SET_USER_AREA();
    signed char m_0d;     // +0x0d（vill）
    signed char m_0e;     // +0x0e（area）
    unsigned short m_0f;  // +0x0f（x）
    unsigned short m_11;  // +0x11（y）
    signed char m_13;     // +0x13（z，sbyte 符号扩展后传 move_area）
    unsigned short m_14;  // +0x14（prevVill）
    unsigned short m_16;  // +0x16（prevArea）
    char m_pad[0xf5];     // +0x18..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

class MSG_SKILL_COMMAND : public MSG_BASE
{
public:
    MSG_SKILL_COMMAND();
    char m_pad[0x100];
};

class MSG_SKILL_INIT : public MSG_BASE
{
public:
    MSG_SKILL_INIT();
    char m_pad[0x100];
};

class MSG_SOLO_TELEPORT : public MSG_BASE
{
public:
    MSG_SOLO_TELEPORT();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_SORT_ITEM : public MSG_BASE
{
public:
    MSG_SORT_ITEM();
    // 布局证据：DisPatcher_SortItem::process(0x081c9c54)：仅读 +0x0d byte。
    unsigned char m_invenType;        // +0x0d
    char m_padTail[0xff];             // +0x0e..+0x10c（保持原 sizeof 0x10d）
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

class MSG_TELEPORT : public MSG_BASE
{
public:
    MSG_TELEPORT();
    char m_pad[0x100];
};

class MSG_TEMP_STRUCT : public MSG_BASE
{
public:
    MSG_TEMP_STRUCT();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_GET : public MSG_BASE
{
public:
    MSG_TITLEBOOK_GET();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_OTHERUSER : public MSG_BASE
{
public:
    MSG_TITLEBOOK_OTHERUSER();
    char m_pad[0x100];
};

class MSG_TITLEBOOK_PUT : public MSG_BASE
{
public:
    MSG_TITLEBOOK_PUT();
    char m_pad[0x100];
};

class MSG_UPDATE_CONTRACT_OF_CUBE_INFO : public MSG_BASE
{
public:
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_USERINFO_REQ : public MSG_BASE
{
public:
    MSG_USERINFO_REQ();
    unsigned short m_targetId;  // +0xd（GameWorld::get_user_info 目标 id）
    unsigned char m_slot;       // +0xf
    char m_pad10[0xf0];         // +0x10 → 负载总 0x100
};
#pragma pack(pop)

class MSG_USER_HISTORY_LOG : public MSG_BASE
{
public:
    MSG_USER_HISTORY_LOG();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_USER_SET_POS : public MSG_BASE
{
public:
    MSG_USER_SET_POS();
    unsigned short m_0d;  // +0x0d（x）
    unsigned short m_0f;  // +0x0f（y）
    signed char m_11;     // +0x11（z，sbyte）
    unsigned short m_12;  // +0x12（dir，传 GameWorld::move_position 第 5 参）
    char m_pad[0xf9];     // +0x14..+0x10c（保持原 sizeof 0x10d）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USE_COIN : public MSG_BASE
{
public:
    MSG_USE_COIN();
    unsigned short m_0d;    // +0x0d（UseCoin process 回显至 param+0x8；u16 @ msg+0x0d）
    char m_pad[0xfe];
};
#pragma pack(pop)

class MSG_USE_DYE : public MSG_BASE
{
public:
    MSG_USE_DYE();
    char m_pad[0x100];
};

class MSG_USE_LIMIT_CUBE : public MSG_BASE
{
public:
    MSG_USE_LIMIT_CUBE();
    char m_pad[0x100];
};

class MSG_USE_RENAME_CARD : public MSG_BASE
{
public:
    MSG_USE_RENAME_CARD();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_USE_SKILL : public MSG_BASE
{
public:
    MSG_USE_SKILL();
    // ORIG DisPatcher_UseSkill::process：技能槽 u8@0xe；目标 u16@0xf（use_skill 第 3 参）
    unsigned char m_slot;        // +0x0e
    unsigned short m_targetId;   // +0x0f（SKILL_USING_VALIDATE_INFO 槽位 / CParty::use_skill 目标）
    char m_pad11[0xf8];          // +0x11..（总大小保持 0x10d）
};
#pragma pack(pop)

class MSG_WALKOUT_PARTY : public MSG_BASE
{
public:
    MSG_WALKOUT_PARTY();
    char m_walkoutSlot;       // +0xd（包头后首字节：被踢成员槽位，walkout_member 第二参）
    char m_pad[0xff];
};

class MSG_WARROOM_WP_PER_MONSTER : public MSG_BASE
{
public:
    MSG_WARROOM_WP_PER_MONSTER();
    char m_pad[0x100];
};

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
    MsgLimitNpcBuyItem();
    char m_pad[0x100];
};

class MsgMercenaryCompetition : public MSG_BASE
{
public:
    MsgMercenaryCompetition();
    char m_pad[0x100];
};

class MsgMercenaryReturn : public MSG_BASE
{
public:
    MsgMercenaryReturn();
    char m_pad[0x100];
};

class MsgRequestAssaultPrice : public MSG_BASE
{
public:
    MsgRequestAssaultPrice();
    char m_pad[0x100];
};

class MsgTournamentRewardSelect : public MSG_BASE
{
public:
    MsgTournamentRewardSelect();
    char m_pad[0x100];
};

// ---------------------------------------------------------------------------
// Param 类型（占位大小 0x40，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
class AbnormalUseStackable : public ParamBase
{
public:
    AbnormalUseStackable();
    char m_pad[0x40];
};

class BuyItemParam : public ParamBase
{
public:
    BuyItemParam();
    Inven_Item m_item;          // +0x04（ORIG C1 调 Inven_Item C1）
    Inven_Item m_resultItem;    // +0x41
private:
    char m_gap7e[2];            // +0x7e 对齐填充
public:
    int m_80;                   // +0x80
    int m_84;                   // +0x84
    int m_88;                   // +0x88
    int m_8c;                   // +0x8c
    int m_90;                   // +0x90
    int m_94;                   // +0x94
    std::pair<int, int> m_pair; // +0x98
    int m_a0;                   // +0xa0（ctor = -1）
    int m_a4;                   // +0xa4（ctor = -1）
    CItem* m_a8;                // +0xa8（get_data find_item 结果商城物品；ORIG 布局 int 位宽不变）
    CItem* m_ac;                // +0xac（材料 item idx 的 find_item 结果）
    int m_b0;                   // +0xb0
};

class CheckSecurityProtection : public ParamBase
{
public:
    CheckSecurityProtection();
    char m_pad[0x40];
};

class InformNoticeData : public ParamBase
{
public:
    InformNoticeData();
    char m_pad[0x40];
};

class InformNoticeData2nd : public ParamBase
{
public:
    InformNoticeData2nd();
    char m_pad[0x40];
};

class LimitNpcBuyItem : public ParamBase
{
public:
    LimitNpcBuyItem();
    char m_pad[0x40];
};

class PcRoomPlayTimeRewardParam : public ParamBase
{
public:
    PcRoomPlayTimeRewardParam();
    char m_pad[0x40];
};

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
    char m_pad10[0x30];        // +0x10..+0x3f
};

class STQuestShopBuyParam : public ParamBase
{
public:
    STQuestShopBuyParam();
    char m_pad[0x40];
};

class STQuestShopParam : public ParamBase
{
public:
    STQuestShopParam();
    char m_pad[0x40];
};

class STUdateItemParam : public ParamBase
{
public:
    STUdateItemParam();
    char m_pad[0x40];
};

class ScanBotByDllParam : public ParamBase
{
public:
    ScanBotByDllParam();
    char m_pad[0x40];
};

class ScanBotByDrvParam : public ParamBase
{
public:
    ScanBotByDrvParam();
    char m_pad[0x40];
};

class ScriptHashParam : public ParamBase
{
public:
    ScriptHashParam();
    char m_pad[0x40];
};

class SecuCardRpyParam : public ParamBase
{
public:
    SecuCardRpyParam();
    char m_pad[0x40];
};

class SendMercenaryCompetitionResult : public ParamBase
{
public:
    SendMercenaryCompetitionResult();
    char m_pad[0x40];
};

class SendMercenaryReturnResult : public ParamBase
{
public:
    SendMercenaryReturnResult();
    char m_pad[0x40];
};

class TempParam : public ParamBase
{
public:
    TempParam();
    char m_pad[0x40];
};

class saveQuestNotifyParam : public ParamBase
{
public:
    saveQuestNotifyParam();
    int m_errorCode;   // +0x04（Dispatcher_SaveQuestNotify::process 写入 check_error 结果）
    char m_pad08[0x3c];
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
    char m_pad[0x40];
};

class stBingoReward : public ParamBase
{
public:
    stBingoReward();
    char m_pad[0x40];
};

class stBlueMarble : public ParamBase
{
public:
    stBlueMarble();
    char m_pad[0x40];
};

class stBlueMarbleConfirmInfo : public ParamBase
{
public:
    stBlueMarbleConfirmInfo();
    char m_pad[0x40];
};

class stBossDieCheckParam : public ParamBase
{
public:
    stBossDieCheckParam();
    char m_pad[0x40];
};

class stBreakAwayParam : public ParamBase
{
public:
    stBreakAwayParam();
    int m_resultCode;  // +0x04（process：突破检查未完成时写 1；send 读之回包）
    char m_pad08[0x3c];
};

class stCallMemberListParam : public ParamBase
{
public:
    stCallMemberListParam();
    char m_pad[0x40];
};

class stChangeAnotherSkillTree : public ParamBase
{
public:
    stChangeAnotherSkillTree();
    char m_pad[0x40];
};

class stChangeAvatarOption : public ParamBase
{
public:
    stChangeAvatarOption();
    char m_pad[0x40];
};

class stChangeEmotionParam : public ParamBase
{
public:
    stChangeEmotionParam();
    char m_pad[0x40];
};

class stChangeHostWarRoom : public ParamBase
{
public:
    stChangeHostWarRoom();
    char m_pad[0x40];
};

class stChangeLetterStatParam : public ParamBase
{
public:
    stChangeLetterStatParam();
    char m_pad[0x40];
};

class stCharacSlotExtendEffect : public ParamBase
{
public:
    stCharacSlotExtendEffect();
    char m_pad[0x40];
};

class stClientSpecStatisticParam : public ParamBase
{
public:
    stClientSpecStatisticParam();
    char m_pad[0x40];
};

class stCloseDisjointStoreParam : public ParamBase
{
public:
    stCloseDisjointStoreParam();
    char m_pad[0x40];
};

class stCollectItems : public ParamBase
{
public:
    stCollectItems();
    char m_pad[0x40];
};

class stComboSkill : public ParamBase
{
public:
    stComboSkill();
    char m_pad[0x40];
};
class stCreateCharac : public ParamBase
{
public:
    stCreateCharac();
    int m_errNo;         // +0x04（process 错误码；G2/G3 统一命名约定）
    char m_pad8[0x3c];
};

class stComboSkillExtensionQuickSlotReset : public ParamBase
{
public:
    stComboSkillExtensionQuickSlotReset();
    char m_pad[0x40];
};

class stCreateDisjointStoreParam : public ParamBase
{
public:
    stCreateDisjointStoreParam();
    char m_pad[0x40];
};

class stDeleteCharac : public ParamBase
{
public:
    stDeleteCharac();
    int m_errNo;         // +0x04（process 错误码）
    char m_pad8[0x3c];
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
    char m_pad[0x40];
};

class stDieCharacInDungeon : public ParamBase
{
public:
    stDieCharacInDungeon();
    int m_errNo;    // +0x04（ORIG process 写 check_error 结果/0x7fffffff）
    char m_pad[0x3c];
};

class stDropItemParam : public ParamBase
{
public:
    stDropItemParam();
    int m_errNo;            // +0x04（check_error / drop_item>0 时的返回码）
    unsigned char m_8;      // +0x08（回显 MSG_DROP_ITEM m_11）
    char m_pad[0x3b];
};

class stEnchantByBeadParam : public ParamBase
{
public:
    stEnchantByBeadParam();
    char m_pad[0x40];
};

class stEnchantParam : public ParamBase
{
public:
    stEnchantParam();
    char m_pad[0x40];
};

class stEnterDisjointStoreParam : public ParamBase
{
public:
    stEnterDisjointStoreParam();
    char m_pad[0x40];
};

class stEquipslotSwitch : public ParamBase
{
public:
    stEquipslotSwitch();
    char m_pad[0x40];
};

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

class stExpandEquipslotFlagUpdate : public ParamBase
{
public:
    stExpandEquipslotFlagUpdate();
    char m_pad[0x40];
};

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
    char m_pad[0x40];
};

class stFinishLoadParam : public ParamBase
{
public:
    stFinishLoadParam();
    int m_errNo;          // +0x04（check_error 结果；<0 时 process 走 LineFunc 路径）
    char m_pad[0x3c];
};

class stGetItemParam : public ParamBase
{
public:
    stGetItemParam();
    int m_errNo;            // +0x04（check_error 结果；0/0xd8/0x13/0x7fffffff 状态码）
    char m_pad[0x3c];
};

class stGoldTakeIncreasingAmount : public ParamBase
{
public:
    stGoldTakeIncreasingAmount();
    char m_pad[0x40];
};

class stGrowthCreature : public ParamBase
{
public:
    stGrowthCreature();
    char m_pad[0x40];
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
    char m_pad[0x40];
};

class stGuildBoardParam : public ParamBase
{
public:
    stGuildBoardParam();
    char m_pad[0x40];
};

class stGuildCargoHistoryParam : public ParamBase
{
public:
    stGuildCargoHistoryParam();
    char m_pad[0x40];
};

class stGuildCargoMoveItemParam : public ParamBase
{
public:
    stGuildCargoMoveItemParam();
    char m_pad[0x40];
};

class stGuildCargoParam : public ParamBase
{
public:
    stGuildCargoParam();
    char m_pad[0x40];
};

class stGuildCargoPopItemParam : public ParamBase
{
public:
    stGuildCargoPopItemParam();
    char m_pad[0x40];
};

class stGuildCargoPushItemParam : public ParamBase
{
public:
    stGuildCargoPushItemParam();
    char m_pad[0x40];
};

class stHatchCreature : public ParamBase
{
public:
    stHatchCreature();
    char m_pad[0x40];
};

class stImageCommunicationEquipmentUse : public ParamBase
{
public:
    stImageCommunicationEquipmentUse();
    char m_pad[0x40];
};

class stItemDictionary : public ParamBase
{
public:
    stItemDictionary();
    char m_pad[0x40];
};

class stItemHyperLinkMessage : public ParamBase
{
public:
    stItemHyperLinkMessage();
    char m_pad[0x40];
};

class stLeavePartyParam : public ParamBase
{
public:
    stLeavePartyParam();
    int m_errNo;              // +0x4（ORIG process 写入 check_error/业务结果码，send 消费）
    char m_pad[0x3c];
};

class stLogIngameAdvertisement : public ParamBase
{
public:
    stLogIngameAdvertisement();
    char m_pad[0x40];
};

class stMailboxExtractParam : public ParamBase
{
public:
    stMailboxExtractParam();
    char m_pad[0x40];
};

class stMailboxOpenParam : public ParamBase
{
public:
    stMailboxOpenParam();
    char m_pad[0x40];
};

class stMailboxSendParam : public ParamBase
{
public:
    stMailboxSendParam();
    char m_pad[0x40];
};

class stMemberEnterReplyParam : public ParamBase
{
public:
    stMemberEnterReplyParam();
    char m_pad[0x40];
};

class stMemberSecedeParam : public ParamBase
{
public:
    stMemberSecedeParam();
    char m_pad[0x40];
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
    char m_pad[0x3c];
};

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

class stOnChangeSkillParam : public ParamBase
{
public:
    stOnChangeSkillParam();
    // ORIG DisPatcher_ChangeSkill::process 写入（skill_move 前快照，send 回读）
    unsigned char m_curSlot;   // +0x04
    unsigned char m_newSlot;   // +0x05
    char m_pad06[0x3a];        // +0x06..（总大小保持 0x44 含 vptr）
};

class stOnGetUserInfoParam : public ParamBase
{
public:
    stOnGetUserInfoParam();
    char m_pad[0x40];
};

class stOnPvPTimeOut : public ParamBase
{
public:
    stOnPvPTimeOut();
    int m_timeOutResult;      // +0x4（ORIG PvPTimeout::process 写入 time_out 结果码，send 消费）
    char m_pad[0x3c];
};

class stOnRecoverStaminaParam : public ParamBase
{
public:
    stOnRecoverStaminaParam();
    int m_errNo;              // +0x4（ORIG process 写入 serviceOfShusia 结果码，send 消费）
    char m_pad[0x3c];
};
class stOnSetPvPReadyParam : public ParamBase
{
public:
    stOnSetPvPReadyParam();
    int m_errNo;          // +0x04（PvP_Room::set_ready_state 结果码；<0 时 start_pvp）
    char m_pad[0x3c];
};

class stOnSortItemParam : public ParamBase
{
public:
    stOnSortItemParam();
    unsigned char m_invenType;        // +0x04（回显 msg+0xd，ORIG process 写入并回读分支）
    unsigned char m_gap5[3];          // +0x05..+0x07
    int m_result;                     // +0x08（sort 返回值；ctor 清零）
    char m_padC[0x34];                // +0x0c..+0x3f（总大小保持 0x44 含 vptr）
};

class stOnUseEquipParam : public ParamBase
{
public:
    stOnUseEquipParam();
    int m_errNo;            // +0x04（use_equip 返回码）
    unsigned char m_8;      // +0x08（回显 MSG_DECREASE_DURABILITY m_0d）
    char m_pad[0x3b];
};

class stOpenCeraPackage : public ParamBase
{
public:
    stOpenCeraPackage();
    char m_pad[0x40];
};

class stPartyTeleport : public ParamBase
{
public:
    stPartyTeleport();
    char m_pad[0x40];
};

class stPartyTeleportConfirm : public ParamBase
{
public:
    stPartyTeleportConfirm();
    char m_pad[0x40];
};

class stQuickChatParam : public ParamBase
{
public:
    stQuickChatParam();
    char m_pad[0x40];
};

class stQuickPartyParam : public ParamBase
{
public:
    stQuickPartyParam();
    char m_pad[0x40];
};

class stRefundSkill : public ParamBase
{
public:
    stRefundSkill();
    char m_pad[0x40];
};

class stRegenROIResult : public ParamBase
{
public:
    stRegenROIResult();
    // ORIG Dispather_RegenerationRandomOption::process 写入、send 消费：
    int m_errNo;          // +0x04（0=成功；否则 SendCmdErrorPacket/use_money 错码）
    short m_insertedSlot; // +0x08（insertItemIntoInventory 返回槽位，>=0 时有效）
    char m_pad0a[0x36];   // +0x0a..（总大小保持 0x44 含 vptr）
};

class stRegisterToBlackListParam : public ParamBase
{
public:
    stRegisterToBlackListParam();
    char m_pad[0x40];
};

class stRemoveFriendParam : public ParamBase
{
public:
    stRemoveFriendParam();
    int m_errNo;    // +0x04（ORIG process 写 check_error 结果）
    char m_pad[0x3c];
};

class stRenameCreature : public ParamBase
{
public:
    stRenameCreature();
    char m_pad[0x40];
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
    char m_pad[0x40];
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
    char m_pad[0x36];
};

class stRequestAssaultPrice : public ParamBase
{
public:
    stRequestAssaultPrice();
    char m_pad[0x40];
};

class stRequestBlackListParam : public ParamBase
{
public:
    stRequestBlackListParam();
    char m_pad[0x40];
};

class stRequestCharacSkillInfo : public ParamBase
{
public:
    stRequestCharacSkillInfo();
    char m_pad[0x40];
};

class stRequestDisjointItemParam : public ParamBase
{
public:
    stRequestDisjointItemParam();
    char m_pad[0x40];
};

class stRequestIngameAdvertisement : public ParamBase
{
public:
    stRequestIngameAdvertisement();
    char m_pad[0x40];
};

class stRequestMemberEnterParam : public ParamBase
{
public:
    stRequestMemberEnterParam();
    char m_pad[0x40];
};

class stRightOfChangeGrowType : public ParamBase
{
public:
    stRightOfChangeGrowType();
    char m_pad[0x40];
};

class stSaveCharacOption : public ParamBase
{
public:
    stSaveCharacOption();
    char m_pad[0x40];
};

class stSecretShopBuyItemParam : public ParamBase
{
public:
    stSecretShopBuyItemParam();
    char m_pad[0x40];
};

class stSecretShopOpenCloseParam : public ParamBase
{
public:
    stSecretShopOpenCloseParam();
    char m_pad[0x40];
};

class stSecurityLogParam : public ParamBase
{
public:
    stSecurityLogParam();
    char m_pad[0x40];
};

class stSelectCharac : public ParamBase
{
public:
    stSelectCharac();
    int m_errNo;         // +0x04（process 错误码）
    char m_pad8[0x3c];
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
    char m_pad[0x40];
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
    char m_pad[0x40];
};

class stSetAreaParam : public ParamBase
{
public:
    stSetAreaParam();
    int m_errNo;          // +0x04（三参 check_error 结果；>=1 直接返回 0）
    char m_pad[0x3c];
};

class stSetPartyInfo : public ParamBase
{
public:
    stSetPartyInfo();
    int m_errNo;              // +0x4（ORIG process 写入 check_error/业务结果码，send 消费）
    char m_pad[0x3c];
};

class stSkillCommandParam : public ParamBase
{
public:
    stSkillCommandParam();
    char m_pad[0x40];
};

class stSkillInit : public ParamBase
{
public:
    stSkillInit();
    char m_pad[0x40];
};

class stSoloTeleportParam : public ParamBase
{
public:
    stSoloTeleportParam();
    char m_pad[0x40];
};

class stStackableActionUse : public ParamBase
{
public:
    stStackableActionUse();
    char m_pad[0x40];
};

class stStartGameParam : public ParamBase
{
public:
    stStartGameParam();
    int m_errNo;                      // +0x04（check_error 结果；ORIG process 写入）
    char m_pad8[0x3c];                // +0x08..+0x3f（总大小保持 0x44 含 vptr）
};

class stTeleportParam : public ParamBase
{
public:
    stTeleportParam();
    int m_4;              // +0x04
    short m_8;            // +0x08
    char m_gapA[6];       // +0x0a..+0x0f（ORIG 未初始化区）
    unsigned char m_10;   // +0x10
    unsigned char m_11;   // +0x11
    char m_gap12[2];      // +0x12 对齐填充
    int m_14;             // +0x14
};

class stTournamentRewardSelect : public ParamBase
{
public:
    stTournamentRewardSelect();
    char m_pad[0x40];
};

class stTournamentRewardSelectState : public ParamBase
{
public:
    stTournamentRewardSelectState();
    char m_pad[0x40];
};

class stUpdateContractOfCubeInfo : public ParamBase
{
public:
    stUpdateContractOfCubeInfo();
    char m_pad[0x40];
};

class stUpgradeDisjointMachineParam : public ParamBase
{
public:
    stUpgradeDisjointMachineParam();
    char m_pad[0x40];
};

class stUseCoinParam : public ParamBase
{
public:
    stUseCoinParam();
    int m_errNo;            // +0x04（check_error 结果）
    unsigned short m_8;     // +0x08（回显 MSG_USE_COIN m_0d）
    char m_pad[0x3a];
};

class stUseDye : public ParamBase
{
public:
    stUseDye();
    char m_pad[0x40];
};

class stUseLimitCube : public ParamBase
{
public:
    stUseLimitCube();
    char m_pad[0x40];
};

class stUseRenameCard : public ParamBase
{
public:
    stUseRenameCard();
    char m_pad[0x40];
};

class stUseSkillParam : public ParamBase
{
public:
    stUseSkillParam();
    // ORIG DisPatcher_UseSkill::process 写入 check_error 结果（send 消费）：
    int m_errNo;        // +0x04（0=可施放；0x7fffffff=状态非 5）
    char m_pad08[0x38]; // +0x08..（总大小保持 0x44 含 vptr）
};

class stWalkoutPartyParam : public ParamBase
{
public:
    stWalkoutPartyParam();
    int m_errNo;              // +0x4（ORIG process 写入 walkout_member 结果码，send 消费）
    char m_pad[0x3c];
};

class stWarRoomWp : public ParamBase
{
public:
    stWarRoomWp();
    char m_pad[0x40];
};

#endif  // DNF_GAME_PACKET_TYPES_H_
