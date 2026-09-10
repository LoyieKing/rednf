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
#include "Inven_Item.h"   // RandomOption（SIG_MAILBOX_SEND_NEW_MAIL +0x47 真类型）
#include "CEnvironment.h" // ENUM_SERVER_GROUP（SIG_MAILBOX_SEND_NEW_MAIL +0x15f 真型）

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

// [S216/P235] SIG_LOGIN_DATA::m_characData（+0x00e0，0x24×0x94 角色槽）的数组视角。
// 依据：ORIG DB_Login::_getGameDBCharacView @0x084149ba 循环体
//   `mov -0xc(%ebp),%eax; mov 0xc(%ebp),%edx; imul $0x94,%eax,%eax;
//    lea (%edx,%eax,1),%eax; add $0xe0,%eax; mov (%eax),%eax`
//   ⇒ i 先、base 后、成员偏移 $0xe0 最后加 = **完整类型成员直访问**；
//   reinterpret_cast<View*>(sig) 形在 4.4.7 恒先载 base（探针 /tmp/s216_probe1.cpp
//   fA vs fD 逐条对照）。本视角只增并列成员，两视角等宽 0x14d0，
//   SIG_LOGIN_DATA sizeof/m_characData 偏移/全部 mangled 零影响。
struct SIG_CHARAC_SLOT_VIEW
{
    int  m_characNo;               // +0x00 槽内 charac_no（ORIG `mov (%eax),%eax`）
    char m_name[0x16];             // +0x04..0x19 角色名（ORIG strncpy 0x1e 越读进
                                   //            m_level/m_rest 照抄，P171）
    unsigned short m_level;        // +0x1a 槽内等级（ORIG GetMercenarySystemInfo
                                   //            0x084156a5 `movzwl 0xa(%eax),%ebx`）
    char m_rest[0x78];             // +0x1c..0x93（job 等，本视角不展开）
};
struct SIG_CHARAC_SLOTS_VIEW
{
    SIG_CHARAC_SLOT_VIEW m_slots[0x24];   // 36 槽（blob 0x14d0 = 36×0x94）
};

// [S216/P235] SIG_LOGIN_DATA +0x3fa0 区（m_characSlotCurCount..m_ingameEventHistory）
// 的历史表数组视角。依据 ORIG DB_Login::GetUserInGameEventHistory 0x08415391 地址链
//   `mov i,%edx; mov %edx,%eax; add %edx,%eax; add %edx,%eax; shl $0x2,%eax;
//    add $0x3fa0,%eax; add 0xc(%ebp),%eax; add $0xc,%eax`
//   ⇒ 成员偏移 $0x3fa0 先加、base 后加、行内字段偏移 $0xc 最后加 = 完整类型成员
//   直访问（探针 /tmp/s216_probe3.cpp fH 逐条同形）；裸 char* 算术恒把 $0x3fa0+$0xc
//   折成 $0x3fac（N1）。视角等宽（struct 侧 0x184 > 视角 0x174），
//   SIG_LOGIN_DATA sizeof/全部偏移/mangled 零影响。
struct SIG_INGAME_HISTORY_ROW
{
    int m_type;        // +0x00 seLect type
    int m_history1;    // +0x04 history_1
    int m_history2;    // +0x08 history_2
};
struct SIG_INGAME_HISTORY_VIEW
{
    char m_pad[0x0c];                          // +0x00..0x0b（0x3fa0..0x3fab 既有标量）
    SIG_INGAME_HISTORY_ROW m_rows[30];         // +0x0c 起 30 行 × 0xc
};

