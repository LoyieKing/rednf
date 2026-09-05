#ifndef DNF_GAME_SIG_TYPES_E_H_
#define DNF_GAME_SIG_TYPES_E_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 E（SIG_SAVE_* 尾段 + 杂项/统计域）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================

#include <cstddef>  // offsetof static_assert

#include "Inven_Item.h"   // SIG_SAVE_INVENTORY 背包 Inven_Item 数组（pack(1)，0x3d）
#include "MissionTypes.h"  // MissionInfo（SIG_REQUEST_PVP_MISSION_LIST 任务列表）
#include "PvpResultType.h"  // SIG_SAVE_PVP 前段 PvpResultType 镜像（无依赖叶子头）
#include "Packet_PvPResult.h"  // 权威定义（shared/packet/include；原占位 char m_pad[0x80] 移除）

// 字段化（W8DB，ORIG 0x0842ff4e DB_SaveGameOption_1::dispatch 证据：
// NumberToString(*(uint*)sig)=m_id、compress(sig+4,0x48)；写侧
// Dispatchers.cpp:10054 `*(unsigned int*)save = get_acc_id()` 同槽）：
struct SIG_SAVE_GAME_OPTION_1
{
    unsigned int m_mId;      // +0x00（member_key_option.m_id ← accId）
    char m_option[0x48];     // +0x04（压缩存档区 0x48）
};
static_assert(sizeof(SIG_SAVE_GAME_OPTION_1) == 0x4c,
              "SIG_SAVE_GAME_OPTION_1 ABI drift");
struct SIG_SAVE_GAME_OPTION_QUICKCHAT
{
    char m_pad[0x25c];
};

// SIG_LIMIT_ITEM_INFO —— 限购物品信息（ORIG 唯一消费者 CUser::SetLimitInfo 0x086888f8；
// 条目区按 ORIG 读点 +0x08 long createTime / +0x0c itemId / +0x10=i*0x10 照抄）。
struct SIG_LIMIT_ITEM_INFO
{
    struct Entry
    {
        int m_field0;        // +0x00（写 usage.m_field19，i+1 读侧见报告）
        int m_field4;        // +0x04
        long m_createTime;   // +0x08（CommonTime::SetCreateTime 实参，32 位 long=4B）
        int m_itemId;        // +0x0c（与 stLimitItemUsageInfo::LimitInfo::m_a 匹配）
    };                       // sizeof 0x10
    int m_count;             // +0x00 条目数
    char m_pad4[8];          // +0x04..0x0b
    Entry m_entries[1];      // +0x0c 起，可变长（sizeof 之外按 count 扩展）
};

struct SIG_SAVE_GEO_REJECT
{
    char m_pad[0x20];
};

// 具名化（Fix11 real10b；proceed_trade @0x8530e64 填包 + DB_SaveGoldZeroTradeLog
// @0x084227f6 读包交叉实证）：
//   +0x00 to_m_id(uint) / +0x04 from_m_id(uint) / +0x08 from_nat_ip[0x10] /
//   +0x1c from_peer_ip[0x14] / +0x30 from_nat_type(byte) / +0x31 from_mac_addr[0x41] /
//   +0x72 to_nat_ip[0x10] / +0x86 to_peer_ip[0x14] / +0x9a to_nat_type(byte) /
//   +0x9b to_mac_addr[0x41] / +0xdc injury_money(uint)；sizeof 0xe0。
struct SIG_SAVE_GOLD_ZERO_TRADE_LOG
{
    unsigned int m_to_m_id;        // +0x00
    unsigned int m_from_m_id;      // +0x04
    char m_from_nat_ip[0x10];      // +0x08
    char m_pad18[4];               // +0x18（ORIG 该处 4B pad：from_peer_ip 落 +0x1c，非 pack(1) 连续 +0x18）
    char m_from_peer_ip[0x14];     // +0x1c
    unsigned char m_from_nat_type; // +0x30
    char m_from_mac_addr[0x41];    // +0x31
    char m_to_nat_ip[0x10];        // +0x72
    char m_pad82[4];               // +0x82（ORIG 该处 4B pad：to_peer_ip 落 +0x86，非 pack(1) 连续 +0x82）
    char m_to_peer_ip[0x14];       // +0x86
    unsigned char m_to_nat_type;   // +0x9a
    char m_to_mac_addr[0x41];      // +0x9b
    unsigned int m_injury_money;   // +0xdc
};  // ORIG sizeof 0xe0（＋0x18/0x82 两个 4B pad；此前 pack(1) 连续得 0xd8，-8）

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

