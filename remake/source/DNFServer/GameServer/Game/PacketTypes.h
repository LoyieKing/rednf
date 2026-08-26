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

class MSG_ADD_FRIEND
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x37];         // +0xd..（帧推导总长 0x37；内部布局未还原）
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
    // ORIG C1 0x0822dc66：+0xd int 清零（其余未出线）
    int m_0d;                 // +0x0d
    char m_pad11[0xfc];       // 保持占位总长 0x10d
};

class MSG_BLUE_MARBLE : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE();
    // ORIG C1：+0xd byte 清零
    unsigned char m_0d;       // +0x0d
    char m_pad0e[0xff];       // 保持占位总长 0x10d
};

class MSG_BLUE_MARBLE_CONFIRM_INFO : public MSG_BASE
{
public:
    MSG_BLUE_MARBLE_CONFIRM_INFO();
    // ORIG C1：+0xd byte 清零、+0xe word=-1
    unsigned char m_0d;       // +0x0d
    unsigned short m_0e;      // +0x0e（ctor = -1）
    char m_pad10[0xfd];       // 保持占位总长 0x10d
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
class MSG_BUY_ITEM
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    // ORIG DisPatcher_BuyItem（process/get_data/check_error 逐字节）：
    int m_itemIdx;      // +0x0d（购买目标 item idx；get_data find_item 输入）
    int m_buyCount;     // +0x11（购买数量；>1 时 get_nMaxBuyableCount 校验）
    int m_deliveryType; // +0x15（eventDeliveryBuyItem 首参回传）
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
    // ORIG C1：+0xd byte = 0xff（未选中的技能树哨兵）
    unsigned char m_0d;       // +0x0d
    char m_pad0e[0xff];       // 保持占位总长 0x10d
};
class MSG_CHANGE_AVATAR_OPTION : public MSG_BASE
{
public:
    MSG_CHANGE_AVATAR_OPTION();
    char m_padD;              // +0x0d（对齐填充，ORIG 未出线）
    unsigned short m_0e;      // +0x0e（ctor = -1）
    int m_10;                 // +0x10（ctor = -1）
    unsigned short m_14;      // +0x14（ctor = -1）
    int m_16;                 // +0x16（ctor = -1）
    unsigned char m_1a;       // +0x1a（ctor = 0xff）
    char m_pad1b[0xf2];       // 保持占位总长 0x10d
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
    // ORIG C1：+0xe/+0x10 word 清零、+0x12 byte 清零（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    unsigned short m_10;      // +0x10
    unsigned char m_12;       // +0x12
    char m_pad13[0xfa];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_CHANGE_SKILLSLOT
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    // ORIG DisPatcher_ChangeSkill::process：param+4/5 ← msg[0xe]/msg[0xf]
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
    char m_pad28[0xe5];       // 保持占位总长 0x10d
};
#pragma pack(pop)

class MSG_CLIENT_SPEC_STATISTIC : public MSG_BASE
{
public:
    MSG_CLIENT_SPEC_STATISTIC();
    // ORIG C1：Client_Spec 出线构造于 +0xe。本仓库 Client_Spec（CUser.h）为 pad
    // 占位模型且 ctor 空体 —— 调用形态与 ORIG Client_SpecC1 等价。
    char m_padD;               // +0x0d
    Client_Spec m_spec;        // +0xe（sizeof 0xc10；类总长随真实布局扩大）
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
    // ORIG C1：+0xd byte 清零
    unsigned char m_0d;       // +0x0d
    char m_pad0e[0xff];       // 保持占位总长 0x10d
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
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_CREATE_CHARACTER
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_padE[4];          // +0xe..+0x11（ORIG name 前间隙）
    unsigned char m_slot;    // +0xd
    char m_name[0x1e];       // +0x12..0x2f（ORIG strncpy 长 0x1e；末字节即 +0x2f 标志位）
    char m_padTail[8];        // +0x30..+0x37（帧推导总长 0x38）
};
#pragma pack(pop)

