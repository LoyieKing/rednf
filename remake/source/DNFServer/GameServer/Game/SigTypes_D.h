#ifndef DNF_GAME_SIG_TYPES_D_H_
#define DNF_GAME_SIG_TYPES_D_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 D（SIG_LOGIN_DATA 登录域 + MAILBOX..SAVE_EVENTITEM）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================

#include "SigTypes_C.h"   // CHARAC_LOAD_MERCENARY / user_gold_info / 登录域转发视图
#include "CHARAC_QUEST_SHOP_DATA.h"  // CHARAC_QUEST_SHOP_DATA（SIG_QUEST_SHOP_DATA 双实例元素）
#include "TODLayer.h"    // TOD_Layer（SIG_REQUEST_TOD_USER_STATE m_todayLayer 成员构造）
#include "Redeem_Item__CRedeemItem.h"  // Redeem_Item::Inven_Item_Expand_RedeemInfo（0x44）

#pragma pack(push, 1)
// ============================================================================
// SIG_LOGIN_DATA —— 登录数据包权威布局（Inter_LoginInfo::dispatch_sig.md
// @0x084c5294 全量反汇编逐字段具名化；另含 ARAD_LoginSecurity/PVP_CHANNEL/
// punish_check_by_HUB 等复核槽位）。未访问区域保留显式 pad，总 sizeof 0x4358。
// 动态表（premium/restrictedGoods/punishInfo）按已知相邻槽位留出安全容量，
// 剩余区间仍为 pad；rebuild 侧禁止再对该结构做裸偏移强转访问。
// ============================================================================
struct SIG_LOGIN_PREMIUM_ENTRY     // stride 0x10（dispatch_sig 预remium循环实证）
{
    unsigned int m_type;           // +0x00 → handleSetUserPremium 第2参(char cast)
    unsigned int m_startTime;      // +0x04 → start
    unsigned int m_endTime;        // +0x08 → end
    int            m_flag;         // +0x0c → flag
};

struct SIG_LOGIN_PUNISH_INFO       // stride 0x18（@0x3a54，ARAD 复核同型）
{
    int m_type;                    // +0x00 ∈(99,200)：0x65→stdDropRate、4/0xb/0xc/0xd/0xe/0x67→TradePunish
    int m_field04;                 // +0x04（本函数未访问）
    int m_stdDropRate;             // +0x08 type==0x65 时 setStdDropRate
    int m_field0c;                 // +0x0c（未访问）
    int m_field10;                 // +0x10（未访问）
    int m_endTime;                 // +0x14 与 getCurSec 比较的有效期
};