// +0/+4/+8；ORIG ctor _ZN26SIG_SAVE_PC_ROOM_PLAY_TIMEC1Ev @0x0848fb64 清零三段；
// ORIG GetInBuffer 传 sizeof=9 → pack(1)。
#pragma pack(push, 1)
struct SIG_SAVE_PC_ROOM_PLAY_TIME
{
    unsigned int m_accId;        // +0x00 ←get_acc_id()
    unsigned int m_totalTime;    // +0x04 ←totalPcRoomPlayTime
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

// +0/+4/+8；ORIG ctor _ZN26SIG_SAVE_PC_ROOM_PLAY_TIMEC1Ev @0x0848fb64 清零三段。
#pragma pack(push, 1)
struct SIG_SAVE_PROXY_LOG
{
    unsigned int m_accId;         // +0x00（CUser::get_acc_id）
    char m_strIp[0x10];           // +0x04（CNetwork::get_str_ip，strncpy 0x10）
    char m_pad14[4];              // +0x14 对齐驻留（两 strncpy 目标相距 0x14）
    char m_peerIp[0x10];          // +0x18（GetPeerIP2，strncpy 0x10）
    char m_pad28[4];              // +0x28 驻留（m_natType 实测落位 +0x2c）
    unsigned char m_natType;      // +0x2c（PacketBuf get_byte natType）
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_PROXY_LOG) == 0x2d,
              "SIG_SAVE_PROXY_LOG ABI drift (ORIG InBuffer enable_in $0x2d)");

// 具名化（HdrFieldC）：sizeof 0x88 不变。
// 写侧 CUser::UpdateData @0x08653…（docs/class_func_reports/CUser/UpdateData.md）：
//   memset 0x88 后 *(sig+0x84)=getCurCharacNo()，PvpResultType::Set(sig,result)
//   将 33 个 int（0x84）整体 rep movsl 拷入前段（PvpResultType::Set 报告）。
// 读侧 DB_SavePvp::SavePvp @0x084172c8（docs/class_func_reports/DB_SavePvp/SavePvp.md）
//   SQL upDate pvp_result：win=+0x00 lose=+0x04 pvp_point=+0x08 pvp_grade=+0x14
//   avg_kill..avg_hit_penalty=+0x18..+0x50（15 项）pvp_count=+0x54 win_point=+0x58
//   play_time=+0x60 play_count=+0x64 where charac_no=+0x84。
// 具名化（HdrFieldC）：sizeof 0x88 不变；前段保留 POD blob（PvpResultType 有
//   ctor/dtor，内嵌会把本 SIG 变非 POD 并触发 GCC4.4 offsetof 警告）。
// 写侧 CUser::UpdateData @0x08653810（docs/class_func_reports/CUser/UpdateData.md）：
//   memset 0x88 后 *(sig+0x84)=getCurCharacNo()，PvpResultType::Set(sig,result)
//   将 33 个 int（0x84）整体 rep movsl 拷入前段（PvpResultType::Set 报告 @0x85d58e6）。
// 读侧 DB_SavePvp::SavePvp @0x084172c8（docs/class_func_reports/DB_SavePvp/SavePvp.md）
//   SQL upDate pvp_result：win=+0x00 lose=+0x04 pvp_point=+0x08 pvp_grade=+0x14
//   avg_kill..avg_hit_penalty=+0x18..+0x50（15 项）pvp_count=+0x54 win_point=+0x58
//   play_time=+0x60 play_count=+0x64 where charac_no=+0x84。
//   前段布局即 PvpResultType（PvpResultType.h 权威：m_totalWinCount@+0x00 …）。
struct SIG_SAVE_PVP
{
    char m_result[0x84];      // +0x00 PvpResultType 33-int 镜像（Set 整体拷入）
    unsigned int m_characNo;  // +0x84（UpdateData 写 getCurCharacNo / SQL charac_no）
};
static_assert(sizeof(SIG_SAVE_PVP) == 0x88, "SIG_SAVE_PVP ABI drift");
static_assert(offsetof(SIG_SAVE_PVP, m_characNo) == 0x84,
              "SIG_SAVE_PVP characNo offset drift");

// 具名化（HdrFieldC）：sizeof 0x20 不变。三侧证据闭环：
// 写侧 Inter_LoadCharac::dispatch_sig（InterDispatcher.cpp 0x13f9 区，异号 MID
//   记账分支）：+0x00=curAccId(get_acc_id)、+0x04=reqAccId(登录包)、
//   +0x08=登录包 data+0xc、+0x0c..0x20=GetPeerIP2(0x14)。
// 读侧 DB_SaveSameMIDOtherCharLoginHack::dispatch @0x08422680（报告）：
//   "inSert into login_view_log values(now(),%s,%s,%d,'%s')"
//   ← NumberToString(+0x00,0)/NumberToString(+0x04,1)/%d=+0x08/'%s'=+0x0c。
struct SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK
{
    unsigned int m_curAccId;    // +0x00 当前登录 acc_id
    unsigned int m_reqAccId;    // +0x04 登录包 acc_id（与 +0x00 异号触发）
    unsigned int m_characNo;    // +0x08 登录包 charac_no（data+0xc）
    char m_peerIp[0x14];        // +0x0c GetPeerIP2（login_view_log 第 5 列）
};
static_assert(sizeof(SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK) == 0x20,
              "SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK ABI drift");