// [SweepARADLoginSecurity 实验] 仅本结构用 pack(4)：ORIG 4.1 对 m_punishInfos
// （pack(1) 外层）成员数组访问合并常量（`add $0x3a68; mov (%eax)`），4.4 在
// pack(1)/pack(2) 下恒折分（`add $0x3a60; mov 0x8(%eax)`，探针 /tmp/p_arad/t15.cpp
// G1/G2 vs G4）；pack(4) 下 4.4 合并且保持「下标先、基址后」装载序。
#pragma pack(push, 4)
struct SIG_LOGIN_DATA
{
    // +0x0000..0x00a0 头部区（DisPatcher_Login::dispatch_sig 实证拆具名）：
    char           m_userId[0x18];          // +0x0000..0x0017 登录账号串（strncpy 0x18）
    char           m_pad18[0x84];           // +0x0018..0x009b memset 清零区
    int            m_gcNo;                  // +0x009c 服务端 gc 号（login 复核写入）
    char           m_loginType;             // +0x00a0 登录类型 byte（0/1/2）
    char           m_blackIpCheckBuf[0x14]; // +0x00a1 黑名单 IP 串缓冲（isBlackIP 实参）
    bool  m_geoAllowSkipFlag;             // +0x00b5 ==1 时跳过 RestrictGeolocation 检测（ORIG bool：movzbl;xor $0x1;test %al,%al，dispatch_sig 084c5cd3 段）
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
    union
    {
        char m_characData[0x14d0];          // +0x00e0 _Charac_info 区域视图（SetCharacInfo）
        SIG_CHARAC_SLOTS_VIEW m_characView; // +0x00e0 同址数组视角（P235，见上）
    };
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
    char           m_passPadVersion;        // +0x390c CPad::setVersionInfo（char，直取地址 lea 形态）
    bool           m_passPadAdvideChangePwd;// +0x390d setAdvideChangePwd
    bool           m_passPadCertified;      // +0x390e setCertified 数据源
    char           m_pad390f[1];            // +0x390f
    long           m_passPadCheckTime;      // +0x3910 setCheckTime（32 位 long，long& 直取地址 lea 形态）
    int            m_passPadFailCnt;        // +0x3914 setFailCnt / E_AUTH_MOUSE put_byte
    int            m_passPadCancelCnt;      // +0x3918 setCancelCnt
    char           m_pad391c[0x3950 - 0x391c]; // +0x391c
    int            m_accountFail;           // +0x3950 ARAD 登录复核写入（account_password_check）
    int            m_totalAccountFail;      // +0x3954 同上
    // [SweepARADLoginSecurity 类型直证 2026-09-10] int（非 unsigned）：
    // ORIG account_password_check 0x0818f8b6 / DB_Login::GetAccountInfo 0x08413713 的
    // store 均 3 条形 RHS-first 无转换中转（`mov 槽,%edx; mov sig,%eax; mov %edx,disp`）
    // ⇒ 与 stOutputData_t::m_lastPlayTime 同型；RHS 侧由 ORIG 0x08602da6 调
    // `_ZN5MySQL7get_intEiRi`（int&）硬直证为 int（4.1.2/4.4.7 探针：异型恒 4 条带中转）。
    int            m_lastPlayTime;          // +0x3958 上次游戏秒（ReturnUser/每日窗口基准）
    unsigned int   m_dungeonGainedGold;     // +0x395c CDungeonGainedGold::setGainedGold
    int            m_reportMannerlessUserCount; // +0x3960 当日无礼举报数
    // 同上：ORIG 0x0818f8ce / 0x0841374f 3 条形无中转；RHS 由 0x08602da6
    // `_ZN5MySQL8get_byteEiRc`（char&）直证为 char ⇒ 本字段 char。
    char           m_reliablePersonFlag;    // +0x3964 SetReliablePerson(!=0)
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
    // +0x39dc：ORIG account_password_check 0x0818f8e8 写入（源 ssoOut.m_securityFlag，
    // 来自 member_login.security_flag 列）⇒ pad 拆具名（推断：SSO 安全标志字节）
    char           m_ssoSecurityFlag;       // +0x39dc
    char           m_pad39dd[3];            // +0x39dd
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
    // +0x3a50：写入侧（GetPunishUserInfo）ORIG 08410da9 为 RHS-first+edx 中转
    // ⇒ 字段与 int RHS 异型；消费侧 T2 直证 signed（ARAD_LoginSecurity 0x0818f970
    // `jg`）⇒ 双视角（P235/P211 档③，探针 /tmp/punp.cpp f↔g）。A/B 实测：
    // ARAD_LoginSecurity 53/100/2/3/49 与 InterDispatcher 121/1331/5/133/700
    // 在 union 前后逐项零变化（int 视角消费点不受影响）。
    union
    {
        int m_punishInfoCount;          // +0x3a50 punish 信息条数（err_code==4 亦作索引）
        unsigned int m_punishInfoCountU;// +0x3a50 写入视角（GetPunishUserInfo 专用）
    };
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
    union
    {
        struct
        {
            unsigned char  m_characSlotCurCount;    // +0x3fa0 setCharacSlotCountInfo 参数1
            unsigned char  m_characSlotMaxCount;    // +0x3fa1 参数2
            char           m_pad3fa2[2];            // +0x3fa2
            int            m_ontimeLastRecvIdx;     // +0x3fa4 set_ontime_last_recv_idx
            bool           m_checkPickUpRandomOptionItemFlag; // +0x3fa8 true→setCheckPickUpRandomOptionItem(true)
            unsigned char  m_fatigueKind;           // +0x3fa9 set_fatigue_count_db 参数1
            short          m_fatigueValue;          // +0x3faa 参数2
            SIG_INGAME_EVENT_HISTORY m_ingameEventHistory; // +0x3fac set_ingame_event_history 整包
        };
        SIG_INGAME_HISTORY_VIEW m_ingameEventHistoryView;  // +0x3fa0 同址数组视角（P235）
    };
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
    // [b28c-34] 换型 int 已回退（C28L20 反例：DB_Multi_MailBox_Req_Send::dispatch
    // 0x0843bb89 load→int 形参段 ORIG 有 eax→edx 中转 ⇒ 字段须与 int 异型；
    // 与 store 段（0x084cffc9 LHS-first）方向相反，共享头维持 unsigned int。
    // store 点同型化改由 InterDispatcher.cpp 源侧 int 引用视图承载。
    unsigned int m_senderCharacNo;    // +0x2d game 写 get_charac_no(-1)
    char m_payload[0x3f88];           // +0x31 MSG_CHECK_MAIL_RECEIVER 载荷透传
    unsigned int m_resultFlag;        // +0x3fb9 DB 结果（0 ok / 0x4d 名称非法）
    // 2026-09-05 §4.3（DBCharac 批次）：类型 int——DB_Multi_MailBox_Req_Send::
    // dispatch 0x0843bb89 `lea 0x3fbd` 绑 InsertLetter 形参 `int&`
    // （mangled ...PKcS3_Ril...，DB_MailBox_Req_Send.h）；按名消费者仅该 TU，
    // 同宽改型 sizeof/偏移全不变（下方 assert 锁定）。
    int m_letterId;                   // +0x3fbd DB_Multi_MailBox_Req_Send 读 letterId
    char m_senderName[0x1d];          // +0x3fc1 SendDBThread_forchina 写发件人名（strncpy 0x1d）
};
#pragma pack(pop)
static_assert(sizeof(SIG_MAILBOX_CHECK_RECEIVER) == 0x3fde,
              "SIG_MAILBOX_CHECK_RECEIVER ABI drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_resultFlag) == 0x3fb9,
              "SIG_MAILBOX_CHECK_RECEIVER resultFlag offset drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_senderName) == 0x3fc1,
              "SIG_MAILBOX_CHECK_RECEIVER senderName offset drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_payload) == 0x31,
              "SIG_MAILBOX_CHECK_RECEIVER payload offset drift");
static_assert(offsetof(SIG_MAILBOX_CHECK_RECEIVER, m_letterId) == 0x3fbd,
              "SIG_MAILBOX_CHECK_RECEIVER letterId offset drift");

