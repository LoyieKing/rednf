#ifndef DNF_GAME_SIG_TYPES_H_
#define DNF_GAME_SIG_TYPES_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构类型（批量生成）
// 各结构成员布局待细化；当前用 char 数组占位以生成 GetInBuffer<T> 实例。
// ============================================================================
#include <cstddef>  // offsetof（SIG_* 布局 static_assert）

#include "Inven_Item.h"  // SIG_SAVE_INVENTORY 背包 Inven_Item 数组（pack(1)，0x3d）
#include "MissionTypes.h"  // MissionInfo（SIG_REQUEST_PVP_MISSION_LIST 任务列表）

struct stAvatarEmblemInfo_t;  // GamePODTypes.h 完整定义（SIG_AVATAR_INSERT::setJewelSocket 引用）

struct InformNoticeFlagData
{
    char m_pad[0x14];
};

// ORIG sizeof = 0x3f84（MSG_CHECK_MAIL_RECEIVER::set<1, MSG_BUY_CERASHOP_ITEM> @0x82378ee
// `movl $0x3f84,0x8(%esp); call memcpy`；交叉验证 Dispatcher_BuyAutomatItem / ARAD db_dispatch）。
// 条目区：0x36 itemCount，0x39 起逐条目步长 0x1cf（+0x37 槽位 uchar、+0x38 标签 uchar、
//  +0x39 productNo、+0x3d subCount、+0x3e 子项数组、+0xed optCount、+0xee 选项数组）。
// 保持 m_pad 形态（避免与 Dispatcher_BuyCeraShopItem_impl body[offset] 裸访问冲突），
// 仅修正缓冲尺寸以匹配 ORIG。
struct MSG_BUY_CERASHOP_ITEM
{
    char m_pad[0x3f84];
};
static_assert(sizeof(MSG_BUY_CERASHOP_ITEM) == 0x3f84, "MSG_BUY_CERASHOP_ITEM ABI drift");

struct Packet_Tcp_Server_Connect
{
    char m_pad[0xb];
};

struct SIG_ACCOUNT_CARGO_DATA
{
    char m_pad[0xd60];
};

struct SIG_ARRANGE_USER_LOGIN_INFO
{
    char m_pad[0xc];
};

#pragma pack(push, 1)

// ---- 拍卖/点券域 SIG_AUCTION_*（ORIG 尺寸权威：Stream::GetInBuffer<T> 显式
// 实例的 enable_in 常量，objdump 实测，见 /tmp/df_batches/PacketServerDispatch.md）----

// ORIG sizeof=5（实例 0x847de2e enable_in(5)）。
struct SIG_AUCTION_NOTIFY_AUCTION_SERVICE
{
    int m_result;            // +0x00（AG 域写 0，PG 域写 1）
    unsigned char m_flag;    // +0x04 附加标志（0/1）
};
static_assert(sizeof(SIG_AUCTION_NOTIFY_AUCTION_SERVICE) == 5,
              "SIG_AUCTION_NOTIFY_AUCTION_SERVICE ABI drift");

// ORIG sizeof=0x24（实例 0x847de98）。
struct SIG_AUCTION_RESULT_ASK_PRICE
{
    int m_result;            // +0x00（AG 路径 0，PG 路径 1）
    int m_characNo;          // +0x04 【推断：询价角色】←包+0x16
    int m_itemIdx;           // +0x08 【推断：物品编号】←包+0x1a
    int m_priceCur[3];       // +0x0c AG←包+0x1e 起 3×int；PG 全置 0【推断】
    int m_priceTop[3];       // +0x18 AG←包+0x2a 起 3×int；PG 全置 0【推断】
};
static_assert(sizeof(SIG_AUCTION_RESULT_ASK_PRICE) == 0x24,
              "SIG_AUCTION_RESULT_ASK_PRICE ABI drift");

// ORIG sizeof=0x2d（实例 0x847df02）。
struct SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM
{
    int m_result;            // +0x00（AG 路径 0，PG 路径 1，均最后写）
    int m_characNo;          // +0x04 【推断】←包+0x16
    unsigned short m_count;  // +0x08 ←包+0x1a
    unsigned char m_flagA;   // +0x0a ←包+0x1c
    unsigned short m_flagB;  // +0x0b ←包+0x1d
    int m_itemNums[8];       // +0x0d AG←包+0x1f 起 8×int；PG 路径不写（保持流缓冲原值，忠实 ORIG）
};
static_assert(sizeof(SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM) == 0x2d,
              "SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM ABI drift");

// ORIG sizeof=0x0a（REGIST 实例 0x847df6c）。
struct SIG_AUCTION_RESULT_REGIST
{
    int m_result;             // +0x00（AG 路径 0，PG 路径 1）
    int m_characNo;           // +0x04 【推断】←包+0x16
    unsigned char m_okFlag;   // +0x08 ←包+0x1a
    unsigned char m_errCode;  // +0x09 ←包+0x1b
};
static_assert(sizeof(SIG_AUCTION_RESULT_REGIST) == 0x0a,
              "SIG_AUCTION_RESULT_REGIST ABI drift");

// ORIG sizeof=0x3a（实例 0x847dfd6）。
struct SIG_AUCTION_RESULT_BIDDING
{
    int m_result;             // +0x00（AG 路径 0，PG 路径 1）
    int m_characNo;           // +0x04 【推断】←包+0x16
    int m_money;              // +0x08 【推断：出价额】←包+0x1a
    unsigned char m_flagA;    // +0x0c ←包+0x1e
    unsigned char m_flagB;    // +0x0d ←包+0x1f
    int m_field0e;            // +0x0e ←包+0x20
    int m_bidPrice;           // +0x12 【推断】←包+0x24
    char m_sellerName[0x20];  // +0x16 【推断：卖家名】strncpy←包+0x28（仅 PG 路径写入）
    int m_field36;            // +0x36 ←包+0x48（仅 PG 路径写入）
};
static_assert(sizeof(SIG_AUCTION_RESULT_BIDDING) == 0x3a,
              "SIG_AUCTION_RESULT_BIDDING ABI drift");

// ORIG sizeof=0x0a（CANCEL 实例 0x847e040，与 REGIST 同布局）。
struct SIG_AUCTION_RESULT_REGIST_CANCEL
{
    int m_result;             // +0x00（AG 路径 0，PG 路径 1）
    int m_characNo;           // +0x04 【推断】←包+0x16
    unsigned char m_okFlag;   // +0x08 ←包+0x1a
    unsigned char m_errCode;  // +0x09 ←包+0x1b
};
static_assert(sizeof(SIG_AUCTION_RESULT_REGIST_CANCEL) == 0x0a,
              "SIG_AUCTION_RESULT_REGIST_CANCEL ABI drift");

// ORIG sizeof=0x21（实例 0x847e0aa）。
// 消费方 cUserHistoryLog::Auction（ORIG 0x8685452，"Auction_reg_s/_f/bid_s/_f"）。
struct SIG_AUCTION_LOG_MESSAGE_AG
{
    unsigned char m_type;      // +0x00（0=reg_s 1=reg_f 2=bid_s 3=bid_f）
    unsigned long long m_key;  // +0x01 【推断：日志主键/characNo】←包+0x17:+0x1b（两条 4B 写拼 8B）
    unsigned int m_arg09;      // +0x09 ←包+0x1f
    int m_arg0d;               // +0x0d ←包+0x23（bid 路径日志打印）
    unsigned int m_arg11;      // +0x11 ←包+0x27
    unsigned char m_arg15;     // +0x15 ←包+0x2b
    int m_arg16;               // +0x16 ←包+0x2c
    unsigned short m_arg1a;    // +0x1a ←包+0x30
    int m_arg1c;               // +0x1c ←包+0x32
    unsigned char m_arg20;     // +0x20 ←包+0x36
};
static_assert(sizeof(SIG_AUCTION_LOG_MESSAGE_AG) == 0x21,
              "SIG_AUCTION_LOG_MESSAGE_AG ABI drift");

// ORIG sizeof=0x20（实例 0x847e17e；与 AG 同前段、无尾字节）。
// 消费方 cUserHistoryLog::AuctionPoint（ORIG 0x8685654，"POINT_*"）。
struct SIG_AUCTION_LOG_MESSAGE_PG
{
    unsigned char m_type;      // +0x00
    unsigned long long m_key;  // +0x01 【推断】←包+0x17:+0x1b
    unsigned int m_arg09;      // +0x09 ←包+0x1f
    int m_arg0d;               // +0x0d ←包+0x23
    unsigned int m_arg11;      // +0x11 ←包+0x27
    unsigned char m_arg15;     // +0x15 ←包+0x2b
    int m_arg16;               // +0x16 ←包+0x2c
    unsigned short m_arg1a;    // +0x1a ←包+0x30
    int m_arg1c;               // +0x1c ←包+0x32
};
static_assert(sizeof(SIG_AUCTION_LOG_MESSAGE_PG) == 0x20,
              "SIG_AUCTION_LOG_MESSAGE_PG ABI drift");

// ORIG sizeof=0x28（实例 0x823b368）。
// 生产方 Dispatcher_AuctionBidding/BuyItemApiece、DB_AuctionSubMoney::dispatch
// （memset 0x28 后写 +0/+4，strncpy(+8, name, 0xd)）。
struct SIG_AUCTION_MONEY_SUB
{
    int m_count;                  // +0x00（DB 路径写计数）
    int m_arg04;                  // +0x04 【推断：characNo/金额，按调用方】
    char m_name[0xd];             // +0x08 【推断：角色名】strncpy 0xd
    unsigned char m_pad15[0x13];  // +0x15..0x27 ORIG 未用区（memset 覆盖）
};
static_assert(sizeof(SIG_AUCTION_MONEY_SUB) == 0x28,
              "SIG_AUCTION_MONEY_SUB ABI drift");

// ORIG sizeof=0x2a（实例 0x847e1e8）。
struct SIG_AUCTION_OWNER_IS_VIP
{
    int m_characNo;        // +0x00 ←包+0x12
    int m_arg04;           // +0x04 ←包+0x16
    int m_arg08;           // +0x08 ←包+0x1a
    unsigned char m_flagC; // +0x0c ←包+0x1e
    int m_arg0d;           // +0x0d ←包+0x1f
    int m_arg11;           // +0x11 ←包+0x23
    char m_ownerName[0xd]; // +0x15 【推断：所有者名】strcpy←包+0x27
    int m_arg22;           // +0x22 ←包+0x34
    int m_arg26;           // +0x26 ←包+0x38
};
static_assert(sizeof(SIG_AUCTION_OWNER_IS_VIP) == 0x2a,
              "SIG_AUCTION_OWNER_IS_VIP ABI drift");