struct SIG_SECOND_USER_DATA
{
    char m_pad[0x14];
};

struct SIG_SECOND_USER_DATA_SET
{
    char m_pad[0x18];
};

// 字段化（SecuLink 批次 2026-09-05，裸偏移清理；union 叠加保留 m_pad 基名）。
// 证据：makeRequest ORIG 0x0842bb5c（docs/class_func_reports/
// DB_SecurityCardActivate/makeRequest.md：`mov %edx,(%eax)` accId 4B、
// strncpy(sig+4,certKey,6)、`mov %dl,0xb(%eax)` 1B）；dispatch ORIG 0x0842b86c
// （同目录 dispatch.md：SQL cert_key='%s' ← sig+4、`movzbl 0xb` history 模式、
// `mov %dl,0xa` 激活结果回写，均 1B）。成员名沿用同族
// SIG_SECURITY_CARD_ADD_CERT_CNT 的 m_mId 约定。sizeof 0xc 不变。
struct SIG_SECURITY_CARD_ACTIVATE
{
    union
    {
        char m_pad[0xc];
        struct
        {
            unsigned int m_mId;               // +0x00（member_security_card.m_id ← accId）
            char m_certKey[6];                // +0x04（strncpy 6；SQL cert_key）
            unsigned char m_activateResult;   // +0x0a（dispatch 回写 ok，1B）
            unsigned char m_historyFlag;      // +0x0b（history modify_type 选择，1B）
        };
    };
};
static_assert(sizeof(SIG_SECURITY_CARD_ACTIVATE) == 0xc,
              "SIG_SECURITY_CARD_ACTIVATE ABI drift");

// 字段化（W8DB，ORIG 0x08436388 makeRequest 写 +0 accId；dispatch
// NumberToString(m_id,0)——成员名沿用消费视图
// OrigDBInter20.cpp SecurityCardAddCertCntView::m_mId）：
struct SIG_SECURITY_CARD_ADD_CERT_CNT
{
    unsigned int m_mId;      // +0x00（member_security_card.m_id ← accId）
};
static_assert(sizeof(SIG_SECURITY_CARD_ADD_CERT_CNT) == 0x4,
              "SIG_SECURITY_CARD_ADD_CERT_CNT ABI drift");

struct SIG_SECURITY_CARD_DISUSE
{
    // 字段化（W8DB，ORIG 0x0842b790 makeRequest 写 +0 accId；dispatch
    // 0x842b6e6 `*(uint*)sig` = m_id，见 W8LT_DBTask.cpp SecuDisuseView）：
    unsigned int m_mId;      // +0x00（member_security_card.m_id ← accId）
};
static_assert(sizeof(SIG_SECURITY_CARD_DISUSE) == 0x4,
              "SIG_SECURITY_CARD_DISUSE ABI drift");

// 字段化（SecuLink 批次 2026-09-05，裸偏移清理；union 叠加保留 m_pad 基名）。
// 证据：makeRequest ORIG 0x0842b57a（docs/class_func_reports/DB_SecurityCardIssue/
// makeRequest.md：`mov %edx,(%eax)` 4B、strncpy +0x31(6)/+0x04(0x21)/+0x25(0xc)/
// +0x37(0x27)/+0x63(0x29)）；dispatch ORIG 0x0842af94（同目录 dispatch.md 汇编
// SQL 列对应实证：phone='%s' ← +0x63、cert_key ← +0x31、server_key ← +0x04、
// card ← +0x37、q_pos strncpy ← +0x5e(5)、m_id ← +0x00）；_sendResult 回写
// +0x8c ENUM_ERROR（OrigDBInter19.cpp SecuIssueView 同布局实证）。sizeof 0x90
// 不变；仅 +0x00/+0x8c 为 4B 成员且偏移自然对齐，无需 pack。
struct SIG_SECURITY_CARD_ISSUE
{
    union
    {
        char m_pad[0x90];
        struct
        {
            unsigned int m_mId;         // +0x00（member_security_card.m_id ← accId）
            char m_serverKey[0x21];     // +0x04（SQL server_key）
            char m_serial[0xc];         // +0x25（makeRequest 侧为 phone 入参；_sendResult 回传）
            char m_certKey[6];          // +0x31（SQL cert_key）
            char m_card[0x27];          // +0x37（SQL card）
            char m_qPos[5];             // +0x5e（问题位置；dispatch strncpy 回写）
            char m_phone[0x29];         // +0x63（SQL phone）
            int m_error;                // +0x8c（_sendResult 回写 ENUM_ERROR，4B；
                                        //      本头不引 CUser.h，按 int 落宽）
        };
    };
};
static_assert(sizeof(SIG_SECURITY_CARD_ISSUE) == 0x90,
              "SIG_SECURITY_CARD_ISSUE ABI drift");

struct SIG_SECURITY_CARD_RETRANSFER
{
    char m_pad[0x8c];
};

