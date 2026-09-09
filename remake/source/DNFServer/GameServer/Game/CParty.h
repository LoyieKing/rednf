#ifndef GAME_CPARTY_H_
#define GAME_CPARTY_H_

// df_game_r CParty（G2 队伍聚合根）还原（2026-08-17）。
// 布局依据 ORIG 构造（0x082a5516）/析构（0x082a57fe）逐子对象推导：
//   +0x000 CItemRoutingData(0x4c)  +0x04c Mutex(0x18)
//   +0x078 cMember[4](0x18 each)   +0x0d8 cElection<int,4,4>(0x40)
//   +0x118 结果接收标志/标题区      +0x148 int[37]（快速队伍状态数组）
//   +0x1dc CPartyResultRecvFlag(0x34) +0x210 CTraceMobDieHack(0x94)
//   +0x2a4 secretshop::SECRET_SHOP_DATA(0x88) +0x32c BattleData(0x7f8)
//   +0xb24 CBattle_Field(0xcd0)    +0x17f4 GameResultSet(0x50)
//   +0x1844 std::map<int,int>(0x18)+0x185c 命中/硬币统计区
//   +0x1abc Secu_HackLogCheckByParty(0x14) +0x1ad0 CPartyTelePort(0x24)
//   +0x1af4 std::vector<MapInfo>(0xc)（总尺寸 0x1b00）
// 依赖子对象（CBattle_Field/BattleData/... 后续批次）以 char 数组占位，
// CParty.cpp 内以 asm-label extern 调用访问；主 agent 集成时统一替换。

#include <list>
#include <map>
#include <cstring>
#include <utility>
#include <vector>

#include "GameTypes.h"
#include "GameEnums.h"   // ENUM_DUNGEON_MODE / eCoinSubReason 权威定义（不依赖 CUser.h 传递）
#include "CBattle_Field.h"     // batch-16 内嵌（环已断：BF.h 对 CParty 仅前向）
#include "Inven_Item.h"
#include "BattleData.h"   // m_battleData 具名成员（ORIG DWARF CParty+0x32c）
#include "CUser.h"
#include "PvPTypes.h"   // cElection<int,4,4> 模板（m_election @+0x0d8，ORIG 0x40）
#include "CPartyResultRecvFlag.h"
#include "CTraceMobDieHack.h"
#include "CDungeonClearTracer.h"   // +0xc7c 内嵌追踪器（ORIG giveup_game 0x85b2baa 实证）

// ---- 本批次临时类型（主 agent 集成后迁移到权威头） ----
#ifndef DNF_ENUM_PARTY_INFO_TYPE_DEFINED
#define DNF_ENUM_PARTY_INFO_TYPE_DEFINED
enum ENUM_PARTY_INFO_TYPE { ENUM_PARTY_INFO_TYPE_0 = 0 };
#endif

enum ENUM_WALKOUT_TYPE
{
    ENUM_WALKOUT_TYPE_0 = 0
};

#ifndef DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
enum ENUM_DUNGEON_TYPE
{
    ENUM_DUNGEON_TYPE_0 = 0
};
#endif

enum ENUM_USER_DUNGEON_STATE
{
    ENUM_USER_DUNGEON_STATE_0 = 0
};

// ENUM_DUNGEON_MODE：权威定义在 GameEnums.h（经 CUser.h include）。

#ifndef DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
#define DNF_ENUM_ENUM_MAZE_QUEST_TYPE_DEFINED
enum ENUM_MAZE_QUEST_TYPE
{
    ENUM_MAZE_QUEST_TYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_TIMER_MESSAGE_DEFINED
#define DNF_ENUM_TIMER_MESSAGE_DEFINED
enum TIMER_MESSAGE
{
    TIMER_MESSAGE_0 = 0
};
#endif

enum eClearRewardCardType_t
{
    eClearRewardCardType_t_0 = 0
};

#ifndef DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
#define DNF_ENUM_ERIDABLEOBJECTSTATE_DEFINED
enum eRidableObjectState
{
    eRidableObjectState_0 = 0
};
#endif

namespace RetryFailReason
{
enum T
{
    T_0 = 0
};
}

namespace ManageBonusType
{
enum T
{
    T_0 = 0
};
}

// QuickParty 奖励域权威类型（BasicRewardType/RandomBuffType/RandomBuffDungeon、
// RewardData_Param/Result、IRewardType 族、QuickPartyData）统一由
// CQuickPartyRewardManager.h 提供（本头原占位枚举已并入）。
#include "CQuickPartyRewardManager.h"
#include "CTournamentDungeonReward.h"  // +0x17bc 值成员 m_tournamentReward 完整类型

// ---- 前向声明（权威定义属后续批次 / 已有头） ----
class CDungeon;
class CGameManager;
class Quest;
class CLuckPoint;
class CWorldMap;
class CTournamentDungeonReward;
class GameResultSet;
struct MSG_MONSTER_DIE;
struct MSG_MOVE_MAP;
struct MSG_PARTY_SET_INFO;
// ----------------------------------------------------------------------------
// STGainedEachExpData（ORIG 权威布局）：
//   ctor _ZN19STGainedEachExpDataC1Ev = W 0x08151a56：
//     movl $0x0,(%eax)          → +0x00 int    置 0
//     movl $0x3f800000,0x4(%eax)→ +0x04 float  置 1.0f
//   ORIG 无 D1/D2 符号（平凡析构），故不声明析构。nm 仅见 C1/C2。
// ----------------------------------------------------------------------------
struct STGainedEachExpData
{
    STGainedEachExpData();       // ORIG W 0x08151a56（C1/C2 同址）

    int m_exp;                   // +0x00 经验基数（ctor 置 0）
    float m_expPartyDivisor;     // +0x04 sumGainedEachExp 队伍加成除数（ctor 置 1.0f）
};                               // sizeof == 8
// ============================================================================
// stClearRewardExpParameter_t / stClearRewardBonusExp_t —— 权威布局
//   （ORIG ctor 0x0814a8b8 / 0x0814a800 反汇编 + CParty 报告
//    set_clear_reward_exp_parameter.md / getClearRewardBonusExp.md 逐字段核实）
// ============================================================================
struct stClearRewardExpParameter_t
{
    stClearRewardExpParameter_t();   // ORIG W 0x0814a8b8