// ORIG sizeof=0x12（实例 0x847e114）。
struct SIG_AUCTION_RESULT_BUY_ITEM_APIECE
{
    int m_result;           // +0x00 ←包+0x16（直拷，非固定值）
    int m_characNo;         // +0x04 【推断】←包+0x1a
    unsigned char m_flagA;  // +0x08 ←包+0x1e
    unsigned char m_flagB;  // +0x09 ←包+0x1f
    int m_arg0a;            // +0x0a ←包+0x20
    int m_price;            // +0x0e 【推断：成交价】←包+0x24
};
static_assert(sizeof(SIG_AUCTION_RESULT_BUY_ITEM_APIECE) == 0x12,
              "SIG_AUCTION_RESULT_BUY_ITEM_APIECE ABI drift");

#pragma pack(pop)

struct SIG_AUTO_MARKET_CONDITIONS_CONTROL
{
    char m_pad[0x6c];
};

struct SIG_AUTO_PUNISH_RULE_BLACK_IP
{
    char m_pad[0x3e84];
};

struct SIG_AUTO_PUNISH_RULE_HACK_TYPE
{
    char m_pad[0x2720];
};

struct SIG_AVATAR_CHANGE_OWNER
{
    char m_pad[0xd];
};

struct SIG_AVATAR_CHANGE_STAT
{
    char m_pad[0x51];
};

struct SIG_AVATAR_COLOR_CHANGE
{
    char m_pad[0xc];
};

struct SIG_AVATAR_INSERT
{
    // ORIG 布局（set 0x850d3c8 / setJewelSocket 0x850d44c）：
    //  +0x00 int / +0x04 uint / +0x08 uchar(a2) / +0x09 uchar(bSet) / +0x0a int(expire)
    //  +0x0e uchar(expire low) / +0x0f agency[0x18] / +0x28 int / +0x2c uchar
    //  +0x2d stAvatarEmblemInfo_t(0x1e)
    unsigned char m_data[0x53];   // ORIG GetInBuffer 实例化 sizeof=0x53（0x850d3c8 布局尾 +0x2d+0x1e=0x4b 对齐补足）

    static void set(SIG_AVATAR_INSERT* ins, int a1, unsigned int a2, int a3, int expire,
                    bool bSet, char c, const char* agency, int a7, unsigned long a8);
    static void setJewelSocket(SIG_AVATAR_INSERT* ins,
                               const stAvatarEmblemInfo_t& emblem);
};
struct SIG_AVATAR_OPTION_CHANGE
{
    char m_pad[0xc];
};

struct SIG_BEST_CLEAR_TIME
{
    char m_pad[0x5dc8];
};

struct SIG_BLOOD_DUNGEON
{
    char m_pad[0x28];
};

struct SIG_BREAK_AWAY_PREVENT_MESSAGE
{
    char m_pad[0x8];
};

struct SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER
{
    char m_pad[0x2e4];
};

struct SIG_BREAK_AWAY_UV_LIST
{
    char m_pad[0x1f];
};

struct SIG_CERASHOP_PURCHASE_COUNT
{
    char m_pad[0x10];
};

struct SIG_CHANGE_CHARAC_NAME
{
    char m_pad[0x30];
};

struct SIG_CHARACTER_OPTION
{
    char m_pad[0x204];
};

struct SIG_CHECK_ADD_FRIEND
{
    char m_pad[0x24];
};

struct SIG_CHECK_CHARAC_NAME
{
    char m_pad[0x24];
};

// 具名化（G6 HackAnM 批次；DB_SelectCharacMid::makeRequest ORIG T 0x0844676c
// 报告偏移：+0/+4/+8/+0xc uint 四域，+0x10 strncpy(name,0x1e)，尾 2 字节对齐）。
struct SIG_CHECK_CHAR_MID
{
    unsigned int m_field00;
    unsigned int m_field04;
    unsigned int m_field08;
    unsigned int m_field0c;
    char m_name[0x1e];           // +0x10（strncpy 目标）
    char m_pad2e[2];             // +0x2e 对齐尾（sizeof 保持 0x30）
};

struct SIG_CHECK_CHAR_MID_RESULT
{
    char m_pad[0x14];
};

struct SIG_CHECK_JOIN_GUILD
{
    char m_pad[0x1c];
};

struct SIG_CHECK_JOIN_GUILD_DB_REPLY
{
    char m_pad[0x44];
};

struct SIG_CHECK_MY_ACCOUNT_IN_DB
{
    char m_pad[0x29];
};

struct SIG_CHECK_MY_ACCOUNT_OK
{
    char m_pad[0xc];
};

struct SIG_CHECK_USER_SECU_REWARD
{
    char m_pad[0x8];
};

// ---- 组合技数据（0xe 字节：技能索引 + 6 个连招技能）----
#pragma pack(push, 1)
struct comoboSkillData
{
    short m_comboSkillIndex;     // +0x00
    short m_comboSkillList[6];   // +0x02
};
#pragma pack(pop)

// ---- ComboSkill 存档结构 SIG_COMBO_SKILL（pack(1)；+0xa 起奇数偏移由 ORIG 确认）----
// ORIG mangled 名 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL（0x864dcf2）。
// sizeof = 0xb2；SIG_LOAD_ETC 中 combo 字段占 [0x14aaa, 0x14b60) = 0xb6（尾 4 字节未用）。
// 原定义在 SkillSlot.h（FixStubS2 曾以 pad 镜像占位并因 ODR 冲突移除）；现迁移为本头权威，
// SkillSlot.h 改 #include "SigTypes.h" 引用（FixMissingSigTypes，广播周知）。
#pragma pack(push, 1)
struct SIG_COMBO_SKILL
{
    int m_characNo;                  // +0x00
    unsigned char m_tree1Count;      // +0x04
    unsigned char m_tree2Count;      // +0x05
    int m_job;                       // +0x06
    comoboSkillData m_tree1Data[6];  // +0x0a
    comoboSkillData m_tree2Data[6];  // +0x5e
};
#pragma pack(pop)
static_assert(sizeof(SIG_COMBO_SKILL) == 0xb2, "SIG_COMBO_SKILL ABI drift");

struct SIG_CREATE_ACCOUNT_CARGO_RESULT
{
    char m_pad[0x8];
};

#pragma pack(push, 1)
// 真实布局（DisPatcher_CreateCharac::process 0x081c041e 反汇编实证，memset 0x5354）：
struct SIG_CREATE_CHARAC
{
    unsigned int m_accId;              // +0x000
    char m_name[0x1e];                 // +0x004（ORIG strncpy(dst+4, msg+0x12, 0x1e)）
    unsigned char m_slot;              // +0x022（← MSG_CREATE_CHARACTER.m_slot）
    unsigned char m_field23;           // +0x023（← msg+0x2f）
    unsigned char m_eventCharacFlag;   // +0x024（CUserCharacInfo::getUserEventCharacterFlag）
    unsigned char m_pad25;
    unsigned short m_field26;          // +0x026（角色创建表 job*0x7dc +0 字）
    unsigned char m_jobData[80];       // +0x028（同表 +2..0x51 共 80 字节）
    unsigned char m_field78;           // +0x078（同表 +0x7d4 字节）
    unsigned char m_masteredSkillA[0x198]; // +0x079（addSkillOnCreateCharacter 树 A，
                                           //       元素=_Mastered_skill(2B)×204，定义见 SkillSlot.h）
    unsigned char m_masteredSkillB[0x198]; // +0x211（树 B）
    unsigned char m_pad3a9[0x4e01 - 0x3a9]; // +0x3a9（未建模区）
    Inven_Item m_giftItem;             // +0x4e01（makeGift @0x81c0a1a：职业赠礼道具，
                                       //       id=case(job) 映射，find_item 后 make_item 填充）
    unsigned char m_pad4e3e[0x4e7b - 0x4e3e]; // +0x4e3e
    Inven_Item m_weaponItem;           // +0x4e7b（CREATECHARACTEREQUIP[job*2] 初始武器）
    unsigned char m_pad4eb8[0x4ef5 - 0x4eb8]; // +0x4eb8
    Inven_Item m_clothesItem;          // +0x4ef5（CREATECHARACTEREQUIP[job*2+1] 初始衣服）
    unsigned char m_pad4f32[0x533f - 0x4f32]; // +0x4f32
    unsigned char m_giftFindOk;        // +0x533f（makeGift：赠礼 find_item 命中 1/失败 0）
    unsigned char m_equipFindOk;       // +0x5340（makeGift：武器/衣服 find_item 均命中 1/否则 0）
    unsigned char m_pad5341[3];        // +0x5341..+0x5343
    unsigned int m_resultCode;         // +0x5344（DB_CreateCharac::dispatch 各步骤错误码，
                                       //       DB_CreateCharac.md 反汇编实证）
    unsigned int m_characNo;           // +0x5348（GetCharacNo ← WongWork::DBCommon::GetIdentity）
    unsigned char m_ssnCheckFail;      // +0x534c（SsnString 长度/第7位检查→1/0）
    unsigned char m_pad534d[3];        // +0x534d（ORIG +0x534c 字节与 +0x5350 字之间间隙）
    unsigned int m_spAtLevelUp;        // +0x5350（CDataManager::GetSpAtLevelUp(1)）
};
#pragma pack(pop)
static_assert(sizeof(SIG_CREATE_CHARAC) == 0x5354, "SIG_CREATE_CHARAC ABI drift");

