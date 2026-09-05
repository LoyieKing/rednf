#ifndef DNF_GAME_SIG_TYPES_A_H_
#define DNF_GAME_SIG_TYPES_A_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 A（InformNoticeFlagData .. CHECK 域）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================


#include "SIG_BEST_CLEAR_TIME_DATA.h"  // SIG_BEST_CLEAR_TIME::m_data[1000] 元素
#include "SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER.h"  // SYSTEM_MANAGER::m_rewards[30] 元素

// SIG_AVATAR_INSERT::m_jewelSocket 字段需完整类型（GamePODTypes.h 为纯 POD 头、
// 无 include 依赖，无循环；原前向声明升级为定义 include）。
#include "GamePODTypes.h"
#include <cstddef>  // offsetof（SIG_AVATAR_INSERT 偏移断言）
struct InformNoticeFlagData
{
    char m_pad[0x14];
};

// ORIG sizeof = 0x3f84（MSG_CHECK_MAIL_RECEIVER::set<1, MSG_BUY_CERASHOP_ITEM> @0x82378ee
// `movl $0x3f84,0x8(%esp); call memcpy`；交叉验证 Dispatcher_BuyAutomatItem / ARAD db_dispatch）。
// 字段化（2026-09-04 裸偏移清理，GlobalData 批次；m_pad 保留兼容既有 body[offset] 消费方）。
// 宽度证据：docs/class_func_reports/ItemVendingMachine/ProcessBuyRequestEx.md（ORIG 0x081769f6）
//  movzwl 0xd(%eax)=buytype / movzbl 0xf=buyFlag / mov 0x2e=accId / mov 0x32=characNo /
//  movzbl 0x36=itemCount；条目区基址 +0x30、步长 imul $0x1cf（entry+0x07/0x08 movzbl 槽位/
//  标签、entry+0x09 mov productNo、entry+0x0d movzbl subCount、entry+0x0e 子项 stride5
//  {int idx; uchar val}、entry+0xbd movzbl optCount、entry+0xbe 选项 stride8
//  {mov 0xe / movzwl 0x12 / movzwl 0x14 形如 u32,u16,u16}）。
#pragma pack(push, 1)
struct MSG_CERA_SHOP_BUY_SCALARS
{
    char m_pad0[0x0d];         // +0x00..0x0c
    unsigned short m_buytype;  // +0x0d（movzwl 实证）
    unsigned char m_buyFlag;   // +0x0f（movzbl+movsbl 实证，char 语义）
    char m_pad10[0x1e];        // +0x10..0x2d
    unsigned int m_accId;      // +0x2e（movl 实证）
    int m_characNo;            // +0x32（movl 实证）
    unsigned char m_itemCount; // +0x36（movzbl 实证，≤0x23 clamp）
};
struct MSG_CERA_SHOP_BUY_ENTRY   // 条目（相对条目基址偏移；stride 0x1cf）
{
    char m_pad0[0x07];           // +0x00..0x06
    unsigned char m_avatarType0; // +0x07（movzbl %al 直用）
    unsigned char m_avatarType1; // +0x08（movzbl; cbtw; movzwl → ushort 形参）
    int m_productNo;             // +0x09（mov 实证）
    unsigned char m_subCount;    // +0x0d（movzbl 实证）
    struct SubItem { int m_itemIdx; unsigned char m_value; };   // stride 5
    SubItem m_subEntries[0x16];  // +0x0e（ORIG subCount>0x16 时越入 optCount 区，线格式遗留）
    char m_pad7c[0x41];          // +0x7c..0xbc
    unsigned char m_optCount;    // +0xbd（movzbl 实证）
    struct OptItem { unsigned long m_itemUid; unsigned short m_a; unsigned short m_b; };
    OptItem m_optEntries[0x22];  // +0xbe（ORIG k 达 0x22 时越入邻条目尾，线格式遗留 P156）
    char m_pad1ce;               // +0x1ce（补齐 stride 0x1cf）
};
struct MSG_CERA_SHOP_BUY_ENTRIES  // 条目区组件视图（基址 +0x30，P161 两步寻址）
{
    char m_pad0[0x30];
    MSG_CERA_SHOP_BUY_ENTRY m_arr[0x23];   // 0x30+0x23*0x1cf=0x3f4b ≤ 0x3f84
};
struct MSG_BUY_CERASHOP_ITEM
{
    union
    {
        char m_pad[0x3f84];               // 线格式缓冲（既有消费方 body[offset] 兼容）
        MSG_CERA_SHOP_BUY_SCALARS m_scalars;
        MSG_CERA_SHOP_BUY_ENTRIES m_entrySection;
    };
};
#pragma pack(pop)
static_assert(sizeof(MSG_CERA_SHOP_BUY_ENTRY::SubItem) == 5, "sub item stride drift");
static_assert(sizeof(MSG_CERA_SHOP_BUY_ENTRY::OptItem) == 8, "opt item stride drift");
static_assert(sizeof(MSG_CERA_SHOP_BUY_ENTRY) == 0x1cf, "entry stride drift");
static_assert(offsetof(MSG_CERA_SHOP_BUY_ENTRY, m_optCount) == 0xbd, "optCount drift");
static_assert(offsetof(MSG_CERA_SHOP_BUY_ENTRY, m_optEntries) == 0xbe, "optEntries drift");
static_assert(offsetof(MSG_CERA_SHOP_BUY_ENTRY, m_subEntries) == 0x0e, "subEntries drift");
static_assert(offsetof(MSG_CERA_SHOP_BUY_SCALARS, m_itemCount) == 0x36, "itemCount drift");
static_assert(sizeof(MSG_BUY_CERASHOP_ITEM) == 0x3f84, "MSG_BUY_CERASHOP_ITEM ABI drift");