// 字段化（W8DB，ORIG makeRequest 0x0843a34e：+0 uint accId、+4 uint type、
// +8 ushort flag、+0xa strncpy 0x10 uid；dispatch 0x0843a11e 双义读：
// +8 flag==8/0x10 分支、+4 既作 %d（时效秒差）又作 '%s'（validity_ip，
// ORIG 缓冲区复用，消费点以 (char*)&m_type 表达）：
struct SIG_SECURITY_CARD_SERVICE_VALIDITYTIME
{
    unsigned int m_mId;       // +0x00（m_id ← accId）
    unsigned int m_type;      // +0x04（服务类型/时效；dispatch 兼作 validity_ip 串首）
    unsigned short m_flag;    // +0x08（8=security_card / 0x10=goblin）
    char m_uid[0x12];         // +0x0a（strncpy 0x10）
};
static_assert(sizeof(SIG_SECURITY_CARD_SERVICE_VALIDITYTIME) == 0x1c,
              "SIG_SECURITY_CARD_SERVICE_VALIDITYTIME ABI drift");

// 字段化（W8DB，ORIG 0x0842bfaa makeRequest 写 +0 accId / +4 failCnt；
// DB_AccountCargoSync.cpp 0x08423eb4/0x0842403a 同型写 +0 m_id / +4
// fail|cancelCnt / +8 strncpy(webAddr,0x10)；消费视图
// OrigDBInter20.cpp SecurityCardFail/CancelCntView 同布局）：
struct SIG_SECURITY_CARD_UPDATE_FAIL_CNT
{
    unsigned int m_mId;      // +0x00（member_security_card.m_id ← accId）
    unsigned int m_cnt;      // +0x04（fail_cnt / cancel_cnt，opcode 0xa4/0xa5 区分）
    char m_webAddr[0x10];    // +0x08（ip_info / web 地址，strncpy 0x10）
};
static_assert(sizeof(SIG_SECURITY_CARD_UPDATE_FAIL_CNT) == 0x18,
              "SIG_SECURITY_CARD_UPDATE_FAIL_CNT ABI drift");

// 字段化（W8DB，ORIG 0x0842bce8 makeRequest：+0 uint accId、+4 uint flag；
// dispatch 区域 NumberToString(m_id)/+%d flag，同
// OrigDBInter20.cpp SecurityCardValidityTimeView）：
struct SIG_SECURITY_CARD_UPDATE_VALIDITYTIME
{
    unsigned int m_mId;      // +0x00（member_security_card.m_id ← accId）
    unsigned int m_flag;     // +0x04（validity_time 秒差）
};
static_assert(sizeof(SIG_SECURITY_CARD_UPDATE_VALIDITYTIME) == 0x8,
              "SIG_SECURITY_CARD_UPDATE_VALIDITYTIME ABI drift");

// 字段化（W8DB，ORIG makeRequest 0x08437844：+0 uint accId、+4 char flag；
// dispatch 区域 NumberToString(+0)=member_security_grade.m_id）：
struct SIG_SECU_SERVICE_REWARD
{
    unsigned int m_mId;      // +0x00（member_security_grade.m_id ← accId）
    char m_flag;             // +0x04
    char m_pad5[3];          // +0x05..0x07
};
static_assert(sizeof(SIG_SECU_SERVICE_REWARD) == 0x8,
              "SIG_SECU_SERVICE_REWARD ABI drift");

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
    bool m_empty1;            // +0x08（item1->isEmpty() 时置 1；GoalAuditDB7 bool 实证：
                              //   ORIG dispatch 0x841fe0c 以 movzbl+xor $1+test 比较）
    char m_pad09[3];          // +0x09
    int m_itemIdx2;           // +0x0c（item2->get_add_info()）
    int m_slot2;              // +0x10
    bool m_empty2;            // +0x14（item2->isEmpty() 时置 1；同上 bool 实证）
    char m_pad15[3];          // +0x15；ORIG 实例化 sizeof=0x18（GetInBuffer<SIG_SWAP_CREATURE_ITEM>）
};
static_assert(sizeof(SIG_SWAP_CREATURE_ITEM) == 0x18,
              "SIG_SWAP_CREATURE_ITEM ABI drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_itemIdx1) == 0x00,
              "SIG_SWAP_CREATURE_ITEM m_itemIdx1 offset drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_slot1) == 0x04,
              "SIG_SWAP_CREATURE_ITEM m_slot1 offset drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_empty1) == 0x08,
              "SIG_SWAP_CREATURE_ITEM m_empty1 offset drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_itemIdx2) == 0x0c,
              "SIG_SWAP_CREATURE_ITEM m_itemIdx2 offset drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_slot2) == 0x10,
              "SIG_SWAP_CREATURE_ITEM m_slot2 offset drift");
static_assert(offsetof(SIG_SWAP_CREATURE_ITEM, m_empty2) == 0x14,
              "SIG_SWAP_CREATURE_ITEM m_empty2 offset drift");
struct SIG_TOD_RELOADED_USER_APC
{
    char m_pad[0x758];
};