static_assert(offsetof(SIG_CREATE_CHARAC, m_giftItem) == 0x4e01, "SIG_CREATE_CHARAC m_giftItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_weaponItem) == 0x4e7b, "SIG_CREATE_CHARAC m_weaponItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_clothesItem) == 0x4ef5, "SIG_CREATE_CHARAC m_clothesItem offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_giftFindOk) == 0x533f, "SIG_CREATE_CHARAC m_giftFindOk offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_equipFindOk) == 0x5340, "SIG_CREATE_CHARAC m_equipFindOk offset drift");
static_assert(offsetof(SIG_CREATE_CHARAC, m_ssnCheckFail) == 0x534c, "SIG_CREATE_CHARAC m_ssnCheckFail offset drift");
static_assert(sizeof(Inven_Item) == 0x3d, "Inven_Item size drift vs SIG_CREATE_CHARAC gift slots");
// 真实布局（CDBMsgSender::{RecvMail,SendMail,UseFeedCreature,InsertCreatureItem}
// 反汇编实证：memset 0x74；slot 写 +0x2c、uid/addInfo2 +0x30、饱食度 +0x44、
// characNo +0x48、flag50 +0x50、reason +0x51、field53 +0x53、days +0x54、
// expireTick +0x58、ceraOrderNo +0x60、field64 +0x64、mapSize +0x68。
// 物品副本区实际为 +0x04..+0x2b 共 0x28 字节（旧声明 [0x26]+对齐使 +0x51 起
// 全部漂移 -8，本批按 ORIG 偏移修正并补齐具名字段）。
struct SIG_CREATURE_ITEM
{
    int m_hdr;                  // +0x00（RecvMail |0x130、SendMail |0x100、
                                //        UseFeedCreature |0x2）
    unsigned char m_item[0x28]; // +0x04（Inven_Item_Compatible 兼容物品副本区）
    int m_slot;                 // +0x2c
    int m_addInfo2;             // +0x30（RecvMail/SendMail ← item+0x07；
                                //        UseFeedCreature 承载宠物 uid）
    unsigned char m_pad34[0x10];
    int m_stomachValue;         // +0x44（UseFeedCreature=喂食后饱食度；
                                //        InsertCreatureItem 固定写 100）
    int m_characNo;             // +0x48
    unsigned char m_pad4c[4];
    unsigned char m_flag50;     // +0x50（InsertCreatureItem：get_sub_type()==0→1）
    unsigned char m_reason;     // +0x51
    unsigned char m_keepRight;  // +0x52（UseKeepRight W 0x083401ec：flag 低字节）
    unsigned char m_field53;    // +0x53
    int m_days;                 // +0x54（InsertCreatureItem ← 参数 days）
    int m_expireTick;           // +0x58（InsertCreatureItem ← 参数 expireTick）
    unsigned char m_pad5c[4];
    int m_ceraOrderNo;          // +0x60（InsertCreatureItem ← get_cera_item_order_no）
    int m_field64;              // +0x64（RecvMail=4/SendMail=3/InsertCreatureItem=6）
    int m_mapSize;              // +0x68
    int m_field6c;              // +0x6c（CDBMsgSender::ChangeOwner p6；DB 域报告待核对）
    int m_field70;              // +0x70（ChangeOwner p7；同上）→ 总 0x74（ORIG memset 覆盖范围）
};
static_assert(sizeof(SIG_CREATURE_ITEM) == 0x74, "SIG_CREATURE_ITEM ABI drift");
struct SIG_CREATURE_ITEM_LAYOUT_MIRROR
{
    char m_hdr[0x04];
    char m_item[0x28];
    char m_slot[0x04];
    char m_addInfo2[0x04];
    char m_pad34[0x10];
    char m_stomachValue[0x04];
    char m_characNo[0x04];
    char m_pad4c[0x04];
    char m_flag50;
    char m_reason;
    char m_keepRight;
    char m_field53;
    char m_days[0x04];
    char m_expireTick[0x04];
    char m_pad5c[0x04];
    char m_ceraOrderNo[0x04];
    char m_field64[0x04];
    char m_mapSize[0x04];
    char m_pad6c[0x08];
};
static_assert(sizeof(SIG_CREATURE_ITEM) == sizeof(SIG_CREATURE_ITEM_LAYOUT_MIRROR), "SIG_CREATURE_ITEM layout size");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_slot) == 0x2c, "SIG_CREATURE_ITEM m_slot offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_addInfo2) == 0x30, "SIG_CREATURE_ITEM m_addInfo2 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_stomachValue) == 0x44, "SIG_CREATURE_ITEM m_stomachValue offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_characNo) == 0x48, "SIG_CREATURE_ITEM m_characNo offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_flag50) == 0x50, "SIG_CREATURE_ITEM m_flag50 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_reason) == 0x51, "SIG_CREATURE_ITEM m_reason offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_field53) == 0x53, "SIG_CREATURE_ITEM m_field53 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_days) == 0x54, "SIG_CREATURE_ITEM m_days offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_expireTick) == 0x58, "SIG_CREATURE_ITEM m_expireTick offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_ceraOrderNo) == 0x60, "SIG_CREATURE_ITEM m_ceraOrderNo offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_field64) == 0x64, "SIG_CREATURE_ITEM m_field64 offset");
static_assert(offsetof(SIG_CREATURE_ITEM_LAYOUT_MIRROR, m_mapSize) == 0x68, "SIG_CREATURE_ITEM m_mapSize offset");

struct SIG_CREATURE_ITEM_LIST
{
    char m_pad[0x4078];
};

struct SIG_DEATH_TOWER_GET_BEST_RECORD
{
    char m_pad[0x878];
};

struct SIG_DEATH_TOWER_LOAD_TOP_RANKER
{
    char m_pad[0x35b68];
};

struct SIG_DEATH_TOWER_UPDATE_BEST_RECORD
{
    char m_pad[0x6c];
};
#pragma pack(push, 1)
// 真实布局（DisPatcher_DeleteCharac::process 0x081c1182 反汇编实证，memset 0x14）：
struct SIG_DELETE_CHARAC
{
    unsigned int m_accId;      // +0x00
    unsigned int m_characNo;   // +0x04（==0 → process 置 m_errNo=0x15 并回收流）
    unsigned char m_pad8[4];   // +0x08
    unsigned int m_guildId;    // +0x0c（CUser::GetGuildIdBeforeDeleteCharacter）
    unsigned char m_pad10[4];  // +0x10（ORIG memset 覆盖至 0x14，含尾部 4 字节）
};
#pragma pack(pop)
static_assert(sizeof(SIG_DELETE_CHARAC) == 0x14, "SIG_DELETE_CHARAC ABI drift");

struct SIG_DOUBLE_CHECK_GUILD_ADDRESS
{
    char m_pad[0xd];
};

struct SIG_DOUBLE_CHECK_GUILD_NAME
{
    char m_pad[0x17];
};

struct SIG_DUNGEON_DATA_STATISTIC
{
    char m_pad[0x80];
};

struct SIG_EVENT_LEVEUP_SUPPORT_INFO
{
    char m_pad[0x2c];
};

// SIG_EVENT_SERVER_MESSAGE 完整定义唯一化至 CSharedServerMessageManager.h
// （真实布局：byte m_count + stSERVER_MESSAGE m_msg[0x64]；原 pad 占位已删除）。
#include "CSharedServerMessageManager.h"

struct SIG_EVENT_STAGE_LOG
{
    char m_pad[0x78];
};

// DB 载荷 wire 布局（ORIG _saveData 0x0832b0ac / loadData 0x0832b1fe 实证；
// 自然对齐 == packed 布局，总尺寸 0x30 不变）。字段与 CCharacExpertJob 成员
// 一一对应；仅 _saveData/loadData 按此结构读写。
struct SIG_EXPERT_JOB
{
    int            m_characNo;         // +0x00
    unsigned short m_giveupCount;      // +0x04
    unsigned short m_pad06;            // +0x06（wire 保留）
    int            m_endurance;        // +0x08
    int            m_enchantCount;     // +0x0c
    unsigned char  m_recipeFlag[0x1e]; // +0x10（240 位 recipe bitmap）
    char           m_pad2e[2];         // +0x2e（补齐 0x30）
};
static_assert(sizeof(SIG_EXPERT_JOB) == 0x30, "SIG_EXPERT_JOB size");

struct SIG_EXTEND_CHARAC_SLOT
{
    char m_pad[0x8];
};

struct SIG_FAIR_PVP_SCORE
{
    char m_pad[0x688];
};

struct SIG_FIND_FACTORY_HUB
{
    char m_pad[0x24];
};

struct SIG_FIND_FACTORY_HUB_FATIGUE_CHECK
{
    char m_pad[0x24];
};

struct SIG_FIND_FACTORY_HUB_USER
{
    char m_pad[0x20];
};

struct SIG_FIND_FACTORY_HUB_USER_NO_CONN
{
    char m_pad[0xc];
};

struct SIG_FORBID_USER_TO_PLAY
{
    char m_pad[0x9];
};

struct SIG_GET_PREMIUM_SELECTED_CUBE_INFO
{
    char m_pad[0x4];
};

struct SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES
{
    char m_pad[0x8];
};

struct SIG_GET_USER_REGDATE
{
    // ORIG makeRequest(0x8442fae) 写 +0x00=acc_id；dispatch(0x8442e68) 写
    // +0x04=DATE_FORMAT 列/100、+0x08=reg_date 原始值（member_info 查询）。
    unsigned int m_id;        // +0x00（SQL where m_id=%d）
    int m_regDay;             // +0x04（yyyymmdd，列 0 /100）
    unsigned int m_regDate;   // +0x08（member_info.reg_date 原始 epoch）
    // ORIG GetInBuffer 实例化 sizeof=0xc：仅 3 个 dword，无尾部 pad
};

struct SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD
{
    char m_pad[0x4];
};

struct SIG_GOBLIN_UPDATE_REWARD
{
    char m_pad[0x18];
};

struct SIG_GOBLIN_UPDATE_VALIDITYTIME
{
    char m_pad[0x8];
};

struct SIG_GOLD_CONTROL_EXCEPTION_MID
{
    char m_pad[0x2];
};

struct SIG_GUILD_JOIN_LIST
{
    char m_pad[0xd4c];
};

struct SIG_HACK_IP_SAVE
{
    char m_pad[0x28];
};

struct SIG_HADES_PUNISH
{
    char m_pad[0x24];
};

struct SIG_INSERT_0712_WINTER_EVENT_COUPON
{
    char m_pad[0xc];
};

struct SIG_INSERT_ACHIEVE_EVENT
{
    char m_pad[0xc];

    // ORIG 嵌套枚举（_ZN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE）。
    // 调用点观察值：1=CUser::_onLevelUp level 0x12；2=_onLevelUp level 0x30；
    // _onChangeGrowType/_onQuestClear 亦传小整数。ORIG 无成员名符号信息，取中性名。
    enum ENUM_ACHIEVE_TYPE
    {
        ENUM_ACHIEVE_TYPE_NONE = 0,
        ENUM_ACHIEVE_TYPE_1 = 1,
        ENUM_ACHIEVE_TYPE_2 = 2
    };
};