class MSG_CREATE_DISJOINT_STORE : public MSG_BASE
{
public:
    MSG_CREATE_DISJOINT_STORE();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_DECREASE_DURABILITY
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    unsigned char m_0d;     // +0x0d（UseEquip process：use_equip 槽位，回显至 param+0x8）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_DELETE_CHARACTER
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    signed char m_slot;      // +0xd
    char m_padE[0x2a];        // +0xe..+0x37（帧推导总长 0x38）
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

class MSG_DIE_CHARACTER
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x11];         // +0xd..+0x10（帧推导总长 0x11）
};

class MSG_DONATE_GUILD_FUND : public MSG_BASE
{
public:
    MSG_DONATE_GUILD_FUND();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_DROP_ITEM
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    // ORIG C1：std::vector<int> 构造于 +0x10（+0xd..+0xf 未出线）
    char m_padD[3];                    // +0x0d..+0x0f
    std::vector<int> m_rewardItems;    // +0x10（4 对齐推入；ORIG 单 call 实证）
};

class MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE : public MSG_BASE
{
public:
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_EXPERT_EXTRACTION : public MSG_BASE
{
public:
    MSG_EXPERT_EXTRACTION();
    // ORIG C1：+0xd byte 清零、+0x10 word 清零、+0xe ← word(+0x10)（拷贝链）
    unsigned char m_0d;       // +0x0d
    unsigned short m_0e;      // +0x0e（= m_10）
    unsigned short m_10;      // +0x10（ctor = 0）
    char m_pad12[0xfb];       // 保持占位总长 0x10d
};
#pragma pack(pop)

class MSG_FATIGUE_ATTENDANCE : public MSG_BASE
{
public:
    MSG_FATIGUE_ATTENDANCE();
    // ORIG C1：+0xd int 清零
    int m_0d;                 // +0x0d
    char m_pad11[0xfc];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_FINISH_LOADING
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    int m_0d;             // +0x0d（客户端 inven checksum，ORIG process 与
                          //  CUser::getCurCharacInvenCheckSum 比对，不等记 0x25b）
    int m_11;             // +0x11（skill checksum，与 getCurCharacSkillCheckSum 比对）
};
#pragma pack(pop)

class MSG_FLOAT_RDATA_MODULATION : public MSG_BASE
{
public:
    MSG_FLOAT_RDATA_MODULATION();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_GET_ITEM
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    // ORIG C1：+0xd/+0x11 int 各清零
    int m_0d;                 // +0x0d
    int m_11;                 // +0x11
    char m_pad15[0xf8];       // 保持占位总长 0x10d
};

class MSG_GROWTH_POWER : public MSG_BASE
{
public:
    MSG_GROWTH_POWER();
    // ORIG C1：+0xd byte 清零、memset(this+0xe,0,5)
    unsigned char m_0d;       // +0x0d
    char m_0e[5];             // +0x0e..+0x12（ctor 全零）
    char m_pad13[0xfa];       // 保持占位总长 0x10d
};

class MSG_GUILD_ATTENDANCE : public MSG_BASE
{
public:
    MSG_GUILD_ATTENDANCE();
    char m_pad[0x100];
};

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
};  // sizeof = 0x21

class STGuildBoardDBInfo
{
public:
    STGuildBoardDBInfo();            // members C1@0x84、ints 0x78/7c/80 清零、memset(this,0,0x78)
    char m_pad78[0x78];
    int m_78;
    int m_7c;
    int m_80;
    STGuildMemberCharacData m_members;   // +0x84
};  // sizeof = 0xa5

#pragma pack(push, 1)
class MSG_GUILD_BOARD : public MSG_BASE
{
public:
    MSG_GUILD_BOARD();
    STGuildBoardDBInfo m_info;       // +0xd..+0xb1（pack 后真落 0xd；ORIG add $0xd 实证）
    unsigned char m_b2;              // +0xb2（ORIG C1 清零）
    char m_padB3[0x5a];              // +0xb3（保持原总长 0x10d）
};
#pragma pack(pop)

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
    // ORIG C1：+0xd int = 0x80（notice 类型哨兵）
    int m_0d;                 // +0x0d（ctor = 0x80）
    char m_pad11[0xfc];       // 保持占位总长 0x10d
};