// 具名化（HdrFieldC）：sizeof 0x30 不变。写侧 DB_UpdateAutoMarketConditionsControl::
//   makeRequest ORIG T 0x08433d70（LinkMiscSecuTimerSolo.cpp 转写 + 报告）：
//   +0x00/+0x08/+0x10/+0x18 = 4×unsigned long long，+0x20/+0x24/+0x28 = 3×int；
//   +0x2c 驻留（DB 侧按此布局读，尾 4 字节未用）。
struct SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL
{
    unsigned long long m_values0[4];  // +0x00..0x1f（a/b/c/d）
    int m_values20[3];                // +0x20..0x2b（e/f/g）
    char m_pad2c[4];
};
static_assert(sizeof(SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL) == 0x30,
              "SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL ABI drift");

struct SIG_UPDATE_AVATAR_JEWEL_SOCKET
{
    unsigned int m_characNo;          // +0x00（DB_UpdateAvatarJewelSlot::makeRequest param_1）
    unsigned int m_avatarKey;         // +0x04（param_2，CAvatarItemMgr avatar key）
    unsigned char m_emblem[0x1e];     // +0x08（stAvatarEmblemInfo_t 位块 memcpy 0x1e）
    char m_pad26[0x2];                // +0x26 对齐尾：ORIG Stream::GetInBuffer<30SIG_…>W
};                                  //   @0x0845695e enable_in/in_ptr $0x28 实证 sizeof=0x28

struct SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD
{
    char m_pad[0x10];
};

struct SIG_UPDATE_CHARACTER_DAY_EVENT
{
    char m_pad[0x8];
};

// 具名化（HdrFieldC，尾字段）：sizeof 0x14d8 不变。
// 写侧 CUser::UpdateCharacView @0x08651740（CUser.cpp 现状转写 + CUser/UpdateData.md）：
//   memset 全块后 *(sig+0)=get_acc_id()，+0x14d4=GetServerGroup()（1 字节）；
//   +0x00 起为 SigUpdateCharacViewRecord[36]（stride 0x94 = 4 头 + CHARAC_LOGIN_DATA
//   0x90），rec[0] 头=acc_id、rec[i] 头←前一角色 expireSec。因 CHARAC_LOGIN_DATA
//   在 CUser.h（include SigTypes.h，双向环），记录区保留 blob 不内嵌完整类型。
struct SIG_UPDATE_CHARAC_VIEW
{
    char m_records[0x14d0];       // +0x00 SigUpdateCharacViewRecord[36]（stride 0x94；CHARAC_LOGIN_DATA
                                  //   在 CUser.h 与 SigTypes.h 双向环，记录区保留 blob）
    char m_pad[4];                // +0x14d0 驻留（ORIG GetInBuffer<SIG_UPDATE_CHARAC_VIEW>
                                  //   @SigTypes.cpp 模板实例 sizeof 0x14d8 实证）
    unsigned char m_serverGroup;  // +0x14d4 GetServerGroup()
    char m_pad2[3];
};
static_assert(sizeof(SIG_UPDATE_CHARAC_VIEW) == 0x14d8,
              "SIG_UPDATE_CHARAC_VIEW ABI drift");
static_assert(offsetof(SIG_UPDATE_CHARAC_VIEW, m_serverGroup) == 0x14d4,
              "SIG_UPDATE_CHARAC_VIEW serverGroup offset drift");

// 字段化（LinkClosureB-2，DB_UpdateCompoundEmblemStatics::makeRequest 0x08439dec
// 证据，sizeof 不变）：
struct SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC
{
    int m_userIdx;                 // +0x00
    unsigned int m_values[7];      // +0x04..+0x1c（param_2..param_8）
};

struct SIG_UPDATE_DIMENSION
{
    char m_pad[0x10];
};

struct SIG_UPDATE_LOGIN
{
    char m_pad[0x20];
};