// pack(1) 保持与旧版（纯 unsigned char 数组）一致 align==1 属性。
#pragma pack(push, 1)
struct SIG_MAILBOX_DEL_MAIL
{
    // ORIG 布局（ReqDBRemoveMail 0x8556772 + DB_MailBox_Req_Delete::dispatch
    // @0x0841f084 报告汇编实证）：+0x00 count（movl test/cmp $1，并作为
    // getList2inQuery(uint*) 的 list[0] 读取 → unsigned int）；+0x04 起
    // postal idx int 表（单条分支 `mov 0x4(%eax)` 送 %d）；+0x54 type
    // （`mov 0x54(%eax)` 送 SQL delete_flag=%d 实参）。
    // 2026-09-05 §4.3 字段化：m_data 基名保留（union 叠加），既有 blob
    // 视图消费者（CMailBoxHelper.cpp MailBoxDelMailV 经同址 reinterpret）零破坏。
    union
    {
        unsigned char m_data[0x58];
        // [2026-09-09 Lane20] 数组视角（P235）：ReqDBRemoveMail 0x0855681c 的
        // scaled 直存 `mov %ecx,(%eax,%edx,4)`（索引 = 从消息首起算的 dword 号）
        // 需完整类型数组成员访问；经 cast 视图恒 base-first 且折 shl/lea。
        unsigned int   m_words[0x16];  // [0]=count、[1..0x14]=postal idx、[0x15]=type
        struct
        {
            unsigned int   m_count;       // +0x00 邮件条数（getList2inQuery list[0]）
            unsigned int   m_postalIds[20]; // +0x04..0x53 postal idx 表（20×4）
            int            m_type;        // +0x54 SQL delete_flag 实参值
        };
    };
};
static_assert(offsetof(SIG_MAILBOX_DEL_MAIL, m_words) == 0x00,
              "SIG_MAILBOX_DEL_MAIL words view offset drift");
static_assert(sizeof(SIG_MAILBOX_DEL_MAIL) == 0x58, "SIG_MAILBOX_DEL_MAIL ABI drift");
static_assert(offsetof(SIG_MAILBOX_DEL_MAIL, m_postalIds) == 0x04,
              "SIG_MAILBOX_DEL_MAIL postalIds offset drift");
static_assert(offsetof(SIG_MAILBOX_DEL_MAIL, m_type) == 0x54,
              "SIG_MAILBOX_DEL_MAIL type offset drift");
#pragma pack(pop)

// SIG_MAILBOX_LIST 响应区头部字段化（§8.3，广播 2026-09-05 无异议）：
// 证据 docs/class_func_reports/Inter_MailBox_Req_List/dispatch_sig.md ORIG
// 0x084cf845 `mov 0x3332(%eax),%ebx`、同族 0x3336 读（单段 disp）。
// sizeof 0x338a（ReqDBLoadMail memset 证据）不变；consumers 全部经
// reinterpret 视图/偏移宏按绝对偏移访问，不依赖 m_data 起点。
#pragma pack(push, 1)
// SIG_MAILBOX_LIST postal 区具名视图（§8.3 字段化，b28d-solo-DB_MailBox_Req_List 广播）：
// ORIG 对 postal 明细发 `imul $0x61` + `mov (%edx,%eax,1)`（postal_id@+0）与
// `add $0x20; mov 0xe(%edx)`（add_info@+0x2e 两级）——视图经**参数类型自身的成员**
// 访问才能复现 ORIG 4.1 的「下标先、基址后」装载序（cast 视图形恒 base 先，
// P235；探针 /tmp/p2/rm2.cpp 与 ORIG 0x0841cd03 逐条同形）。
// amplify 三字段（ORIG 0x0841e691..0x0841e6b6：一次 `add $0x30` 基址 + 0x9/0xd/0x11
// 连续 store = 结构体整体赋值形）——具名行类型供 dispatch 写入侧整体赋值（消 cast 视图）。
struct SIG_MAILBOX_POSTAL_AMP
{
    unsigned int m_opt;          // abs +0x39（amplify_option）
    unsigned int m_val;          // abs +0x3d（amplify_value）
    unsigned short m_rnd;        // abs +0x41（random_option）
};
// extend_info 14 字节行类型（ORIG dispatch 0x0841e807..0x0841e82c：一次 `add $0x30`
// 基址 + 0x16/0x1a/0x1e/0x22 连续 store 实证）。e2 跨 m_occ(abs 0x50) 边界 ⇒ 双半视角。
struct SIG_MAILBOX_POSTAL_EXT
{
    unsigned int m_e0;           // abs +0x46
    unsigned int m_e1;           // abs +0x4a
    union
    {
        unsigned int m_e2;       // abs +0x4e
        struct
        {
            unsigned short m_e2Lo;   // abs +0x4e
            unsigned short m_e2Hi;   // abs +0x50
        };
    };
    unsigned short m_e3;         // abs +0x52
};
static_assert(sizeof(SIG_MAILBOX_POSTAL_EXT) == 0xe,
              "SIG_MAILBOX_POSTAL_EXT size drift");