struct SIG_INSERT_AUTO_PUNISH_FIRST_USER
{
    char m_pad[0x2c];
};

struct SIG_INSERT_AUTO_PUNISH_SECOND_USER
{
    char m_pad[0x14];
};

struct SIG_INSERT_RESPONSE_TIME
{
    char m_pad[0x7d8];
};

struct SIG_INSERT_SERVER_LOAD_STATISTIC
{
    char m_pad[0xa];
};

struct SIG_INSERT_UNLIMIT_SUPPORT_LOG
{
    char m_pad[0x2c];
};

struct SIG_IN_GAME_ADVERTISEMENT_LOG
{
    char m_pad[0x20];
};

struct SIG_IP_MONITOR_PUNISH
{
    char m_pad[0xc88];
};

// SIG_ITEM_DICTIONARY_FLAG —— 物品词典位图存档（ORIG int characNo + 5000 字节位图，共 0x138c）。
// CItemDictionary.cpp 曾以本地匿名命名空间 struct 定义（与权威冲突）；按 ORIG 迁移至本头。
#pragma pack(push, 1)
struct SIG_ITEM_DICTIONARY_FLAG
{
    int m_characNo;              // +0x00
    unsigned char m_data[5000];  // +0x04
};
#pragma pack(pop)
static_assert(sizeof(SIG_ITEM_DICTIONARY_FLAG) == 0x138c, "SIG_ITEM_DICTIONARY_FLAG ABI drift");

struct SIG_ITEM_LOCK_UPDATE
{
    char m_pad[0x324];
};

struct SIG_ITEM_MAKING_SKILL
{
    char m_pad[0x1c];
};

struct SIG_JOIN_GUILD_INFO
{
    char m_pad[0x64];
};

struct SIG_LETTER_STAT
{
    // ORIG 布局（ReqChangeLetterStat 0x85568a6）：+0 characNo / +4 mailIdx / +8 state(short)
    unsigned char m_data[0xc];
};

struct SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD
{
    char m_pad[0x10];
};

struct SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
{
    char m_pad[0x80];
};

// ORIG sizeof = 0x703（CACHE_CHARACTER_TYPE@+0x0 → +0x703）。
// CHARAC_DATA（SIG_LOAD_CHARAC 的 +0xc 区，0x67a 字节）：getCharacInfoDetail @0x864bdba
// 依偏移填充（源为 _Charac_info 成员；具名字段见 CUser.cpp 实现）。
// 尾部 +0x676（SIG 内绝对偏移 0x682）为 school no：RequestDBCharac @0x81c6ca0
// 写 CUser::GetSchoolNo；CHARAC_DATA::Reset（W 0x822ec9e）不覆盖该 4 字节。
// TAG_CHARAC_DATA（+0x68b 区，0x78 字节）：getTagCharacInfo @0x86904f0 填充。
#pragma pack(push, 1)
struct CHARAC_DATA
{
    union {
        char m_data[0x676];        // +0x000（整体 blob 视图，既有按偏移访问不变）
        struct {
            char m_head[0x130];    // +0x000..+0x12f
            int  m_dword130;       // +0x130（CHARAC_DATA::Reset 尾部 dword 清零，
        };                         //   ORIG 0x822ec9e+0x139 行 movl $0,0x130(%eax)）
    };
    unsigned int m_schoolNo;   // +0x676（RequestDBCharac 写入；Reset 不清）

    void Reset();              // ORIG W 0x822ec9e _ZN11CHARAC_DATA5ResetEv
};
static_assert(sizeof(CHARAC_DATA) == 0x67a, "CHARAC_DATA ABI drift");
static_assert(offsetof(CHARAC_DATA, m_schoolNo) == 0x676, "CHARAC_DATA m_schoolNo offset drift");
#pragma pack(pop)

#pragma pack(push, 1)
struct TAG_CHARAC_DATA
{
    char m_data[0x78];

    void clear();              // ORIG W 0x822ed84 _ZN15TAG_CHARAC_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_DATA) == 0x78, "TAG_CHARAC_DATA ABI drift");
#pragma pack(pop)

// SIG_LOAD_INVENTORY +0xc 的 TAG_CHARAC_INVEN_DATA（0x4024 字节）：getTagCharacInventory @0x869067c 填充。
#pragma pack(push, 1)
struct TAG_CHARAC_INVEN_DATA
{
    char m_data[0x4024];