struct SIG_LOGIN_DATA
{
    // +0x0000..0x00a0 头部区（DisPatcher_Login::dispatch_sig 实证拆具名）：
    char           m_userId[0x18];          // +0x0000..0x0017 登录账号串（strncpy 0x18）
    char           m_pad18[0x84];           // +0x0018..0x009b memset 清零区
    int            m_gcNo;                  // +0x009c 服务端 gc 号（login 复核写入）
    char           m_loginType;             // +0x00a0 登录类型 byte（0/1/2）
    char           m_blackIpCheckBuf[0x14]; // +0x00a1 黑名单 IP 串缓冲（isBlackIP 实参）
    unsigned char  m_geoAllowSkipFlag;      // +0x00b5 ==1 时跳过 RestrictGeolocation 检测
    char           m_pad0b6[2];             // +0x00b6..0x00b7
    int            m_errCode;               // +0x00b8 登录错误码槽（PUNISH_CHECK 复核结论）
    char           m_pad0bc[4];             // +0x00bc
    unsigned int   m_accId;                 // +0x00c0 member m_id（NumberToString/get_acc_info 源）
    char           m_pad0c4[4];             // +0x00c4..0x00c7
    int            m_birthYear;             // +0x00c8 SetAge 计算基准（DB 出生年）
    unsigned int   m_regDate;               // +0x00cc setRegDate
    char           m_pad0d0[4];             // +0x00d0
    unsigned int   m_regTime;               // +0x00d4 setRegTime（32 位 long）
    int            m_loginErrDetail;        // +0x00d8 err_code∈[9,0xc] 时 put_byte 输出
    int            m_characCount;           // +0x00dc SetCharacInfo 计数
    char           m_characData[0x14d0];    // +0x00e0 _Charac_info 区域视图（SetCharacInfo）
    int            m_mercenaryCount;        // +0x15b0 SetCharacMercenaryInfo 计数（上限 0x24）
    CHARAC_LOAD_MERCENARY m_mercenaries[34];// +0x15b4 元素 stride 0x54（hack 检查读 m_field28）
    char           m_pad20dc[0x2184 - 0x20dc]; // +0x20dc mercenary 表（34×0x54）后未访问区
    char           m_userDetailIpPort[0x80];// +0x2184 SetUserDetailInfo 参数2
    char           m_userDetailSsn[0x80];   // +0x2204 SetUserDetailInfo 参数3
    char           m_birth[0x10];           // +0x2284 SetBirth（出生日期串）
    int            m_premiumCount;          // +0x2294 premium 表项数
    SIG_LOGIN_PREMIUM_ENTRY m_premiums[100];// +0x2298（容量上限内安全）
    char           m_pad28d8[0x293a - 0x28d8]; // +0x28d8
    unsigned short m_restrictedGoodsCount;  // +0x293a setRestrictedGoods 循环计数
    int            m_restrictedGoods[512];  // +0x293c 限制物品 idx 表
    char           m_pad313c[0x38dc - 0x313c]; // +0x313c（DisPatcher_Login 写 +0x3905 落于
                                               //   后随 m_passPadPassword 内 +0x24 区，见下）
    bool           m_puUser;                // +0x38dc SetPuUser
    bool           m_hangameUser;           // +0x38dd setHangameUser
    short          m_punishTradeAlertType;  // +0x38de 0xd→0xe7 / 0xe→0xe8 / 其它→0
    bool           m_passPadEnable;         // +0x38e0 GoblinPad 启用标志
    char           m_passPadPassword[0x2b]; // +0x38e1 CPad::setPassword 缓冲
    unsigned char  m_passPadVersion;        // +0x390c CPad::setVersionInfo
    bool           m_passPadAdvideChangePwd;// +0x390d setAdvideChangePwd
    bool           m_passPadCertified;      // +0x390e setCertified 数据源
    char           m_pad390f[1];            // +0x390f
    unsigned int   m_passPadCheckTime;      // +0x3910 setCheckTime（32 位 long）
    int            m_passPadFailCnt;        // +0x3914 setFailCnt / E_AUTH_MOUSE put_byte
    int            m_passPadCancelCnt;      // +0x3918 setCancelCnt
    char           m_pad391c[0x3950 - 0x391c]; // +0x391c
    int            m_accountFail;           // +0x3950 ARAD 登录复核写入（account_password_check）
    int            m_totalAccountFail;      // +0x3954 同上
    unsigned int   m_lastPlayTime;          // +0x3958 上次游戏秒（ReturnUser/每日窗口基准）
    unsigned int   m_dungeonGainedGold;     // +0x395c CDungeonGainedGold::setGainedGold
    int            m_reportMannerlessUserCount; // +0x3960 当日无礼举报数
    unsigned char  m_reliablePersonFlag;    // +0x3964 SetReliablePerson(!=0)
    char           m_pad3965[3];            // +0x3965
    unsigned int   m_overTradeGoldCount;    // +0x3968 当日超限交易金币
    unsigned int   m_schoolNo;              // +0x396c SetSchoolNo
    float          m_rating;                // +0x3970 SetRating
    char           m_schoolName[0x28];      // +0x3974 SetSchoolName
    short          m_dailyBadge[3];         // +0x399c SetDailyBadge(short*)
    unsigned char  m_tutorialSkipable;      // +0x39a2 SetTutorialSkipable
    char           m_pad39a3[5];            // +0x39a3
    unsigned int   m_tournamentServerGroup; // +0x39a8 渠道==7 时 SetServerGroup 数据源（ Tournament 账户判定之一）
    unsigned int   m_tournamentAccountFlag; // +0x39ac setTournamentAccount(m_39ac!=0 && m_39a8!=0)
    char           m_pad39b0[0x39d0 - 0x39b0]; // +0x39b0
    unsigned int   m_onlinePrelimServerGroup;// +0x39d0 渠道==0xe 时 SetServerGroup / setOnlinePreliminaryServerGroup
    unsigned int   m_onlinePrelimCharacNo;  // +0x39d4 setOnlinePreliminaryCharacNo
    int            m_onlinePrelimPlayType;  // +0x39d8 setOnlinePreliminaryPlayType
    char           m_pad39dc[4];            // +0x39dc
    bool           m_secCardActivate;       // +0x39e0 setSecurityCardInfo activate
    bool           m_secCardCertified;      // +0x39e1 certified
    char           m_pad39e2[2];            // +0x39e2
    int            m_secCardCertCnt;        // +0x39e4 certCnt
    char           m_pad39e8[4];            // +0x39e8
    char           m_secCardCertFlag;       // +0x39ec certFlag（char）
    char           m_secCardSerial[0x27];   // +0x39ed serial 串槽（至 +0x3a13）
    int            m_secCardCancelCnt;      // +0x3a14 setSecurityCardInfo cancelCnt
    char           m_secCardAnswers[8];     // +0x3a18 answers 串槽
    unsigned char  m_hubPunishFlag;         // +0x3a20 punish_check_by_HUB 置位
    char           m_pad3a21[3];            // +0x3a21
    unsigned int   m_totalHackUserTradeCnt; // +0x3a24 setTotalHackUserTradeCnt
    unsigned int   m_totalHackUserTradeGold;      // +0x3a28 setTotalHackUserTradeGold（值域）
    unsigned int   m_totalHackUserTradeGoldHi;    // +0x3a2c 高位/未访问（ORIG 按 long 槽占位）
    unsigned int   m_timeStamp;             // +0x3a30 登录时间戳（getTimeStamp 写入）
    unsigned int   m_clientVersion;         // +0x3a34 setClientVersion
    short          m_channelRestrictField;  // +0x3a38 PVP_CHANNEL 非 all-pvp 渠道清零槽
    char           m_webAddress[0x10];      // +0x3a3a setWebAddress（0 结尾串）
    bool           m_joinedSchoolPointEventFlag; // +0x3a4a setJoinedSchoolPointEvent
    char           m_pad3a4b[1];            // +0x3a4b
    int            m_cleanPadDecreaseCnt;   // +0x3a4c clean pad 点数递减基准
    int            m_punishInfoCount;       // +0x3a50 punish 信息条数（err_code==4 亦作索引）
    SIG_LOGIN_PUNISH_INFO m_punishInfos[32];// +0x3a54（stride 0x18；容量 ≤ 相邻已知槽）
    char           m_pad3d54[0x3f04 - 0x3d54]; // +0x3d54 punish 表后未访问区
    bool           m_returnUserDbFlag;      // +0x3f04 有 DB 归还用户记录（走 +0x3f08 过期时间）
    bool           m_returnUserFirstLogin;  // +0x3f05 SetReturnUserFirstUserLogin
    char           m_pad3f06[2];            // +0x3f06
    unsigned int   m_returnUserExpireTime;  // +0x3f08 SetReturnUserExpireTime
    bool           m_restingUserRestrict;   // +0x3f0c setRestingUserRestrict
    unsigned char  m_secuType;              // +0x3f0d setSecuType/setSecuGrade(&0x27)
    bool           m_secuReward;            // +0x3f0e setSecuReward
    unsigned char  m_rewardType;            // +0x3f0f setRewardType
    unsigned char  m_userEventCharacterFlag;// +0x3f10 setUserEventCharacterFlag
    char           m_pad3f11[3];            // +0x3f11
    int            m_mileage;               // +0x3f14 SetMileage
    int            m_breakAwayAccureCera;   // +0x3f18 >=0 时 setBreakAwayAccureCera
    unsigned char  m_breakAwayPlayInfo[0x20]; // +0x3f1c..0x3f3b churn_member_info.play_info
                                              // （有效区 0x1f，updateUvList UV 名单源）
    unsigned int   m_breakAwayLastRewardTime; // +0x3f3c churn_member_info.last_reward_time
    unsigned int   m_breakAwayRewardBoundA; // +0x3f40 A<B→order1 否则 order3
    unsigned int   m_breakAwayRewardBoundB; // +0x3f44
    int            m_breakAwayDungeonClearCnt; // +0x3f48 setBreakAwayDungeonClearCnt
    unsigned int   m_breakAwayQuestTime;    // +0x3f4c setBreakAwayQuestTime
    unsigned int   m_breakAwayLastUpdateTime; // +0x3f50 churn_member_info.last_update_time
    int            m_breakAwayLuckPoint;    // +0x3f54 setUserBreakAwayRewardLuckPoint
    bool           m_breakAwayUvListFlag;   // +0x3f58 updateUvList 触发/复写标志
    char           m_pad3f59[3];            // +0x3f59
    bool           m_characLinkMessageFlag; // +0x3f5c setCharacLinkMessageFlag
    char           m_pad3f5d[1];            // +0x3f5d
    short          m_memberBonusFatigueInc; // +0x3f5e incAccountMemberBonusFatigue 数据源
    unsigned int   m_memberBonusSaveTime;   // +0x3f60 CheckDailyScheduleTime 判定基准（32 位 long）
    user_gold_info m_goldInfo;              // +0x3f64 Secu_GoldControl::SetInfo（sizeof 0x3c）
    unsigned char  m_characSlotCurCount;    // +0x3fa0 setCharacSlotCountInfo 参数1
    unsigned char  m_characSlotMaxCount;    // +0x3fa1 参数2
    char           m_pad3fa2[2];            // +0x3fa2
    int            m_ontimeLastRecvIdx;     // +0x3fa4 set_ontime_last_recv_idx
    bool           m_checkPickUpRandomOptionItemFlag; // +0x3fa8 true→setCheckPickUpRandomOptionItem(true)
    unsigned char  m_fatigueKind;           // +0x3fa9 set_fatigue_count_db 参数1
    short          m_fatigueValue;          // +0x3faa 参数2
    SIG_INGAME_EVENT_HISTORY m_ingameEventHistory; // +0x3fac set_ingame_event_history 整包
    bool           m_ingameHackItemFlag;    // +0x4124 addServerHackCnt(0x261) 触发
    char           m_informNoticeBuf[0x13]; // +0x4125 loadInformNoticeFlag 缓冲
    char           m_bingoWinFlag;          // +0x4138 SetBingoData 读 byte!=0 → BingoData.m_isWin
    char           m_pad4139[3];            // +0x4139
    unsigned int   m_bingoEventId;          // +0x413c SetBingoData 整型读 → BingoData.m_eventId
    SIG_GROWTH_WEAPON_EVENT m_growthWeaponEvent;   // +0x4140 SetGrowthWeaponEventInfo
    SIG_GIVE_GROWTH_CREATURE_EVENT m_growthCreatureEvent; // +0x4154 SetGrowthCreatureEventInfo
    SIG_LEVELUP_GIFT_ACCOUNT_ONCE m_levelupGiftAccountOnce; // +0x4338 setCharacLevelUpGift_AccountOnce
};                                             // sizeof = 0x4358
#pragma pack(pop)