    float m_deadPenaltyRate;         // +0x00 calDeadUserDungeonPenalty(m_deadMemberCount)
    float m_rankBonusWeight;         // +0x04 ServerParameterScript::GetExpBonusWeight(m_rankBonusIdx)
    float m_avatarBonus;             // +0x08 CParty::CalcAvatarBonus()
    float m_creatureBonus;           // +0x0c CParty::CalcCreatureBonus(user)
    float m_pcRoomBurningRate;       // +0x10 CPCRoomBurningEvent::GetExpFactor()/100
    float m_expDoubleRate;           // +0x14 CExpDoubleEvent::GetExpFactor()/100
    float m_channelBonusRate;        // +0x18 channel_script_t::getBonusRate(...)
    int m_burningFatigueBonusExp;    // +0x1c CBurningFatigueEvent::getBonusExpRate(usedFatigue)（原始千分率）
    float m_serverChannelRate;       // +0x20 getBonusRateEventServer/getBonusRate
    float m_premiumAdvantageRate;    // +0x24 CUserPremium::GetAdvantageExpRate()/100
    float m_serverEventExpRate;      // +0x28 CServerEvent::GetExpRate()/100
    float m_periodExpItemRate;       // +0x2c CUserCharacInfo::GetExpAffectRate()
    float m_globalEffectRate;        // +0x30 CGlobalEffectManager::getEffectValue(2)
    float m_pcRoomSameUserBonus;     // +0x34 同网吧奖励（写入方按原始 dword 写 CDataManager+0x7d4，读取按 float）
    float m_returnUserPartyBonus;    // +0x38 回归用户队伍 0.02f
    float m_manageBonus;             // +0x3c CalcurateManageBonus(0, user, a)
    float m_growthPowerExpRate;      // +0x40 get_growth_power_exp_reward 高字节/100
    float m_premiumAddExpRate;       // +0x44 getAddExpRateFromPremium(0x4f)/100
    float m_eventCharacExpRatio;     // +0x48 EventNewCharacterReward::eventExpRatio()
    int m_reserved4c;                // +0x4c ctor 清零（ORIG 未见使用）
    int m_reserved50;                // +0x50 ctor 清零（ORIG 未见使用）
    unsigned char m_reserved54;      // +0x54 ctor 逐字节清零（ORIG 未见使用）
    unsigned char m_reserved55;      // +0x55 ctor 逐字节清零（ORIG 未见使用）
    int m_deadMemberCount;           // +0x58 ctor 置 -1（死亡成员数输入）
    int m_rankBonusIdx;              // +0x5c ctor 置 -1（eRankBonusIndex 输入）
};                                   // sizeof == 0x60

struct stClearRewardBonusExp_t
{
    stClearRewardBonusExp_t();       // ORIG W 0x0814a800
    unsigned int m_avatarBonusExp;   // +0x00 ← m_avatarBonus（非零时下限 1）
    unsigned int m_rankWeightExp;    // +0x04 ← m_rankBonusWeight
    int m_bestType;                  // +0x08 最优加成类型（1/2/4；3 由外部预置）
    unsigned int m_selectedBonusExp; // +0x0c pcRoomBurning/expDouble/globalEffect 择优
    unsigned int m_burningFatigueExp;// +0x10 ← m_burningFatigueBonusExp*exp/100
    unsigned int m_channelBonusExp;  // +0x14 ← m_channelBonusRate
    unsigned int m_premiumAdvExp;    // +0x18 ← m_premiumAdvantageRate
    unsigned int m_serverEventExp;   // +0x1c ← m_serverEventExpRate
    unsigned int m_serverChannelExp; // +0x20 ← m_serverChannelRate
    unsigned int m_creatureBonusExp; // +0x24 ← m_creatureBonus（非零时下限 1）
    int m_upperMemberExp;            // +0x28 CUser::gainExpAsUpperMember(exp)
    unsigned int m_periodItemExp;    // +0x2c ← m_periodExpItemRate
    unsigned int m_pcRoomSameUserExp;// +0x30 ← m_pcRoomSameUserBonus
    unsigned int m_returnUserExp;    // +0x34 ← m_returnUserPartyBonus
    unsigned int m_eventCharacExp;   // +0x38 ← m_eventCharacExpRatio
    unsigned int m_manageExp;        // +0x3c ← m_manageBonus
    unsigned int m_growthPowerExp;   // +0x40 ← m_growthPowerExpRate
    unsigned int m_premiumAddExp;    // +0x44 ← m_premiumAddExpRate
};                                   // sizeof == 0x48

// 声明与 ORIG 一致：返回总加成经验（0x85ad278 返回 eax）。
struct RequiredItem;
struct map_monster;

namespace WongWork
{
struct stGenerateResult_t;
}
class CParty
{
public:
    class CItemRoutingData
    {
    public:
        CItemRoutingData();
        ~CItemRoutingData();

        int GetRoutingState();
        void SetRoutingState(char state);
        Inven_Item* GetRoutingInvenItem();
        unsigned int GetRoutingItemIndex();
        void SetRoutingItemIndex(Inven_Item& item, unsigned int idx);
        unsigned int GetRoutingItemUniqueID();
        char GetMemberRoutingState(int idx);
        void SetMemberRoutingState(int idx, char state);
        void Reset();

        int m_routingState;            // +0x00
        Inven_Item m_item;             // +0x04
        unsigned int m_itemUniqueID;   // +0x44
        char m_memberRoutingState[4];  // +0x48
    };

    // ---- 嵌套：队伍成员槽（0x18） ----
    class cMember
    {
    public:
        cMember();
        void Init();

        CUser* m_pUser;   // +0x00
        char m_field4;    // +0x04
        unsigned int m_field8;  // +0x08（create_party 0x0859b1d8 this 先载 =
                                // 与 CUser::GetUID() 同 unsigned 直存，CInvC
                                // signedness 行序探针实证）
        char m_fieldc;    // +0x0c
        char m_fieldd;    // +0x0d
        char m_fielde[4]; // +0x0e
        unsigned char m_field12;   // +0x12（get_teleport_member_state ORIG movzbl 零扩展实证）
        int m_field14;    // +0x14
    };

    CParty();
    ~CParty();

    // ---- 基础访问器 ----
    void SetIDX(int idx);
    int GetPartyIndex();
    void set_state(char s);
    char get_state();
    bool IsSinglePlay();
    void SetSinglePlay(bool flag);
    char GetEPLPState();
    void SetEPLPState(char state);
    char GetSelectedEPLPCmd();
    void SetSelectedEPLPCmd(char cmd);
    char GetAssaultState();
    void SetAssaultState(char state);
    cMember* GetMember();
    CUser* get_user(int idx);
    char getTitleIndex();
    void setTitleIndex(unsigned char idx);
    char* getTitle();
    void setTitle(const char* title);
    bool IsAutoCreated();
    void SetAutoCreated(bool flag);
    unsigned char getUserMax();
    void setUserMax(unsigned char max);
    unsigned short getDungIndex() const;
    void setDungIndex(unsigned short idx);
    unsigned char getDungDiffi() const;
    void setDungDiffi(unsigned char diffi);