    void clear();              // ORIG W 0x822edd0 _ZN21TAG_CHARAC_INVEN_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_INVEN_DATA) == 0x4024, "TAG_CHARAC_INVEN_DATA ABI drift");
#pragma pack(pop)

// RequestDBETC @0x81c7602 在 SIG_LOAD_ETC +0x10 调用的组合 tag 区：
// 覆盖 tagSkillSlot[0x198] + tagSkillCombo[0x198] = 0x330 字节
//（ORIG W 0x822edf4 _ZN19TAG_CHARAC_ETC_DATA5clearEv = memset(this,0,0x330)）。
#pragma pack(push, 1)
struct TAG_CHARAC_ETC_DATA
{
    char m_data[0x330];

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
    unsigned int     m_tagCharacNo;     // +0x686
    unsigned char    m_strikerSkillIndex; // +0x68a
    TAG_CHARAC_DATA   m_tagCharacData;  // +0x68b（0x78 字节；getTagCharacInfo 写 + CUser.h:7469 直接
                                        //   透传，clear() 为 ORIG W 0x822ed84；类型化保 .clear() 调用）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_CHARAC) == 0x703, "SIG_LOAD_CHARAC ABI drift");
static_assert(offsetof(SIG_LOAD_CHARAC, m_accId) == 0x000, "SIG_LOAD_CHARAC m_accId offset drift");
static_assert(offsetof(SIG_LOAD_CHARAC, m_characNo) == 0x004, "SIG_LOAD_CHARAC m_characNo offset drift");
static_assert(offsetof(SIG_LOAD_CHARAC, m_result) == 0x008, "SIG_LOAD_CHARAC m_result offset drift");
static_assert(offsetof(SIG_LOAD_CHARAC, m_tagCharacNo) == 0x686, "SIG_LOAD_CHARAC m_tagCharacNo offset drift");
static_assert(offsetof(SIG_LOAD_CHARAC, m_strikerSkillIndex) == 0x68a, "SIG_LOAD_CHARAC m_strikerSkillIndex offset drift");

struct SIG_LOAD_CHARAC_HOTKEY_OPTION
{
    char m_pad[0xc4];
};

struct SIG_LOAD_CLEANPAD_POINT
{
    char m_pad[0x3cf0];
};

struct SIG_LOAD_DIMENSION
{
    char m_pad[0x10];
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
    int      m_blackCount;     // +0x008
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
    unsigned char m_isEnabled;   // +0x00（启用标志；GetCharacCashAttendanceInfo 清零，processAttendAnceEvent 读作 bool）
    unsigned char m_pad[0x10];   // +0x01（opaque blob，无 ORIG 字段级证据）
};
static_assert(sizeof(SIG_ATTENDANCE_EVENT) == 0x11, "SIG_ATTENDANCE_EVENT ABI drift");

// ORIG sizeof = 0x14b60（CACHE_CHARACTER_TYPE@+0x1fd20 → +0x34880 cacheTime）。
// CUser::GetEtcInfo @0x867794c 依此偏移填充；子对象区（skill/expand/aura/option）
// 为可重叠序列化区，经命名数组 + 文档化偏移访问（同 GetContinuousItemInfo 约定）。
// 字节打包：ORIG 在 +0xc9b3（奇数偏移）写 dword，故必须 pack(1)。
#pragma pack(push, 1)
struct SIG_LOAD_ETC
{
    unsigned int m_accId;                              // +0x000（RequestDBETC ← get_acc_id）
    unsigned int m_characNo;                           // +0x004（RequestDBETC ← get_charac_no）
    int      m_field8;                                 // +0x008（GetEtcInfo 先清零）
    unsigned int m_fieldC;                             // +0x00c（RequestDBETC ← MSG_CHARAC_SELECT.m_0e，
                                                       //       同 SIG_LOAD_CHARAC.m_tagCharacNo 数据源）
    char     m_tagSkillSlot[0x198];                  // +0x010（get_skill_slot out1）
    char     m_tagSkillCombo[0x198];                 // +0x1a8（get_skill_slot out2）
    char     m_pvpResult[0x84];                      // +0x340（PvpResultType）
    int      m_recordCount;                          // +0x3c4
    char     m_pad3c8[0x29fc - 0x3c8];               // +0x3c8（record 数据区 / continuous item）
    unsigned char m_oneDayLetheFlag;                 // +0x29fc
    char     m_backupSkill[0x199];                   // +0x29fd
    unsigned char m_oneDayLetheFlag2ND;              // +0x2b96
    char     m_backupSkill2ND[0x199];                // +0x2b97
    char     m_pad2d30[0x2d30 - (0x2b97 + 0x199)];     // +0x2d30 前垫
    unsigned int m_serverGroup;                        // +0x2d30（RequestDBETC ← G_CEnvironment()->m_serverEnvir.m_serverGroup）
    int      m_channelNo;                              // +0x2d34（RequestDBETC ← CEnvironment::get_channel_no）
    char     m_pad2d38[0x40c8 - 0x2d38];               // +0x2d38
    unsigned short m_curConditionEventStep;          // +0x40c8
    unsigned short m_curConditionEventRewardStep;    // +0x40ca
    unsigned short m_properDungeonClearCount;        // +0x40cc
    char     m_expandData6[0x466a - 0x40ce];         // +0x40ce（type 6）
    char     m_expandDataE[0xb1a4 - 0x466a];         // +0x466a（type 0xe）
    char     m_expandDataF[0xc9ac - 0xb1a4];         // +0xb1a4（type 0xf）
    unsigned char m_curVillageAttackCount;           // +0xc9ac
    unsigned char m_curRevengeDungeonCount;          // +0xc9ad
    unsigned char m_flagPresentAvengerTitle;         // +0xc9ae
    char     m_padc9af[0xc9b3 - 0xc9af];             // +0xc9af
    int      m_totalPcRoomPlayTime;                  // +0xc9b3
    unsigned char m_pcRoomPlayTimeRewardCnt;         // +0xc9b7
    int      m_curCharacUsedGiftFatigueQuantity;     // +0xc9b8
    unsigned int m_bitset;                           // +0xc9bc（std::bitset<32>，4 字节）
    char     m_mixedRegion[0x14a98 - 0xc9c0];        // +0xc9c0（skill + expand8/7/b/10/d/10 + aura + option）
    unsigned char m_blueMarbleEnterCount;            // +0x14a98
    SIG_ATTENDANCE_EVENT m_attendanceEvent;          // +0x14a99
    char     m_comboSkill[0x14b60 - 0x14aaa];        // +0x14aaa
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_ETC) == 0x14b60, "SIG_LOAD_ETC ABI drift");

struct SIG_LOAD_EVENTITEM
{
    char m_pad[0x5c];
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
    void clear();                               // ORIG W 0x0848f9a2
};
struct SIG_SAVE_PC_ROOM_RENT_ITEM
{
    SIG_SAVE_PC_ROOM_RENT_INFO m_entry[2];
    SIG_SAVE_PC_ROOM_RENT_ITEM() {}               // ORIG W 0x8491cbe（隐式逐元素构造 entry[0..1]）
    void clear();                                 // ORIG W 0x08694c9a
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_PC_ROOM_RENT_ITEM) == 0x18, "SIG_SAVE_PC_ROOM_RENT_ITEM ABI drift");
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
    void clear();                                 // ORIG W 0x0848f9e4
};
struct SIG_SAVE_DIMENSION_RENT_ITEM
{
    SIG_SAVE_DIMENSION_RENT_INFO m_entry[12];
    SIG_SAVE_DIMENSION_RENT_ITEM() {}             // ORIG W 0x8491cf2（隐式逐元素构造 entry[0..11]）
    void clear();                                 // ORIG W 0x08694cd2
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_ITEM) == 0x60, "SIG_SAVE_DIMENSION_RENT_ITEM ABI drift");
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_INFO) == 8, "SIG_SAVE_DIMENSION_RENT_INFO ABI drift");
// (keep original old block tail intact below)
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
    int      m_money;                             // +0x4034
    unsigned int m_coin;                          // +0x4038
    unsigned int m_payCoin;                       // +0x403c
    unsigned int m_eventCoin;                     // +0x4040
    char     m_invenType2[0x1905];                // +0x4044（GetInvenData(2)）
    char     m_invenType1[0x4a58];                // +0x5949（GetInvenData(1)）
    char     m_equipMirror[0x53e];                // +0xa3a1（memcpy inven+0x1c）
    char     m_padA8df[0xa8e0 - 0xa3a1 - 0x53e];  // +0xa8df（equipMirror 与 cargoCapacity 间 1 字节）
    unsigned int m_cargoCapacity;                 // +0xa8e0（CCargo::GetCapacity）
    char     m_cargo[0xcd1c - 0xa8e4];            // +0xa8e4（get_cargo）
    int      m_avatarCount;                       // +0xcd1c
    char     m_avatarDetail[0xeed1 - 0xcd20];     // +0xcd20（AvatarDetailInfo）
    char     m_invenType3[0x39aa];                // +0xeed1（GetInvenData(3)，尾 0xf4 为 creature tail 覆写）
    char     m_pad1287b[0x1287c - 0xeed1 - 0x39aa]; // +0x1287b（creatureCount 前 1 字节）
    int      m_creatureCount;                     // +0x1287c
    char     m_creature[0x1ed8];                  // +0x12880（CCreatureMgr::GetCreatureItem）
    char     m_clearAvatar[0x14780 - 0x14758];    // +0x14758（GetClearAvatar）
    unsigned int m_avatarCoin;                    // +0x14780（AvatarCoin::Get）
    char     m_expandData2[0x14aa4 - 0x14784];    // +0x14784（GetCharacExpandDataR(2)）
    char     m_expandData9[0x1533c - 0x14aa4];    // +0x14aa4（GetCharacExpandDataR(9)）
    int      m_inventoryCapacity;                 // +0x1533c（get_inventory_capacity）
    int      m_creatureCount2;                    // +0x15340（GetCreatureItem 第二区计数）
    char     m_creature2[0x153e4 - 0x15344];      // +0x15344（creature2 条目表，至 rentPcRoom 前）
    char     m_rentPcRoom[0xc];                   // +0x153e4（SIG_SAVE_PC_ROOM_RENT_ITEM）
    char     m_rentDimension[0x15448 - 0x153f0];  // +0x153f0（SIG_SAVE_DIMENSION_RENT_ITEM）
    int      m_maxEquipLevel;                     // +0x15448
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
// charac_no 写于 +0xafc8 尾部）。前部 0xafc8 字节为 DB 侧填充的击杀数据区。
#pragma pack(push, 1)
struct SIG_LOAD_KILL_MONSTER_INFO
{
    char         m_data[0xafc8];    // +0x000（击杀统计 blob，DB 端填充）
    unsigned int m_characNo;        // +0xafc8（RequestDBKillMonsterInfo 写入）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_KILL_MONSTER_INFO) == 0xafcc, "SIG_LOAD_KILL_MONSTER_INFO ABI drift");
static_assert(offsetof(SIG_LOAD_KILL_MONSTER_INFO, m_characNo) == 0xafc8, "SIG_LOAD_KILL_MONSTER_INFO m_characNo offset drift");

struct SIG_LOAD_NO_CACHE_DATA
{
    char m_pad[0x2c];
};

struct SIG_LOAD_POWER_WAR_STATUE_INFO
{
    char m_pad[0x80];
};

struct SIG_LOAD_PU_USER_INFO
{
    char m_pad[0x1];
};

// ORIG sizeof = 0x7608（RequestDBQuest @0x81c70c2 memset 0x7608；布局与
// UserQuest.cpp 本地视图/get_quest@0x869067c 族一致：m_cleared 0x7530 位图 +
// m_authen 0x28）。
#pragma pack(push, 1)
struct SIG_LOAD_QUEST
{
    unsigned int m_characNo;        // +0x000（RequestDBQuest 写入）
    int          m_field04;         // +0x004
    char         m_pad08[4];        // +0x008
    int          m_quest[20];       // +0x00c
    int          m_trigger[20];     // +0x05c
    int          m_urgentQuest;     // +0x0ac
    char         m_cleared[0x7530]; // +0x0b0（任务完成位图）
    char         m_authen[0x28];    // +0x75e0
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_QUEST) == 0x7608, "SIG_LOAD_QUEST ABI drift");
static_assert(offsetof(SIG_LOAD_QUEST, m_cleared) == 0xb0, "SIG_LOAD_QUEST m_cleared offset drift");

struct SIG_LOAD_SKILL
{
    char m_pad[0x414];
};

// ORIG sizeof = 0x19c（CACHE_CHARACTER_TYPE@+0x1d157 → +0x1d2f3）。
// CUser::GetTicket @0x867846e：+0x4 清零 / +0x8 count / +0xc int[100] 票表（list<int> 遍历，
// 上限 99）。字节打包（+0x0c 奇偏移起）。
#pragma pack(push, 1)
struct SIG_LOAD_TICKET
{
    char     m_pad0[0x4];     // +0x000
    int      m_field4;        // +0x004（GetTicket 先清零）
    int      m_count;         // +0x008
    int      m_tickets[100];  // +0x00c
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_TICKET) == 0x19c, "SIG_LOAD_TICKET ABI drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_field4) == 0x004, "SIG_LOAD_TICKET m_field4 offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_count) == 0x008, "SIG_LOAD_TICKET m_count offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_tickets) == 0x00c, "SIG_LOAD_TICKET m_tickets offset drift");

struct SIG_LOGIN_DATA
{
    char m_pad[0x4358];
};

struct SIG_MAILBOX_CHECK_RECEIVER
{
    char m_pad[0x3fde];
};

struct SIG_MAILBOX_DEL_MAIL
{
    // ORIG 布局（ReqDBRemoveMail 0x8556772）：+0 count / +4*count 邮件 idx 表 / +0x54 type
    unsigned char m_data[0x58];
};

struct SIG_MAILBOX_LIST
{
    // ORIG 尺寸 0x338a（ReqDBLoadMail memset 证据）
    unsigned char m_data[0x338a];
};

struct SIG_MAILBOX_SEND_NEW_MAIL
{
    // ORIG 尺寸 0x168（_makeSendMailData/_DoSendNewMail memset 证据），字段见 CMailBoxHelper.cpp
    unsigned char m_data[0x168];
};

struct SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK
{
    // ORIG ≥0x1a4（CreatureMailCashShop 写 +0x1a2）
    unsigned char m_data[0x1a4];
};

struct SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL
{
    // ORIG 尺寸 0x3a9（ReqDBSendNewSystemMultiMail memset 证据）
    unsigned char m_data[0x3a9];
};

struct SIG_MERCENARY_DB_DATA
{
    char m_pad[0xbd4];
};

struct SIG_MILEAGE_STRUCT
{
    char m_pad[0xc];
};

struct SIG_MOUSE_REGISTER
{
    char m_pad[0x59];
};

// 真实布局（GameWorld NAT 统计上报：type/count/time）
struct SIG_NAT_TYPE_USER
{
    int m_type;
    int m_count;
    int m_time;
};

struct SIG_OBJECT_BRING_UP
{
    unsigned int   m_accId;        // +0x00 sendBringUpReward←CUser::get_acc_id
    unsigned int   m_characNo;     // +0x04 ←CUserCharacInfo::getCurCharacNo
    unsigned char  m_serverGroup;  // +0x08 ←*(G_CEnvironment()+0x378)
    unsigned char  m_pad09[3];     // +0x09..+0x0b（无访问证据）
    unsigned char  m_usedMaterialCnt; // +0x0c ←getCurUseBringUpMaterialCount
    unsigned char  m_rewardFlag;      // +0x0d ←getBringUpRewardFlag
    unsigned char  m_pad0e[2];        // +0x0e..+0x0f
};                                 // sizeof = 0x10

struct SIG_ONLINE_PRELIMINARY_MEMBER
{
    char m_pad[0x14];
};

struct SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST
{
    char m_pad[0x4bc];
};

struct SIG_ONLINE_PRELIMINARY_TEAM
{
    char m_pad[0x20];
};

struct SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST
{
    char m_pad[0x328];
};