static_assert(sizeof(SIG_LOGIN_DATA) == 0x4358, "SIG_LOGIN_DATA ABI drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_errCode) == 0x0b8, "SIG_LOGIN_DATA errCode offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_accId) == 0x0c0, "SIG_LOGIN_DATA accId offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_regDate) == 0x0cc, "SIG_LOGIN_DATA regDate offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_characCount) == 0x0dc, "SIG_LOGIN_DATA characCount offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_mercenaryCount) == 0x15b0, "SIG_LOGIN_DATA mercenaryCount offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_premiums) == 0x2298, "SIG_LOGIN_DATA premiums offset drift");
static_assert(sizeof(SIG_LOGIN_PREMIUM_ENTRY) == 0x10, "SIG_LOGIN_PREMIUM_ENTRY ABI drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_restrictedGoodsCount) == 0x293a, "SIG_LOGIN_DATA restrictedGoodsCount offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_passPadEnable) == 0x38e0, "SIG_LOGIN_DATA passPadEnable offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_lastPlayTime) == 0x3958, "SIG_LOGIN_DATA lastPlayTime offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_secCardActivate) == 0x39e0, "SIG_LOGIN_DATA secCardActivate offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_secCardSerial) == 0x39ed, "SIG_LOGIN_DATA secCardSerial offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_hubPunishFlag) == 0x3a20, "SIG_LOGIN_DATA hubPunishFlag offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_webAddress) == 0x3a3a, "SIG_LOGIN_DATA webAddress offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_cleanPadDecreaseCnt) == 0x3a4c, "SIG_LOGIN_DATA cleanPadDecreaseCnt offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_punishInfos) == 0x3a54, "SIG_LOGIN_DATA punishInfos offset drift");
static_assert(sizeof(SIG_LOGIN_PUNISH_INFO) == 0x18, "SIG_LOGIN_PUNISH_INFO ABI drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_breakAwayPlayInfo) == 0x3f1c, "SIG_LOGIN_DATA breakAwayPlayInfo offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_breakAwayLastRewardTime) == 0x3f3c, "SIG_LOGIN_DATA breakAwayLastRewardTime offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_breakAwayLastUpdateTime) == 0x3f50, "SIG_LOGIN_DATA breakAwayLastUpdateTime offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_goldInfo) == 0x3f64, "SIG_LOGIN_DATA goldInfo offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_bingoWinFlag) == 0x4138, "SIG_LOGIN_DATA bingoWinFlag offset drift");
static_assert(offsetof(SIG_LOGIN_DATA, m_bingoEventId) == 0x413c, "SIG_LOGIN_DATA bingoEventId offset drift");