struct Packet_Tcp_Server_Connect
{
    char m_pad[0xa];       // ORIG +0x00..0x09（未初始化读区）
    char m_serverFlag;     // +0x0a（ORIG 0x0863814c mov %dl,0xa(%eax) 单字节写）
};

struct SIG_ACCOUNT_CARGO_DATA
{
    char m_pad[0xd60];
};

// 字段化（W8DB，ORIG 0x0842ad30 makeRequest 实证：+0 uint accId、+4 byte
// flag（写侧 `flag?1:0`）、+8 uint uid；dispatch 0x0842acf4 读 +4 byte==1
// → expireLoginTime(*(uint*)sig)）：
struct SIG_ARRANGE_USER_LOGIN_INFO
{
    unsigned int m_accId;    // +0x00
    char m_flag;             // +0x04
    char m_pad5[3];          // +0x05..0x07
    unsigned int m_uid;      // +0x08
};
static_assert(sizeof(SIG_ARRANGE_USER_LOGIN_INFO) == 0xc,
              "SIG_ARRANGE_USER_LOGIN_INFO ABI drift");

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
    union {
        unsigned long long m_key;   // +0x01 日志主键（消费者按 8B 读）
        unsigned int m_keyWords[2]; // ORIG 生产方为两条 plain 4B 写（dispatch*Packet asm）
    };                             // sizeof 不变（8B），偏移/布局零改动
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
    union {
        unsigned long long m_key;   // +0x01
        unsigned int m_keyWords[2]; // ORIG 生产方两条 plain 4B 写
    };
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

// 字段化（SecuLink 批次 2026-09-05，裸偏移清理；union 叠加保留 m_pad 基名）。
// 布局/宽度证据：写侧 DB_LoadAutoMarketConditionsControl::dispatch
// （W8LT_DBTask.cpp DB_AutoMarketConditionView/DB_AutoMarketDailyRow，SQL 列名
// optimum_gold_supply/over_gold/total_gold/auction_gold/gold_phase/item_phase/
// durability_phase）；读侧 CAutoMarketConditionsControl::LoadDatabase ORIG
// 0x082f8684（docs/class_func_reports/CAutoMarketConditionsControl/LoadDatabase.md
// 汇编逐条 disp 实证：+0x00/+0x08 qword、+0x10..+0x67 daily 两行（row stride
// 0x2c，pack(1) 非对齐 qword）、+0x30/+0x34/+0x38 int 相位、+0x68 movzbl flag）。
// sizeof 0x6c 不变。
#pragma pack(push, 1)
struct SIG_AUTO_MARKET_CONDITIONS_CONTROL
{
    union
    {
        char m_pad[0x6c];
        struct
        {
            unsigned long long m_overGold;      // +0x00（单行 over_gold）
            union
            {
                unsigned long long m_optimumGold;   // +0x08（单行 optimum_gold_supply）
                unsigned int m_optimumGoldWords[2]; // ORIG trace 按两条 4B 读 +0x08/+0x0c
            };
            struct
            {
                unsigned long long m_totalGold;     // row +0x00 total_gold
                unsigned long long m_auctionGold;   // row +0x08 auction_gold
                unsigned long long m_overGold;      // row +0x10 over_gold
                unsigned long long m_optimumGold;   // row +0x18 optimum_gold_supply
                int m_goldPhase;                    // row +0x20 gold_phase
                int m_itemPhase;                    // row +0x24 item_phase
                int m_durabilityPhase;              // row +0x28 durability_phase
            } m_rows[2];                            // +0x10 / +0x3c（daily 两行）
            unsigned char m_flag;                   // +0x68（LoadDatabase movzbl 判定）
            char m_pad69[3];                        // +0x69..0x6b
        } m_view;
    };
};
#pragma pack(pop)
static_assert(sizeof(SIG_AUTO_MARKET_CONDITIONS_CONTROL) == 0x6c,
              "SIG_AUTO_MARKET_CONDITIONS_CONTROL ABI drift");