// SIG_PARTY_STATUS（0x1c8）—— ORIG 权威布局（Dispatcher_GatheringPartyStatus::dispatch_sig @ 0x8203878）
// byte-pack：m_field22@0x22 为 4 字节未对齐存储；m_pad 填补 0x154..0x15e。
#pragma pack(push, 1)
struct SIG_PARTY_STATUS
{
    char m_ip[0x10];                  // +0x000  CNetwork::GetPeerIP2
    unsigned int m_accId;             // +0x010  CUser::get_acc_id
    int m_natType;                    // +0x014  CNetwork::get_nat_type
    int m_field18;                    // +0x018  客户端字段（int）
    int m_field1c;                    // +0x01c  客户端字段（int）
    unsigned char m_field20;          // +0x020  客户端字段（byte）
    unsigned char m_field21;          // +0x021  客户端字段（byte）
    unsigned int m_field22;           // +0x022  客户端字段（4 字节，未对齐）
    unsigned short m_tryNum;          // +0x026  客户端尝试次数（<= 0x1e）
    unsigned char m_tryEntries[0x1e][10];  // +0x028  每行 10 字节
    char m_pad[0xa];                  // +0x154  未建模字段（0x154..0x15e）
    unsigned short m_partyNum;        // +0x15e  队伍人数（pvp<=8, 普通<=4）
    unsigned char m_partyEntries[8][0xd];   // +0x160  每行 0xd 字节
};
#pragma pack(pop)

static_assert(sizeof(SIG_PARTY_STATUS) == 0x1c8, "SIG_PARTY_STATUS ABI drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_ip) == 0x000, "SIG_PARTY_STATUS m_ip offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_accId) == 0x010, "SIG_PARTY_STATUS m_accId offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_natType) == 0x014, "SIG_PARTY_STATUS m_natType offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_field18) == 0x018, "SIG_PARTY_STATUS m_field18 offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_field1c) == 0x01c, "SIG_PARTY_STATUS m_field1c offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_field20) == 0x020, "SIG_PARTY_STATUS m_field20 offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_field21) == 0x021, "SIG_PARTY_STATUS m_field21 offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_field22) == 0x022, "SIG_PARTY_STATUS m_field22 offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_tryNum) == 0x026, "SIG_PARTY_STATUS m_tryNum offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_tryEntries) == 0x028, "SIG_PARTY_STATUS m_tryEntries offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_partyNum) == 0x15e, "SIG_PARTY_STATUS m_partyNum offset drift");
static_assert(offsetof(SIG_PARTY_STATUS, m_partyEntries) == 0x160, "SIG_PARTY_STATUS m_partyEntries offset drift");

struct SIG_PREMIUM_LETHE_FLAG
{
    char m_pad[0x1a0];
};

struct SIG_PURCHASE_CASHITEM_BONUS
{
    char m_pad[0x4];
};

struct SIG_QUERY_CHARAC_INFO
{
    char m_pad[0x30];
};
// ORIG 布局（CQuestShop::getData 0x85ef868 / _saveData 0x85efb66 /
// loadData 0x85efa2c 反汇编实证；单实例 0xaa，SIG_LOAD_ETC 内双实例区
// [0xd5a7,0xd651)=0xaa 与 loadData 0x154 flag 对齐）。
struct SIG_QUEST_SHOP_DATA
{
    unsigned int m_characNo;               // +0x000（_saveData 写 get_charac_no(-1)）
    unsigned int m_qp;                     // +0x004
    unsigned int m_initCount;              // +0x008
    unsigned short m_statusCounts[0x4d];   // +0x00c..0x0a5（status i 已购计数）
    unsigned int m_questPiece;             // +0x0a6
};
static_assert(sizeof(SIG_QUEST_SHOP_DATA) == 0xaa,
              "SIG_QUEST_SHOP_DATA ABI drift");

struct SIG_REDEEMINFO_LIST_TO_DB
{
    char m_pad[0x59c];
};

struct SIG_RELOAD_IN_GAME_ADVERTISEMENT
{
    char m_pad[0x67b60];
};

struct SIG_REQUEST_CHARAC_SKILL_INFO
{
    char m_pad[0x1a0];
};

struct SIG_REQUEST_JOIN_GUILD
{
    char m_pad[0x44];
};

struct SIG_REQUEST_JOIN_GUILD_DB_REPLY
{
    char m_pad[0x48];
};

struct SIG_REQUEST_TAG_CHARAC_INFO
{
    char m_pad[0x414c];
};

struct SIG_REQUEST_TOD_USER_STATE
{
    char m_pad[0x18];
};

struct SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
{
    char m_pad[0x8];
};

struct SIG_REQ_LOAD_GAME_OPTION
{
    char m_pad[0x4];
};

struct SIG_RESTING_USER_RESTRICT
{
    char m_pad[0x4];
};

struct SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO
{
    char m_pad[0x198];
};

struct SIG_SAVE_BLOOD_CLEAR_COUNT
{
    unsigned int m_dungeonIdx;   // +0x00（_saveProperDungeonClearCount ORIG 写序实证）
    short m_subNo;               // +0x04
    short m_pad6;
};

struct SIG_SAVE_CHARAC
{
    char m_pad[0x10d];
};

struct SIG_SAVE_CHARAC_INFORM_NOTICE
{
    char m_pad[0x14];
};

struct SIG_SAVE_CHATTING_EMOTICON
{
    char m_pad[0x214];
};

struct SIG_SAVE_CLIENT_HACK_LOG
{
    char m_pad[0x12];
};

struct SIG_SAVE_CONDITION_EVENT
{
    unsigned int m_value;        // +0x00（_saveConditionEventInfo ORIG 写序实证）
    short m_a2;                  // +0x04
    short m_a3;                  // +0x06
};

struct SIG_SAVE_CRAZY_LEVELUP_EVENT
{
    char m_pad[0xd];
};

struct SIG_SAVE_DUNGEON_CLEAR
{
    char m_pad[0x180c];
};

#pragma pack(push, 1)   // 尺寸 0x19（非对齐尾字段 +0x18）；具名化批次缺 pack 致断言漂移
struct SIG_SAVE_EVENTITEM
{
    // 0x086c1910 写入序具名化（语义未证实处保持字段名推断）。
    unsigned int m_field00;   // +0x00 ←param_1
    unsigned int m_field04;   // +0x04 ←param_2
    unsigned int m_field08;   // +0x08 ←param_5
    unsigned int m_field0c;   // +0x0c ←param_3
    unsigned int m_field10;   // +0x10 ←param_4
    unsigned int m_field14;   // +0x14 ←param_6
    unsigned char m_field18;  // +0x18（bool 尾参）
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_EVENTITEM) == 0x19, "SIG_SAVE_EVENTITEM ABI drift");

struct SIG_SAVE_GAME_OPTION_1
{
    char m_pad[0x4c];
};

struct SIG_SAVE_GAME_OPTION_QUICKCHAT
{
    char m_pad[0x25c];
};

struct SIG_SAVE_GEO_REJECT
{
    char m_pad[0x20];
};

struct SIG_SAVE_GOLD_ZERO_TRADE_LOG
{
    char m_pad[0xe0];
};

// 具名化（G6 HackAnM 批次；DB_ReqSaveHackUserPunish::makeRequest ORIG T
// 0x08425494 报告偏移：+0 accId / +4 punish / +8 ip[16]（NULL→"000.000.000.000"，
// 否则 strncpy 0x10）；sizeof 保持 0x18）。
struct SIG_SAVE_HACK_USER_PUNISH
{
    unsigned int m_accId;
    unsigned int m_punish;
    char m_ip[0x10];
};

// SIG_SAVE_INVENTORY（0xad99）—— ORIG 权威布局（CUser::SaveInventory @0x864fe52 /
// CUser::_GetSaveInvenBase @0x864fd80 / CUser::removeRentalItemInfoFromInvenBuf
// @0x8677584 实测偏移）：
//   header 区（SaveInventory 序列化时 memset 整块为 0，_GetSaveInvenBase 填充）：
//     +0x00 acc_id / +0x04 charac_no / +0x08 单字节标志 / +0x09 money /
//     +0x0d coin / +0x11 pay_coin / +0x15 event_coin（各 uint，未对齐 → pack(1)）。
//   背包区（各为 Inven_Item[0x3d] 数组，由 CInventory::GetInvenStart memcpy 填充）：
//     +0x19 类型0（22 项） / +0x557 类型1（312 项） / +0x4faf 类型3（238 项） /
//     +0x8865 宠物尾（4 项，来自 inven+0x55a） /
//     +0x8959 cargo 容量 uint / +0x895d cargo（152 项）/ +0xad95 avatar_coin uint。
// 尺寸 0xad99（SaveInventory `memset(0xad99)` 实证）。
#pragma pack(push, 1)
struct SIG_SAVE_INVENTORY
{
    unsigned int m_accId;              // +0x000
    unsigned int m_characNo;           // +0x004
    char m_saveFlag;                   // +0x008（0=完整背包保存，1=仅 AvatarCoin 保存）
    unsigned int m_money;              // +0x009
    unsigned int m_coin;               // +0x00d
    unsigned int m_payCoin;            // +0x011
    unsigned int m_eventCoin;          // +0x015
    Inven_Item m_invenType0[22];       // +0x019（CInventory::GetInvenStart(0)）
    Inven_Item m_invenType1[312];      // +0x557（CInventory::GetInvenStart(1)）
    Inven_Item m_invenType3[238];      // +0x4faf（CInventory::GetInvenStart(3)，前 238 项）
    Inven_Item m_creatureTail[4];      // +0x8865（类型3 区最后 4 项，覆写自 inven+0x55a）
    unsigned int m_cargoCapacity;      // +0x8959（CCargo::GetCapacity）
    Inven_Item m_cargo[152];           // +0x895d（CCargo::get_cargo_start）
    unsigned int m_avatarCoin;         // +0xad95（AvatarCoin::Get）
};
#pragma pack(pop)