// 具名化（HdrFieldC）：sizeof 0x3fde 不变。三侧证据闭环：
// 写侧（game→DB）ReqDBCheckReceiver_forchina @0x08555486（WongWork__CMailBoxHelper
//   报告汇编）：strncpy(+0x00,param_2,0x1d) 收件人名；*(int*)(sig+0x2d)=
//   get_charac_no(-1)（汇编 mov %edx,0x2d(%eax)）；memcpy(sig+0x31,param_3,0x3f88)。
// DB 侧 DB_MailBox_Req_CheckReceiver::dispatch @0x0841c882（报告）：
//   SQL "seLect charac_no,lev,delete_flag,m_id from charac_info where
//   charac_name='%s'" 四列依次回填 +0x1d/+0x21/+0x25/+0x29；
//   memcpy(+0x31,+0x31,0x3f88) 载荷透传；结果标志 +0x3fb9 = 0 或 0x4d。
// 读侧（DB→game）Inter_MailBox_Req_CheckReceiver::dispatch_sig @0x084cf95a
//   （报告汇编字节偏移）：+0x1d 判 0（无收件人→err 3）、+0x25 判 0（无附件走
//   GM/单附件线）、+0x29 isGM/accId、+0x31 检查类型(1=cera/2=多附件)、
//   +0x35 MSG_BUY_CERASHOP_ITEM 视图、+0x3fb9==0x4d 报错、+0x63/+0x67 回写。
#pragma pack(push, 1)
struct SIG_MAILBOX_CHECK_RECEIVER
{
    char m_receiverName[0x1d];        // +0x00 收件人名（strncpy 0x1d）
    unsigned int m_receiverCharacNo;  // +0x1d DB 回填 charac_no
    unsigned int m_receiverLevel;     // +0x21 DB 回填 lev
    unsigned int m_receiverDeleteFlag;// +0x25 DB 回填 delete_flag（==0 正常用户）
    unsigned int m_receiverAccId;     // +0x29 DB 回填 m_id（accId）
    unsigned int m_senderCharacNo;    // +0x2d game 写 get_charac_no(-1)
    char m_payload[0x3f88];           // +0x31 MSG_CHECK_MAIL_RECEIVER 载荷透传
    unsigned int m_resultFlag;        // +0x3fb9 DB 结果（0 ok / 0x4d 名称非法）
    char m_pad[0x21];
};
#pragma pack(pop)
static_assert(sizeof(SIG_MAILBOX_CHECK_RECEIVER) == 0x3fde,
              "SIG_MAILBOX_CHECK_RECEIVER ABI drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_resultFlag) == 0x3fb9,
              "SIG_MAILBOX_CHECK_RECEIVER resultFlag offset drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_payload) == 0x31,
              "SIG_MAILBOX_CHECK_RECEIVER payload offset drift");

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