class MSG_INFORM_NOTICE_2ND : public MSG_BASE
{
public:
    MSG_INFORM_NOTICE_2ND();
    // ORIG C1：+0xd int = 0x80
    int m_0d;                 // +0x0d（ctor = 0x80）
    char m_pad11[0xfc];       // 保持占位总长 0x10d
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

#pragma pack(push, 1)
class MSG_ITEM_UPGRADE : public MSG_BASE
{
public:
    MSG_ITEM_UPGRADE();
    // ORIG C1：+0x25 word 清零、memset(this+0x28,0,0x81)、+0x27 byte 清零
    char m_padD[0x18];        // +0x0d..+0x24（ORIG 未出线）
    unsigned short m_25;      // +0x25
    unsigned char m_27;       // +0x27
    char m_28[0x81];          // +0x28..+0xa8（ctor memset 全零）
    char m_padA9[0x64];       // 保持占位总长 0x10d
};
#pragma pack(pop)

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
    // ORIG C1：word 拷贝链 +0x11 ← 0，+0xf ← +0x11，+0xd ← +0xf
    unsigned short m_0d;      // +0x0d（= m_0f）
    unsigned short m_0f;      // +0x0f（= m_11）
    unsigned short m_11;      // +0x11（ctor = 0）
    char m_pad13[0xfa];       // 保持占位总长 0x10d
};

class MSG_MOTION_HACK : public MSG_BASE
{
public:
    MSG_MOTION_HACK();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_MOVE_ITEMSPACE
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    char m_padTail[0x18];             // +0x25..+0x3c（帧推导总长 0x3d）
};   // MSG_MOVE_ITEMSPACE
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_MOVE_MAP
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    signed char m_x;              // +0x0d（move_map 目标格 x，movsbl 有符号扩展）
    signed char m_y;              // +0x0e（move_map 目标格 y）
    unsigned int m_fieldF;        // +0x0f（Secu_HackLogCheckByParty::
                                  //  CheckMoveMapFitInPathHackCnt 第 3 实参）
    unsigned int m_field13;       // +0x13（同上第 4 实参）
    unsigned char m_field17;      // +0x17（==1 时 CParty::move_map 先 checkLayeredMap）
    char m_pad18[2];              // +0x18..+0x19
    unsigned char m_1a;           // +0x1a（ORIG process 校验 ==1 跳过 0x25a hack 记数）
    unsigned short m_1b[4];       // +0x1b..+0x22（[0]=mapIndex：CBossTowerFunc::
                                  //   MoveMap 以 (ushort*)(msg+0x1b) 传入；
                                  //   [i] 亦为 CParty::checkMoveMapHackCnt
                                  //   按成员槽读取的击杀数 short 数组）
    unsigned int m_23[4];         // +0x23..+0x33（每成员 playtime tick，
                                  //  CheckOneMapPlayTimeHackCnt 逐槽修正/上报）
    char m_pad33[6];              // +0x33..+0x38
    unsigned char m_39;           // +0x39（process 回写 CParty::get_party_seatno）
    char m_padTail[8];            // +0x3a..+0x41（帧推导总长 0x42）
};
#pragma pack(pop)

class MSG_OBJECT_BRING_UP : public MSG_BASE
{
public:
    MSG_OBJECT_BRING_UP();
    char m_pad[0x100];
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

class MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE : public MSG_BASE
{
public:
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE();
    // ORIG C1：+0xd byte、+0xe int 清零
    unsigned char m_0d;       // +0x0d
    int m_0e;                 // +0x0e（pack 后紧贴；ORIG 单指令 movl 写入）
    char m_pad12[0xfb];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
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
#pragma pack(pop)

// ORIG 布局（SetPartyInfoUI 0x0859b234 读取实证）：+0x0d titleIndex、
// +0x12 title[0x20]、+0x32 userMax、+0x33 dungIndex(ushort)、+0x35 dungDiffi。
class MSG_PARTY_SET_INFO
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    unsigned char m_titleIndex;   // +0x0d
    char m_pad0e[4];              // +0x0e..0x11
    char m_title[0x20];           // +0x12（setTitle 拷贝源，0x20 同 CParty::m_title）
    unsigned char m_userMax;      // +0x32
    unsigned short m_dungIndex;   // +0x33
    unsigned char m_dungDiffi;    // +0x35
    char m_padTail[8];        // +0x36..+0x3d（帧推导总长 0x3e）
};

class MSG_PARTY_TELEPORT : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT();
    // ORIG C1：+0xd/+0xe byte、+0xf/+0x11 word、+0x13 byte 全零
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    unsigned short m_0f;      // +0x0f
    unsigned short m_11;      // +0x11
    unsigned char m_13;       // +0x13
    char m_pad14[0xf9];       // 保持占位总长 0x10d
};