#pragma pack(push, 1)
struct SIG_UPDATE_LOGOUT
{
    // 头部 0x22 字段化：CUser::UpdateLogout @0x086554aa（ORIG store 宽度逐条
    // 实证，报告 docs/class_func_reports/CUser/UpdateLogout.md）；m_pad 保留兼容。
    union
    {
        char m_pad[0x22];
        struct
        {
            int            m_accId;             // +0x00（movl ←this+0x704ac）
            unsigned int   m_characNo;          // +0x04（movl；getCurCharacNo）
            unsigned int   m_gcNo;              // +0x08（movl；get_gc_no[_hardcode]）
            unsigned short m_fatigue;           // +0x0c（mov %dx；GetFatigue）
            char           m_pad0e[4];          // +0x0e..0x11（无写点）
            unsigned char  m_stateFlag;         // +0x12（movb $0/$1；get_state()>1）
            int            m_playTime;          // +0x13（movl；now-m_loginTime，this+0x71ba4）
            unsigned int   m_field17;           // +0x17（movl ←this+0x8d10c）
            unsigned int   m_playExpAdd;        // +0x1b（movl ←this+0x703a4=gameOption.m_playExpAdd）
            unsigned short m_totalFatigueUsed;  // +0x1f（movzwl ←this+0x79642）
            unsigned char  m_puUser;            // +0x21（movzbl ←this+0x8d0df 直存 movb）
        };
    };
    unsigned char m_pcbangFlag;  // +0x22 pcbang/会员管理工具标志（SetUserManagerToolIP 置位）
    unsigned int m_ipDword;      // +0x23 ip 相关 dword（SetUserManagerToolIP 写）
    char m_ipBuf[0x10];          // +0x27 peer IP 串（GetPeerIP2 实参）
    char m_ipPrefix[0xc];        // +0x37 IP 前段（'.' 拆分；≤0xc 字节实证）
    char m_ipSuffix[4];          // +0x43 IP 后段（≤4 字节实证）
    // 尾区字段化：UpdateLogout 0x08655735 `lea 0x47(%eax)` strncpy mac 0x41；
    // 0x88 `mov %al`（IsEnableSaveMemberBonusFatigue 返字节直存）；
    // 0x89 `mov %ax`（getAccountMemberBonusFatigue 2B）。
    union
    {
        char m_pad47[0x8b - 0x47];
        struct
        {
            char            m_mac[0x41];              // +0x47..0x87（mac 串）
            unsigned char   m_saveMemberBonusFlag;    // +0x88
            unsigned short  m_memberBonusFatigue;     // +0x89
        };
    };
};
#pragma pack(pop)

// 字段化（CUserMain 2026-09-04）：CUser::UpdateData @0x08652xxx quest 脏块
// （报告 docs/class_func_reports/CUser/UpdateData.md）：+0 movl characNo、
// +0x04/+0x54 两组 0x14 dword（m_quest/m_trigger）、+0xa4 memcpy 30000 完成位图、
// +0x75d4 movb $1、+0x75d5 movl urgentQuest、+0x75d9 memcpy authen 0x28、
// memset 全块 0x7601。非对齐 dword ⇒ pack(1)。
#pragma pack(push, 1)
struct SIG_UPDATE_QUEST
{
    unsigned int   m_characNo;       // +0x0000
    unsigned int   m_quest[0x14];    // +0x0004
    unsigned int   m_trigger[0x14];  // +0x0054
    char           m_clearedQuest[30000]; // +0x00a4..0x75d3
    unsigned char  m_clearedFlag;    // +0x75d4
    unsigned int   m_urgentQuest;    // +0x75d5
    char           m_authen[0x28];   // +0x75d9..0x7600
};
static_assert(sizeof(SIG_UPDATE_QUEST) == 0x7601, "SIG_UPDATE_QUEST ABI drift");
static_assert(offsetof(SIG_UPDATE_QUEST, m_urgentQuest) == 0x75d5, "quest urgent drift");
static_assert(offsetof(SIG_UPDATE_QUEST, m_clearedFlag) == 0x75d4, "quest flag drift");
#pragma pack(pop)

// 字段化（W8DB，ORIG dispatch 0x0842f05e 实证：+0 m_id（NumberToString）
// / +4 point（%d 增量），见 W8LT_DBTask.cpp 注释；makeRequest 写侧同款）：
struct SIG_UPDATE_SCHOOL_POINT
{
    unsigned int m_mId;      // +0x00（event_school_point.m_id）
    int m_point;             // +0x04（point 增量）
};
static_assert(sizeof(SIG_UPDATE_SCHOOL_POINT) == 0x8,
              "SIG_UPDATE_SCHOOL_POINT ABI drift");

// 真实布局（GameWorld::UpdateStatistic4LEV ORIG memset 0x124；原 0x100 pad 已替换）
struct SIG_UPDATE_STATISTIC4LEV
{
    int m_gcNo;              // +0x00
    int m_count[71];         // +0x04..+0x11f（UpdateStatistic4ChannelUserLev memset 0x124、按 level 直接下标）
    int m_time;              // +0x120（getCurSec）
};

// 字段化（LinkClosureB-2，DB_SaveUserHandicap::makeRequest 0x0842801a 证据，
// sizeof 不变）：
struct SIG_UPDATE_USER_HANDICAP
{
    int m_param2;                  // +0x00
    unsigned int m_accId;          // +0x04
    int m_params[4];               // +0x08..+0x14（param_3..param_6）
};
struct SIG_UPGRADE_ACCOUNT_CARGO_RESULT
{
    // ORIG DB_UpgradeAccountCargo::dispatch 0x0843af76：GetInBuffer 后 memset 0x8；
    // affected==0 → +0=0x15 发包；否则 +0=0、+4=capacity 发包（MsgQueueMgr::put Q1）。
    char m_flag;                 // +0x00
    char m_pad[3];               // +0x01
    unsigned int m_capacity;     // +0x04
};