// SIG_MOUSE_REGISTER（0x59）—— ORIG 权威布局
// （Dispatcher_MouseRegister::dispatch_sig @0x082637fe / doPassPadReplay @0x082633c6 写入点）
#pragma pack(push, 1)
struct SIG_MOUSE_REGISTER
{
    unsigned int m_accId;              // +0x00 CUser::get_acc_id
    unsigned int m_pkgType;            // +0x04 包内字节扩展（movzbl 后写 4 字节）
    char m_password1[7];               // +0x08 解密明文密码 1（strncpy 7）
    char m_password2[7];               // +0x0f 解密明文密码 2（strncpy 7）
    char m_pad16[0xd];                 // +0x16..0x22 未建模填充（无读写证据）
    char m_padPassword[0x24];          // +0x23 Sanicova::CPad::getPassword 快照（memcpy 0x24）
    char m_field47;                    // +0x47 ORIG 无读写证据（0x23+0x24 与 0x48 间的保留字节）
    char m_webAddress[0x10];           // +0x48 CUser::getWebAddress（strncpy 0x10）
    char m_advideChangePwd;            // +0x58 getAdvideChangePwd 单字节
};
#pragma pack(pop)
static_assert(sizeof(SIG_MOUSE_REGISTER) == 0x59, "SIG_MOUSE_REGISTER ABI drift");

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