// +0x50 (abs) 具名子结构（MailHelper 族消费点 m_tail.m_occ.* 路径）：
//   0x0841cc8f `add $0x50; mov 0xc(%eax)`（letter_id 统计，GetOnlyMailCount）
//   0x0841cf7f/0x0841cfdd `add $0x50; mov 0x5(%eax)`（FindLastOccTimeMail）
//   0x0841d3a0 同形（ArrangeMailAndLetter）；m_field55 = charac_no + 0x13c680。
// 起点 abs +0x50（TAIL +0x30）为**直接成员** ⇒ 4.4 折叠锚点 add $0x50 与 ORIG
// 0x0855254c 段一致（CMailBoxHelper isUnlimitMail AE 前提，勿改其位置）。
// 前 5 字节（abs 0x50..0x54）与 extend_info 尾部（ext2 高半/ext3/field54）同址 ——
// ORIG dispatch 该 5 字节是 ext 组整体赋值的一部分（0x0841e80a..0x0841e82c），
// 故按具名字段化（ext2 拆 2+2 字节，零 cast）。
struct SIG_MAILBOX_POSTAL_OCC
{
    unsigned short m_ext2Hi;     // +0x50..0x51（extend_info 第 3 dword 高半）
    unsigned short m_ext3;       // +0x52..0x53（extend_info 第 4 字段）
    char m_field54;              // +0x54（get_byte 19）
    int m_field55;               // +0x55（send_charac_no + 0x13c680）
    char m_avataFlag;            // +0x59（ORIG 0x0841e70a `add $0x59` 写 1/0）
    char m_creatureFlag;         // +0x5a（ORIG 0x0841e75f `add $0x5a` 写 1/0）
    // +0x5b 无限信标志（isUnlimitMail ORIG 0x08552580 `add $0x5b,%eax;
    // movzbl (%eax),%eax` 单字节直返 ⇒ bool；偏移/sizeof 与 m_pad2[3] 等价）
    bool m_unlimit;              // +0x5b
    unsigned int m_letterId;     // +0x5c
    char m_field60;              // +0x60（get_byte 20，ORIG 0x0841e7e3 `add $0x60`）
};
struct SIG_MAILBOX_POSTAL_TAIL
{
    unsigned char m_pad[5];      // +0x00 (abs 0x20..0x24，send_charac_name 尾部)
    int m_itemId;                // +0x05 (abs 0x25)
    char m_sealFlag;             // +0x09 (abs 0x29)
    int m_occTime;               // +0x0a (abs 0x2a)
    unsigned int m_addInfo;      // +0x0e (abs 0x2e)
    char m_endurance;            // +0x12 (abs 0x32)
    short m_upgrade;             // +0x13 (abs 0x33)
    unsigned int m_itemGuid;     // +0x15 (abs 0x35)
    // +0x19 (abs 0x39) 起（§8.3 字段化，b28d DB_MailBox_Req_List dispatch 写入侧实证：
    // ORIG 0x0841e5ae..0x0841e89a 逐字段写序）——顺序具名成员，无 union：
    SIG_MAILBOX_POSTAL_AMP m_amp;    // +0x19 (abs 0x39)
    char m_field43;                  // +0x23 (abs 0x43，seperate_upgrade)
    short m_field44;                 // +0x24 (abs 0x44，type)
    unsigned int m_ext0;             // +0x26 (abs 0x46，extend_info dword 0)
    unsigned int m_ext1;             // +0x2a (abs 0x4a，extend_info dword 1)
    unsigned short m_ext2Lo;         // +0x2e (abs 0x4e，extend_info 第 3 dword 低半)
    SIG_MAILBOX_POSTAL_OCC m_occ;    // +0x30 (abs 0x50) —— 直接成员（锚点见上）
};
struct SIG_MAILBOX_POSTAL
{
    // 匿名 union 双视角（§8.3 字段化，b28d DB_MailBox_Req_List dispatch 写入侧证据）：
    //   视角 A = 既有布局（MailHelper 族消费点 m_postals[i].m_postalId/m_tail 路径不变）；
    //   视角 B = dispatch 写入侧具名行首字段（ORIG 0x0841e5f5 `mov %ecx,0x4(%edx,%eax,1)`
    //     = send_charac_no@+0x04；0x0841e848 strncpy dst=base+8、len 0x1d = send_name@+0x08）。
    union
    {
        struct
        {
            int m_postalId;              // +0x00
            unsigned char m_head[0x20 - 4];
            SIG_MAILBOX_POSTAL_TAIL m_tail;   // +0x20
        };
        struct
        {
            unsigned char m_headB[4];    // +0x00（postal_id 经视角 A 的 m_postalId 访问）
            int m_sendCharacNo;          // +0x04
            char m_sendName[0x1d];       // +0x08..+0x24
        };
    };
};
struct SIG_MAILBOX_LIST_BOX
{
    SIG_MAILBOX_POSTAL m_postals[0x14];   // 0x14*0x61 = 0x794
    int m_count;                          // +0x794
    int m_remain;                         // +0x798
    unsigned int m_maxPostalId;           // +0x79c
};
// creature_items 明细视图（stride 0x25，基址 +0xd30；item[0] 首 4 字节即条数，
// ORIG 0x0841dd0f `mov %edx,0xd30(%eax)` 与 0x0841dddb `add $0xd30; mov %ecx,0x4` 实证）
struct SIG_MAILBOX_CREATURE_ITEM
{
    union
    {
        unsigned char m_pad[4];
        unsigned int m_count;    // +0x00（仅 item[0] 用；与 get_n_rows int 异型触发
                                 // ORIG 0x0841dd0a `mov %eax,%edx` 中转，P211/P217）
    };
    int m_uiId;                  // +0x04
    int m_expireDate;            // +0x08
    int m_regDate;               // +0x0c
    unsigned char m_rest[0x25 - 0x10];
};
struct SIG_MAILBOX_CREATURE_AREA
{
    unsigned char m_pad[0xd30];
    SIG_MAILBOX_CREATURE_ITEM m_items[0x100];   // +0xd30
};
// user_items 明细视图（stride 0x47，基址 +0x7a0；ORIG 0x0841ee1a 段实证：
// +0x04 ui_id / +0x08 expire_date / +0x0c reg_date / +0x10 ipg_agency(0x18) /
// +0x29 jewel_socket(0x1e)；item[0] 首 4 字节为条数）
struct SIG_MAILBOX_USER_ITEM
{
    union
    {
        unsigned char m_pad[4];
        int m_count;             // +0x00（仅 item[0] 用）
    };
    int m_uiId;                  // +0x04
    unsigned int m_expireDate;   // +0x08
    unsigned int m_regDate;      // +0x0c
    char m_ipgAgency[0x18];      // +0x10
    char m_field28;              // +0x28
    unsigned char m_jewel[0x1e]; // +0x29
};
struct SIG_MAILBOX_USER_AREA
{
    unsigned char m_pad[0x7a0];
    SIG_MAILBOX_USER_ITEM m_items[0x80];    // +0x7a0（0x7a0+0x80*0x47=0x2b20 ≤ 0x3332）
};
struct SIG_MAILBOX_LIST
{
    // ORIG 尺寸 0x338a（ReqDBLoadMail memset 证据）
    union
    {
        unsigned char m_head[0x3332];   // +0x00 请求头/ postal 区（原字段，保留）
        SIG_MAILBOX_LIST_BOX m_box;     // +0x00 postal 区具名视图（0x7a0，b28d 新增）
        SIG_MAILBOX_USER_AREA m_user;   // +0x7a0 user_items 视图（b28d 新增）
        SIG_MAILBOX_CREATURE_AREA m_creature;   // +0xd30 creature_items 视图（b28d 新增）
    };
    int m_curCharacNo;                  // +0x3332（请求角色号）
    int m_mailLoadCount;                // +0x3336（装载计数，loaded-twice 比对）
    unsigned char m_data[0x338a - 0x333a];   // +0x333a 明细尾区
};
#pragma pack(pop)
static_assert(sizeof(SIG_MAILBOX_LIST) == 0x338a,
              "SIG_MAILBOX_LIST ORIG size drift");