// GCC4.4 下 offsetof 对 pack(1) 非标准布局成员的访问（"invalid access to non-static
// data member of NULL object"）在 -Werror 时转硬错误。改为 sizeof 累积链：pack(1) 下
// 每字段偏移 = 前置成员 sizeof 的字节和，仅用 sizeof（不访问 NULL 对象），全变体
// （c6/c6444r/c6446r）与 -Werror 下零告警；末字段 end == ORIG sizeof 0xad99 交叉验证。
#define SIG_SAVE_INV_SZOF(field) (sizeof(((SIG_SAVE_INVENTORY*)0)->field))
static_assert(sizeof(SIG_SAVE_INVENTORY) == 0xad99, "SIG_SAVE_INVENTORY ABI drift");
// 首成员 m_accId 恒在 +0x00；后续字段偏移由 sizeof 累积链校验（与 ORIG 偏移一致）：
static_assert(0x000 + SIG_SAVE_INV_SZOF(m_accId)          == 0x004, "SIG_SAVE_INVENTORY m_characNo at +0x004");
static_assert(0x004 + SIG_SAVE_INV_SZOF(m_characNo)       == 0x008, "SIG_SAVE_INVENTORY m_saveFlag at +0x008");
static_assert(0x008 + SIG_SAVE_INV_SZOF(m_saveFlag)       == 0x009, "SIG_SAVE_INVENTORY m_money at +0x009");
static_assert(0x009 + SIG_SAVE_INV_SZOF(m_money)          == 0x00d, "SIG_SAVE_INVENTORY m_coin at +0x00d");
static_assert(0x00d + SIG_SAVE_INV_SZOF(m_coin)           == 0x011, "SIG_SAVE_INVENTORY m_payCoin at +0x011");
static_assert(0x011 + SIG_SAVE_INV_SZOF(m_payCoin)        == 0x015, "SIG_SAVE_INVENTORY m_eventCoin at +0x015");
static_assert(0x015 + SIG_SAVE_INV_SZOF(m_eventCoin)      == 0x019, "SIG_SAVE_INVENTORY m_invenType0 at +0x019");
static_assert(0x019 + SIG_SAVE_INV_SZOF(m_invenType0)     == 0x557, "SIG_SAVE_INVENTORY m_invenType1 at +0x557");
static_assert(0x557 + SIG_SAVE_INV_SZOF(m_invenType1)     == 0x4faf, "SIG_SAVE_INVENTORY m_invenType3 at +0x4faf");
static_assert(0x4faf + SIG_SAVE_INV_SZOF(m_invenType3)    == 0x8865, "SIG_SAVE_INVENTORY m_creatureTail at +0x8865");
static_assert(0x8865 + SIG_SAVE_INV_SZOF(m_creatureTail)  == 0x8959, "SIG_SAVE_INVENTORY m_cargoCapacity at +0x8959");
static_assert(0x8959 + SIG_SAVE_INV_SZOF(m_cargoCapacity) == 0x895d, "SIG_SAVE_INVENTORY m_cargo at +0x895d");
static_assert(0x895d + SIG_SAVE_INV_SZOF(m_cargo)         == 0xad95, "SIG_SAVE_INVENTORY m_avatarCoin at +0xad95");
static_assert(0xad95 + SIG_SAVE_INV_SZOF(m_avatarCoin)    == 0xad99, "SIG_SAVE_INVENTORY end +0xad99");
#undef SIG_SAVE_INV_SZOF

// SIG_SAVE_LOGGAMECHANNEL —— ORIG 权威布局（WongWork::CLogGameChannel::ReqSaveLog
// @0x8550456 填充 / DoSaveLog @0x8550612 读取；memset 0x3b、enable_in 0x3b）。
// 未对齐（gc_no 位于 +0x01），pack(1) 保持字节偏移：
//   +0x00 char  m_game     gc_game
//   +0x01 int   m_no       gc_no（未对齐）
//   +0x05 char  m_channel[0x1e]  gc_channel
//   +0x23 short m_chGroup  gc_ch_group
//   +0x25 char  m_ip[0x10] gc_ip
//   +0x35 short m_new      gc_new
//   +0x37 short m_now      gc_now
//   +0x39 short m_out      gc_out
#pragma pack(push, 1)
struct SIG_SAVE_LOGGAMECHANNEL
{
    char  m_game;            // +0x00
    int   m_no;              // +0x01
    char  m_channel[0x1e];   // +0x05
    short m_chGroup;         // +0x23
    char  m_ip[0x10];        // +0x25
    short m_new;             // +0x35
    short m_now;             // +0x37
    short m_out;             // +0x39
};
#pragma pack(pop)

struct SIG_SAVE_PC_ROOM_PLAY_TIME
{
    char m_pad[0x9];
};

struct SIG_SAVE_PROXY_LOG
{
    char m_pad[0x2d];
};

struct SIG_SAVE_PVP
{
    char m_pad[0x88];
};

struct SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK
{
    char m_pad[0x20];
};

struct SIG_SECOND_USER_DATA
{
    char m_pad[0x14];
};

struct SIG_SECOND_USER_DATA_SET
{
    char m_pad[0x18];
};

struct SIG_SECURITY_CARD_ACTIVATE
{
    char m_pad[0xc];
};

struct SIG_SECURITY_CARD_ADD_CERT_CNT
{
    char m_pad[0x4];
};

struct SIG_SECURITY_CARD_DISUSE
{
    char m_pad[0x4];
};

struct SIG_SECURITY_CARD_ISSUE
{
    char m_pad[0x90];
};

struct SIG_SECURITY_CARD_RETRANSFER
{
    char m_pad[0x8c];
};

struct SIG_SECURITY_CARD_SERVICE_VALIDITYTIME
{
    char m_pad[0x1c];
};

struct SIG_SECURITY_CARD_UPDATE_FAIL_CNT
{
    char m_pad[0x18];
};

struct SIG_SECURITY_CARD_UPDATE_VALIDITYTIME
{
    char m_pad[0x8];
};

struct SIG_SECU_SERVICE_REWARD
{
    char m_pad[0x8];
};

struct SIG_SEND_USER_REWARD
{
    char m_pad[0x4];
};

struct SIG_STATISTIC_CERASHOP_GIFT_LOG
{
    char m_pad[0x18];
};

struct SIG_SWAP_CREATURE_ITEM
{
    // ORIG CDBMsgSender::SwapCreatureItem(W 0x833fa0c) memset 0x74 后填充：
    int m_itemIdx1;           // +0x00（item1->get_add_info()）
    int m_slot1;              // +0x04
    unsigned char m_empty1;   // +0x08（item1->isEmpty() 时置 1）
    char m_pad09[3];          // +0x09
    int m_itemIdx2;           // +0x0c（item2->get_add_info()）
    int m_slot2;              // +0x10
    unsigned char m_empty2;   // +0x14（item2->isEmpty() 时置 1）
    char m_pad15[3];          // +0x15；ORIG 实例化 sizeof=0x18（GetInBuffer<SIG_SWAP_CREATURE_ITEM>）
};
struct SIG_TOD_RELOADED_USER_APC
{
    char m_pad[0x758];
};

struct SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL
{
    char m_pad[0x30];
};

struct SIG_UPDATE_AVATAR_JEWEL_SOCKET
{
    char m_pad[0x28];
};

struct SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD
{
    char m_pad[0x10];
};

struct SIG_UPDATE_CHARACTER_DAY_EVENT
{
    char m_pad[0x8];
};

struct SIG_UPDATE_CHARAC_VIEW
{
    char m_pad[0x14d8];
};

struct SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC
{
    char m_pad[0x20];
};

struct SIG_UPDATE_DIMENSION
{
    char m_pad[0x10];
};

struct SIG_UPDATE_LOGIN
{
    char m_pad[0x20];
};

struct SIG_UPDATE_LOGOUT
{
    char m_pad[0x8b];
};

struct SIG_UPDATE_QUEST
{
    char m_pad[0x7601];
};

struct SIG_UPDATE_SCHOOL_POINT
{
    char m_pad[0x8];
};

// 真实布局（GameWorld::UpdateStatistic4LEV ORIG memset 0x124；原 0x100 pad 已替换）
struct SIG_UPDATE_STATISTIC4LEV
{
    int m_gcNo;              // +0x00
    int m_count[71];         // +0x04..+0x11f（UpdateStatistic4ChannelUserLev memset 0x124、按 level 直接下标）
    int m_time;              // +0x120（getCurSec）
};

struct SIG_UPDATE_USER_HANDICAP
{
    char m_pad[0x18];
};

struct SIG_UPGRADE_ACCOUNT_CARGO_RESULT
{
    char m_pad[0x8];
};

struct SIG_USER_PLAY_TIME_STATISTIC
{
    char m_pad[0xf4];
};

// Wave5 AltarProc 批次字段化（sizeof/偏移不变；证据 docs/class_func_reports/
// CAdvanceAltarLog/incrementEntranceData.md + updateDatabase.md trace 字段序）
struct STAdvanceAltarEntranceData
{
    unsigned int m_stageIndex;     // +0x00
    unsigned int m_enterCount;     // +0x04
    unsigned int m_successCount;   // +0x08
    unsigned int m_obtainStar;     // +0x0c
};                                 // 0x10

struct STAdvanceAltarLayerStatData
{
    unsigned short m_itemType;     // +0x00
    unsigned short m_field2;       // +0x02【名推断】
    unsigned int m_itemIndex;      // +0x04
    unsigned short m_itemLevel;    // +0x08
    unsigned short m_useCount;     // +0x0a
    unsigned short m_useStar;      // +0x0c
    unsigned short m_padE;         // +0x0e（补齐至 0x10；自然布局由对齐插入）
};                                 // 0x10

// STBossDungeonTowerLog 权威定义见 CBossDungeonEntranceLog.h（0x10 POD，
// 四个 uint 字段；ORIG memset 0x10 + 偏移 +0x0/+0x4/+0x8/+0xc 证据）。
#include "CBossDungeonEntranceLog.h"