// ORIG 布局（DB_UpdateOneDayLetheSkill::dispatch @0x8434077-0x84340b4 实证）：
//   +0x00 m_flag(int, dispatch 写回处理结果)、+0x04 m_state、
//   +0x05 m_skills(0x198, memcpy 0x199 覆盖 skills+m_state2)、
//   +0x19d m_state2、+0x19e m_treeKind。总尺寸 0x1a0 不变。
struct SIG_PREMIUM_LETHE_FLAG
{
    int m_flag;               // +0x00
    unsigned char m_state;    // +0x04
    char m_skills[0x198];     // +0x05
    unsigned char m_state2;   // +0x19d
    unsigned char m_treeKind; // +0x19e
    char m_pad19f;            // +0x19f
};
static_assert(sizeof(SIG_PREMIUM_LETHE_FLAG) == 0x1a0,
              "SIG_PREMIUM_LETHE_FLAG ABI drift");

struct SIG_PURCHASE_CASHITEM_BONUS
{
    char m_pad[0x4];
};

struct SIG_QUERY_CHARAC_INFO
{
    char m_pad[0x30];
};
// ORIG 真实类型 = 双实例 + 尾随 flag（GetInBuffer<SIG_QUEST_SHOP_DATA> size
// 0x155，CQuestShop.cpp 注释实证）。ORIG ctor 0x0848f92e：
// CHARAC_QUEST_SHOP_DATA C1(this+0) → C1(this+0xaa) → clear(this+0) →
// clear(this+0xaa) → movb $0,this+0x154。
#pragma pack(push, 1)
struct SIG_QUEST_SHOP_DATA
{
    CHARAC_QUEST_SHOP_DATA m_main;    // +0x000（0xaa）
    CHARAC_QUEST_SHOP_DATA m_tag;     // +0x0aa（0xaa）
    unsigned char m_tagFlag;          // +0x154

    SIG_QUEST_SHOP_DATA()             // ORIG W 0x0848f92e（C1/C2 同址）
        : m_main(), m_tag()
    {
        m_main.clear();
        m_tag.clear();
        m_tagFlag = 0;
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_QUEST_SHOP_DATA) == 0x155,
              "SIG_QUEST_SHOP_DATA ABI drift");

// ORIG ctor 0x0848f894：+0x4 int=0 → Redeem_Item::Inven_Item_Expand_RedeemInfo
// C1 × 0x15（esi=0x14 倒计数，步长 0x44：0x15×0x44 = 0x594 → +0x4 = 0x598 → 尾
// 0x59c）。前 4 字节 ctor 不触碰。
#pragma pack(push, 1)
struct SIG_REDEEMINFO_LIST_TO_DB
{
    char m_pad0[4];                                // +0x000
    int  m_field4;                                 // +0x004（ctor 清零）
    Redeem_Item::Inven_Item_Expand_RedeemInfo m_items[0x15];  // +0x008（0x594）