static_assert(offsetof(SIG_MAILBOX_LIST, m_box.m_maxPostalId) == 0x79c,
              "SIG_MAILBOX_LIST postal max offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_field55) == 0x55,
              "SIG_MAILBOX_POSTAL occ field55 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_unlimit) == 0x5b,
              "SIG_MAILBOX_POSTAL occ unlimit offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_letterId) == 0x5c,
              "SIG_MAILBOX_POSTAL occ letterId offset drift");
// [b28d DB_MailBox_Req_List] 字段化偏移断言（ORIG dispatch 地址链见结构内注释）
static_assert(sizeof(SIG_MAILBOX_POSTAL) == 0x61, "SIG_MAILBOX_POSTAL ORIG size drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_sendCharacNo) == 0x04,
              "SIG_MAILBOX_POSTAL sendCharacNo offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_sendName) == 0x08,
              "SIG_MAILBOX_POSTAL sendName offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_itemId) == 0x25,
              "SIG_MAILBOX_POSTAL itemId offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_endurance) == 0x32,
              "SIG_MAILBOX_POSTAL endurance offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_upgrade) == 0x33,
              "SIG_MAILBOX_POSTAL upgrade offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_itemGuid) == 0x35,
              "SIG_MAILBOX_POSTAL itemGuid offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_amp.m_opt) == 0x39,
              "SIG_MAILBOX_POSTAL ampOpt offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_amp.m_val) == 0x3d,
              "SIG_MAILBOX_POSTAL ampVal offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_amp.m_rnd) == 0x41,
              "SIG_MAILBOX_POSTAL rndOpt offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_field43) == 0x43,
              "SIG_MAILBOX_POSTAL field43 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_field44) == 0x44,
              "SIG_MAILBOX_POSTAL field44 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_ext0) == 0x46,
              "SIG_MAILBOX_POSTAL ext0 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_ext1) == 0x4a,
              "SIG_MAILBOX_POSTAL ext1 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_ext2Lo) == 0x4e,
              "SIG_MAILBOX_POSTAL ext2Lo offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_ext2Hi) == 0x50,
              "SIG_MAILBOX_POSTAL ext2Hi offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_ext3) == 0x52,
              "SIG_MAILBOX_POSTAL ext3 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_field54) == 0x54,
              "SIG_MAILBOX_POSTAL field54 offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_avataFlag) == 0x59,
              "SIG_MAILBOX_POSTAL avataFlag offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_creatureFlag) == 0x5a,
              "SIG_MAILBOX_POSTAL creatureFlag offset drift");
static_assert(offsetof(SIG_MAILBOX_POSTAL, m_tail.m_occ.m_field60) == 0x60,
              "SIG_MAILBOX_POSTAL field60 offset drift");

// [2026-09-09 Lane20] 字段化（§8.3）：原 opaque m_data[0x168] → 具名字段。
// 证据 = ORIG CMailBoxHelper::_makeSendMailData 0x08555756 / _DoSendNewMail
// 0x08555916 逐偏移写入（与 CMailBoxHelper.cpp 原 MailBoxSendNewMailV 视图
// 完全一致，逐字段偏移已由该视图 static_assert 核过）。sizeof 0x168 与全部
// 偏移零变化；改字段化的原因是视图 cast 使 4.4 在赋值点/实参处提前把 msg 装
// callee-saved 跨 call 保活（P235 同族），直访问即复现 ORIG 形。
// +0x3a guid / +0x47 random 两区保留字节数组：消费侧以 stAmplifyGuidView /
// RandomOption 视图整体赋值（结构块拷贝语义，P247/P263）。
#pragma pack(push, 1)
struct SIG_MAILBOX_SEND_NEW_MAIL
{
    char m_senderName[0x1d];         // +0x00（strncpy 0x1d）
    char m_pad1d;                    // +0x1d
    unsigned int m_senderCharacNo;   // +0x1e（P211 异型化：ORIG 0x0855576x RHS-first）
    unsigned int m_amount;           // +0x22
    unsigned int m_destCharacNo;     // +0x26
    unsigned char m_itemField0;      // +0x2a
    unsigned int m_addInfo;          // +0x2b（与 Inven_Item::m_addInfo unsigned 同型直存）
    int m_addInfo2;                  // +0x2f
    unsigned char m_itemAttr;        // +0x33
    unsigned short m_fieldb;         // +0x34
    unsigned int m_fieldd;           // +0x36（与 Inven_Item::m_fieldd unsigned 同型直存）
    stAmplifyGuidView m_guid;        // +0x3a..0x43（amp guid，_makeSendMailData 结构赋值）
    unsigned char m_ampType;         // +0x44
    unsigned short m_ampIdx;         // +0x45
    RandomOption m_rand;             // +0x47..0x54（_makeSendMailData 结构赋值）
    unsigned char m_upgradeSep;      // +0x55
    unsigned char m_isAvatarSend;    // +0x56（m_field1==8）
    unsigned char m_cashShopFlag;    // +0x57（CashShop 置 1）
    bool m_isCreature;               // +0x58（IsCreatureItemType 返 bool，P136/P292）
    unsigned short m_titleLen;       // +0x59
    char m_title[0x100];             // +0x5b
    unsigned int m_nB;               // +0x15b
    ENUM_SERVER_GROUP m_serverGroup; // +0x15f（GetServerGroup 返回值同型直存，P211）
    int m_nType;                     // +0x163
    unsigned char m_flag;            // +0x167
};
#pragma pack(pop)
static_assert(sizeof(SIG_MAILBOX_SEND_NEW_MAIL) == 0x168,
              "SIG_MAILBOX_SEND_NEW_MAIL ORIG size drift");