struct SIG_USER_PLAY_TIME_STATISTIC
{
    // ORIG CUserPlayTimeStatisticMgr::InsertUserPlayTime 0x0860cd14：
    // +0 计数；+4 起每条 12B {accId, playSec, count}，批量上限 20（0x13）。
    struct Entry
    {
        unsigned int m_accId;     // +0x00
        unsigned int m_playSec;   // +0x04
        unsigned int m_count;     // +0x08
    };                            // 0x0c
    int m_count;                  // +0x00
    Entry m_entries[20];          // +0x04（0x14 条 × 0xc = 0xf0）
};                                // 0xf4

// Wave5 AltarProc 批次字段化（sizeof/偏移不变；证据 docs/class_func_reports/
// CAdvanceAltarLog/incrementEntranceData.md + updateDatabase.md trace 字段序）
struct STAdvanceAltarEntranceData
{
    unsigned int m_stageIndex;     // +0x00
    unsigned int m_enterCount;     // +0x04
    unsigned int m_successCount;   // +0x08
    unsigned int m_obtainStar;     // +0x0c

    // ORIG W C1/C2 @0x081573be：4×movl $0（+0/+4/+8/+0xc 全字段清零）。
    STAdvanceAltarEntranceData()
        : m_stageIndex(0), m_enterCount(0), m_successCount(0), m_obtainStar(0)
    {
    }
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

    // ORIG W C1/C2 @0x081573ea：movw 0@+0 / movl 0@+4 / movw 0@+8 / movw 0@+0xa /
    // movw 0@+0xc（+0xe pad 不写，14 字节清零）。
    STAdvanceAltarLayerStatData()
        : m_itemType(0), m_field2(0), m_itemIndex(0), m_itemLevel(0),
          m_useCount(0), m_useStar(0)
    {
    }
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



// ---- Additional types for GetOutBuffer instantiations ----
struct MSG_STAT_BREAKJAR { char m_pad[0xc]; };
struct MSG_STAT_COMPOSITON { char m_pad[0xd]; };
struct MSG_STAT_DUNGEONCLEAR { char m_pad[0x19]; };
struct MSG_STAT_PVPPLAY { char m_pad[0x10]; };
#include "MSG_STAT_RANKRES.h"   // 权威 MSG_STAT_RANKRES（0x6c，替换占位）
// ORIG _ZN33Packet_Mannerless_User_AccusationC1Ev W @0x0822c1e6（0xaf）：
// PacketHeader(0x1b66,0x15f)；+0xa=-1；+0xe=0；+0x12=4；+0x1e=0；memset(+0x22,0,0x1e)；memset(+0x5e,0,0x100)
// isNeedTypeForChattingMsg ORIG 0x0822c1ca：return (unsigned)arg <= 2
struct __attribute__((packed)) Packet_Mannerless_User_Accusation
{
    char m_pad0[6];                    // +0x00 PacketHeader(type=0x1b66, size=0x15f) 区
    Packet_Mannerless_User_Accusation();   // ORIG W C1 0x0822c1e6（定义在 Dispatchers.cpp D1 依赖区）
    unsigned int m_uid;                // +0x06
    int m_characNo;                    // +0x0a（ctor 置 -1）
    int m_type;                        // +0x0e（ctor 置 0）
    int m_serverGroup;                 // +0x12（ctor 置 4）
    int m_reporterGroup;               // +0x16
    unsigned int m_accIdLo;            // +0x1a
    unsigned int m_accIdHi;            // +0x1e（ctor 置 0）
    char m_name[30];                   // +0x22（memset 0）
    char m_targetName[30];             // +0x40（ORIG 无 +0x5d 单字节字段）
    char m_chatMsg[0x100];             // +0x5e（memset 0；get_str 读举报文本）
    char m_result;                     // +0x15e（ORIG 尾字节；使 sizeof=0x15f，匹配 GetOutBuffer 尺寸）