    SIG_REDEEMINFO_LIST_TO_DB()                    // ORIG W 0x0848f894（C1/C2 同址）
    {
        m_field4 = 0;
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_REDEEMINFO_LIST_TO_DB) == 0x59c,
              "SIG_REDEEMINFO_LIST_TO_DB ABI drift");

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

// 具名化（W9_StructMisc 批次；ORIG TOD_UserState::loadData 0x086434fc /
// _saveData 0x08643642 写读序实证，sizeof 保持 0x18）。
struct SIG_REQUEST_TOD_USER_STATE
{
    int m_characNo;                 // +0x00（save=CUser::get_charac_no(-1)）
    int m_firstEnterTime;           // +0x04（loadData 第 2 dword / save ←this+0xc）
    int m_enterCount;               // +0x08
    TOD_Layer m_todayLayer;         // +0x0c（ORIG ctor 0x08491f68：TOD_Layer C1(0)）
    unsigned short m_lastEnterLayer;// +0x0e
    int m_lastEnterTime;            // +0x10
    int m_lastClearTime;            // +0x14

    SIG_REQUEST_TOD_USER_STATE()    // ORIG W 0x08491f68（C1/C2 同址）
        : m_todayLayer(0)
    {
    }
};
static_assert(sizeof(SIG_REQUEST_TOD_USER_STATE) == 0x18,
              "SIG_REQUEST_TOD_USER_STATE ABI drift");

struct SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
{
    char m_pad[0x8];
};

struct SIG_REQ_LOAD_GAME_OPTION
{
    unsigned int m_accId;   // +0x00（Inter_LoginInfo::dispatch_sig 写 get_acc_id；ORIG 全结构仅首 dword 使用）
};

struct SIG_RESTING_USER_RESTRICT
{
    char m_pad[0x4];
};

// ORIG ctor 0x08491eca：仅构造 +0x4 的 CONTINUOUS_ITEM_INFO（0x194；count@+0 +
// entries{int,uint}[0x32]，定义于 SigTypes_C.h）。
#pragma pack(push, 1)
struct SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO
{
    char m_pad0[4];                 // +0x000
    CONTINUOUS_ITEM_INFO m_info;    // +0x004（0x194）

    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO()  // ORIG W 0x08491eca（C1/C2 同址）
        : m_info()
    {
    }
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO) == 0x198,
              "SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO ABI drift");

struct SIG_SAVE_BLOOD_CLEAR_COUNT
{
    unsigned int m_dungeonIdx;   // +0x00（_saveProperDungeonClearCount ORIG 写序实证）
    short m_subNo;               // +0x04
    short m_pad6;
};