static_assert(offsetof(SIG_AUTO_MARKET_CONDITIONS_CONTROL, m_view.m_rows[1]) == 0x3c,
              "SIG_AUTO_MARKET_CONDITIONS_CONTROL m_rows[1] offset drift");
static_assert(offsetof(SIG_AUTO_MARKET_CONDITIONS_CONTROL, m_view.m_flag) == 0x68,
              "SIG_AUTO_MARKET_CONDITIONS_CONTROL m_flag offset drift");

struct SIG_AUTO_PUNISH_RULE_BLACK_IP
{
    char m_pad[0x3e84];
};

struct SIG_AUTO_PUNISH_RULE_HACK_TYPE
{
    char m_pad[0x2720];
};

// 具名布局（GoalAuditDB7；ORIG DB_AvatarChangeOwner::dispatch 0x84004bc /
// makeRequest 0x840055c 实证）：+0x00 ui_id、+0x04 原 charac_no、+0x08 新
// charac_no、+0x0c slot byte。pack(1)：sizeof 0xd 不变（ORIG 13 字节载荷）。
#pragma pack(push, 1)
struct SIG_AVATAR_CHANGE_OWNER
{
    unsigned int m_uiId;        // +0x00
    unsigned int m_characNo;    // +0x04
    unsigned int m_newCharacNo; // +0x08
    unsigned char m_slot;       // +0x0c
};
#pragma pack(pop)
static_assert(sizeof(SIG_AVATAR_CHANGE_OWNER) == 0xd,
              "SIG_AVATAR_CHANGE_OWNER ABI drift");

// 字段化（CInv 批次 2026-09-04，裸偏移清理）。宽度/偏移证据：
// SendAvatarEvent 0x36 分支（docs/class_func_reports/CInventory/SendAvatarEvent.md
// 汇编 0x8508eee-0x8508f0a）：movl(param_2)@+0x00、movl(param_3)@+0x04、
// movb(param_4 低字节)@+0x08、movl(charac_no)@+0x09（非对齐 4 字节写）。
// 消费侧语义（W8LT_DBTask.cpp DB_AvatarChangeStat 视图注释实证）：+0x00=ui_id、
// +0x04=it_id、+0x08=slot byte、+0x09 起 4 字节 charac_no。
#pragma pack(push, 1)
struct SIG_AVATAR_CHANGE_STAT
{
    int m_uiId;              // +0x00
    int m_itId;              // +0x04
    unsigned char m_slot;    // +0x08
    int m_characNo;          // +0x09（ORIG 非对齐 movl）
    char m_pad0d[0x44];      // +0x0d..0x50
};
#pragma pack(pop)
static_assert(sizeof(SIG_AVATAR_CHANGE_STAT) == 0x51,
              "SIG_AVATAR_CHANGE_STAT ABI drift");

// ORIG GetInBuffer 实例化（SendAvatarEvent 0x8508ed2 分支）sizeof 0x51；
// 前 10 字节载荷：+0x00 b、+0x04 c、+0x08 (char)d、+0x09 (char)e。
struct SIG_AVATAR_CHANGE_STATE
{
    char m_pad[0x51];
};

// 字段化（InvC10 batch-10，ORIG SendAvatarEvent 0x08508f43 段写侧实证：
// mov %edx,(%eax)=characNo / mov %edx,0x4=uiId / mov %dx,0x8=short / mov %dx,0xa=short；
// sizeof 0xc 与 m_pad 原形一致）。
struct SIG_AVATAR_COLOR_CHANGE
{
    int m_characNo;      // +0x00
    int m_uiId;          // +0x04
    short m_field8;      // +0x08
    short m_fieldA;      // +0x0a
};