    // ---- 结果接收标志区（+0x1dc） ----
    char get_dungeon_clear_state();
    void set_dungeon_clear_state(char state);
    void reset_recv_flag();
    void SetRecvResultFlag(bool flag);

    // ---- CTraceMobDieHack 区（+0x210） ----
    unsigned short getStraightVictories();
    void setStraightVictories(unsigned short v);
    void incStraightVictories();
    void resetStraightVictories();
    int getStartGamePartyCount();
    void setStartGamePartyCount(int v);

    // ---- SECRET_SHOP_DATA 区（+0x2a4） ----
    bool IsFirstMapClear();
    void SetFirstMapClear(bool flag);
    bool IsPremiumGoldCardParty();
    void SetPremiumGoldCardParty();
    void ResetPremiumGoldCardParty();
    unsigned int GetPremiumGoldCardDefaultItem();
    void SetPremiumGoldCardDefaultItem(unsigned int v);
    void ResetPremiumGoldCardDefaultItem();
    void* GetSecretShopData();

    // ---- CBattle_Field 转发 ----
    bool checkBossRoom();   // ORIG W 0x0822d8f6（al=check_end_point 返回）
    bool checkStartRoom();   // ORIG W 0x0826525e：转发 CBattle_Field::check_start_point
                             // （FinishLoad::process 按 bool 测试，原 void 声明与调用方不符）
    int DeleteDungeonDropItem(int idx);   // ORIG W 0x0860b62a（返回 pickup_item 结果）

    // ---- 传送 / 快速队伍区（+0x1ad0） ----
    int get_quick_party_index();
    void set_quick_party_index(int idx);
    bool is_quick_party();
    void set_quick_party(bool flag);
    void set_gm_random_buff_type(QuickParty::RandomBuffType type);
    int get_random_buff_type();
    bool IsWeekendEvent();
    void SetWeekendEvent(bool flag);
    bool getDungeonMapSaving();
    void setDungeonMapSaving(bool flag);
    void* GetPartyTelePort();
    void* getPassedMapInfoList();

    // ---- 硬币 / 掉落率区 ----
    int GetUsedCoinCount();
    void SetUsedCoinCount(int v);
    void IncreaseUsedCoinCount();
    int Get_party_overlapped_drop_ratio_rare() const;
    int Get_party_overlapped_drop_ratio_unique() const;
    void Reset_party_overlapped_drop_ratio();
    void ClearMapHitCount();
    // ---- 物品路由 ----
    bool IsRoutingState();
    void SetStartRouting();
    void SetEndRouting();
    void SetRoutingItem(Inven_Item& item, unsigned int idx);
    int GetRoutingItemID();
    Inven_Item* GetRoutingInvenItem();
    unsigned int GetRoutingItemUniqueID();
    char GetMemberRoutingState(CUser* user);
    void SetMemberRoutingState(CUser* user, char state);
    void SetRoutingTimerOver();

    // ---- 生命周期 ----
    void init();
    void destroy();
    void init_quick_party_data();
    CUser* getManager();

    // ---- 成员管理 ----
    int create_party(CUser* user);   // ORIG 返回 eax=1（0x859b1be）
    int join_user(CUser* user);   // ORIG 0x0859b2b6：-1 空指针/满员，-2 state==2，成功 1
    int leave_user(CUser* user, ENUM_PARTY_INFO_TYPE type);
    int walkout_member(CUser* user, int param);   // ORIG 0x859ca4a：8=非队长 4=槽位空 1=leave_user==1 0=成功
    int change_manager(CUser* user, int slot);   // ORIG T 0x0859c978：0 成功/8 非本人/18 已是/21 参数槽错
    int change_manager();
    void battle_data_reset();     // 0x859aec2
    int set_host();
    int set_single_play(CUser* user);   // ORIG 返回 eax=1（0x859b142）
    void game_start(CUser* user);
    bool IsExistUser(CUser* user);
    int isMyMember(unsigned int userNo);
    bool _checkValidUser(int idx);
    bool checkValidUser(int idx);   // ORIG W 0x08145868（转发 _checkValidUser；定义 CParty.cpp:1902）
    bool CheckQuestConnection(int questIdx, ENUM_MAZE_QUEST_TYPE type);   // ORIG T 0x085b6478（定义 PartyClosureC2.cpp）
    void enumPartyMember(bool (*fn)(CUser*));   // ORIG W 0x0814253c（成员遍历回调；定义 PartyClosureC2.cpp）
    int change_manager_for_quick_party();   // ORIG 0x859b404：清 manager 槽位后
                                            //   改选首个有效成员为 host/manager
    int delete_party_for_quick_party(CUser* user,
                                     std::list<CUser*>& list);   // ORIG 0x859b52c
    int get_member_count();
    char* getMemberNames(char* buf);   // ORIG W 0x08145882（转发 _getMemberNames）
    int get_live_count();
    int get_live_count_enter_map();
    int get_party_seatno(CUser* user);
    CUser* GetMemberSlotNo(unsigned int characNo);  // ORIG T 0x0859acc2：按角色编号返回 CUser*
    unsigned int GetMemberSlotNo(CUser const* user) const;
    void ClearBadP2PUser();
    int CheckMemberArea();   // ORIG eax：全员同区=1（0x859d706）
    int GetUserPosition(CUser* user);
    bool check_allmember_die();
    void check_allmember_loading(bool flag);
    bool IsExistCharacter(int characNo);
    bool IsExistInvisible();
    bool IsThereGhostInParty();
    int FindUserByCharId(unsigned int characNo);
    bool hasPCRoomPlayer();
    bool isSamePCRoomParty();
    bool existSamePCRoomUser(unsigned int pcbangNo);
    bool isGuildParty();
    bool IsEventCharacParty();
    bool IsReturnUserParty();
    void SetPartyInfoUI(MSG_PARTY_SET_INFO const& info);