class MSG_PARTY_TELEPORT_CONFIRM : public MSG_BASE
{
public:
    MSG_PARTY_TELEPORT_CONFIRM();
    // ORIG C1：+0xd byte 清零
    unsigned char m_0d;       // +0x0d
    char m_pad0e[0xff];       // 保持占位总长 0x10d
};

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
    char m_pad19[0xf4];       // 保持占位总长 0x10d
};
#pragma pack(pop)

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

class MSG_PVP_TIMEOUT
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x3f];         // +0xd..（帧推导总长 0x3f；内部布局未还原）
};

class MSG_QUEST_SHOP : public MSG_BASE
{
public:
    MSG_QUEST_SHOP();
    // ORIG C1：+0xd/+0xe byte 清零
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    char m_pad0f[0xfe];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_QUICK_CHAT : public MSG_BASE
{
public:
    MSG_QUICK_CHAT();
    // ORIG C1：+0xd int、+0x11 byte 清零；CQuickChattingOption 构造于 +0x12
    int m_0d;                      // +0x0d
    unsigned char m_11;            // +0x11
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
    char m_pad11[0xfc];       // 保持占位总长 0x10d
};

class MSG_REFUND_SKILL : public MSG_BASE
{
public:
    MSG_REFUND_SKILL();
    // ORIG C1：+0xd int 清零、+0x11..+0x13 byte 清零
    int m_0d;                 // +0x0d
    unsigned char m_11;       // +0x11
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    char m_pad14[0xf9];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_REGENERATION_RANDOM_OPTION
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    // ORIG Dispather_RegenerationRandomOption::process：
    //   两装备槽 u16@0xd/0xf；两源选项序号 s16@0x11/0x13（Regenerate 第 3/4 参）
    unsigned short m_equipSlot0;  // +0x0d
    unsigned short m_equipSlot1;  // +0x0f
    short m_srcOptIndex0;         // +0x11
    short m_srcOptIndex1;         // +0x13
};
#pragma pack(pop)

class MSG_REGISTER_TO_BLACKLIST : public MSG_BASE
{
public:
    MSG_REGISTER_TO_BLACKLIST();
    char m_pad[0x100];
};

class MSG_REMOVE_FRIEND
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x37];         // +0xd..（帧推导总长 0x37；内部布局未还原）
};

class MSG_RENAME_CREATURE : public MSG_BASE
{
public:
    MSG_RENAME_CREATURE();
    // ORIG C1：memset(this+0x14,0,0xd)
    char m_padD[7];           // +0x0d..+0x13
    char m_14[0xd];           // +0x14..+0x20（ctor memset 全零）
    char m_pad21[0xec];       // 保持占位总长 0x10d
};

class MSG_RENT_ITEM : public MSG_BASE
{
public:
    MSG_RENT_ITEM();
    // ORIG C1：+0xd/+0xe byte 清零、memset(this+0xf,0,2)
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    char m_0f[2];             // +0x0f..+0x10（ctor memset 全零）
    char m_pad11[0xfc];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_REPAIR_EQUIP
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    char m_pad0e[0xff];       // 保持占位总长 0x10d
};

class MSG_REQUEST_DISJOINT_ITEM : public MSG_BASE
{
public:
    MSG_REQUEST_DISJOINT_ITEM();
    char m_pad[0x100];
};

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
    char m_pad46[0xc7];       // 保持占位总长 0x10d
};
#pragma pack(pop)

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
    // ORIG DispatcherResetRandomOption::process 与 C1 偏移一致：+0xd 为 gap
    char m_padD;
    unsigned short m_equipSlot;    // +0x0e（目标装备槽；GetInvenRef(1, slot)）
    unsigned short m_scrollSlot;   // +0x10（再铸卷轴槽；use_item 消耗）
    unsigned char m_rarityChoice;  // +0x12（玩家选择稀有度，须 <=2）
    char m_padTail[1];        // +0x13（帧推导总长 0x14）
};
#pragma pack(pop)