// 字段化（CInv 批次 2026-09-04，裸偏移清理）。写侧证据 = SIG_AVATAR_INSERT::set
// （ORIG 0x850d3c8，objdump：movl a1@+0x00 / movl a2@+0x04 / movb a3低字节@+0x08 /
//  movb c@+0x09 / movl expire@+0x0a / movb bSet@+0x0e / strncpy 0x18@+0x0f /
//  movl a7@+0x28（单 4B 写；DB 消费侧 0x83ffe0e 按 +0x28/+0x2a 两 word 读，
//  同函数 int-vs-word 粒度先例由 DB_LoadInventory 侦察交叉确认）/ movb a8低字节@+0x2c）；
// setJewelSocket（ORIG 0x850d44c）块拷 emblem 0x1e 字节 → +0x2d；
// _SaveAvatarItem（0x8509fbf）movl expansion 前 4 字节 → +0x4f；
// +0x4b = DB_AvatarItemInsert::_SendResult（0x84001fe）写 GetIdentity（W8LT 视图实证）。
#pragma pack(push, 1)
struct SIG_AVATAR_INSERT
{
    int m_characNo;               // +0x00（_SaveAvatarItem 传 get_charac_no(-1)）
    unsigned int m_itId;          // +0x04（DB 侧读 it_id）
    unsigned char m_slot;         // +0x08（槽位+10 低 8 位）
    unsigned char m_abilityNo;    // +0x09（set 形参 c；DB 侧 ability_no）
    int m_period;                 // +0x0a（到期/周期，DB expire_date 分支）
    unsigned char m_obtainFrom;   // +0x0e（set 形参 bSet；DB obtain_from）
    char m_agency[0x18];          // +0x0f（strncpy 0x18；DB ipg_agency_no）
    char m_pad27;                 // +0x27（agency 尾 NUL 槽，set  strncpy 到 +0x26
                                  //      为止；ORIG a7 movl 在 +0x28）
    int m_a7;                     // +0x28（set 单 movl；DB 分读 +0x28 hidden_option /
                                  //      +0x2a emblem_endurance 两 word）
    unsigned char m_a8;           // +0x2c（set 形参 a8 低字节；DB stat）
    stAvatarEmblemInfo_t m_jewelSocket;  // +0x2d..0x4a（setJewelSocket 块拷 emblem）
    int m_identity;               // +0x4b（_SendResult 0x84001fe 写 GetIdentity(db)）
    int m_expansion;              // +0x4f（_SaveAvatarItem 0x8509fbf movl =
                                  //      stAvatarExpansionInfo_t 前 4 字节 m_color[0..1]）
    // 非静态成员（ORIG mangled _ZN17SIG_AVATAR_INSERT3setEijiibcPKcim，this=ins）：
    void set(int a1, unsigned int a2, int a3, int expire, bool bSet, char c,
             const char* agency, int a7, unsigned long a8);
    void setJewelSocket(const stAvatarEmblemInfo_t& emblem);  // 非静态：ORIG 0x850d44c this=ins
};
#pragma pack(pop)
static_assert(sizeof(SIG_AVATAR_INSERT) == 0x53,
              "SIG_AVATAR_INSERT ABI drift");
static_assert(offsetof(SIG_AVATAR_INSERT, m_jewelSocket) == 0x2d,
              "SIG_AVATAR_INSERT m_jewelSocket offset drift");
static_assert(offsetof(SIG_AVATAR_INSERT, m_identity) == 0x4b,
              "SIG_AVATAR_INSERT m_identity offset drift");
static_assert(offsetof(SIG_AVATAR_INSERT, m_expansion) == 0x4f,
              "SIG_AVATAR_INSERT m_expansion offset drift");
// 字段化（InvC10 batch-10，ORIG SendAvatarEvent 0x08508f0f 段写侧实证：
// mov %edx,(%eax)=uiId(int) / mov %dl,0x4=slot(char) / mov characNo,0x8(int)；
// sizeof 0xc 不变）。
struct SIG_AVATAR_OPTION_CHANGE
{
    int m_uiId;          // +0x00
    char m_field4;       // +0x04
    int m_characNo;      // +0x08
};

// ORIG W ctor 0x0848fb02：2×movl(+0/+4) 清零 + 循环 1000 次 SIG_BEST_CLEAR_TIME_DATA
// ctor（+0x8 起步进 0x18；循环初值 0x3e7=999，P151 口径 1000 次）；sizeof 0x5dc8。
struct SIG_BEST_CLEAR_TIME
{
    int m_field0;                  // +0x00（ctor 清零）
    int m_field4;                  // +0x04（ctor 清零）
    SIG_BEST_CLEAR_TIME_DATA m_data[1000];  // +0x08（ctor 隐式构造循环）
    SIG_BEST_CLEAR_TIME() : m_field0(0), m_field4(0) {}
};