    // ---- 队伍状态 / 移动 ----
    void changePartyMemberInfo(MSG_MOVE_MAP& info, char flag);
    bool canFatigueUp(unsigned char flag);   // ORIG T 0x085a8c88（changePartyMemberInfo 消费）
    void changePartyMemberInfoOnMoveMap(MSG_MOVE_MAP& info, bool flag);
    void changeBattleDataOnMoveMap(bool flag);   // ORIG T 0x085a8ee0（move_map 调用）
    bool move_map(MSG_MOVE_MAP& info);   // ORIG T 0x085a8f4e 返回 eax=0/1
                                         // （原 void 声明与 ORIG 返回不符）
    int switchPostion(char a, char b);   // ORIG T 0x085ba680 返回 1
    void useItem(CUser* user, Inven_Item const& item);
    void use_skill(CUser* user, int skillIdx, int param);
    bool isCheckUseSkill(int skillIdx, int param);
    bool OnUsedSkill(CUser* user, int skillIdx, int param);   // ORIG 0x085b5f8e 尾 mov $0x1,%eax;ret ⇒ 返回 bool（非 void）
    void die_user(CUser* user);
    void set_charac_live(CUser* user, ENUM_USER_DUNGEON_STATE state);
    void finish_loading(CUser* user);
    bool OnStartMapFinishLoading(CUser* user);   // ORIG 返回 al（finish_loading 测试，0x85b170a）
    void map_load_forced_finish();
    void send_loadingcheck_msg();
    void resetFinishLoadingVar();
    bool CanDoPartyActionAssalutState();
    bool TimeoutCompleteAfterAssault();   // b25c Lane21：ORIG 尾 mov$1 常量岛（0x859a34e+0x12c），调用侧 test%al
    void SelectEPLPCommand(int cmd);   // ORIG 0x85a22a8：SetEPLPState(3)+
                                       // InsertTimer(1,idx,0x14,cmd,key,0)
    bool check_timer_key(TIMER_MESSAGE msg, int param);
    int gen_timer_key(TIMER_MESSAGE msg);
    bool checkMoveMapHackCnt(MSG_MOVE_MAP& info);
    void setMonsterMoveHack();
    void CheckHackAverageHitCount();

    // ---- 连接管理 ----
    void ConnectP2P(CUser* user, char* ip, int port);
    int _ClearConnectP2PAbsolute(int slot);   // P9: ORIG 0x0859a664 尾 mov$1 实返 1
    int _ClearConnectP2PRelative(int slot);
    bool _IsCannotConnectP2P(int a, int b);
    int _GetConnectP2PPoint(int slot);
    void _GetConnectP2PMaxMinPoint(int& max, int& min);
    void send_party_ipinfo();

    // ---- 发包 ----
    void send_to_party(PacketGuard& packet);
    void send_to_alive_member(PacketGuard& packet);
    void send_to_party_cmd_error(ENUM_CMDPACKET cmd, unsigned char error);
    void send_host_info();
    bool get_party_realtime_info(PacketGuard& packet);  // ORIG 调用点 test %al 直判（bool 透传）
    void send_party_realtime_info();   // 0x859cd24

    void send_invite_pvp(int mapIdx);
    void send_invite_warroom(int mapIdx);
    void send_invite_bluemarble(int mapIdx);
    void send_quick_party_matching_result(bool flag, CUser* user);
    int SendWalkout(int param, ENUM_WALKOUT_TYPE type);   // ORIG 返回 eax=1
    bool SendDungeonCharacInfo(CUser* user, bool flag);   // ORIG T 0x085b5098 尾部 mov $0x1,%eax
    bool SendDungeonStatisticParty(bool flag, CUser* user);        // ORIG 尾部 mov $0x1,%eax
    bool SendDungeonStatisticPartyJob(bool flag, CUser* user);     // ORIG T 0x085b52a6 同上
    bool SendDungeonStatisticPartyCharac(bool flag, CUser* user);  // ORIG T 0x085b53aa 同上
    void SendPartyMemberTagChracInfo();
    void SendSecretShopItemList(CUser* user);
    void sendPremiumGoldCardStatistic(int a, int b, int c);
    void send_hellParty_value();
    void send_dungeon_fail_msg();
    void sendInoutConditionDungeon(RetryFailReason::T reason);
    void makeBestClearTimePacket(PacketGuard& packet, int idx);
    bool make_blood_dungeon_info(CUser* user, PacketGuard& packet);
    void make_blood_dungeon_info_blank(PacketGuard& packet);   // ORIG T 0x0859e480
                                                               // 无返回值（原 bool
                                                               // 声明与 ORIG 不符）
    void setBestClearTime(int a, int b, int c);