// ORIG 布局（ItemVendingMachine::ProcessBuyRequestEx @0x081769f6 逐字节读序；
// Inter_ItemLimitEdition_Reply/dispatch_sig @0x84dc365 同布读写），sizeof = 0x27c。
struct stCeraShopIPGRequest_t
{
    unsigned int  m_productNo;       // +0x00（条目 product_no）
    int           m_groupIdx;        // +0x04
    int           m_itemIdx;         // +0x08
    unsigned short m_stackCount;     // +0x0c
    unsigned int  m_ceraPrice;       // +0x10
    unsigned int  m_winPoint;        // +0x14
    unsigned int  m_goldPrice;       // +0x18
    unsigned int  m_avatarField;     // +0x1c（头像选择写 local_60/local_1d0）
    unsigned int  m_itemCount;       // +0x20（发送条目总数）
    int           m_seq;             // +0x24（当前条目序号 = 下标+1）
    int           m_featuredIdx;     // +0x28
    unsigned char m_avatarType0;     // +0x2c
    unsigned char m_avatarType1;     // +0x2d
    unsigned char m_pad2e[2];        // +0x2e
    unsigned int  m_curSec;          // +0x30（CSystemTime::getCurSec）
    unsigned char m_buyFlag;         // +0x34（param_2+0xf）
    unsigned char m_pad35[3];        // +0x35
    int           m_characNo;        // +0x38
    unsigned short m_buytype;        // +0x3c（param_2+0xd）
    unsigned char m_pad3e[2];        // +0x3e
    unsigned int  m_accId;           // +0x40
    unsigned char m_subCount;        // +0x44（子项数 ≤ 0x23）
    unsigned char m_pad45[3];        // +0x45
    // +0x48 子项数组：每项 8 字节 { int idx; unsigned char val; 3 填充 }，步长 8，
    // 下标 0..subCount-1，落在 (local_24+8)*8 位置（即 0x48 + local_24*8）。
    // subCount 最大 0x23（下标 0x22），末项 +0xc 止于 0x15c < 0x160。
    unsigned char m_subEntries[0x23][8];   // +0x48
    unsigned char m_optCount;              // +0x160（选项数 ≤ 0x23）
    unsigned char m_pad161[3];             // +0x161
    // +0x164 选项数组：每项 8 字节 { unsigned long itemIdx; unsigned short a;
    // unsigned short b; }，步长 8，基址 (0x2c+local_20)*8 = 0x160 + local_20*8；
    // 实际 2 字节字段写 +0x8/+0xa（即 0x168+local_20*8 / 0x16a+local_20*8）。
    // 选项数最大 0x23（下标 0x22），末项 +0xa 止于 0x27a < 0x280。
    unsigned char m_optEntries[0x23][8];   // +0x164
};
static_assert(sizeof(stCeraShopIPGRequest_t) == 0x27c, "stCeraShopIPGRequest_t ABI drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_productNo) == 0x000, "stCeraShopIPGRequest_t productNo offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_itemIdx) == 0x008, "stCeraShopIPGRequest_t itemIdx offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_stackCount) == 0x00c, "stCeraShopIPGRequest_t stackCount offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_ceraPrice) == 0x010, "stCeraShopIPGRequest_t ceraPrice offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_goldPrice) == 0x018, "stCeraShopIPGRequest_t goldPrice offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_itemCount) == 0x020, "stCeraShopIPGRequest_t itemCount offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_seq) == 0x024, "stCeraShopIPGRequest_t seq offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_curSec) == 0x030, "stCeraShopIPGRequest_t curSec offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_characNo) == 0x038, "stCeraShopIPGRequest_t characNo offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_buytype) == 0x03c, "stCeraShopIPGRequest_t buytype offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_accId) == 0x040, "stCeraShopIPGRequest_t accId offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_subCount) == 0x044, "stCeraShopIPGRequest_t subCount offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_subEntries) == 0x048, "stCeraShopIPGRequest_t subEntries offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_optCount) == 0x160, "stCeraShopIPGRequest_t optCount offset drift");
static_assert(offsetof(stCeraShopIPGRequest_t, m_optEntries) == 0x164, "stCeraShopIPGRequest_t optEntries offset drift");

// 金币控制统计结构（Secu_GoldControl 用），ORIG sizeof = 0x3c。
// 6 组 64 位值（gold/import/sub/importSub/auctionAdd/auctionSub），后随 6 个计数器。
struct user_gold_info
{
    unsigned int m_goldLow;        // +0x00
    unsigned int m_goldHigh;       // +0x04
    unsigned int m_importLow;      // +0x08
    unsigned int m_importHigh;     // +0x0c
    unsigned int m_subLow;         // +0x10
    unsigned int m_subHigh;        // +0x14
    unsigned int m_importSubLow;   // +0x18
    unsigned int m_importSubHigh;  // +0x1c
    unsigned int m_auctionAddLow;  // +0x20
    unsigned int m_auctionAddHigh; // +0x24
    unsigned int m_auctionSubLow;  // +0x28
    unsigned int m_auctionSubHigh; // +0x2c
    unsigned short m_count[6];     // +0x30..0x3a
};


// ---- Additional types for GetOutBuffer instantiations ----
struct MSG_STAT_BREAKJAR { char m_pad[0xc]; };
struct MSG_STAT_COMPOSITON { char m_pad[0xd]; };
struct MSG_STAT_DUNGEONCLEAR { char m_pad[0x19]; };
struct MSG_STAT_PVPPLAY { char m_pad[0x10]; };
#include "MSG_STAT_RANKRES.h"   // 权威 MSG_STAT_RANKRES（0x6c，替换占位）
struct Packet_Mannerless_User_Accusation { char m_pad[0x15f]; };
struct Packet_PvPResult { char m_pad[0x80]; };
struct SIG_AVATAR_MOVE { char m_pad[0x16]; };
struct SIG_BILLING_RECORD { char m_pad[0x10e]; };
struct SIG_BOOSTER_GAGE_DATA { char m_pad[0x8]; };
struct SIG_CERASHOP_ERROR_LOG { char m_pad[0x1c]; };
struct SIG_CIRCULATION_STATISTIC { char m_pad[0xc8]; };
struct SIG_DELETE_CHANNEL { char m_pad[0xc]; };
struct SIG_INSERT_CHANNEL { char m_pad[0x5c]; };
struct SIG_INSERT_TICKET { char m_pad[0x8]; };
struct SIG_MANNERLESS_USER { char m_pad[0x13a]; };
struct SIG_NAT_TYPE { char m_pad[0x14]; };
struct SIG_REMOVE_EVENTITEM { char m_pad[0x18]; };
struct SIG_REMOVE_FEATURED_GOODSIDX { char m_pad[0x14]; };
// SIG_REQUEST_PVP_MISSION_LIST —— PvP 任务存档结构（ORIG 0x14a 字节）。
// CMissionList_Charac.cpp 曾以本地 struct 定义（与权威冲突）；按 ORIG 迁移至本头。
// ORIG 证据（df_game_r）：
//   - ctor _ZN28SIG_REQUEST_PVP_MISSION_LISTC1Ev @0x848f8d4：逐项构造 m_missionList[36]
//     （+0x04 起步长 8），并写 +0x144/+0x146/+0x148 三个 short（无尾部字节）。
//   - Stream::GetOutBuffer<SIG_REQUEST_PVP_MISSION_LIST> @0x8453bbc：enable_out/out_ptr
//     均传 0x14a ⇒ sizeof 恰为 0x14a。
//   - CMissionList_Charac::getData @0x85e4ac4：memset(param_1, 0, 0x14a)，
//     +0x00 int、+0x04 m_missionList[36]、+0x124 m_cleared[32]、
//     +0x144/+0x146/+0x148 三 short。
#pragma pack(push, 1)
struct SIG_REQUEST_PVP_MISSION_LIST
{
    int m_characNo;                      // +0x00
    MissionInfo m_missionList[36];       // +0x04（8*36 = 0x120）
    unsigned char m_cleared[32];         // +0x124
    short m_field144;                    // +0x144
    short m_field146;                    // +0x146
    short m_field148;                    // +0x148（末字段，无尾部 pad）
};
#pragma pack(pop)
static_assert(sizeof(SIG_REQUEST_PVP_MISSION_LIST) == 0x14a, "SIG_REQUEST_PVP_MISSION_LIST ABI drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_characNo) == 0x000, "SIG_REQUEST_PVP_MISSION_LIST characNo offset drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_missionList) == 0x004, "SIG_REQUEST_PVP_MISSION_LIST missionList offset drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_cleared) == 0x124, "SIG_REQUEST_PVP_MISSION_LIST cleared offset drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_field144) == 0x144, "SIG_REQUEST_PVP_MISSION_LIST field144 offset drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_field146) == 0x146, "SIG_REQUEST_PVP_MISSION_LIST field146 offset drift");
static_assert(offsetof(SIG_REQUEST_PVP_MISSION_LIST, m_field148) == 0x148, "SIG_REQUEST_PVP_MISSION_LIST field148 offset drift");
// SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD 真实定义（原在 CPremiumLetheManager.cpp；DB 队列负载）
// ORIG 实证：ProcessInitLetheSkill@0x8434614 存取 +0x1a6(treeKind)、+0xd(skills,0x198)、+4(characNo)。
// 权威迁移至本头，CPremiumLetheManager.cpp 删本地定义（FixMissingSigTypes 广播周知）。
#pragma pack(push, 1)
struct SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD
{
    int m_accID;                     // +0x00
    int m_characNo;                  // +0x04
    char m_pad08[4];                 // +0x08
    unsigned char m_state;           // +0x0c
    char m_skills[0x198];            // +0x0d
    unsigned char m_state2;          // +0x1a5
    unsigned char m_treeKind;        // +0x1a6
    unsigned char m_pad1a7;          // +0x1a7（ORIG 实例化 sizeof=0x1a8）
};
#pragma pack(pop)
static_assert(sizeof(SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD) == 0x1a8, "SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD ABI drift");
struct SIG_SAVE_ACCOUNT_INFO { char m_pad[0x70]; };
struct SIG_SAVE_CHARACINFO4GUILD { char m_pad[0x14]; };
struct SIG_SAVE_EXPAND_CHARAC { char m_pad[0x89c]; };
struct SIG_SAVE_FEATURED_GOODSIDX { char m_pad[0x14]; };
struct SIG_SAVE_PREMIUM_INFO { char m_pad[0x18]; };
struct SIG_SAVE_RANK { char m_pad[0x1f48]; };
struct SIG_SELECT_SERVER_MESSAGE { char m_pad[0x4]; };
struct SIG_UPDATE_CHANNEL { char m_pad[0x3a]; };
struct SIG_UPDATE_DETAIL_CHANNEL { char m_pad[0x10]; };
struct SIG_UPDATE_GROWTYPE { char m_pad[0x5]; };
struct SIG_UPDATE_NPC_RELATIONSHIP { char m_pad[0x260]; };
struct SIG_UPDATE_SERVER_MESSAGE { char m_pad[0x60]; };
struct SIG_VALUE_STATISTIC { char m_pad[0x80]; };
// stAchievement_DBSaveData 权威定义见 CAchievement.h（此处不重复）
struct stTitleBook_Save { char m_pad[0x6b3e]; };

#endif  // DNF_GAME_SIG_TYPES_H_