struct SIG_BLOOD_DUNGEON
{
    // ORIG 0x832a330 _saveData / 0x832a4c6 loadData 字节映射实证；
    // 字段语义 [推断]（与 CCharacBloodDungeon 内存镜像对应，见 W9_EventSmall.h）。
    int m_characNo;            // +0x00
    int m_point;               // +0x04
    int m_weeklyEnterCount;    // +0x08
    int m_useGold;             // +0x0c
    int m_dailyInitTime;       // +0x10（getDailyInitializeTime(6,0,0)）
    int m_field14;             // +0x14 [推断]
    int m_enterCount;          // +0x18
    unsigned char m_flag1c;    // +0x1c byte
    int m_field20;             // +0x20 [推断]
    int m_field24;             // +0x24 [推断]（sizeof = 0x28）
};
static_assert(sizeof(SIG_BLOOD_DUNGEON) == 0x28, "SIG_BLOOD_DUNGEON ABI drift");

struct SIG_BREAK_AWAY_PREVENT_MESSAGE
{
    char m_pad[0x8];
};

// ORIG W ctor 0x0831d2b0：m_rewards[30] 隐式构造循环（+0x10 起步进 0x18，循环初值
// 0x1d=29，P151 口径 30 次）→ 7×movw(+0..+0xc) → movb(+0xe) → movl(+0x2e0)；sizeof 0x2e4。
// 使用方（GlobalData.cpp loadDbData / InterDispatcher）均为指针/裸偏移访问，无成员名依赖。
struct SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER
{
    unsigned short m_field0;   // +0x00
    unsigned short m_field2;   // +0x02
    unsigned short m_field4;   // +0x04
    unsigned short m_field6;   // +0x06
    unsigned short m_field8;   // +0x08
    unsigned short m_fieldA;   // +0x0a
    unsigned short m_fieldC;   // +0x0c
    unsigned char m_fieldE;    // +0x0e
    SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER m_rewards[30];  // +0x10
    int m_field2e0;            // +0x2e0
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER()
    {
        m_field0 = 0;
        m_field2 = 0;
        m_field4 = 0;
        m_field6 = 0;
        m_field8 = 0;
        m_fieldA = 0;
        m_fieldC = 0;
        m_fieldE = 0;
        m_field2e0 = 0;
    }
};

struct SIG_BREAK_AWAY_UV_LIST
{
    char m_pad[0x1f];
};

struct SIG_CERASHOP_PURCHASE_COUNT
{
    char m_pad[0x10];
};

// 具名化（证据：docs/class_func_reports/DB_ReqChangeCharacName/_sendResult.md 反编译段，
// GetInBuffer<SIG_CHANGE_CHARAC_NAME> 偏移写入 +0 characNo / +4 strncpy(name,0x1d) /
// +0x24 int / +0x28 uint / +0x2c uint；DB_ReqChangeCharacName/dispatch.md 同证 get_uint(col0,+0x28)）。
struct SIG_CHANGE_CHARAC_NAME
{
    unsigned int m_characNo;    // +0x00（_sendResult ← param_1[0]，请求上下文 id）
    char m_name[0x1d];          // +0x04（strncpy 目标，0x1d 字节，未含 NUL 尾）
    char m_pad21[3];            // +0x21（0x1d 对齐尾，ORIG memset 覆盖）
    int m_arg24;                // +0x24（_sendResult ← param_3）
    unsigned int m_arg28;       // +0x28（dispatch ← MySQL col0 get_uint）
    unsigned int m_arg2c;       // +0x2c（_sendResult ← param_1[0x28]）
};
static_assert(sizeof(SIG_CHANGE_CHARAC_NAME) == 0x30, "SIG_CHANGE_CHARAC_NAME ABI drift");


struct SIG_CHARACTER_OPTION
{
    // 字段化（W8DB，ORIG 0x08430144 DB_SaveCharacterOption::dispatch 实证：
    // +0 uint characNo、+4 options blob 0x200（blob_to_str）；写侧
    // CUserOnlyOrig.cpp:0x08652dde SIG_CHARACTER_OPTION_VIEW 同布局）：
    unsigned int m_characNo;  // +0x000
    char m_options[0x200];    // +0x004（charac_option.options 存档区）
};
static_assert(sizeof(SIG_CHARACTER_OPTION) == 0x204,
              "SIG_CHARACTER_OPTION ABI drift");