    // ---- 副本流程 ----
    // ORIG 返回 int（>0 为副本 idx；DisPatcher_SelectDungeon::process 消费，
    // asm mov %eax 写 param）。返回类型不参与 mangled，当前无定义（后续批次补）。
    int dungeon_start(int dungeonIdx, char type, ENUM_DUNGEON_TYPE dtype);
    int do_after_dungeon_start(CDungeon const* dungeon, CUser* user);   // ORIG 返回 eax
                                                                        //（OnStartMapFinishLoading test）
    int check_dungeon_start(CDungeon const* dungeon);
    int check_dungeon_start(CDungeon const* dungeon, int& param);   // ORIG 错误码（0x859f7fa）
    bool CheckEnterEventDungeon(CDungeon const* dungeon, unsigned char& flag,
                                Inven_Item* item, int* param);
    bool CheckEnterAncientDungeon(CDungeon const* dungeon, unsigned char& flag,
                                  Inven_Item* item, int* param);
    // ORIG _ZN6CParty24CheckEnterAncientDungeonEPK8CDungeon 单参版（0x859e7fe，
    //   别名报告 CheckEnterAncientDungeon_0859e7fe.md）：dungeon 未组队入场券
    //   检查，已组队时转调上方 4 参事件版。
    int CheckEnterAncientDungeon(CDungeon const* dungeon);
    int CheckEnterDimensionDungeon(CDungeon const* dungeon);
    int CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                   char mode);          // ORIG 返回错误码 0xab/1（0x0859f36a）
    int CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                   bool flag2);         // ORIG 返回错误码 0xab~0xad/1（0x0859f1ce）
    int CheckEnterVillageAttackRevenge(CDungeon const* dungeon);
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7d4b 尾 `movzbl 0x1865(%eax),%ebx;
    // ...; mov %ebx,%eax; ret`（无 setne 归一化，P9）⇒ 返回类型 int 非 bool；
    // mangled 不变，调用点 CParty.cpp:3197 `if (!call())` 语义不变。
    unsigned char CheckHaveHellPartyPassItem(CDungeon const* dungeon, ENUM_DUNGEON_TYPE type);
    bool CheckHellDungeonFreepassItemHaveAndDel(CUser* user, bool flag);
    // b28d-solo-CPartyOnlyOrig：ORIG checkLimitUsedCoinCondition 0x085be6xx 调用点
    // `call; xor $0x1,%eax; test %al,%al; je`（P292/P168a）⇒ bool 返回。
    bool CheckHellCoinLimint(int param);  // ORIG eax=0/1（checkLimitUsedCoinCondition 消费）
    bool checkDugeonInoutTime(int param);  // ORIG 尾部 isDungeonOpen 直落透传（无 movzbl）
    int checkInoutConditionDungeon(CDungeon const* dungeon, ENUM_DUNGEON_MODE mode,
                                   ENUM_CMDPACKET cmd);   // ORIG 返回错误码（0x85abc80）
    int checkInoutConditionDungeon();
    bool checkDungeonDifficulty(char difficulty, CDungeon const* dungeon);
    bool checkFreeRevivalCondition(CDungeon const* dungeon);
    int checkLimitUsedCoinCondition(CDungeon const* dungeon);   // ORIG 返回错误码 0x13/0x16/0
    bool checkUnLimitUsedCoinCondition(CDungeon const* dungeon);
    int getStandardDimensionLevel();
    bool setStandardDimensionLevel(CDungeon const* dungeon);   // ORIG 恒返回 al=1
    bool UseAncientDungeonItems(CDungeon const* dungeon, Inven_Item* item, int* param);
    bool UseSealDoorItems(std::vector<int>& itemList);   // ORIG 返回 al（0x85a8038）
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 调用点 `call; test %al,%al;
    // je`（2 条，P292 bool 形）⇒ 返回类型 bool 非 int；mangled 不变。
    // 定义在 CParty.cpp:1582（他人 TU）需同步改 bool。
    bool CheckDestroyConditionSealDoor(CDungeon const* dungeon,
                                       std::vector<int>& list1,
                                       std::vector<int>& list2);
    void CheckClearQuestHellParty(std::vector<int>& list);
    int do_after_dungeon_start_checkndelete_item(RequiredItem const& item,
                                                 CUser* user);   // ORIG T 0x085a0756 返回 eax

    // ---- 副本结算 ----
    void dungeon_clear();
    void prepare_dungeon_clear();
    void clear_reward(int param);
    float calDeadUserDungeonPenalty(int slot);   // ORIG T 0x085ad9b4（死亡成员经验惩罚率；
                                                 //   clear_reward/set_clear_reward_exp_parameter 消费）
    // ORIG T 0x085bddfc：维度/绝望塔副本次数记账（DungeonPermission diff==2 消费）；
    //   命中成员记录槽且次数未达 dm 上限时 CUser::AddDungeonClear(dungeonIdx,3)。
    bool addDungeonClear(const CDungeon* dungeon, int param, int slot);
    void DungeonPermission(int param);           // ORIG T 0x085b12f8（clear_reward 尾部调用）
    int OnDungeonClear(CUser* user, bool success);   // b28d-solo-PartyClosureC2：ORIG 0x085b5518 尾 `mov $0x1,%eax` ⇒ int 返回 1（P159）
    void OnFailedDungeonClear(CUser* user);
    void OnSuccessedDungeonClear(CUser* user);
    void RestartDungeon();
    void ReturnToVillage();
    void ClearDungeon();
    void giveup_game(CUser* user, bool a, bool b, bool c);
    void party_dungeon_fail();
    int set_play_result(CUser* user, GameResultSet& result);   // ORIG 返回 eax=0（0x85b2068）
    void forced_set_play_result();
    bool SetPlayResult(CUser* user);   // ORIG 0x85b2412 返回 0/1；[b28d-Main·检查点14 P292] 调用点 xor $1 ⇒ bool
    void set_clear_reward_exp_parameter(CUser* user, unsigned int a,
                                       stClearRewardExpParameter_t& param);
    unsigned int getClearRewardBonusExp(CUser* user, unsigned int& a,
                                stClearRewardExpParameter_t const& param,
                                stClearRewardBonusExp_t& bonus);
    void set_basic_reward(QuickParty::BasicRewardType type,
                          QuickParty::RewardData_Param param,
                          QuickParty::RewardData_Result& result);
    void set_random_reward(QuickParty::RandomBuffType type,
                           QuickParty::RewardData_Param param,
                           QuickParty::RewardData_Result& result);
    int choose_random_buff(QuickParty::RandomBuffDungeon dungeon);   // ORIG 返回选中类型/11
    void increase_dungeon_coin_count(int param);
    int GetPartyMemberCoinLimit();
    void SetPartyMemberCoinLimit(int limit);
    void resetMemberUseCoin();
    void increase_ancient_dungeon_ticket();
    void increase_dimension_inout_count(int param);
    int get_blood_dungeon_admission_fee(CUser* user);

    // ---- 怪物击杀 / 血之副本 ----
    bool CheckMapClear();                   // ORIG T 0x085bea4a：转发 CBattle_Field::check_grid_clear
    bool IsUseFatigueDungeon();             // ORIG T 0x085be9c8（定义见 UserCoreDepsB2.cpp）
    void OnKilledMonster(CUser* user, int monsterIdx, map_monster monster,
                         MSG_MONSTER_DIE const& dieInfo);
    void MemberRegisterKillMonster(map_monster& monster, unsigned char flag);
    int OnFightVillageMonster(CUser* user, int monsterIdx);   // ORIG 返回 0/1/0x15（0x85b9596）
    int OnFinishVillageMonsterFighting(CUser* user, bool flag);   // ORIG 返回 0/3（0x85b99ac）
    int kill_monster(CUser* user, int monsterIdx, unsigned short mapIdx,
                     MSG_MONSTER_DIE const& dieInfo, int* param);  // ORIG 返回 0/1
    void kill_blood_monster(CUser* user, int monsterIdx, unsigned short* param);
    bool BloodHandleDieUser(CUser* user);   // b28d-53 P159：ORIG 尾 mov$1/错误路径 mov$0 ⇒ bool
    void beginBloodClearProcess(bool flag);
    void onBloodClear(bool flag, int param);
    bool onBloodRewardTimer(TIMER_MESSAGE msg);  // ORIG 返回 0/1（0x85b6a48；caller 0x08638d17 xor$1 取反形⇒bool）
    void finishBloodDungeon();
    void generateBloodRewardExp(int a, int b, unsigned int c, short d);
    void generateBloodRewardItem(int a, int b, unsigned int c, unsigned int d,
                                 bool flag);
    void BloodClearRewardPacket(int a, int b, int c, Inven_Item const* items1,
                                int d, Inven_Item const* items2, int e,
                                Inven_Item const* items3, int f,
                                Inven_Item const* items4);
    void historyBloodRoundClear(int a, unsigned int b, unsigned int c,
                                char const* str);
    void checkBloodRenewMyRecord(int a, int b, int c, int d);

    // ---- 卡片选择 ----
    void SendEachCardInfo();
    void CompleteCardSelect();
    void SendCardSelectComplete();
    bool IsSelectCard(char a, eClearRewardCardType_t type);
    bool HaveAlreadySelectedCard(char a, eClearRewardCardType_t type);
    bool IsCompleteCardSelectOfMembers(eClearRewardCardType_t type);
    bool HasAllMemberSelection();
    bool SetCardNumber(CUser* user, char a, eClearRewardCardType_t type);   // ORIG 返回 al（0x85b415a）
    bool _checkGoldCardSelectable(CUser* user, eClearRewardCardType_t type,
                                  int param);
    void insert_time_card_select_msg(int param);
    void insert_time_card_select_right_msg(int param);
    void insert_time_clear_dungeon_reward_msg(int param);
    void insert_time_dungeon_clear_msg(int param);
    void insert_time_dungeon_score_scroll_msg(int param);

    // ---- 锦标赛 / TOD ----
    bool OnTODRewardTimer(TIMER_MESSAGE msg);   // ORIG 0x85ba906；调用点 0x0863a901 xor $0x1 ⇒ bool（GData 同步定义体）
    int OnTimeoutBossDie(int a, unsigned int b);   // ORIG 0x85aa5d4 返回 0/1
    int _Send_TOD_APCInfo(int a);   // ORIG 恒返回 eax=1（0x85bac26）
    int _Send_TOD_ClearReward();   // ORIG 恒返回 eax=1（0x85bac00）
    int GetDungeonIndex_isTOD(int& a, int& b);   // ORIG 返回错误码（0x85bacc0）
    void InsertTournamentDungeonTimer(TIMER_MESSAGE msg);
    bool isTournamentVictory();
    void TournamentDungeonClear(bool flag);   // ORIG 0x85bd236 无返回值
    void MakeTournamentReward(CTournamentDungeonReward& reward);
    char GetTournamentDungeonClearState();
    void SetTournamentDungeonClearState(char state);

    // ---- 技能 / 任务 ----
    void SetProperDungeonEvent(CUser* user, int param);
    void SetProperLevelDungeon(CUser* user);
    bool SetRidableObjectState(int state, eRidableObjectState type);   // ORIG 0x085b9f28（纯转发 call;leave;ret = bool 链）
    unsigned char DestroyPassiveObject(int param);   // ORIG 返回 al（0x85b6540）
    void check_passive_skill();
    bool HasNoQuestGoWithApcInParty(Quest const* quest);
    bool IsClearedQuest(int questIdx);
    void MakeQuestRewardPacket(PacketGuard* packet, CUser* user);
    bool CheckRecvFromAllMember(CUser* user);   // ORIG al（0x85b401b；Dispatchers 等待此 bool）
    void set_recv_party_packet_flag(CUser* user);
    bool recv_party_packet_all();   // ORIG al：全员已回包=1（0x85b3fd7）

    // ---- 收益 / 奖励 ----
    void* GetUserSecretShop(CUser* user);   // ORIG 0x85b9e80：返回逐成员 secret 数据视图
    float CalcurateManageBonus(ManageBonusType::T type, CUser* user,
                               unsigned int param);   // ORIG 返回 st0 浮点
    float CalcAvatarBonus();
    int get_total_level();   // ORIG T 0x0859a1ae：有效成员等级合计
    float CalcCreatureBonus(CUser* user);
    float getAvatarBonus();
    float getCreatureBonus(CUser* user);
    void GetLuckPoint(CLuckPoint* luckPoint, bool flag);
    int GetExpPenaltyPercent(int a, int b) const;
    int MemberPenaltyRate();
    int getMemberMaxLevel();
    int getMemberMinLevel();
    int getMemberMaxLevelBeforeDungeon();
    int getMemberLevelGap();
    int _getMemberLevelGap();
    int get_rate_level_difference();
    unsigned int sumGainedEachExp(CUser* user, STGainedEachExpData const& data);
    // ORIG _ZN6CParty18getMonsterTotalExpEjciffb（0x085a23dc，返回 eax=低 32 位）
    unsigned int getMonsterTotalExp(unsigned int monsterIdx, char flag, int a, float b,
                                    float c, bool d);
    // ---- 物品 ----
    int get_item(CUser* user, int itemIdx, bool flag, char c, int& param,
                 unsigned char& flag2, bool& flag3);   // ORIG 返回 0/1
    void _onGetItem(CUser* user, unsigned int itemIdx, unsigned int count);
    int drop_item(CUser* user, int itemIdx, int count, char a, int b, int c);
    void drop_item_gm(CUser* user, unsigned int itemIdx);
    bool isMonsterDrop_item(int monsterIdx);
    void _makeRewardItemInfo(CUser const* user, Inven_Item* item,
                             PacketGuard& packet);
    int _putItemIntoUser(CUser* user, Inven_Item* item, PacketGuard& packet);   // ORIG 返回放入数（0x85aab84）
    void increase_result_card_gold(WongWork::stGenerateResult_t& result);
    void increase_hell_party_ratio(char* str);
    void cal_hellparty_value(int a, int b, int c);
    void sub_hellparty_value();

    // ---- 其它 ----
    int ReviveUser(CUser* user, unsigned short a);   // ORIG 返回错误码（0x85b27c8）
    int VoteLackUser(CUser* user, int param);   // ORIG 0x85b3c90 返回 0/1
    bool OnTimerWalkOutLackUser();   // ORIG 0x85b3e12；调用点 xor $0x1,%eax 实证 bool（GData 同步定义体）
    void SaveCoin(eCoinSubReason reason);
    int GetCoinCount(CUser* user);
    // ORIG T 0x085bdd2a 返回 int（0 普通/1 脚本 boss/2 维度/3 血轮普通/
    //   4 血轮疯狂/5 绝望塔/6 锦标赛/8 空指针；addDungeonClear 消费），
    //   原 void 声明与返回值不符（LinkKillChain 批次修正）。
    int getDungeonTypeKind(CDungeon const* dungeon);
    unsigned int GetMapPlayingTime();   // ORIG T 0x85b6768（CheckOneMapPlayTimeHackCnt
                                        //  以返回值/1000 为基准，原 void 声明与调用方不符）
    bool getDungeonDropTestFlag();   // ORIG T 0x085bdc76 返回 al（GM 成员且
                                     // getDungeonMapSaving 时 1；原 void 声明与
                                     // move_map 调用方 test al 不符）
    // [b28c-L7][SIGFIX 候选] int→bool：①两函数 ORIG 体尾仅发 `mov $0x0/$0x1,%eax`
    // 两值出口（docs/class_func_reports/CParty/is_limit_minimum_age.md @0x0859a269
    // /0x0859a27f）；②调用点按**单字节**直测/取反：0x0859a25d 内层
    // `call CheckLimitMiniumAge; xor $0x1,%eax; test %al,%al; je`（Ghidra C 亦
    // `cVar1 != '\x01'`），本仓外部调用点 0x082f0xxx（AvailableAssault）同形；
    // ③探针 /tmp/xprobe.cpp 实证 int callee 的调用点写法无解（恒 sete/栈化），
    // 唯 bool callee + 裸 `if(!call())` 直产 ORIG `xor $1,%eax; test %al,%al`。
    // mangled 不含返回类型⇒符号不变；全仓消费点 grep 实证均为条件/取反调用
    // （无 `>=0`/`<0` 有符号判定，避 C28L12 m_arg 回退教训）。
    bool is_limit_minimum_age(int age, int& param);     // ORIG 0x0859a210 出口 0/1
    bool is_limit_minimum_level(int level, int& param); // ORIG 0x0859a286，param=违规成员uid
    bool isPremiumGoldCardReward(CDungeon const* dungeon, unsigned int& param);
    int getPartyMemberAvgLevel();   // ORIG T 0x0859f582 返回 eax（有效成员平均等级）
    int CheckMemberFatigue();
    int CheckMemberLastTryAssaultTime();   // ORIG 0x085bcab6：0/1
    char GetFirstValidMemberState();   // ORIG 返回 al（ProcessGiveUp 比较 5/0xc）
    void GetValidPartyMember(std::vector<int>& list);
    void GetRoutingDiceMemberList(std::vector<std::pair<int, int> >& list,
                                  CItem* item);
    int GetPartyType();
    const char* GetNoSendSetPlayResultUser(int param);   // ORIG T 0x85b2384 返回昵称/空串
    void IncreaseSetPlayResultCount(CUser* user);
    int GetSetPlayResultCount();
    int GetPartyMemberForceDropFlag();
    bool isDungeonDifficultyInfromNotice(bool flag);   // ORIG 返回 al（0x85be8a0）
    int getMemberPassParty(CUser* user);   // ORIG eax（0x85b900c）
    bool isHelpAbuseParty(CUser* user);
    bool isHelpAbuseParty();