#pragma pack(push, 1)
// 字段化依据：DB_SaveCharac 报告（dispatch 0x8415c98 / _SaveCharacInfo 0x8415eda /
// _SaveCharacStat 0x84163f2 / _SaveCharacManageInfo 0x841690e /
// _SaveContractOfCubePremiumInfo 0x8415dbe）movzbl/movzwl/movsbl/cwtl/movl 偏移逐项转写，
// 语义名取 SQL 列名/CUser::_SaveCharacData 写入名。多处跨对齐边界访问 ⇒ pack(1)。
struct SIG_SAVE_CHARAC
{
    unsigned int m_characNo;                 // +0x00
    unsigned char m_bSaveInfo;               // +0x04
    unsigned char m_bSaveStat;               // +0x05
    unsigned char m_bSaveInfoSub;            // +0x06
    unsigned char m_bSaveManageInfo;         // +0x07
    short m_lev;                             // +0x08
    char m_growType;                         // +0x0a
    int m_maxHP;                             // +0x0b
    int m_maxMP;                             // +0x0f
    unsigned short m_phyAttack;              // +0x13
    unsigned short m_phyDefense;             // +0x15
    unsigned short m_magAttack;              // +0x17
    unsigned short m_magDefense;             // +0x19
    char m_specProperty[8];                  // +0x1b
    char m_elementResist[0x22];              // +0x23
    int m_invenWeight;                       // +0x45
    short m_hpRegen;                         // +0x49
    short m_mpRegen;                         // +0x4b
    int m_moveSpeed;                         // +0x4d
    unsigned short m_attackSpeed;            // +0x51
    unsigned short m_castSpeed;              // +0x53
    short m_hitRecovery;                     // +0x55
    short m_jump;                            // +0x57
    int m_characWeight;                      // +0x59
    char m_expertJob;                        // +0x5d
    int m_exp;                               // +0x5e
    char m_village;                          // +0x62
    short m_fatigue;                         // +0x63
    unsigned short m_premiumFatigue;         // +0x65
    unsigned char m_hp;                      // +0x67
    unsigned short m_usedFatigue;            // +0x68
    int m_tutorialFlag;                      // +0x6a
    char m_escaladeTutorialFlag[0x10];       // +0x6e
    int m_tradeGoldTotal;                    // +0x7e
    int m_tradeGoldTotalBillion;             // +0x82
    int m_tradeGoldDaily;                    // +0x86
    int m_helpAbuseRatio;                    // +0x8a
    int m_helpAbuseExp;                      // +0x8e
    int m_chaosPoint;                        // +0x92
    int m_chaosExp;                          // +0x96
    int m_chaosModeCount;                    // +0x9a
    int m_chaosKillCount;                    // +0x9e
    int m_chaosDieCount;                     // +0xa2
    int m_chaosDieTime;                      // +0xa6
    char m_openFlag;                         // +0xaa
    int m_assaultCount;                      // +0xab
    int m_chaosKillTime;                     // +0xaf
    int m_luckPoint;                         // +0xb3
    int m_dungeonPlayCount;                  // +0xb7
    int m_expertJobExp;                      // +0xbb
    short m_fatigueBatteryCharging;          // +0xbf
    short m_powerWarPoint;                   // +0xc1
    char m_villagePrev;                      // +0xc3
    int m_powerWarAssaultCount;              // +0xc4
    int m_powerWarAssaultVictoryCount;       // +0xc8
    unsigned short m_fatigueGrownupBuff;     // +0xcc
    unsigned char m_memberDungeonFlag;       // +0xce
    short m_memberBonusFatigue;              // +0xcf
    int m_lastPlayDungeonIndex;              // +0xd1
    unsigned char m_growthPowerReward;       // +0xd5
    int m_chaosResponTime;                   // +0xd6
    unsigned char m_guildRight;              // +0xda
    unsigned short m_maxPremiumFatigue;      // +0xdb
    char m_eventCharacLevel;                 // +0xdd
    int m_lastPlayTime;                      // +0xde（CUser::_SaveCharacData 实证 last_play_tick）
    int m_lastPlayTimePowerwar;              // +0xe2（getCurCharacLastPlayTickPowerWar）
    unsigned int m_ssoId;                    // +0xe6
    unsigned int m_reportCnt;                // +0xea
    unsigned int m_ssoTradeGoldDaily;        // +0xee
    unsigned int m_dungeonGainGold;          // +0xf2
    float m_rating;                          // +0xf6
    unsigned char m_tutorialSkipable;        // +0xfa
    short m_emotion;                         // +0xfb
    char m_skillTreeIndex;                   // +0xfd
    char m_addSlotFlag;                      // +0xfe
    char m_addEquipslotFlag;                 // +0xff
    char m_channelEquipslotSwitch;           // +0x100
    char m_expandEquipslotSwitch;            // +0x101
    char m_visibleFlags;                     // +0x102
    unsigned char m_strikerSkillIndex;       // +0x103
    int m_tagCharacNo;                       // +0x104
    unsigned short m_maxEquipLevel;          // +0x108
    bool m_bSaveCubePremiumInfo;             // +0x10a（_SaveContractOfCubePremiumInfo ORIG xor $1 比较）
    unsigned char m_cubeSelected;            // +0x10b
    unsigned char m_cubeType;                // +0x10c
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_CHARAC) == 0x10d, "SIG_SAVE_CHARAC");

struct SIG_SAVE_CHARAC_INFORM_NOTICE
{
    int m_characNo;              // +0x00（CCharacInformNotice::_saveData ORIG 0x080e4b52）
    unsigned char m_flags[0x10]; // +0x04
};
static_assert(sizeof(SIG_SAVE_CHARAC_INFORM_NOTICE) == 0x14, "SIG_SAVE_CHARAC_INFORM_NOTICE");

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

#endif  // DNF_GAME_SIG_TYPES_D_H_