// 具名化（DisPatcherCore 批次 2026-09-04；证据
// docs/class_func_reports/DisPatcher_AddFriend/process_packet_jpn.md
// ORIG T 0x081cb30a：+0x04 单字节写 `mov %al,0x4(%edx)`（!isGM(acc_id) 标志）、
// +0x05 起 `strncpy(sig+5, msg+0x11, 0x1e)`（0x081cb476/0x081cb487）。
// +0x00..0x03 与 +0x23 无 ORIG 出线证据，保留同尺寸字节域（sizeof 0x24 不变）。）
struct SIG_CHECK_ADD_FRIEND
{
    char m_pad0[4];            // +0x00..0x03（未建模）
    char m_notGmFlag;          // +0x04（非 GM 账号标志，1B）
    char m_characName[0x1e];   // +0x05..0x22（申请方角色名）
    char m_pad23[1];           // +0x23（保持 sizeof 0x24）
};
static_assert(sizeof(SIG_CHECK_ADD_FRIEND) == 0x24, "SIG_CHECK_ADD_FRIEND ABI drift");

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

// 具名化（证据：Dispatcher_CheckJoinGuild::process ORIG 0x081d4xxx memset 0x1c 后
// strncpy(+0,name,0x16)，serverGroup 写 +0x16；DB_CheckJoinGuild::makeRequest 0x0843cc06
// 仅 memcpy 0x1c 转发）。+0x18 起 4 字节无字段级证据保留 pad。
struct SIG_CHECK_JOIN_GUILD
{
    char m_guildName[0x16];          // +0x00（strncpy 目标 0x16，← MSG_BASE+0x11 名区）【推断：待查公会名】
    unsigned short m_serverGroup;    // +0x16（IsIntegratedPvPBaseChannel ? GetServerGroup : CEnvironment+0x378）
    // m_serverId 字段化（SecuLink 批次）：DB_CheckJoinGuild::dispatch ORIG
    // 0x0843c98e +0x59 `mov 0x18(%eax),%eax` → SQL `server_id=%d`（4B 有符号）
    // （docs/class_func_reports/DB_CheckJoinGuild/dispatch.md）；m_pad18 基名保留。
    union
    {
        char m_pad18[4];             // +0x18
        int m_serverId;              // +0x18（guild_info.server_id 查询键）
    };
};
static_assert(sizeof(SIG_CHECK_JOIN_GUILD) == 0x1c, "SIG_CHECK_JOIN_GUILD ABI drift");


// 具名化（证据：DB_CheckJoinGuild::dispatch ORIG 0x0843cc06 族，MySQL 逐列填充：
// +0 int result(0/0x15) / +4 get_str(col1,0x1e) / +0x22 get_str(col0,0x17) /
// +0x3c get_int(col2) / +0x40 get_int(col3)）。0x39..0x3c 无证据保留 pad。
struct SIG_CHECK_JOIN_GUILD_DB_REPLY
{
    int m_result;                 // +0x00（0=成功；0x15=无行）
    char m_name[0x1e];            // +0x04（get_str col1，0x1e）
    char m_guildName[0x17];       // +0x22（get_str col0，0x17）【推断：公会名】
    char m_pad39[3];              // +0x39（0x17 对齐尾，ORIG memset 覆盖）
    int m_arg3c;                  // +0x3c（get_int col2）
    int m_arg40;                  // +0x40（get_int col3）
};
static_assert(sizeof(SIG_CHECK_JOIN_GUILD_DB_REPLY) == 0x44, "SIG_CHECK_JOIN_GUILD_DB_REPLY ABI drift");


#pragma pack(push, 1)
struct SIG_CHECK_MY_ACCOUNT_IN_DB
{
    unsigned int m_param0;          // +0x00（DB_AccountHack_CheckMyAccount 实参 3）
    unsigned int m_accId;           // +0x04（实参 2）
    char m_name[0x1d];              // +0x08（memset 0x1d 后 strncpy）
    unsigned int m_money;           // +0x25（实参 4，非对齐）
};
#pragma pack(pop)

struct SIG_CHECK_MY_ACCOUNT_OK
{
    char m_pad[0xc];
};

struct SIG_CHECK_USER_SECU_REWARD
{
    char m_pad[0x8];
};

#endif  // DNF_GAME_SIG_TYPES_A_H_