    static bool isNeedTypeForChattingMsg(int type);   // ORIG W 0x0822c1ca：调用点只 push 1 参（静态）

};
struct SIG_AVATAR_MOVE { char m_pad[0x16]; };
// SIG_BILLING_RECORD —— 0x10e 字节计费记录（DB_BillingRecord 消费）。
// 字段化证据（写侧 ItemVendingMachine::BillingRecord ORIG 0x0817991e 逐条栈槽，
// docs/class_func_reports/ItemVendingMachine/BillingRecord.md；读侧
// DB_BillingRecord::InsertBuyUser 0x08424602 / InsertSaleEntry 0x084247e0，
// W8LT_DBTask.cpp DB_BillingRecordSigView 同布局）：全 int 字段 movl 实证、
// m_userId[0x19]/m_birthday[0xb]/m_resultMessage[0x64]/m_userIp[0x10] strcpy/
// sprintf/GetPeerIP2 实证、m_buyType 单字节写（ORIG movb %al,-0x2e(%ebp)）。
#pragma pack(push, 1)
struct SIG_BILLING_RECORD
{
    char m_agencyNo[0x15];      // +0x00（写 strcpy(rec,itemName)，读 '%s'=(char*)sig：agency_no 列）
    int m_sealFlag;             // +0x15
    int m_saleTime;             // +0x19
    int m_price;                // +0x1d
    int m_slotPartType;         // +0x21
    int m_slotPartCode;         // +0x25
    int m_ipgNo;                // +0x29
    unsigned int m_mId;         // +0x2d（get_acc_id）
    int m_characNo;             // +0x31
    char m_userId[0x19];        // +0x35（get_acc_name strcpy）
    int m_job;                  // +0x4e
    int m_sex;                  // +0x52（世纪类型 1/2）
    char m_birthday[0xb];       // +0x56（sprintf "19yy-mm-dd"）
    int m_prizeFlag;            // +0x61
    int m_resultCode;           // +0x65
    char m_resultMessage[0x64]; // +0x69（accName strcpy）
    int m_serverId;             // +0xcd
    int m_growType;             // +0xd1
    int m_level;                // +0xd5
    int m_giftServerInfo;       // +0xd9（= m_serverId 复制）
    int m_giftGetCharacNo;      // +0xdd
    int m_chargePrice;          // +0xe1（= m_saleTime 复制）
    int m_freePrice;            // +0xe5
    int m_countNo;              // +0xe9
    int m_eachPrice;            // +0xed（= m_saleTime 复制）
    int m_priceType;            // +0xf1（mileage=2/cera=3/其他=1）
    char m_userIp[0x10];        // +0xf5（GetPeerIP2；失败 "NONE"）
    int m_tranNoLow;            // +0x105（uid 低 32 位）
    int m_tranNoHigh;           // +0x109（uid 高 32 位）
    char m_buyType;             // +0x10d（payFlag）
};
#pragma pack(pop)
static_assert(sizeof(SIG_BILLING_RECORD) == 0x10e, "SIG_BILLING_RECORD ABI drift");
// SIG_BOOSTER_GAGE_DATA（ORIG W ctor 0x0848fb4c：2×movl 清零；sizeof 0x8）。
// 与 CBoosterGage.cpp 本地定义同名同布局（跨 TU 视图一致）。
struct SIG_BOOSTER_GAGE_DATA
{
    unsigned int m_dwAccountNo;  // +0x00
    int m_dwBoosterGage;         // +0x04
    SIG_BOOSTER_GAGE_DATA() : m_dwAccountNo(0), m_dwBoosterGage(0) {}
};
// SIG_CERASHOP_ERROR_LOG（ORIG W ctor 0x08328110：4×movl + movw(+0x10) + 2×movl；
// sizeof 0x1c；CCeraShop::LogError 消费 +0x00/+0x04/+0x10/+0x14/+0x18）。
struct SIG_CERASHOP_ERROR_LOG
{
    int m_field0;             // +0x00
    int m_field4;             // +0x04
    int m_field8;             // +0x08
    int m_fieldc;             // +0x0c
    unsigned short m_field10; // +0x10
    int m_field14;            // +0x14
    int m_field18;            // +0x18
    SIG_CERASHOP_ERROR_LOG()
        : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0),
          m_field10(0), m_field14(0), m_field18(0) {}
};
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

    // ORIG W 0x0848f8d4：MissionInfo[0x24] 隐式构造（0x24×8=0x120）后 3×movw 清零。
    SIG_REQUEST_PVP_MISSION_LIST()
    {
        m_field144 = 0;
        m_field146 = 0;
        m_field148 = 0;
    }
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
// 字段化（W8DB，ORIG 0x0842945e DB_InsertValueStatistic::dispatch 实证：
// +0x00..+0x68 共 28 个 uint，log_value_stat 列序 occ_time(from_unixtime
// [0])/level[1]/uv[2]/drop_gold[3]/drop_item[4]/result_card_gold[5]/
// result_card_item[6]/gold_card_item[7]/store_item_buy[8]/jar_item[9]/
// disjoint_create[10]/upgrade_faild_forced_disjoint[11]/quest_reward[12]/
// deathtower_card_gold[13]/deathtower_card_item[14]/consume_store_item_buy
// [15]/consume_upgrade_attempt[16]/consume_upgrade_faild[17]/
// consume_stamina_recovery[18]/consume_quest_consume[19]/
// consume_auction_commision[20]/consume_item_disjoint[21]/
// consume_item_repair[22]/consume_item_use[23]/consume_item_drop[24]/
// consume_gold_drop[25]/consume_gold_card_price[26]... 末列 [27]）：
struct SIG_VALUE_STATISTIC
{
    unsigned int m_stats[28];  // +0x00..+0x6c（log_value_stat 统计列，序见上）
    char m_pad70[0x10];        // +0x70..0x7f
};
static_assert(sizeof(SIG_VALUE_STATISTIC) == 0x80,
              "SIG_VALUE_STATISTIC ABI drift");
// stAchievement_DBSaveData 权威定义见 CAchievement.h（此处不重复）
struct stTitleBook_Save { char m_pad[0x6b3e]; };

#endif  // DNF_GAME_SIG_TYPES_E_H_