static_assert(offsetof(SIG_MAILBOX_SEND_NEW_MAIL, m_guid) == 0x3a,
              "SIG_MAILBOX_SEND_NEW_MAIL guid offset drift");
static_assert(offsetof(SIG_MAILBOX_SEND_NEW_MAIL, m_rand) == 0x47,
              "SIG_MAILBOX_SEND_NEW_MAIL rand offset drift");
static_assert(offsetof(SIG_MAILBOX_SEND_NEW_MAIL, m_serverGroup) == 0x15f,
              "SIG_MAILBOX_SEND_NEW_MAIL serverGroup offset drift");
static_assert(offsetof(SIG_MAILBOX_SEND_NEW_MAIL, m_title) == 0x5b,
              "SIG_MAILBOX_SEND_NEW_MAIL title offset drift");

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
    // Lane9 字段化（§8.3）：ORIG CUser::LoadMileage 0x086507b0 三写实证
    // (0)=acc_id、(4)=characNo、(8)=0；布局/sizeof 不变。
    unsigned int m_accId;      // +0x00（get_acc_id 返 unsigned 同型直存）
    unsigned int m_characNo;   // +0x04（b28d-Main：getCurCharacNo 已改 unsigned ⇒ 同型直存恢复 ORIG 0x086507b0 LHS-first；int 时翻 RHS-first 中转）
    int m_mileage;             // +0x08（GetMileage 返 int 同型）
};

// SIG_MOUSE_REGISTER（0x59）—— ORIG 权威布局
// （Dispatcher_MouseRegister::dispatch_sig @0x082637fe / doPassPadReplay @0x082633c6 写入点）
#pragma pack(push, 1)
struct SIG_MOUSE_REGISTER
{
    unsigned int m_accId;              // +0x00 CUser::get_acc_id
    // [b28d-47] int（非 unsigned）：_sendResult 第 3 形参为 int（ORIG mangled
    // `..._sendResultEjiiiP18SIG_MOUSE_REGISTER`）；字段与形参同型 ⇒ 4.4 出 ORIG
    // 0x08422c45 的单段 `mov 0x4(%eax),%edx`（unsigned 字段反产 eax 中转 2 条）。
    int m_pkgType;                     // +0x04 包内字节扩展（movzbl 后写 4 字节）
    char m_password1[7];               // +0x08 解密明文密码 1（strncpy 7）
    char m_password2[7];               // +0x0f 解密明文密码 2（strncpy 7）
    char m_pad16[0xd];                 // +0x16..0x22 未建模填充（无读写证据）
    char m_padPassword[0x24];          // +0x23 Sanicova::CPad::getPassword 快照（memcpy 0x24）
    char m_field47;                    // +0x47 ORIG 无读写证据（0x23+0x24 与 0x48 间的保留字节）
    char m_webAddress[0x10];           // +0x48 CUser::getWebAddress（strncpy 0x10）
    bool m_advideChangePwd;            // +0x58 getAdvideChangePwd 单字节（ORIG bool）
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
    // [b28d-W8EventB 实测] 4B 字段：ORIG 0x0810ffb8（sendBringUpReward）
    // `movzbl %al,%edx; mov %edx,0x8(%eax)` 与 CUser::UpdateData 0x08654089 同形
    // ⇒ 原码按 4 字节存（旧 1B+pad09 只覆盖写宽度）。sizeof 0x10 与 +0x0c/+0x0d
    // 偏移不变。
    unsigned int   m_serverGroup;  // +0x08 ←*(G_CEnvironment()+0x378)
    unsigned char  m_usedMaterialCnt; // +0x0c ←getCurUseBringUpMaterialCount
    unsigned char  m_rewardFlag;      // +0x0d ←getBringUpRewardFlag
    unsigned char  m_pad0e[2];        // +0x0e..+0x0f
};                                 // sizeof = 0x10

// SIG_ONLINE_PRELIMINARY_MEMBER（0x14）—— ORIG 权威布局（OnlinePvP 批次字段化）：
// 证据 docs/class_func_reports/online_preliminary__COnlinePreliminaryMember/
//   _saveData.md 0x08585fd6：movw %dx,(%buf) ←GetServerGroup（+0 2 字节）；
//   movl characNo→0x4(%buf)；this+8/+0xc/+0x10 → buf+8/+0xc/+0x10 逐条 movl；
//   loadData.md 0x08585f9e：buf+8/+0xc/+0x10 → this+8/+0xc/+0x10 同宽回读。
// rating 为 float 成员 4B 直拷（Ghidra undefined4；成员侧 float 实证）。
#pragma pack(push, 1)
struct SIG_ONLINE_PRELIMINARY_MEMBER
{
    unsigned short m_serverGroup;  // +0x00 movw @0x085860a1（CUser::GetServerGroup）
    char           m_pad02[2];     // +0x02..0x03 无读写证据
    // [b28d-solo-OnlinePreliminaryTeam] P211 档③：ORIG _saveData 0x08586085
    // `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)` = LHS-first 直存无中转 ⇒ 字段与
    // RHS 同型（getCurCharacNo 返 unsigned int，CUserCharacInfo.h:776）。int 时
    // 4.4 产 RHS-first + `mov %eax,%edx` 中转（实测 +1 条）。宽度/偏移/mangled 不变。
    unsigned int   m_characNo;     // +0x04 movl（同型直存）
    float          m_rating;       // +0x08 4B 位拷贝（成员 m_rating float）
    int            m_winCount;     // +0x0c movl
    int            m_loseCount;    // +0x10 movl
};
#pragma pack(pop)
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_MEMBER) == 0x14,
              "SIG_ONLINE_PRELIMINARY_MEMBER ABI drift");

// SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST（0x4bc）—— ORIG 权威布局：
// 证据 online_preliminary__COnlinePreliminaryMemberMatchList/_saveData.md
//   0x08586510：buf+0 movw（group）/buf+4 movl（characNo）/buf+8 movl count=0；
//   条目 stride 12B：movw %bx→0xc+idx*0xc（key.m_serverId 先 movzwl 读）、
//   movl→0x10（key.m_uniqueId）、movl→0x14（PlayCountInfo::m_count）；
//   loadData.md 0x08586466：count@+8（int）、sid movzwl@+0xc、uid@+0x10、
//   playCount@+0x14。0xc + 12*100 = 0x4bc。
// [b28d-solo-OnlinePreliminaryTeam] 去 pack(1)：全部成员偏移已自然对齐
// （u16@+0/u16@+2/u32@+4/u32@+8 ⇒ Entry 12B；u16@+0/pad/u32@+4/int@+8/
// Entry@+0xc ⇒ 0x4bc），pack(1) 只把对齐压到 1 并使 4.4 把
// `d->m_entries[i].m_uniqueId/m_playCount` 折成 scaled disp 单条
// （探针 /tmp/p3_probe2.cpp），而 ORIG 0x0858648b/0x085864e3 为
// `lea (%ecx,%eax,1); add $0x10,%eax; mov (%eax)` 两段 ⇒ 原码是自然对齐
// 结构体（探针 /tmp/p3_probe.cpp f1 逐条全等）。sizeof/偏移/static_assert 不变。
struct SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST
{
    struct Entry
    {
        unsigned short m_serverId;   // +0x00（角色键 serverId，16 位读写实证）
        unsigned short m_pad02;      // +0x02..0x03 无读写证据（stride 12 填充）
        unsigned int   m_uniqueId;   // +0x04（角色键 uniqueId，movl）
        unsigned int   m_playCount;  // +0x08（PlayCountInfo::m_count，movl）
    };
    unsigned short m_serverGroup;    // +0x00 movw @0x085865b1
    char           m_pad02[2];       // +0x02..0x03 无读写证据
    unsigned int   m_characNo;       // +0x04 movl @0x085865c2
    int            m_count;          // +0x08 条目计数（int，100 分批比较 @0x08586695）
    Entry          m_entries[100];   // +0x0c stride 12B
};
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST) == 0x4bc,
              "SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST ABI drift");
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST::Entry) == 0xc,
              "SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST::Entry ABI drift");

// SIG_ONLINE_PRELIMINARY_TEAM（0x20）—— ORIG 权威布局：
// 证据 online_preliminary__COnlinePreliminaryTeam/_saveTeam.md 0x08587ee4：
//   buf+0 ←this+4 movl（teamId）；buf+0x14 ←this+8 movl（rating 位模式，
//   成员侧 int 容器）；buf+0x18 ←this+0xc（win）；buf+0x1c ←this+0x10（lose）。
//   loadDataPreliminaryTeam.md 0x085882b8：+0 int、+4 strncpy(dst,data+4,0xf)
//   即队名区 0x10 字节、+0x14/+0x18/+0x1c 同宽回读。
#pragma pack(push, 1)
struct SIG_ONLINE_PRELIMINARY_TEAM
{
    int            m_teamId;         // +0x00（COnlinePreliminaryTeam::m_field04）
    char           m_teamName[0x10]; // +0x04 队名（strncpy 0xf + 终止位）
    float          m_rating;         // +0x14 rating 位模式（成员 float，ORIG movl 4B 直拷无 cvt）
    int            m_winCount;       // +0x18 movl
    int            m_loseCount;      // +0x1c movl
};
#pragma pack(pop)
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_TEAM) == 0x20,
              "SIG_ONLINE_PRELIMINARY_TEAM ABI drift");

// SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST（0x328）—— ORIG 权威布局：
// 证据 _saveTeamMatchList.md 0x0858800c：buf+0 ←this+4 movl（teamId）、
//   buf+4 movl count（0 初始化/自增/==100 比较 @0x08588149）；
//   条目 stride 8B：movl key→0x8+idx*8（@0x0858811e）、
//   movl PlayCountInfoEx::m_count→0xc+idx*8（@0x08588130）。
//   loadDataPreliminaryTeamMatchList.md 0x0858831a：count@+4、key@+8+i*8、
//   value@+0xc+i*8。8 + 8*100 = 0x328。
#pragma pack(push, 1)
struct SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST
{
    struct Entry
    {
        int           m_otherTeamId;  // +0x00（map key，movl 读写实证）
        unsigned int  m_playCount;    // +0x04（PlayCountInfoEx::m_count，movl）
    };
    int    m_teamId;       // +0x00（序列化键）
    int    m_count;        // +0x04 条目计数
    Entry  m_entries[100]; // +0x08 stride 8B
};
#pragma pack(pop)
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST) == 0x328,
              "SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST ABI drift");
static_assert(sizeof(SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST::Entry) == 8,
              "SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST::Entry ABI drift");

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
    // §8.3 字段化（2026-09-10）：ORIG DB_UpdatePurchaseCashItemBonus::dispatch
    // 0x0843e4e5..eb `mov -0x1c(%ebp),%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`
    // 为 RHS-first 3 条形。探针 /tmp/bonusprobe/q.o：字段 unsigned + RHS 经
    // get_uint(&x) 取址写入 ⇒ f6/f8 3 条 RHS-first（= ORIG）；字段 int ⇒ f5/f7
    // 多 `mov %eax,%edx` 中转（4 条）；`*(unsigned*)p=x` 指针解引用 ⇒ LHS-first
    // （P211 pad 解引用族）⇒ 必须具名成员访问 + unsigned。4 字节，sizeof 不变。
    unsigned int m_purchaseCnt;   // +0x00
};
static_assert(sizeof(SIG_PURCHASE_CASHITEM_BONUS) == 0x4,
              "SIG_PURCHASE_CASHITEM_BONUS ABI drift");