class MSG_RIGHT_OF_CHANGE_GROW_TYPE : public MSG_BASE
{
public:
    MSG_RIGHT_OF_CHANGE_GROW_TYPE();
    // ORIG C1：+0xd word、+0xf byte 清零
    unsigned short m_0d;      // +0x0d
    unsigned char m_0f;       // +0x0f
    char m_pad10[0xfd];       // 保持占位总长 0x10d
};

class MSG_SAVE_CHARACTER_OPTION : public MSG_BASE
{
public:
    MSG_SAVE_CHARACTER_OPTION();
    // ORIG C1：CETCOption 构造于 +0x11（CGameOption.h，inline clear）
    char m_padD[4];            // +0x0d..+0x10
    CETCOption m_option;       // +0x11（sizeof 0x48）
};

class MSG_SAVE_GAME_OPTION_1 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_1();
    // ORIG C1：CETCOption 构造于 +0x11（CGameOption.h，inline clear）
    char m_padD[4];            // +0x0d..+0x10
    CETCOption m_option;       // +0x11（sizeof 0x48）
};

class MSG_SAVE_GAME_OPTION_2 : public MSG_BASE
{
public:
    MSG_SAVE_GAME_OPTION_2();
    // ORIG C1：CETCOption 构造于 +0x11（CGameOption.h，inline clear）
    char m_padD[4];            // +0x0d..+0x10
    CETCOption m_option;       // +0x11（sizeof 0x48）
};

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
        unsigned char m_slot;   // +0x00
        unsigned char m_b;      // +0x01
        std::string m_str;      // +0x02
    } __attribute__((packed));

    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON();
    // ORIG C1：vector<_slot_String> 构造于 +0xf
    char m_padD[2];                 // +0x0d..+0x0e
    std::vector<_slot_String> m_slots;   // +0xf
};

#pragma pack(push, 1)
class MSG_SAVE_QUEST_NOTIFY : public MSG_BASE
{
public:
    MSG_SAVE_QUEST_NOTIFY();
    // ORIG C1：memset(this+0xd,0,0x28)
    char m_0d[0x28];          // +0x0d..+0x34（ctor memset 全零）
    char m_pad35[0xd8];       // 保持占位总长 0x10d
};
#pragma pack(pop)

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
class MSG_SELECT_DUNGEON
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    char m_pad0f[0xfe];       // 保持占位总长 0x10d
};

class MSG_SERIAROOM_DECO_EVENT : public MSG_BASE
{
public:
    MSG_SERIAROOM_DECO_EVENT();
    char m_pad[0x100];
};

#pragma pack(push, 1)
class MSG_SET_READY_STATE
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    unsigned char m_0d;   // +0x0d（ready 标志，ORIG process 以 (m_0d!=0) 传入
                          //  PvP_Room::set_ready_state）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SET_USER_AREA
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    char m_pad[0x100];
};

class MSG_SKILL_INIT : public MSG_BASE
{
public:
    MSG_SKILL_INIT();
    // ORIG C1：+0xd byte 清零
    unsigned char m_0d;       // +0x0d
    char m_pad0e[0xff];       // 保持占位总长 0x10d
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
    stPartyTelePort m_tp;     // +0x38（sizeof 0xc，ctor 调 reset()）
    int m_44;                 // +0x44
};  // 总长 0x48 真实布局
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_SORT_ITEM
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    // 布局证据：DisPatcher_SortItem::process(0x081c9c54)：仅读 +0x0d byte。
    unsigned char m_invenType;        // +0x0d
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
    // ORIG C1：+0xd/+0xe byte 清零
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    char m_pad0f[0xfe];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_USERINFO_REQ
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    unsigned short m_targetId;  // +0xd（GameWorld::get_user_info 目标 id）
    unsigned char m_slot;       // +0xf
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
class MSG_USER_SET_POS
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    unsigned short m_0d;  // +0x0d（x）
    unsigned short m_0f;  // +0x0f（y）
    signed char m_11;     // +0x11（z，sbyte）
    unsigned short m_12;  // +0x12（dir，传 GameWorld::move_position 第 5 参）
};
#pragma pack(pop)