public:
    // ORIG 无访问控制；DisPatcherCore 等跨 TU 按 +0xcac(m_dungeon) 等偏移语义
    // 访问，故数据区整体置于 public（Main 协调指令 2026-08-24）。
    char* _getMemberNames(char* buf);   // ORIG 0x85b4bde

    CItemRoutingData m_routingData;   // +0x000
    Mutex m_mutex;                    // +0x04c

    int m_partyIndex;        // +0x064
    char m_field68;          // +0x068
    bool m_singlePlay;       // +0x069
    char m_state;            // +0x06a
    char m_eplpState;        // +0x06b
    char m_selectedEPLPCmd;  // +0x06c
    char m_assaultState;     // +0x06d
    char m_pad6e[2];         // +0x06e
    CUser* m_host;           // +0x070
    CUser* m_manager;        // +0x074

    cMember m_member[4];     // +0x078

    cElection<int, 4, 4> m_election;    // +0x0d8（ORIG 布局 0x40；ctor 置零票；HdrFieldF）
    bool m_recvResultFlag;              // +0x118
    char m_pad119;                      // +0x119
    bool m_tournamentVictory;           // +0x11a
    char m_titleIndex;                  // +0x11b
    char m_title[0x20];                 // +0x11c
    bool m_autoCreated;                 // +0x13c
    unsigned char m_userMax;            // +0x13d
    unsigned short m_dungIndex;         // +0x13e
    unsigned char m_dungDiffi;          // +0x140
    char m_pad141[3];                   // +0x141
    int m_routingDice;                  // +0x144（get_item 路由骰子：game_start 写
                                        //   get_rand_int(成员数) @game_start.md:750；
                                        //   get_item 递增回写 @get_item.md:3235/3261）
    unsigned int m_field148[37];        // +0x148（b28d-49：check_timer_key 0x85be3xx
                                        //   ORIG `mov tbl,%edx; mov param,%eax; cmp %eax,%edx`
                                        //   = 元素 unsigned + 形参 int；sizeof/偏移不变）
    // ---- CPartyResultRecvFlag 区（+0x1dc，0x34 字节） ----
    // b28d-solo-CPartyOnlyOrig 备注：曾试「匿名 union + 具名 m_recvFlag」以复现
    // ORIG receiver `lea 0x1dc(%eax),%edx`，但 GCC 4.4 拒绝 union 含带构造函数
    // 成员（CPartyResultRecvFlag 有用户 C1）⇒ 回退；残余 lea/add 归 N107。
    unsigned char m_pad1dc[0x4];     // +0x1dc
    char m_dungeonClearState;        // +0x1e0
    int m_field1e4[2][4];            // +0x1e4 (compiler aligns to 4)
    char m_field204[2][4];           // +0x204
    char m_tournamentDungeonClearState;  // +0x20c
    unsigned char m_pad20d[0x3];     // +0x20d
    // ---- CTraceMobDieHack 区（+0x210，0x94 字节） ----
    unsigned char m_pad210[0x88];    // +0x210
    int m_memberLevelGap;            // +0x298
    unsigned short m_straightVictories;  // +0x29c
    unsigned char m_pad29e[2];       // +0x29e
    int m_startGamePartyCount;       // +0x2a0
    // +0x2a4 SECRET_SHOP_DATA(0x88) 区域按 POD 视图拆分：ORIG GetUserSecretShop
    // 0x085b9eaa 的 `lea 0x2a0(%edx),%eax; add this; add $8` 只在「this 直接
    // 含具名数组成员」形下由 4.4 复现（cast 取址形恒产 `mov this,%ecx` 中转，
    // 探针 /tmp/b28d_s187/p14/p17/p18）。SECRET_SHOP_DATA.m_info 在 +0x04
    // （SECRET_SHOP_INFO 0x1c×4 = 0x70）⇒ 数组落 +0x2a8..+0x317。
    // sizeof/全部偏移/mangled 不变；`&m_padShop` 仍 = this+0x2a4（CParty.cpp
    // placement-new/cast 消费点语义不变）。
    unsigned char m_padShop[0x4];    // +0x2a4（SECRET_SHOP_DATA 头 m_openCode）
    char m_secretShopInfo[4][0x1c];  // +0x2a8（SECRET_SHOP_INFO POD 镜像）
    unsigned char m_padShopTail[0x4];// +0x318
    bool m_premiumGoldCardParty;     // +0x31c
    unsigned char m_pad31d[3];       // +0x31d
    unsigned int m_premiumGoldCardDefaultItem;  // +0x320
    bool m_firstMapClear;            // +0x324
    unsigned char m_pad325[3];       // +0x325
    int m_partyMemberCoinLimit;      // +0x328
    // ORIG DWARF：CParty::m_battleData @+0x32c，sizeof(BattleData)=0x7f8
    // → 0x32c+0x7f8=0xb24（m_padBattleField1 起点，零空洞；两侧偏移证据：
    // CPartyC1 0x085b6d?? placement 构造 +0x32c 与 dtor 0x085b?? 析构调用；
    // 旧 CParty 侧 0x37c..0x38f 五 char[4] 与 0xb14 int[4] 实为 BattleData
    // 自身字段 m_arr50/m_arr54/m_arr58/m_arr5c/m_arr60/m_arr7e8 的双名）。
    BattleData m_battleData;               // +0x32c..0xb23
    // batch-16：BF 内嵌段具名化（B2/B5/0x1c9 会签终案）。
    //   CParty+0xb24 = CBattle_Field 内嵌对象（ORIG CPartyC1/D1 对 0xb24 有构造/
    //   析构调用实证；sizeof 0xcd0 → 0xb24+0xcd0=0x17f4=m_padResult 零空洞）。
    //   CParty 层旧直名字段全部折入：m_dungeon→m_battleField.m_pDungeon；
    //   m_partyType→m_battleField.m_bloodState（B2 双域票：单槽唯一名，CParty 域
    //   语义=partyType 以消费点注释标注）；m_fieldCC4→m_dungeonDiff；
    //   m_fieldCD4→m_hellPartyMode；m_standardDimensionLevel→同名归一(B5)；
    //   m_sealDoorItemsOk→+0x1c9 同名落 BF.h；tracer/padc94/padcb0/padcc5/padcdc/
    //   padcee/padCBattleField2 由 BF.h 既有/占位成员接管。
    CBattle_Field m_battleField;             // +0xb24..0x17f3
    // [b28d-solo-CParty] P235 匿名 union 双视角：ORIG onBloodClear 0x085b7cd8
    // `movzwl 0x183e(%eax),%eax` 为单条 disp（=GameResultSet+0x4a）⇒ 需要
    // +0x183e 处的具名 unsigned short 视角。m_padResult 原样保留（C1/D1 零影响），
    // 仅增别名视角；sizeof/偏移/mangled 不变。
    union {
        unsigned char m_padResult[0x50];    // +0x17f4 GameResultSet
        struct {
            // [b28d-solo-CPartyDepsD2] P235/P311 具名标量视角（ORIG SetPlayResult
            // 0x085b24be 单条 movzbl 0x17fd / 0x085b251b mov %ax,0x183c /
            // 0x085b2525 mov %edx,0x1800 / 0x085b2539 mov %dl,0x1804 实证）。
            unsigned char m_resultPad[9];           // +0x17f4..+0x17fc
            unsigned char m_resultField9;           // +0x17fd GameResultType+0x09
            unsigned char m_resultPadA[2];          // +0x17fe..+0x17ff
            int m_resultFieldC;                     // +0x1800 GameResultType+0x0c
            unsigned char m_resultField10;          // +0x1804 GameResultType+0x10
            unsigned char m_resultPad11[0x37];      // +0x1805..+0x183b
            unsigned short m_resultField48;         // +0x183c GameResultType+0x48
            unsigned short m_resultField4a;         // +0x183e GameResultType 尾字段
            unsigned char m_resultPad4c[4];         // +0x1840..+0x1843
        } m_resultView;
    };
    std::map<int, int> m_killMobCounter;   // +0x1844（ORIG DieMob dispatch_sig
                                           //   击杀计数 map，size 0x18 同位）

    int m_field185c;        // +0x185c
    int m_usedCoinCount;    // +0x1860
    // [b28d-solo-BattleDungeonMapA2] ORIG check_random_appear_hell_dungeon
    // 0x0830a9ea/0x0830aa03：`movzbl 0x1864/0x1865(%eax),%eax; xor $0x1,%eax;
    // test %al,%al; je` ⇒ P248 档②：两字段为 bool（char/uchar 的 `!f` 4.4 恒
    // 产 `test; jne` 缺 xor 段，实测本 TU v4 diff）。1 字节/偏移不变；
    // CPartyDepsA2 的 movzbl 零扩展证据与 bool 相容（bool 读亦 movzbl）。
    bool m_field1864;       // +0x1864（game_start 字节写：hell 无未清任务=1）
    bool m_sealDoorFlag;    // +0x1865（CheckHaveHellPartyPassItem 置位；dungeon_start 读）
    unsigned short m_field1866;             // +0x1866（ORIG 未单独命名，保持占位）
    CWorldMap* m_hellWorldMap;  // +0x1868（CheckHellDungeonFreepassItemHaveAndDel 缓存的世界地图指针）
    int m_overlapRare;      // +0x186c
    int m_overlapUnique;    // +0x1870
    int m_mapHitCount;      // +0x1874
    int m_mapHitSum;        // +0x1878
    int m_mapHitList1[0x2d];   // +0x187c（0xb4 字节 = 45 int）
    int m_mapHitSum2;       // +0x1930
    int m_mapHitList2[0x2d];   // +0x1934
    int m_mapHitSum3;       // +0x19e8
    int m_mapHitList3[0x2d];   // +0x19ec
    char m_field1aa0;       // +0x1aa0
    char m_pad1aa1[3];      // +0x1aa1
    int m_field1aa4[4];     // +0x1aa4（ORIG OnUsedSkill/CheckHackAverageHitCount/
                            //   checkMoveMapHackCnt 按 int 逐槽读改写，共 0x10 字节）
    int m_field1ab4;        // +0x1ab4
    int m_field1ab8;        // +0x1ab8

    unsigned char m_padSecu[0x14];     // +0x1abc Secu_HackLogCheckByParty
    // ---- CPartyTelePort 区（+0x1ad0，0x24 字节） ----
    unsigned char m_pad1ad0[0x10];   // +0x1ad0
    int m_quickPartyIndex;           // +0x1ae0
    bool m_isQuickParty;             // +0x1ae4
    unsigned char m_pad1ae5[3];      // +0x1ae5
    // +0x1ae8 同址双视角（union 等宽 4B，sizeof/偏移/mangled 零影响）：
    //   m_field1ae8    —— P211：与 int RHS 符号异型→store RHS-first=ORIG @0x0859b814；
    //   m_field1ae8Type—— 与 RandomBuffType 形参同型 mem 直比=ORIG @0x0859b925（P209）。
    union {
        unsigned int m_field1ae8;
        QuickParty::RandomBuffType m_field1ae8Type;
    };
    QuickParty::RandomBuffType m_randomBuffType;  // +0x1aec（ORIG set_gm 赋值为同型无转换：eax 先取址）
    bool m_weekendEvent;             // +0x1af0
    bool m_dungeonMapSaving;         // +0x1af1
    unsigned char m_pad1af2[0x2];    // +0x1af2
    unsigned char m_padPassedMap[0xc]; // +0x1af4 std::vector<MapInfo>
};
#endif  // GAME_CPARTY_H_