// 2026-09-05 §4.3（DBCharac 批次）字段化。证据：
//   DB_ReqQueryCharacInfoMailbox::dispatch @0x0843bd46 报告：基址 char* 送
//   escape_string（首字节即角色名）；_sendResult @0x0843c224 报告：+0x20/+0x24
//   movl（errCode/characNo）、+0x28/+0x2c/+0x2d movb（job/growType/sex）、
//   +0x2a movw（lev）、strncpy(base,…,0x1d)。OrigDBInter19.cpp QueryCharacInfoView
//   （@0x08426e04 线，+0x2e 另有一 byte 写点）同址同布局。
//   m_pad 基名保留（union 叠加）：W8LT_DBTask.cpp makeRequest strncpy(sig->m_pad,…)
//   消费者零破坏。
#pragma pack(push, 1)
struct SIG_QUERY_CHARAC_INFO
{
    union
    {
        char m_pad[0x30];
        struct
        {
            char m_name[0x1d];    // +0x00 角色名（strncpy/escape_string 0x1d）
            char m_pad1d[3];      // +0x1d..+0x1f
            int  m_errCode;       // +0x20 结果码（0 成功）
            int  m_characNo;      // +0x24 charac_no
            char m_job;           // +0x28 job（byte 存）
            char m_pad29;         // +0x29
            short m_lev;          // +0x2a lev（word 存）
            char m_growType;      // +0x2c grow_type（byte 存）
            char m_sex;           // +0x2d sex（byte 存）
            char m_pad2e[2];      // +0x2e..+0x2f（@0x08426e04 线 +0x2e 有写点，本线无）
        };
    };
};
#pragma pack(pop)
static_assert(sizeof(SIG_QUERY_CHARAC_INFO) == 0x30, "SIG_QUERY_CHARAC_INFO ABI drift");
static_assert(offsetof(SIG_QUERY_CHARAC_INFO, m_errCode) == 0x20,
              "SIG_QUERY_CHARAC_INFO errCode offset drift");
static_assert(offsetof(SIG_QUERY_CHARAC_INFO, m_lev) == 0x2a,
              "SIG_QUERY_CHARAC_INFO lev offset drift");
static_assert(offsetof(SIG_QUERY_CHARAC_INFO, m_sex) == 0x2d,
              "SIG_QUERY_CHARAC_INFO sex offset drift");
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

    SIG_REDEEMINFO_LIST_TO_DB() : m_field4(0) {}   // ORIG W 0x0848f894（C1/C2 同址）：m_items[0x15] 自动数组先于 m_field4=0，故置 init-list
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
    // 字段化（W8DB，ORIG 0x0842f3a0 dispatch 读 +0 accId(uint)/+4
    // keyType(int)；写侧 CUserMissingA2.cpp makeRequest(0x749a) 区域
    // View 同布局具名）：
    unsigned int m_accId;    // +0x00（member_key_option.m_id）
    int m_hotkeyType;        // +0x04（key_type）
};
static_assert(sizeof(SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION) == 0x8,
              "SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION ABI drift");

struct SIG_REQ_LOAD_GAME_OPTION
{
    unsigned int m_accId;   // +0x00（Inter_LoginInfo::dispatch_sig 写 get_acc_id；ORIG 全结构仅首 dword 使用）
};

struct SIG_RESTING_USER_RESTRICT
{
    // 字段化（W8DB，ORIG 写侧 MouseRegisterEJ.cpp 0x0843741e 区域
    // `*reinterpret_cast<unsigned int*>(sig) = uid`；dispatch 0x0843712e
    // NumberToString(+0)=member_punish_info.m_id，原 m_pad）：
    unsigned int m_uid;      // +0x00（member_punish_info.m_id）
};
static_assert(sizeof(SIG_RESTING_USER_RESTRICT) == 0x4,
              "SIG_RESTING_USER_RESTRICT ABI drift");

// ORIG ctor 0x08491eca：仅构造 +0x4 的 CONTINUOUS_ITEM_INFO（0x194；count@+0 +
// entries{int,uint}[0x32]，定义于 SigTypes_C.h）。
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
    unsigned int m_characNo;     // +0x00（CCharacInformNotice::_saveData ORIG 0x080e4c21：
                                 //   `call get_charac_no; mov %eax,%edx; mov -0xc(%ebp),%eax;
                                 //   mov %edx,(%eax)` = RHS 先+中转（P211 call 返回值异型）；
                                 //   get_charac_no 权威返回 int（GlobalData.h/CUser.h:2917）
                                 //   ⇒ 字段必非 int。b28c-L15）
    unsigned char m_flags[0x10]; // +0x04
};
static_assert(sizeof(SIG_SAVE_CHARAC_INFORM_NOTICE) == 0x14, "SIG_SAVE_CHARAC_INFORM_NOTICE");

struct SIG_SAVE_CHATTING_EMOTICON
{
    char m_pad[0x214];
};

// pack(1)：m_a@+0x0a/m_b@+0x0e 非对齐（dispatch 读序实证）。
#pragma pack(push, 1)
struct SIG_SAVE_CLIENT_HACK_LOG
{
    // ORIG 写序具名化（ReqDBSaveHackInfo 0x080f8f62 / dispatch 读序
    // W8LT_DBTask SIG_SAVE_CLIENT_HACK_LOG_VIEW 同布局佐证）。
    unsigned int m_uid;          // +0x00
    unsigned int m_type;         // +0x04
    unsigned short m_cnt;        // +0x08
    int m_a;                     // +0x0a
    int m_b;                     // +0x0e
};
static_assert(sizeof(SIG_SAVE_CLIENT_HACK_LOG) == 0x12, "SIG_SAVE_CLIENT_HACK_LOG");
#pragma pack(pop)

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
    bool m_field18;           // +0x18（bool 尾参；ORIG 0x086c1910 store 序 view先+movzbl
                              //   回读，探针实证 bool 字段+bool 形参直赋唯一匹配）
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_EVENTITEM) == 0x19, "SIG_SAVE_EVENTITEM ABI drift");

#endif  // DNF_GAME_SIG_TYPES_D_H_