#pragma pack(push, 1)
class MSG_USE_COIN
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
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
    char m_pad12[0xfb];       // 保持占位总长 0x10d
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
    // ORIG C1：+0xe word 清零（+0xd 未出线）
    char m_padD;
    unsigned short m_0e;      // +0x0e
    char m_pad10[0xfd];       // 保持占位总长 0x10d
};

#pragma pack(push, 1)
class MSG_USE_SKILL
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    // ORIG DisPatcher_UseSkill::process：技能槽 u8@0xe；目标 u16@0xf（use_skill 第 3 参）
    unsigned char m_slot;        // +0x0e
    unsigned short m_targetId;   // +0x0f（SKILL_USING_VALIDATE_INFO 槽位 / CParty::use_skill 目标）
    char m_padTail[2];        // +0x11..+0x12（帧推导总长 0x13）
};
#pragma pack(pop)

class MSG_WALKOUT_PARTY
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_walkoutSlot;       // +0xd（包头后首字节：被踢成员槽位，walkout_member 第二参）
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

class MsgLimitNpcBuyItem
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x100];
};

class MsgMercenaryCompetition
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x100];
};

class MsgMercenaryReturn
{
public:
    char m_head[0xd];         // +0x0 包头（standalone POD：ORIG 该类型无 C1/D1，不派生 MSG_BASE）
    char m_pad[0x100];
};

class MsgRequestAssaultPrice : public MSG_BASE
{
public:
    MsgRequestAssaultPrice();
    // ORIG C1：+0xd word = -1（未定价哨兵）
    short m_0d;               // +0x0d（ctor = -1）
    char m_pad0f[0xfe];       // 保持占位总长 0x10d
};

class MsgTournamentRewardSelect : public MSG_BASE
{
public:
    MsgTournamentRewardSelect();
    // ORIG C1：+0xd/+0xe byte = 0xff（未选择哨兵）
    unsigned char m_0d;       // +0x0d
    unsigned char m_0e;       // +0x0e
    char m_pad0f[0xfe];       // 保持占位总长 0x10d
};

// ---------------------------------------------------------------------------
// Param 类型（占位大小 0x40，后续按 ORIG 构造函数修正）
// ---------------------------------------------------------------------------
class AbnormalUseStackable : public ParamBase
{
public:
    AbnormalUseStackable();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
    char m_pad05[0x3f];       // 保持占位总长 0x44
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
    // ORIG C1 直接六连清零 +0x80..+0x94（ORIG 无独立 Mid 类型/符号 —— 平铺字段）
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
    // ORIG C1：+0x04 byte 清零（CSecu_ProtectionField::Check 首数据字节）
    unsigned char m_4;       // +0x04
    char m_pad05[0x3f];       // 保持占位总长 0x44
};

class InformNoticeData : public ParamBase
{
public:
    InformNoticeData();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class InformNoticeData2nd : public ParamBase
{
public:
    InformNoticeData2nd();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class LimitNpcBuyItem : public ParamBase
{
public:
    LimitNpcBuyItem();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 byte、+0x06 word、+0x08 int 清零
    unsigned char m_4;       // +0x04
    short m_6;               // +0x06（自然对齐）
    int m_8;                 // +0x08
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class STQuestShopParam : public ParamBase
{
public:
    STQuestShopParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class STUdateItemParam : public ParamBase
{
public:
    STUdateItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class ScanBotByDrvParam : public ParamBase
{
public:
    ScanBotByDrvParam();
    // ORIG C1：+0x04 int、+0x08 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    char m_pad09[0x3b];       // 保持占位总长 0x44
};

class ScriptHashParam : public ParamBase
{
public:
    ScriptHashParam();
    // ORIG C1：+0x04/+0x05 byte、+0x08 int 清零
    unsigned char m_4;       // +0x04
    unsigned char m_5;       // +0x05
    int m_8;                 // +0x08（自然对齐推入）
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class SecuCardRpyParam : public ParamBase
{
public:
    SecuCardRpyParam();
    // ORIG C1：+0x04/+0x05 byte 清零
    unsigned char m_4;       // +0x04
    unsigned char m_5;       // +0x05
    char m_pad06[0x3e];       // 保持占位总长 0x44
};

// ORIG _ZN25competition_reward_resultC1Ev：+0 int、+4 int 清零、+8 byte 清零，sizeof 0xc
class competition_reward_result
{
public:
    competition_reward_result();
    int m_0;
    int m_4;
    unsigned char m_8;
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
    // ORIG C1：competition_reward_result 出线构造于 +0x0c
    char m_pad04[8];                     // +0x04..+0x0b
    competition_reward_result m_result;  // +0x0c（类型定义已前移至本类上方）
};

class TempParam : public ParamBase
{
public:
    TempParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stBlueMarbleConfirmInfo : public ParamBase
{
public:
    stBlueMarbleConfirmInfo();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

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
    char m_pad10[0x34];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 word 清零
    short m_4;               // +0x04
    char m_pad06[0x3e];       // 保持占位总长 0x44
};

class stChangeHostWarRoom : public ParamBase
{
public:
    stChangeHostWarRoom();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int、+0x08/+0x09 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    unsigned char m_9;       // +0x09
    char m_pad0a[0x3a];       // 保持占位总长 0x44
};

class stClientSpecStatisticParam : public ParamBase
{
public:
    stClientSpecStatisticParam();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
    char m_pad05[0x3f];       // 保持占位总长 0x44
};

class stCloseDisjointStoreParam : public ParamBase
{
public:
    stCloseDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stCollectItems : public ParamBase
{
public:
    stCollectItems();
    // ORIG C1：+0x04 int、+0x08/+0x0a word 清零
    int m_4;                 // +0x04
    short m_8;               // +0x08
    short m_a;               // +0x0a
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class stComboSkill : public ParamBase
{
public:
    stComboSkill();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stCreateDisjointStoreParam : public ParamBase
{
public:
    stCreateDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stDieCharacInDungeon : public ParamBase
{
public:
    stDieCharacInDungeon();
    int m_errNo;    // +0x04（ORIG process 写 check_error 结果/0x7fffffff）
};

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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stEnchantParam : public ParamBase
{
public:
    stEnchantParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stEnterDisjointStoreParam : public ParamBase
{
public:
    stEnterDisjointStoreParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stEquipslotSwitch : public ParamBase
{
public:
    stEquipslotSwitch();
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
    char m_pad05[0x3f];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 byte 清零
    unsigned char m_4;       // +0x04
    char m_pad05[0x3f];       // 保持占位总长 0x44
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
    char m_pad09[0x3b];       // 保持占位总长 0x44
};

class stGrowthCreature : public ParamBase
{
public:
    stGrowthCreature();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildBoardParam : public ParamBase
{
public:
    stGuildBoardParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildCargoHistoryParam : public ParamBase
{
public:
    stGuildCargoHistoryParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildCargoMoveItemParam : public ParamBase
{
public:
    stGuildCargoMoveItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildCargoParam : public ParamBase
{
public:
    stGuildCargoParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildCargoPopItemParam : public ParamBase
{
public:
    stGuildCargoPopItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stGuildCargoPushItemParam : public ParamBase
{
public:
    stGuildCargoPushItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stHatchCreature : public ParamBase
{
public:
    stHatchCreature();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stImageCommunicationEquipmentUse : public ParamBase
{
public:
    stImageCommunicationEquipmentUse();
    // ORIG C1：+0x04/+0x08/+0x0c int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    int m_c;                 // +0x0c
    char m_pad10[0x34];       // 保持占位总长 0x44
};

class stItemDictionary : public ParamBase
{
public:
    stItemDictionary();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stItemHyperLinkMessage : public ParamBase
{
public:
    stItemHyperLinkMessage();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad[0x40];
};

class stMailboxExtractParam : public ParamBase
{
public:
    stMailboxExtractParam();
    // ORIG C1：+0x04/+0x08 int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class stMailboxOpenParam : public ParamBase
{
public:
    stMailboxOpenParam();
    // ORIG C1：+0x04/+0x08 int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class stMailboxSendParam : public ParamBase
{
public:
    stMailboxSendParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
};

class stOnGetUserInfoParam : public ParamBase
{
public:
    stOnGetUserInfoParam();
    // ORIG C1：+0x04 word、+0x06 byte 清零
    short m_4;               // +0x04
    unsigned char m_6;       // +0x06
    char m_pad07[0x39];       // 保持占位总长 0x44
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
    unsigned char m_invenType;        // +0x04（回显 msg+0xd，ORIG process 写入并回读分支）
    unsigned char m_gap5[3];          // +0x05..+0x07
    int m_result;                     // +0x08（sort 返回值；ctor 清零）
};

class stOnUseEquipParam : public ParamBase
{
public:
    stOnUseEquipParam();
    int m_errNo;            // +0x04（use_equip 返回码）
    unsigned char m_8;      // +0x08（回显 MSG_DECREASE_DURABILITY m_0d）
};

class stOpenCeraPackage : public ParamBase
{
public:
    stOpenCeraPackage();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x145];      // ORIG 实证用到 +0x148（Dispatcher_CeraPackageOpen read），总长 0x149
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
};

class stRegisterToBlackListParam : public ParamBase
{
public:
    stRegisterToBlackListParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad0c[0x38];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stRequestIngameAdvertisement : public ParamBase
{
public:
    stRequestIngameAdvertisement();
    char m_pad[0x644];        // ORIG 实证用到 +0x643（RequestIngameAdvertisement 包字段区），总长 0x648
};

class stRequestMemberEnterParam : public ParamBase
{
public:
    stRequestMemberEnterParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stSecretShopBuyItemParam : public ParamBase
{
public:
    stSecretShopBuyItemParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stSecretShopOpenCloseParam : public ParamBase
{
public:
    stSecretShopOpenCloseParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    // ORIG C1：+0x04 int、+0x08/+0x0a word 清零
    int m_4;                 // +0x04
    short m_8;               // +0x08
    short m_a;               // +0x0a
    char m_pad0c[0x38];       // 保持占位总长 0x44
};

class stSetAreaParam : public ParamBase
{
public:
    stSetAreaParam();
    int m_errNo;          // +0x04（三参 check_error 结果；>=1 直接返回 0）
    unsigned char m_8;    // +0x08（ORIG C1 清零）
    unsigned char m_9;    // +0x09（ORIG C1 清零）
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

class stSkillInit : public ParamBase
{
public:
    stSkillInit();
    // ORIG C1：+0x04 int、+0x08 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    char m_pad09[0x3b];       // 保持占位总长 0x44
};

class stSoloTeleportParam : public ParamBase
{
public:
    stSoloTeleportParam();
    // ORIG C1：+0x04/+0x08/+0x0c int 清零
    int m_4;                 // +0x04
    int m_8;                 // +0x08
    int m_c;                 // +0x0c
    char m_pad10[0x34];       // 保持占位总长 0x44
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
    int m_8;                          // +0x08（ORIG C1 清零）
    int m_c;                          // +0x0c（ORIG C1 = 2）
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
    // ORIG C1：+0x04 int、+0x08/+0x09 byte 清零
    int m_4;                 // +0x04
    unsigned char m_8;       // +0x08
    unsigned char m_9;       // +0x09
    char m_pad0a[0x3a];       // 保持占位总长 0x44
};

class stUpgradeDisjointMachineParam : public ParamBase
{
public:
    stUpgradeDisjointMachineParam();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad12[0x32];      // 保持占位总长 0x44
};

class stUseRenameCard : public ParamBase
{
public:
    stUseRenameCard();
    // ORIG C1：+0x04 int 清零
    int m_4;                 // +0x04
    char m_pad08[0x3c];       // 保持占位总长 0x44
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
    char m_pad08[0x3c];       // 保持占位总长 0x44
};

#endif  // DNF_GAME_PACKET_TYPES_H_

